
/* 
   (c) COPYRIGHT URI/MIT 1994-1999
   Please read the full copyright statement in the file COPYRIGHT.

   Authors:
        jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
*/

/*
   Scanner for the DDS. This file works with gnu's flex scanner generator. It
   returns either DATASET, INDEPENDENT, DEPENDENT, ARRAY, MAPS, LIST,
   SEQUENCE, STRUCTURE, FUNCTION, GRID, BYTE, INT32, FLOAT64, STRING, URL, ID
   or one of the single character tokens `{', `}', `;', `=' or `\n' as
   integers. In the case of an ID, the scanner stores a pointer to the lexeme
   in yylval (whose type is char *).

   The scanner discards all comment text.

   The scanner is not reentrant, but can share name spaces with other
   scanners.
   
   Note:
   1) The `defines' file dds.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `ddslex'.
   3) When bison builds the dds.tab.h file, it uses `dds' instead of `yy' for
   variable name prefixes (e.g., yylval --> ddslval).
   
   jhrg 8/29/94 
*/

/* 
 * $Log: dds.lex,v $
 * Revision 1.26  2000/08/16 18:29:02  jimg
 * Added dot (.) to the set of characters allowed in a variable name
 *
 * Revision 1.25  2000/06/07 18:07:00  jimg
 * Merged the pc port branch
 *
 * Revision 1.24.20.1  2000/06/02 18:36:38  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.24  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.23  1999/04/29 02:29:36  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.22  1999/03/24 23:32:49  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.21  1999/02/23 01:30:57  jimg
 * Added a YYINPUT define that looks for `Data:\n'. When seen it returns
 * YY_NULL which tells the scanner that EOF has been found. This hack keeps
 * flex from reading into the data while buffering from the input source. This
 * means that the calling code does not have to rewind the input to find the
 * start of the data. Of course, multi-part MIME docs would also solve this.
 * Replace this hack with MP/MIME code!
 *
 * Revision 1.20  1999/01/21 02:51:27  jimg
 * The dds scanner now recognizes the token `Data:' as an EOF marker. This
 * means that the data document can be scanned without splitting the DDS and
 * binary data parts into two files.
 *
 * Revision 1.19.6.1  1999/02/05 09:32:36  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.19  1998/03/26 00:26:24  jimg
 * Added % to the set of characters that can start and ID
 *
 * Revision 1.18  1998/02/05 20:14:01  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.17  1997/12/16 00:46:29  jimg
 * Added `-' to characters allowed in NAME lexeme.
 *
 * Revision 1.16  1997/11/20 20:12:14  jimg
 * Added the NAME lexeme. This is explicitly for datasets with `.' in their
 * names. That character cannot be used in the name of a variable since in
 * DODS it separates parts of a constructor type. However, it can be part of
 * a dataset name. Using a separate lexeme is required by the parser.
 *
 * Revision 1.15  1997/08/11 18:19:34  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.14  1996/10/28 23:43:17  jimg
 * Added unsigned int to set of possible datatypes.
 *
 * Revision 1.13  1996/10/08 17:10:50  jimg
 * Added % to the set of characters allowable in identifier names
 *
 * Revision 1.12  1996/08/26 21:13:13  jimg
 * Changes for version 2.07
 *
 * Revision 1.11  1996/08/13 18:51:52  jimg
 * Added not_used to definition of char rcsid[].
 *
 * Revision 1.10  1996/05/31 23:30:55  jimg
 * Updated copyright notice.
 *
 * Revision 1.9  1996/04/05 00:22:16  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.8  1995/10/23  23:00:24  jimg
 * Removed code enclosed in #ifdef NEVER ... #endif
 *
 * Revision 1.7  1995/07/08  20:18:25  jimg
 * Removed declaration of yywrap().
 *
 * Revision 1.6  1995/02/10  02:57:40  jimg
 * Switched to sh style comments. C and C++ style comments are no longer
 * supported.
 *
 * Revision 1.5  1994/12/21  16:54:50  jimg
 * Repaired damage done to NEVER's definition in previous version.
 *
 * Revision 1.4  1994/12/16  22:22:14  jimg
 * Changed NEVER to be anything not caught by the earlier rules.
 * Fixed // style comments so that // ... <eof> works.
 *
 * Revision 1.3  1994/12/09  21:40:44  jimg
 * Added `=' to the set of recognized lexemes.
 * Added `[' and `]' to the set of rejected characters.
 *
 * Revision 1.2  1994/11/10  19:46:49  jimg
 * Added `/' to the set of characters that make up an identifier.
 *
 * Revision 1.1  1994/09/08  21:10:46  jimg
 * DDS Class test driver and parser and scanner.
 */

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: dds.lex,v 1.26 2000/08/16 18:29:02 jimg Exp $"};

