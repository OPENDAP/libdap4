
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

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: gse.lex,v 1.6 2001/09/28 17:50:07 jimg Exp $"};

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

%option noyywrap

NAN     [Nn][Aa][Nn]
INF     [Ii][Nn][Ff]

SCAN_ID		[a-zA-Z_/%.][-a-zA-Z0-9_/%.#:+\\]*
SCAN_INT	[-+]?[0-9]+

SCAN_MANTISA	([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+)
SCAN_EXPONENT	(E|e)[-+]?[0-9]+

SCAN_FLOAT	([-+]?{SCAN_MANTISA}{SCAN_EXPONENT}?)|({NAN})|({INF})

SCAN_EQUAL	=
SCAN_NOT_EQUAL	!=
SCAN_GREATER	>
SCAN_GREATER_EQL >=
SCAN_LESS	<
SCAN_LESS_EQL	<=

NEVER		[^a-zA-Z0-9_/%.#:+\-,]

%%

{SCAN_ID}	store_id(); return SCAN_ID;

{SCAN_INT}	store_int32(); return SCAN_INT;
{SCAN_FLOAT}	store_float64(); return SCAN_FLOAT;

{SCAN_EQUAL}	store_op(SCAN_EQUAL); return SCAN_EQUAL;
{SCAN_NOT_EQUAL} store_op(SCAN_NOT_EQUAL); return SCAN_NOT_EQUAL;
{SCAN_GREATER}	store_op(SCAN_GREATER); return SCAN_GREATER;
{SCAN_GREATER_EQL} store_op(SCAN_GREATER_EQL); return SCAN_GREATER_EQL;
{SCAN_LESS}	store_op(SCAN_LESS); return SCAN_LESS;
{SCAN_LESS_EQL}	store_op(SCAN_LESS_EQL); return SCAN_LESS_EQL;

%%

// Three glue routines for string scanning. These are not declared in the
// header gse.tab.h nor is YY_BUFFER_STATE. Including these here allows them
// to see the type definitions in lex.gse.c (where YY_BUFFER_STATE is
// defined) and allows callers to declare them (since callers outside of this
// file cannot declare YY_BUFFER_STATE variable).

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

// Note that the grid() CE funxtion only deals with numeric maps (8/28/2001
// jhrg) and that all comparisons are done using doubles. 

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

/*
 * $Log: gse.lex,v $
 * Revision 1.6  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.4.4.3  2001/09/25 20:21:05  jimg
 * Fixed EQUAL token. Added NOT EQUAL.
 *
 * Revision 1.5  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.4.4.2  2001/08/16 17:26:20  edavis
 * Use "%option noyywrap" instead of defining yywrap() to return 1.
 *
 * Revision 1.4.4.1  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.4  2000/09/22 02:17:23  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.3  2000/06/07 18:07:01  jimg
 * Merged the pc port branch
 *
 * Revision 1.2.20.1  2000/06/02 18:39:03  rmorris
 * Mod's for port to win32.
 *
 * Revision 1.2  1999/04/29 02:29:37  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.1  1999/01/21 02:07:44  jimg
 * Created
 *
 */

