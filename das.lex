
/*
   Scanner for the DAS. This file works with gnu's flex scanner generator. It
   returns either ATTR, ID, VAL or one of the single character tokens `{',
   `}', `;', or `\n' as integers. In the case of an ID or VAL, the scanner
   stores a pointer to the lexeme in yylval (whose type is char *).

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
   
   jhrg 7/12/94 */

/* $Log: das.lex,v $
/* Revision 1.1  1994/07/25 14:23:01  jimg
/* First attempt at the DAS parsing software.
/*
 */

%{
static char rcsid[]={"$Id: das.lex,v 1.1 1994/07/25 14:23:01 jimg Exp $"};

#include <string.h>

#define YYSTYPE char *
#define YY_DECL int daslex YY_PROTO(( void ))
    
#include "das.tab.h"

void trunc1(void);
void rmbslash(void);
int line_num = 1;
%}
    
%x quote
%x comment
%x comment_new

ID  	[a-zA-Z_][a-zA-Z0-9_]*
VAL 	[a-zA-Z0-9_.+-]+
ATTR 	attributes|Attributes|ATTRIBUTES

%%

{ATTR}	    	    	return ATTR;
{ID}  	    	    	daslval = yytext; return ID;
{VAL}	    	    	daslval = yytext; return VAL;
"{" 	    	    	return (int)*yytext;
"}" 	    	    	return (int)*yytext;
";" 	    	    	return (int)*yytext;

[ \t]+
\n	    	    	++line_num;
<INITIAL><<EOF>>    	yyterminate();

\"			BEGIN(quote);
<quote>[^"\n\\]*	yymore();
<quote>[^"\n\\]*\n	yymore(); ++line_num;
<quote>\\.		yymore();
<quote>\"		{ BEGIN(INITIAL); trunc1(); rmbslash();
			  daslval = yytext; return VAL; }
<quote><<EOF>>		YY_FATAL_ERROR("Unterminated quote");

"/*"	    	    	BEGIN(comment);
<comment>[^*\n]*
<comment>[^*\n]*\n  	++line_num;
<comment>"*"+[^*/\n]*
<comment>"*"+[^*/\n]*\n ++line_num;
<comment>"*"+"/"    	BEGIN(INITIAL);
<comment><<EOF>>	YY_FATAL_ERROR("Unterminated comment");
			
"//"	    	    	BEGIN(comment_new);
<comment_new>[^\n]*\n	++line_num; BEGIN(INITIAL);


%%

/*
   Remove the last character from yytext. This is used to remove the trailing
   quote (") from a quotation (the fifth `quote' patern causes the trailing
   quote to be accumulated by yytext).
*/

void
trunc1(void)
{
    *(yytext + yyleng - 1) = '\0';
    --yyleng;
}

/*
   Remove (cut) the backslash (\) character in each instance of backslash
   followed by some character. If yytext holds `t \" \+ \\ t', then rmbslash
   will change that to `t " + \ t'.
*/

void
rmbslash(void)
{
    char *slash = strchr(yytext, '\\');

    while (slash) {
        strcpy(slash, slash + 1);
	slash = strchr(slash + 1, '\\');
    }
}
