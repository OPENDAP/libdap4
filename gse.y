
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// $Log: gse.y,v $
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

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: gse.y,v 1.4 2000/07/09 21:43:30 rmorris Exp $"};

#include <iostream>

#include "Error.h"
#include "GSEClause.h"
#include "parser.h"

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed into the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro does not explicitly casts OBJ to an
// ERROR *.

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

%token <id> SCAN_ID
%token <id> SCAN_FIELD

%token <op> SCAN_GREATER
%token <op> SCAN_GREATER_EQL
%token <op> SCAN_LESS
%token <op> SCAN_LESS_EQL
%token <op> SCAN_EQUAL

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

identifier:	SCAN_ID 
;

constant:       SCAN_INT
		| SCAN_FLOAT
;

relop:		SCAN_GREATER
		| SCAN_GREATER_EQL
		| SCAN_LESS
		| SCAN_LESS_EQL
                | SCAN_EQUAL
;

%%

void
gse_error(const char *str)
{
    cerr << "GSE Error: " << str << endl;
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
