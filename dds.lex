
/*
   Scanner for the DDS. This file works with gnu's flex scanner generator. It
   returns either DATASET, INDEPENDENT, DEPENDENT, ARRAY, MAPS, LIST,
   SEQUENCE, STRUCTURE, FUNCTION, GRID, BYTE, INT32, FLOAT64, STRING, URL, ID
   or one of the single character tokens `{', `}', `;', or `\n' as
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
/* Revision 1.1  1994/09/08 21:10:46  jimg
/* DDS Class test driver and parser and scanner.
/*
 */

%{
static char rcsid[]={"$Id: dds.lex,v 1.1 1994/09/08 21:10:46 jimg Exp $"};

#include <string.h>

#define YYSTYPE char *
#define YY_DECL int ddslex YY_PROTO(( void ))

#include "dds.tab.h"

int dds_line_num = 1;
static int start_line;		/* used in comment error handlers */
int yywrap(void);

%}
    
%x comment
%x comment_new

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
NEVER		[^a-zA-Z0-9_{};]

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

[ \t]+
\n	    	    	++dds_line_num;
<INITIAL><<EOF>>    	yy_init = 1; dds_line_num = 1; yyterminate();

"/*"	    	    	BEGIN(comment); start_line = dds_line_num;
<comment>[^*\n]*
<comment>[^*\n]*\n  	++dds_line_num;
<comment>"*"+[^*/\n]*
<comment>"*"+[^*/\n]*\n ++dds_line_num;
<comment>"*"+"/"    	BEGIN(INITIAL);
<comment><<EOF>>	{
                          char msg[256];
			  sprintf(msg,
				  "Unterminated comment (starts on line %d)\n",
				  start_line);
			  YY_FATAL_ERROR(msg);
                        }
			
"//"	    	    	BEGIN(comment_new);
<comment_new>[^\n]*\n	++dds_line_num; BEGIN(INITIAL);

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
