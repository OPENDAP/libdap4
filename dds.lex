
/* 
   (c) COPYRIGHT URI/MIT 1994-1996
   Please read the full copyright statement in the file COPYRIGH.  

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
 * The dds scanner now recognizes the token `Data:' as an EOF marker. This means
 * that the data document can be scanned without splitting the DDS and binary
 * data parts into two files.
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
 * Added __unused__ to definition of char rcsid[].
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

static char rcsid[] __unused__ = {"$Id: dds.lex,v 1.22 1999/03/24 23:32:49 jimg Exp $"};

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

DATASET 	DATASET|Dataset|dataset 
INDEPENDENT 	INDEPENDENT|Independent|independent
DEPENDENT 	DEPENDENT|Dependent|dependent
ARRAY		ARRAY|Array|array
MAPS 		MAPS|Maps|maps
LIST 		LIST|List|list
SEQUENCE 	SEQUENCE|Sequence|sequence
STRUCTURE 	STRUCTURE|Structure|structure
FUNCTION 	FUNCTION|Function|function
GRID 		GRID|Grid|grid
BYTE 		BYTE|Byte|byte
INT16 		INT16|Int16|int16
UINT16 		UINT16|UInt16|uint16
INT32 		INT32|Int32|int32
UINT32 		UINT32|UInt32|uint32
FLOAT32 	FLOAT32|Float32|float32
FLOAT64 	FLOAT64|Float64|float64
STRING 		STRING|String|string
URL 		URL|Url|url

ID  		[a-zA-Z_%][-a-zA-Z0-9_/%]*
NAME            [a-zA-Z0-9_/%.][-a-zA-Z0-9_/%.]*
INTEGER		[0-9]+
NEVER		[^][{}:;=a-zA-Z0-9_%]

%%

{DATASET}		ddslval = yytext; return DATASET;
{INDEPENDENT}		ddslval = yytext; return INDEPENDENT;
{DEPENDENT}		ddslval = yytext; return DEPENDENT;
{ARRAY}			ddslval = yytext; return ARRAY;
{MAPS}			ddslval = yytext; return MAPS;
{LIST}			ddslval = yytext; return LIST;
{SEQUENCE}		ddslval = yytext; return SEQUENCE;
{STRUCTURE}		ddslval = yytext; return STRUCTURE;
{FUNCTION}		ddslval = yytext; return FUNCTION;
{GRID}			ddslval = yytext; return GRID;
{BYTE}			ddslval = yytext; return BYTE;
{INT16}			ddslval = yytext; return INT16;
{UINT16}		ddslval = yytext; return UINT16;
{INT32}			ddslval = yytext; return INT32;
{UINT32}		ddslval = yytext; return UINT32;
{FLOAT32}		ddslval = yytext; return FLOAT32;
{FLOAT64}		ddslval = yytext; return FLOAT64;
{STRING}		ddslval = yytext; return STRING;
{URL}			ddslval = yytext; return URL;

{ID}  	    	    	ddslval = yytext; return ID;
{INTEGER}		ddslval = yytext; return INTEGER;
{NAME}                  ddslval = yytext; return NAME;

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
