
/* 
   (c) COPYRIGHT URI/MIT 1994-2000
   Please read the full copyright statement in the file COPYRIGHT.

   Authors:
        jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
*/

/*
   Scanner for the DAS. This file works with gnu's flex scanner generator. It
   returns either ATTR, ID, VAL, TYPE or one of the single character tokens
   `{', `}', `;', `,' or `\n' as integers. In the case of an ID or VAL, the
   scanner stores a pointer to the lexeme in yylval (whose type is char *).

   The scanner discards all comment text.

   The scanner returns quoted strings as VALs. Any characters may appear in a
   quoted string except backslash (\) and quote("). To include these escape
   them with a backslash.
   
   The scanner is not reentrant, but can share name spaces with other
   scanners.
   
   Note:
   1) The `defines' file das.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `daslex'.
   3) When bison builds the das.tab.h file, it uses `das' instead of `yy' for
   variable name prefixes (e.g., yylval --> daslval).
   4) The quote stuff is very complicated because we want backslash (\)
   escapes to work and because we want line counts to work too. In order to
   properly scan a quoted string two C functions are used: one to remove the
   escape characters from escape sequences and one to remove the trailing
   quote on the end of the string. 

   jhrg 7/12/94 

   NB: We don't remove the \'s or ending quotes any more -- that way the
   printed das can be reparsed. 9/28/94. 
*/

%{
#include "config_dap.h"

static char rcsid[] not_used ={"$Id: das.lex,v 1.33 2001/09/28 17:50:07 jimg Exp $"};

#include <string.h>
#include <assert.h>
#include "parser.h"

#define YYSTYPE char *
#define YY_DECL int daslex YY_PROTO(( void ))

#include "das.tab.h"

int das_line_num = 1;
static int start_line;		/* used in quote and comment error handlers */

%}
    
%option noyywrap
%x quote
%x comment

NAN     [Nn][Aa][Nn]
INF     [Ii][Nn][Ff]

