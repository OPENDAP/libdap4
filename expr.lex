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
/* Revision 1.7  1996/04/05 00:22:19  jimg
/* Compiled with g++ -Wall and fixed various warnings.
/*
 * Revision 1.6  1996/03/02 01:19:04  jimg
 * Fixed comments.
 * Fixed a bug in store_str(); leading and trailing double quotes are now
 * stripped from strings.
 *
 * Revision 1.5  1996/02/01 17:43:16  jimg
 * Added support for lists as operands in constraint expressions.
 *
 * Revision 1.4  1995/12/09  01:07:39  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.3  1995/12/06  18:57:37  jimg
 * Because the %union{} changed, the return types of some of the rules also
 * changed.
 * Returns integer codes for relops.
 * Returns a tagged union for most other values.
 *
 * Revision 1.2  1995/10/23  23:11:31  jimg
 * Fixed scanner to use the new definition of YYSTYPE.
 *
 * Revision 1.1  1995/10/13  03:03:17  jimg
 * Scanner. Incorporates Glenn's suggestions.
 *
 */

%{
static char rcsid[]={"$Id: expr.lex,v 1.7 1996/04/05 00:22:19 jimg Exp $"};

#include <string.h>

#include <String.h>
#include <SLList.h>

#define YY_DECL int exprlex YY_PROTO(( void ))

#include "parser.h"
#include "expr.h"
#include "expr.tab.h"

void store_int32();
void store_float64();
void store_id();
void store_str();
void store_op(int op);

%}

%x quote
    
ID		[a-zA-Z_][a-zA-Z0-9_]*
FIELD           {ID}\.{ID}(\.{ID})*
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

NEVER		[^][*)(,:.&a-zA-Z0-9_]

%%

{ID}		store_id(); return ID;
{FIELD}		store_id(); return FIELD;
{INT}		store_int32(); return INT;

{FLOAT}		store_float64(); return FLOAT;

{STR}		store_str(); return STR;

{EQUAL}		store_op(EQUAL); return EQUAL;
{NOT_EQUAL}	store_op(NOT_EQUAL); return NOT_EQUAL;
{GREATER}	store_op(GREATER); return GREATER;
{GREATER_EQL}	store_op(GREATER_EQL); return GREATER_EQL;
{LESS}		store_op(LESS); return LESS;
{LESS_EQL}	store_op(LESS_EQL); return LESS_EQL;
{REGEXP}	store_op(REGEXP); return REGEXP;

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
                          store_str();
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

void
store_int32()
{
    exprlval.val.type = d_int32_t;
    exprlval.val.v.i = atoi(yytext);
}

void
store_float64()
{
    exprlval.val.type = d_float64_t;
    exprlval.val.v.f = atof(yytext);
}

void
store_id()
{
    strncpy(exprlval.id, yytext, ID_MAX-1);
    exprlval.id[ID_MAX] = '\0';
}

void
store_str()
{
    String *s = new String(yytext);
    int l = s->length();

    *s = s->at(1, l - 2);	/* strip the \"'s from front and back */

    exprlval.val.type = d_str_t;
    exprlval.val.v.s = s;
}

void
store_op(int op)
{
    exprlval.op = op;
}
