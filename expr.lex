/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

/*
  Scanner for constraint expressions. The scanner returns tokens for each of
  the relational and selection operators. It requires GNU flex version 2.5.2
  or newer.

  The scanner is not reentrant, but can share a name space with other
  scanners. 

   Note:
   1) The `defines' file expr.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `exprlex'.
   3) When bison builds the expr.tab.h file, it uses `expr' instead of `yy'
   for variable name prefixes (e.g., yylval --> exprlval).

  jhrg 9/5/95
*/

/* $Log: expr.lex,v $
/* Revision 1.1  1995/10/13 03:03:17  jimg
/* Scanner. Incorporates Glenn's suggestions.
/*
 */

%{
static char rcsid[]={"$Id: expr.lex,v 1.1 1995/10/13 03:03:17 jimg Exp $"};

#include <string.h>

#define YYSTYPE char *
#define YY_DECL int exprlex YY_PROTO(( void ))

#include "expr.tab.h"

%}

%x quote
    
ID		[a-zA-Z_][a-zA-Z0-9_]*
INT		[-+]?[0-9]+

MANTISA		([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+)
EXPONENT	(E|e)[-+]?[0-9]+

FLOAT		[-+]?{MANTISA}{EXPONENT}?

STR		[-+a-zA-Z0-9_/]+

EQUAL		=
NOT_EQUAL	!=
GREATER		>
GREATER_EQL	>=
LESS		<
LESS_EQL	<=
REGEXP		=~

NEVER		[^][":*.)(,&a-zA-Z0-9_]

%%

{ID}		exprlval = yytext; return ID;
{INT}		exprlval = yytext; return INT;

{FLOAT}		exprlval = yytext; return FLOAT;

{STR}		exprlval = yytext; return STR;

{EQUAL}		exprlval = yytext; return EQUAL;
{NOT_EQUAL}	exprlval = yytext; return NOT_EQUAL;
{GREATER}	exprlval = yytext; return GREATER;
{GREATER_EQL}	exprlval = yytext; return GREATER_EQL;
{LESS}		exprlval = yytext; return LESS;
{LESS_EQL}	exprlval = yytext; return LESS_EQL;
{REGEXP}	exprlval = yytext; return REGEXP;

"["    	    	return (int)*yytext;
"]"    	    	return (int)*yytext;
":"    	    	return (int)*yytext;
"*"		return (int)*yytext;
"."		return (int)*yytext;
","		return (int)*yytext;
"&"		return (int)*yytext;
"("		return (int)*yytext;
")"		return (int)*yytext;
"{"		return (int)*yytext;
"}"		return (int)*yytext;

[ \t\n]+
<INITIAL><<EOF>> yy_init = 1; yyterminate();

\"			BEGIN(quote); yymore();
<quote>[^"\\]*       	yymore();
<quote>\\.		yymore();
<quote>\"		{ 
    			  BEGIN(INITIAL); 
			  exprlval = yytext;
			  return STR;
                        }
<quote><<EOF>>		{
                          char msg[256];
			  sprintf(msg, "Unterminated quote\n");
			  YY_FATAL_ERROR(msg);
                        }

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
