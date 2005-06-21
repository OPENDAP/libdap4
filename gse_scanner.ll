
/*
 -*- mode: c++; c-basic-offset:4 -*-

 This file is part of libdap, A C++ implementation of the OPeNDAP Data
 Access Protocol.

 Copyright (c) 2002,2003 OPeNDAP, Inc.
 Author: James Gallagher <jgallagher@opendap.org>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

 (c) COPYRIGHT URI/MIT 1999
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

static char rcsid[] not_used = {"$Id: gse.lex,v 1.11 2003/12/08 18:02:31 edavis Exp $"};

#include <string>
#include <string.h>

#include "Error.h"

#define YY_DECL int gse_lex YY_PROTO(( void ))
#define ID_MAX 256
#define YY_NO_UNPUT 1
#define YY_NO_INPUT 1
#define YY_FATAL_ERROR(msg) throw(Error(string("Error scanning DAS object text: ") + string(msg)))

#include "gse.h"

using namespace std;

static void store_int32();
static void store_float64();
static void store_id();
static void store_op(int op);

%}

%option noyywrap

NAN     [Nn][Aa][Nn]
INF     [Ii][Nn][Ff]

SCAN_INT	[-+]?[0-9]+

SCAN_MANTISA	([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+)
SCAN_EXPONENT	(E|e)[-+]?[0-9]+

SCAN_FLOAT	([-+]?{SCAN_MANTISA}{SCAN_EXPONENT}?)|({NAN})|({INF})

/* See das.lex for comments about the characters allowed in a WORD.
   10/31/2001 jhrg */

SCAN_WORD	[-+a-zA-Z0-9_/%.\\][-+a-zA-Z0-9_/%.\\#]*

SCAN_EQUAL	=
SCAN_NOT_EQUAL	!=
SCAN_GREATER	>
SCAN_GREATER_EQL >=
SCAN_LESS	<
SCAN_LESS_EQL	<=

NEVER		[^a-zA-Z0-9_/%.#:+\-,]

%%

{SCAN_INT}	store_int32(); return SCAN_INT;
{SCAN_FLOAT}	store_float64(); return SCAN_FLOAT;

{SCAN_WORD}	store_id(); return SCAN_WORD;

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
 * Revision 1.11  2003/12/08 18:02:31  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.10.2.1  2003/10/03 16:25:02  jimg
 * I changed the way the scanners handle errors. They were calling
 * YY_FATAL_ERROR and using the default value which prints a msg to stderr
 * and calls exit(1). I've changed that to a new sniplet that throws an
 * exception (Error). In addition, some of the scanners would ignore
 * illegal characters; they now treat those as fatal errors.
 *
 * Revision 1.10  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.9  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.8.2.1  2003/02/21 00:10:08  jimg
 * Repaired copyright.
 *
 * Revision 1.8  2003/01/23 00:22:25  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.7  2002/06/03 22:21:16  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.4.4.4  2001/11/01 00:43:51  jimg
 * Fixes to the scanners and parsers so that dataset variable names may
 * start with digits. I've expanded the set of characters that may appear
 * in a variable name and made it so that all except `#' may appear at
 * the start. Some characters are not allowed in variables that appear in
 * a DDS or CE while they are allowed in the DAS. This makes it possible
 * to define containers with names like `COARDS:long_name.' Putting a colon
 * in a variable name makes the CE parser much more complex. Since the set
 * of characters that people want seems pretty limited (compared to the
 * complete ASCII set) I think this is an OK approach. If we have to open
 * up the expr.lex scanner completely, then we can but not without adding
 * lots of action clauses to teh parser. Note that colon is just an example,
 * there's a host of characters that are used in CEs that are not allowed
 * in IDs.
 *
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

