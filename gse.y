
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// $Log: gse.y,v $
// Revision 1.2  1999/04/29 02:29:37  jimg
// Merge of no-gnu branch
//
// Revision 1.1  1999/01/21 02:07:44  jimg
// Created
//

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: gse.y,v 1.2 1999/04/29 02:29:37 jimg Exp $"};

#include <iostream.h>

#include "Error.h"
#include "GSEClause.h"
#include "parser.h"

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

%token <val> INT
%token <val> FLOAT

%token <id> ID
%token <id> FIELD

%token <op> GREATER
%token <op> GREATER_EQL
%token <op> LESS
%token <op> LESS_EQL
%token <op> EQUAL

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

identifier:	ID 
;

constant:       INT
		| FLOAT
;

relop:		GREATER
		| GREATER_EQL
		| LESS
		| LESS_EQL
                | EQUAL
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
      case GREATER:
	return dods_greater_op;
      case GREATER_EQL:
	return dods_greater_equal_op;
      case LESS:
	return dods_less_op;
      case LESS_EQL:
	return dods_less_equal_op;
      case EQUAL:
	return dods_equal_op;
      default:
	throw Error(malformed_expr, "Unrecognized relational operator");
    }
}

static relop
decode_inverse_relop(int op)
{
    switch (op) {
      case GREATER:
	return dods_less_op;
      case GREATER_EQL:
	return dods_less_equal_op;
      case LESS:
	return dods_greater_op;
      case LESS_EQL:
	return dods_greater_equal_op;
      case EQUAL:
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
