
/* -*- C++ -*- */

/*
  (c) COPYRIGHT URI/MIT 1994-1996
  Please read the full copyright statement in the file COPYRIGH.  

  Authors:
	jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
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

/* 
 * $Log: expr.lex,v $
 * Revision 1.20  1999/03/24 23:30:55  jimg
 * Fixed some of the comments.
 *
 * Revision 1.19  1999/01/21 02:21:44  jimg
 * Made the store_op(), ... functions static.
 * Added glue routines for scanning strings.
 *
 * Revision 1.18  1998/10/23 00:09:03  jimg
 * Fixed an array write error where exprlval.id was over-written by writing to
 * element ID_MAX. The end of the array is ID_MAX-1.
 *
 * Revision 1.17  1998/10/21 16:45:50  jimg
 * Now includes RValue.h. Needed because expr.tab.h needs it.
 *
 * Revision 1.16  1998/03/26 00:26:23  jimg
 * Added % to the set of characters that can start and ID
 *
 * Revision 1.15  1997/08/11 18:19:36  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.14  1997/02/24 18:18:23  jimg
 * Removed `rule' for "." since it cannot be matched.
 *
 * Revision 1.13  1996/11/13 19:23:15  jimg
 * Fixed debugging.
 *
 * Revision 1.12  1996/10/08 17:10:52  jimg
 * Added % to the set of characters allowable in identifier names
 *
 * Revision 1.11  1996/08/26 21:13:17  jimg
 * Changes for version 2.07
 *
 * Revision 1.10  1996/08/13 18:56:24  jimg
 * Added __unused__ to definition of char rcsid[].
 *
 * Revision 1.9  1996/05/31 23:31:03  jimg
 * Updated copyright notice.
 *
 * Revision 1.8  1996/05/14 15:39:01  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.7  1996/04/05 00:22:19  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
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

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: expr.lex,v 1.20 1999/03/24 23:30:55 jimg Exp $"};

#include <string.h>
#include <assert.h>

#include <String.h>
#include <SLList.h>

#define YY_DECL int exprlex YY_PROTO(( void ))

#include "Error.h"
#include "parser.h"
#include "expr.h"
#include "RValue.h"
#include "expr.tab.h"

static void store_int32();
static void store_float64();
static void store_id();
static void store_str();
static void store_op(int op);

%}

%x quote
    
ID		[a-zA-Z_%][a-zA-Z0-9_/%]*
FIELD           {ID}\.{ID}(\.{ID})*
INT		[-+]?[0-9]+

MANTISA		([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+)
EXPONENT	(E|e)[-+]?[0-9]+

FLOAT		[-+]?{MANTISA}{EXPONENT}?

STR		[-+a-zA-Z0-9_/.%]+

EQUAL		=
NOT_EQUAL	!=
GREATER		>
GREATER_EQL	>=
LESS		<
LESS_EQL	<=
REGEXP		=~

NEVER		[^][*)(,:.&a-zA-Z0-9_%.]

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
","		return (int)*yytext;
"&"		return (int)*yytext;
"("		return (int)*yytext;
")"		return (int)*yytext;
"{"		return (int)*yytext;
"}"		return (int)*yytext;

[ \t\n]+
<INITIAL><<EOF>> yy_init = 1; yyterminate();

\"			BEGIN(quote); yymore();
<quote>[^"\\]*       	yymore(); /*"*/
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

static int
yywrap(void)
{
    return 1;
}

// Three glue routines for string scanning. These are not declared in the
// header expr.tab.h nor is YY_BUFFER_STATE. Including these here allows them
// to see the type definitions in lex.expr.c (where YY_BUFFER_STATE is
// defined) and allows callers to declare them (since callers outside of this
// file cannot declare the YY_BUFFER_STATE variable). Note that I changed the
// name of the expr_scan_string function to expr_string because C++ cannot
// distinguish by return type. 1/12/99 jhrg

void *
expr_string(const char *str)
{
    return (void *)expr_scan_string(str);
}

void
expr_switch_to_buffer(void *buf)
{
    expr_switch_to_buffer((YY_BUFFER_STATE)buf);
}

void
expr_delete_buffer(void *buf)
{
    expr_delete_buffer((YY_BUFFER_STATE)buf);
}

// Note that since atoi() (or strtol()) does not care about signedness, this
// will dump an unsigned value into a signed variable. However, if the value
// is used in an unsigned context (i.e., with an operand that is of unsigned
// type) then the signed value can be cast back to unsigned without losing
// information.

static void
store_int32()
{
    exprlval.val.type = dods_int32_c;
    exprlval.val.v.i = atoi(yytext);
}

static void
store_float64()
{
    exprlval.val.type = dods_float64_c;
    exprlval.val.v.f = atof(yytext);
}

static void
store_id()
{
    strncpy(exprlval.id, yytext, ID_MAX-1);
    exprlval.id[ID_MAX-1] = '\0';
}

static void
store_str()
{
    String *s = new String(yytext);
    int l = s->length();

    *s = s->at(1, l - 2);	/* strip the \"'s from front and back */

    exprlval.val.type = dods_str_c;
    exprlval.val.v.s = s;
}

static void
store_op(int op)
{
    exprlval.op = op;
}