#include <string.h>

#define YYSTYPE char *
#define YY_NO_UNPUT
#define YY_DECL int ddslex YY_PROTO(( void ))
#define YY_INPUT(buf,result,max_size) \
{ \
fgets((buf), (max_size), yyin); \
result = (feof(yyin) || strcmp(buf, "Data:\n") == 0) ? YY_NULL : strlen(buf); \
}

#include "dds.tab.h"

int dds_line_num = 1;

%}
    
%x comment

SCAN_DATASET 		DATASET|Dataset|dataset 
SCAN_INDEPENDENT 	INDEPENDENT|Independent|independent
SCAN_DEPENDENT 		DEPENDENT|Dependent|dependent
SCAN_ARRAY			ARRAY|Array|array
SCAN_MAPS 			MAPS|Maps|maps
SCAN_LIST 			LIST|List|list
SCAN_SEQUENCE 		SEQUENCE|Sequence|sequence
SCAN_STRUCTURE 		STRUCTURE|Structure|structure
SCAN_GRID 			GRID|Grid|grid
SCAN_BYTE 			BYTE|Byte|byte
SCAN_INT16 			INT16|Int16|int16
SCAN_UINT16 		UINT16|UInt16|uint16
SCAN_INT32 			INT32|Int32|int32
SCAN_UINT32 		UINT32|UInt32|uint32
SCAN_FLOAT32 		FLOAT32|Float32|float32
SCAN_FLOAT64 		FLOAT64|Float64|float64
SCAN_STRING 		STRING|String|string
SCAN_URL 			URL|Url|url

SCAN_ID				[a-zA-Z_%.][-a-zA-Z0-9_/%.]*
SCAN_NAME			[a-zA-Z0-9_/%.][:]?[-a-zA-Z0-9\\_/%.]*
SCAN_INTEGER		[0-9]+
NEVER				[^][{}:;=a-zA-Z0-9_%]

%%

{SCAN_DATASET}			ddslval = yytext; return SCAN_DATASET;
{SCAN_INDEPENDENT}		ddslval = yytext; return SCAN_INDEPENDENT;
{SCAN_DEPENDENT}		ddslval = yytext; return SCAN_DEPENDENT;
{SCAN_ARRAY}			ddslval = yytext; return SCAN_ARRAY;
{SCAN_MAPS}				ddslval = yytext; return SCAN_MAPS;
{SCAN_LIST}				ddslval = yytext; return SCAN_LIST;
{SCAN_SEQUENCE}			ddslval = yytext; return SCAN_SEQUENCE;
{SCAN_STRUCTURE}		ddslval = yytext; return SCAN_STRUCTURE;
{SCAN_GRID}				ddslval = yytext; return SCAN_GRID;
{SCAN_BYTE}				ddslval = yytext; return SCAN_BYTE;
{SCAN_INT16}			ddslval = yytext; return SCAN_INT16;
{SCAN_UINT16}			ddslval = yytext; return SCAN_UINT16;
{SCAN_INT32}			ddslval = yytext; return SCAN_INT32;
{SCAN_UINT32}			ddslval = yytext; return SCAN_UINT32;
{SCAN_FLOAT32}			ddslval = yytext; return SCAN_FLOAT32;
{SCAN_FLOAT64}			ddslval = yytext; return SCAN_FLOAT64;
{SCAN_STRING}			ddslval = yytext; return SCAN_STRING;
{SCAN_URL}				ddslval = yytext; return SCAN_URL;

{SCAN_ID}				ddslval = yytext; return SCAN_ID;
{SCAN_INTEGER}			ddslval = yytext; return SCAN_INTEGER;
{SCAN_NAME}				ddslval = yytext; return SCAN_NAME;

"{" 	    	    	return (int)*yytext;
"}" 	    	    	return (int)*yytext;
"["			return (int)*yytext;
"]"			return (int)*yytext;
":"			return (int)*yytext;
";" 	    	    	return (int)*yytext;
"="			return (int)*yytext;

[ \t]+
\n	    	    	++dds_line_num;
<INITIAL><<EOF>>    	yy_init = 1; dds_line_num = 1; yyterminate();

"#"	    	    	BEGIN(comment);
<comment>[^\n]*
<comment>\n		++dds_line_num; BEGIN(INITIAL);
<comment><<EOF>>        yy_init = 1; dds_line_num = 1; yyterminate();

"Data:\n"		yyterminate();

{NEVER}                 {
                          if (yytext) {	/* suppress msgs about `' chars */
                            fprintf(stderr, "Character `%c' is not", *yytext);
                            fprintf(stderr, " allowed and has been ignored\n");
			  }
			}
%%

int 
yywrap(void)
{
    return 1;
}
