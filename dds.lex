
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

/* $Log: dds.lex,v $
/* Revision 1.9  1996/04/05 00:22:16  jimg
/* Compiled with g++ -Wall and fixed various warnings.
/*
# Revision 1.8  1995/10/23  23:00:24  jimg
# Removed code enclosed in #ifdef NEVER ... #endif
#
# Revision 1.7  1995/07/08  20:18:25  jimg
# Removed declaration of yywrap().
#
# Revision 1.6  1995/02/10  02:57:40  jimg
# Switched to sh style comments. C and C++ style comments are no longer
# supported.
#
# Revision 1.5  1994/12/21  16:54:50  jimg
# Repaired damage done to NEVER's definition in previous version.
#
# Revision 1.4  1994/12/16  22:22:14  jimg
# Changed NEVER to be anything not caught by the earlier rules.
# Fixed // style comments so that // ... <eof> works.
#
# Revision 1.3  1994/12/09  21:40:44  jimg
# Added `=' to the set of recognized lexemes.
# Added `[' and `]' to the set of rejected characters.
#
# Revision 1.2  1994/11/10  19:46:49  jimg
# Added `/' to the set of characters that make up an identifier.
#
# Revision 1.1  1994/09/08  21:10:46  jimg
# DDS Class test driver and parser and scanner.
#
 */

%{
static char rcsid[]={"$Id: dds.lex,v 1.9 1996/04/05 00:22:16 jimg Exp $"};

#include <string.h>

#define YYSTYPE char *
#define YY_DECL int ddslex YY_PROTO(( void ))

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
INT32 		INT32|Int32|int32
FLOAT64 	FLOAT64|Float64|float64
STRING 		STRING|String|string
URL 		URL|Url|url

ID  		[a-zA-Z_][a-zA-Z0-9_]*
INTEGER		[0-9]+
NEVER		[^][{}:;=a-zA-Z0-9_]

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
{INT32}			ddslval = yytext; return INT32;
{FLOAT64}		ddslval = yytext; return FLOAT64;
{STRING}		ddslval = yytext; return STRING;
{URL}			ddslval = yytext; return URL;

{ID}  	    	    	ddslval = yytext; return ID;
{INTEGER}		ddslval = yytext; return INTEGER;

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
