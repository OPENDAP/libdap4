
/* -*- C++ -*- */

/*
  (c) COPYRIGHT URI/MIT 1999
  Please read the full copyright statement in the file COPYRIGHT.

  Authors:
	jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
*/

/*
  Scanner for grid selection sub-expressions. The scanner is not reentrant,
  but can share a name space with other scanners.

   Note:
   1) The `defines' file gse.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `gse_lex'.
   3) When bison builds the gse.tab.h file, it uses `gse_' instead
   of `yy' for variable name prefixes (e.g., yylval --> gse_lval).

   1/13/99 jhrg
*/

/*
 * $Log: gse.lex,v $
 * Revision 1.1  1999/01/21 02:07:44  jimg
 * Created
 *
 */

%{

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: gse.lex,v 1.1 1999/01/21 02:07:44 jimg Exp $"};

#define YY_DECL int gse_lex YY_PROTO(( void ))
#define ID_MAX 256
#define YY_NO_UNPUT 1
#define YY_NO_INPUT 1

#include <string.h>

#include "gse.tab.h"

static void store_int32();
static void store_float64();
static void store_id();
static void store_op(int op);

%}

ID		[a-zA-Z_%][a-zA-Z0-9_/%]*
INT		[-+]?[0-9]+

MANTISA		([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+)
EXPONENT	(E|e)[-+]?[0-9]+

FLOAT		[-+]?{MANTISA}{EXPONENT}?

EQUAL		=
GREATER		>
GREATER_EQL	>=
LESS		<
LESS_EQL	<=

NEVER		[^a-zA-Z0-9_%.]

%%

{ID}		store_id(); return ID;

{INT}		store_int32(); return INT;
{FLOAT}		store_float64(); return FLOAT;

{EQUAL}		store_op(EQUAL); return EQUAL;
{GREATER}	store_op(GREATER); return GREATER;
{GREATER_EQL}	store_op(GREATER_EQL); return GREATER_EQL;
{LESS}		store_op(LESS); return LESS;
{LESS_EQL}	store_op(LESS_EQL); return LESS_EQL;

%%

// This function must be supplied.

static int
yywrap(void)
{
    return 1;
}

// Three glue routines for string scanning. These are not declared in the
// header expr.tab.h nor is YY_BUFFER_STATE. Including these here allows them
// to see the type definitions in lex.expr.c (where YY_BUFFER_STATE is
// defined) and allows callers to declare them (since callers outside of this
// file cannot declare YY_BUFFER_STATE variable). Note that I changed the name
// of the expr_scan_string function to expr_string because C++ cannot
// distinguish by return type. 1/12/99 jhrg

void *
gse_string(const char *str)
{
    return (void *)gse__scan_string(str);
}

void
gse_switch_to_buffer(void *buf)
{
    gse__switch_to_buffer((YY_BUFFER_STATE)buf);
}

void
gse_delete_buffer(void *buf)
{
    gse__delete_buffer((YY_BUFFER_STATE)buf);
}

// Note that since atoi() (or strtol()) does not care about signedness, this
// will dump an unsigned value into a signed variable. However, if the value
// is used in an unsigned context (i.e., with an operand that  is of unsigned
// type) then the signed value can be cast back to unsigned without losing
// information.

static void
store_int32()
{
    gse_lval.val = atof(yytext);
}

static void
store_float64()
{
    gse_lval.val = atof(yytext);
}

static void
store_id()
{
    strncpy(gse_lval.id, yytext, ID_MAX-1);
    gse_lval.id[ID_MAX-1] = '\0';
}

static void
store_op(int op)
{
    gse_lval.op = op;
}