ID  	[a-zA-Z_/%.][-a-zA-Z0-9_/%.#:+\\()]*
INT	[-+]?[0-9]+

MANTISA ([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+)
EXPONENT (E|e)[-+]?[0-9]+

FLOAT	([-+]?{MANTISA}{EXPONENT}?)|({NAN})|({INF})

STR 	[-+a-zA-Z0-9_./:%+\\()]+

ATTR 	attributes|Attributes|ATTRIBUTES

ALIAS   ALIAS|Alias|alias
BYTE	BYTE|Byte|byte
INT16	INT16|Int16|int16
UINT16	UINT16|UInt16|Uint16|uint16
INT32	INT32|Int32|int32
UINT32	UINT32|UInt32|Uint32|uint32
FLOAT32 FLOAT32|Float32|float32
FLOAT64 FLOAT64|Float64|float64
STRING  STRING|String|string
URL	URL|Url|url

NEVER   [^a-zA-Z0-9_/%.#:+\\()\-{};,[\]]

%%


{ATTR}	    	    	daslval = yytext; return SCAN_ATTR;

{ALIAS}                 daslval = yytext; return SCAN_ALIAS;
{BYTE}                  daslval = yytext; return SCAN_BYTE;
{INT16}                 daslval = yytext; return SCAN_INT16;
{UINT16}                daslval = yytext; return SCAN_UINT16;
{INT32}                 daslval = yytext; return SCAN_INT32;
{UINT32}                daslval = yytext; return SCAN_UINT32;
{FLOAT32}               daslval = yytext; return SCAN_FLOAT32;
{FLOAT64}               daslval = yytext; return SCAN_FLOAT64;
{STRING}                daslval = yytext; return SCAN_STRING;
{URL}                   daslval = yytext; return SCAN_URL;

{INT}	    	    	daslval = yytext; return SCAN_INT;
{FLOAT}	    	    	daslval = yytext; return SCAN_FLOAT;
{ID}  	    	    	daslval = yytext; return SCAN_ID;
{STR}	    	    	daslval = yytext; return SCAN_STR;

"{" 	    	    	return (int)*yytext;
"}" 	    	    	return (int)*yytext;
";" 	    	    	return (int)*yytext;
","                     return (int)*yytext;

[ \t\r]+
\n	    	    	++das_line_num;
<INITIAL><<EOF>>    	yy_init = 1; das_line_num = 1; yyterminate();

"#"	    	    	BEGIN(comment);
<comment>[^\r\n]*
<comment>\n		++das_line_num; BEGIN(INITIAL);
<comment>\r\n		++das_line_num; BEGIN(INITIAL);
<comment><<EOF>>        yy_init = 1; das_line_num = 1; yyterminate();

\"			BEGIN(quote); start_line = das_line_num; yymore();
<quote>[^"\r\n\\]*	yymore();
<quote>[^"\r\n\\]*\n	yymore(); ++das_line_num;
<quote>[^"\r\n\\]*\r\n	yymore(); ++das_line_num;
<quote>\\.		yymore();
<quote>\"		{ 
    			  BEGIN(INITIAL); 

			  daslval = yytext;

			  return SCAN_STR;
                        }
<quote><<EOF>>		{
                          char msg[256];
			  sprintf(msg,
				  "Unterminated quote (starts on line %d)\n",
				  start_line);
			  YY_FATAL_ERROR(msg);
                        }

{NEVER}                 {
                          if (yytext) {	/* suppress msgs about `' chars */
                            fprintf(stderr, "Character `%c' is not", *yytext);
                            fprintf(stderr, " allowed (except within");
			    fprintf(stderr, " quotes) and has been ignored\n");
			  }
			}
%%

/*
 * $Log: das.lex,v $
 * Revision 1.33  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.30.4.4  2001/09/11 03:57:08  jimg
 * Fixed STR regex.
 *
 * Revision 1.32  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.30.4.3  2001/08/16 17:26:19  edavis
 * Use "%option noyywrap" instead of defining yywrap() to return 1.
 *
 * Revision 1.30.4.2  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.31  2001/01/26 19:48:09  jimg
 * Merged with release-3-2-3.
 *
 * Revision 1.30.4.1  2000/12/13 03:27:15  jimg
 * *** empty log message ***
 *
 * Revision 1.30  2000/09/07 16:16:06  jimg
 * Added SCAN_ prefix back onto returned constants. This was also lost during
 * the last merge. Moving comments...
 *
 * Revision 1.29  2000/09/07 15:43:11  jimg
 * Fixed a bungled merge. Watch out for long diffs in merged code when moving
 * log comments from the start to the end of files.
 *
 * Revision 1.28  2000/08/31 23:44:16  jimg
 * Merged with 3.1.10
 *
 * Revision 1.26.6.1 2000/08/31 20:54:36 jimg Added \r to the set of
 * characters that are ignored. This is an untested fix (?) for UNIX clients
 * that read from servers run on win32 machines (e.g., the Java-SQL server
 * can be run on a win32 box).
 *
 * Revision 1.27  2000/06/07 18:07:00  jimg
 * Merged the pc port branch
 *
 * Revision 1.26.20.1  2000/06/02 18:36:38  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.26  1999/04/29 02:29:35  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.25  1999/03/24 23:34:02  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.24.6.1  1999/02/05 09:32:35  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.24  1998/03/26 00:26:24  jimg
 * Added % to the set of characters that can start and ID
 *
 * Revision 1.23  1997/07/01 00:14:24  jimg
 * Removed some old code (commented out).
 *
 * Revision 1.22  1997/05/13 23:32:18  jimg
 * Added changes to handle the new Alias and lexical scoping rules.
 *
 * Revision 1.21  1997/05/06 18:24:01  jimg
 * Added Alias and Global to the set of known tokens.
 * Added many new characters to set of things that can appear in an
 * identifier.
 *
 * Revision 1.20  1996/10/28 23:06:15  jimg
 * Added unsigned int to set of possible attribute value types.
 *
 * Revision 1.19  1996/10/08 17:10:49  jimg
 * Added % to the set of characters allowable in identifier names
 *
 * Revision 1.18  1996/08/26 21:13:10  jimg
 * Changes for version 2.07
 *
 * Revision 1.17  1996/08/13 18:44:16  jimg
 * Added not_used to definition of char rcsid[].
 * Added parser.h to included files.
 *
 * Revision 1.16  1996/05/31 23:30:51  jimg
 * Updated copyright notice.
 *
 * Revision 1.15  1996/05/14 15:38:50  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.14  1995/10/23  22:52:34  jimg
 * Removed code that was NEVER'd or simply not used.
 *
 * Revision 1.13  1995/07/08  18:32:08  jimg
 * Edited comments.
 * Removed unnecessary declarations.
 *
 * Revision 1.12  1995/02/10  02:44:58  jimg
 * Scanner now returns different lexemes for each of the different scalar
 * data types.
 * Comments are now done as for sh; the C and C++ style comments are no
 * longer supported.
 *
 * Revision 1.11  1994/12/21  15:31:07  jimg
 * Undid 'fix' to NEVER definition - it was wrong.
 *
 * Revision 1.10  1994/12/16  22:22:43  jimg
 * Fixed NEVER so that it is anything not caught by the earlier rules.
 *
 * Revision 1.9  1994/12/09  21:39:29  jimg
 * Fixed scanner's treatment of `//' comments which end with an EOF
 * (instead of a \n).
 *
 * Revision 1.8  1994/12/08  16:53:24  jimg
 * Modified the NEVER regexp so that `[' and `]' are not allowed in the
 * input stream. Previously they were not recognized but also not reported
 * as errors.
 *
 * Revision 1.7  1994/12/07  21:17:07  jimg
 * Added `,' (comma) to set of single character tokens recognized by the
 * scanner. Comma is the separator for elements in attribute vectors.
 *
 * Revision 1.6  1994/11/10  19:46:10  jimg
 * Added `/' to the set of characters that make up an identifier.
 *
 * Revision 1.5  1994/10/05  16:41:58  jimg
 * Added `TYPE' to the grammar for the DAS.
 * See Also: DAS.{cc,h} which were modified to handle TYPE.
 *
 * Revision 1.4  1994/08/29  14:14:51  jimg
 * Fixed a problem with quoted strings - previously quotes were stripped
 * when scanned, but this caused problems when they were printed because
 * the printed string could not be recanned. In addition, escape characters
 * are no longer removed during scanning. The functions that performed these
 * operations are still in the scanner, but their calls have been commented
 * out.
 *
 * Revision 1.3  1994/08/02  18:50:03  jimg
 * Fixed error in illegal character message. Arrgh!
 *
 * Revision 1.2  1994/08/02  18:46:43  jimg
 * Changed communication mechanism from C++ class back to daslval.
 * Auxiliary functions now pass yytext,... instead of using globals.
 * Fixed scanning errors.
 * Scanner now sets yy_init on successful termination.
 * Scanner has improved error reporting - particularly in the unterminated
 * comment and quote cases.
 * Better rejection of illegal characters.
 *
 * Revision 1.3  1994/07/25  19:01:17  jimg
 * Modified scanner and parser so that they can be compiled with g++ and
 * so that they can be linked using g++. They will be combined with a C++
 * method using a global instance variable.
 * Changed the name of line_num in the scanner to das_line_num so that
 * global symbol won't conflict in executables/libraries with multiple
 * scanners.
 *
 * Revision 1.2  1994/07/25  14:26:41  jimg
 * Test files for the DAS/DDS parsers and symbol table software.
 *
 * Revision 1.1  1994/07/21  19:21:32  jimg
 * First version of DAS scanner - works with C. 
 */
