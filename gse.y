
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Parse a Grid selection expression. This parser is a little different than
// the other parsers and uses its own argument class. See parser.h.

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: gse.y,v 1.9 2003/02/21 00:14:25 jimg Exp $"};

#include <iostream>

#include "Error.h"
#include "GSEClause.h"
#include "parser.h"

using std::cerr;
using std::endl;

// This macro is used to access the instance of a gse_arg class which is
// passed to the parser through a void *. See parser.h.

#define gse_arg(arg) ((gse_arg *)(arg))

// Assume bison 1.25
#define YYPARSE_PARAM arg

int gse_lex(void);
void gse_error(const char *str);
GSEClause *build_gse_clause(gse_arg *arg, char id[ID_MAX], int op, double val);
GSEClause *build_rev_gse_clause(gse_arg *arg, char id[ID_MAX], int op,
				double val);
GSEClause *
build_dual_gse_clause(gse_arg *arg, char id[ID_MAX], int op1, double val1, 
		      int op2, double val2);

%}

%union {
    bool boolean;

    int op;
    char id[ID_MAX];
    double val;
}

%token <val> SCAN_INT
%token <val> SCAN_FLOAT

%token <id> SCAN_WORD
%token <id> SCAN_FIELD

%token <op> SCAN_EQUAL
%token <op> SCAN_NOT_EQUAL
%token <op> SCAN_GREATER
%token <op> SCAN_GREATER_EQL
%token <op> SCAN_LESS
%token <op> SCAN_LESS_EQL

%type <boolean> clause
%type <id> identifier
%type <op> relop
%type <val> constant

%%

clause:		identifier relop constant
                {
		    ((gse_arg *)arg)->set_gsec(
			build_gse_clause((gse_arg *)(arg), $1, $2, $3));
		    $$ = true;
		}
		| constant relop identifier
                {
		    ((gse_arg *)arg)->set_gsec(
		       build_rev_gse_clause((gse_arg *)(arg), $3, $2, $1));
		    $$ = true;
		}
		| constant relop identifier relop constant
                {
		    ((gse_arg *)arg)->set_gsec(
		       build_dual_gse_clause((gse_arg *)(arg), $3, $2, $1, $4,
					     $5));
		    $$ = true;
		}
;

identifier:	SCAN_WORD 
;

constant:       SCAN_INT
		| SCAN_FLOAT
;

relop:		SCAN_EQUAL
		| SCAN_NOT_EQUAL
		| SCAN_GREATER
		| SCAN_GREATER_EQL
		| SCAN_LESS
		| SCAN_LESS_EQL
;

%%

void
gse_error(const char *str)
{
    throw InternalErr(__FILE__, __LINE__, "gse_error() called.");
}

static relop
decode_relop(int op)
{
    switch (op) {
      case SCAN_GREATER:
	return dods_greater_op;
      case SCAN_GREATER_EQL:
	return dods_greater_equal_op;
      case SCAN_LESS:
	return dods_less_op;
      case SCAN_LESS_EQL:
	return dods_less_equal_op;
      case SCAN_EQUAL:
	return dods_equal_op;
      default:
	throw Error(malformed_expr, "Unrecognized relational operator");
    }
}

static relop
decode_inverse_relop(int op)
{
    switch (op) {
      case SCAN_GREATER:
	return dods_less_op;
      case SCAN_GREATER_EQL:
	return dods_less_equal_op;
      case SCAN_LESS:
	return dods_greater_op;
      case SCAN_LESS_EQL:
	return dods_greater_equal_op;
      case SCAN_EQUAL:
	return dods_equal_op;
      default:
	throw Error(malformed_expr, "Unrecognized relational operator");
    }
}

GSEClause *
build_gse_clause(gse_arg *arg, char id[ID_MAX], int op, double val)
{
    return new GSEClause(arg->get_grid(), (string)id, val, decode_relop(op));
}

// Build a GSE Clause given that the operands are reversed.

GSEClause *
build_rev_gse_clause(gse_arg *arg, char id[ID_MAX], int op, double val)
{
    return new GSEClause(arg->get_grid(), (string)id, val, 
			 decode_inverse_relop(op));
}

GSEClause *
build_dual_gse_clause(gse_arg *arg, char id[ID_MAX], int op1, double val1, 
		      int op2, double val2)
{
    // Check that the operands (op1 and op2) and the values (val1 and val2)
    // describe a monotonic interval.
    relop rop1 = decode_inverse_relop(op1);
    relop rop2 = decode_relop(op2);

    switch (rop1) {
      case dods_less_op:
      case dods_less_equal_op:
	if (rop2 == dods_less_op || rop2 == dods_less_equal_op)
	    throw Error(malformed_expr, 
"GSE Clause operands must define a monotonic interval.");
	break;
      case dods_greater_op:
      case dods_greater_equal_op:
	if (rop2 == dods_greater_op || rop2 == dods_greater_equal_op)
	    throw Error(malformed_expr, 
"GSE Clause operands must define a monotonic interval.");
	break;
      case dods_equal_op:
	break;
      default:
	throw Error(malformed_expr, "Unrecognized relational operator.");
    }

    return new GSEClause(arg->get_grid(), (string)id, val1, rop1, val2, rop2);
}

// $Log: gse.y,v $
// Revision 1.9  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.8  2003/01/23 00:22:25  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.7  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.5.4.2  2001/11/01 00:43:51  jimg
// Fixes to the scanners and parsers so that dataset variable names may
// start with digits. I've expanded the set of characters that may appear
// in a variable name and made it so that all except `#' may appear at
// the start. Some characters are not allowed in variables that appear in
// a DDS or CE while they are allowed in the DAS. This makes it possible
// to define containers with names like `COARDS:long_name.' Putting a colon
// in a variable name makes the CE parser much more complex. Since the set
// of characters that people want seems pretty limited (compared to the
// complete ASCII set) I think this is an OK approach. If we have to open
// up the expr.lex scanner completely, then we can but not without adding
// lots of action clauses to teh parser. Note that colon is just an example,
// there's a host of characters that are used in CEs that are not allowed
// in IDs.
//
// Revision 1.6  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.5.4.1  2001/09/25 20:16:40  jimg
// Fixed the EQUAL token. Added NOT_EQUAL.
//
// Revision 1.5  2000/09/22 02:17:23  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.4  2000/07/09 21:43:30  rmorris
// Mods to increase portability, minimize ifdef's for win32
//
// Revision 1.3  2000/06/07 18:07:01  jimg
// Merged the pc port branch
//
// Revision 1.2.20.1  2000/06/02 18:39:04  rmorris
// Mod's for port to win32.
//
// Revision 1.2  1999/04/29 02:29:37  jimg
// Merge of no-gnu branch
//
// Revision 1.1  1999/01/21 02:07:44  jimg
// Created
//

