
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

/*
  This is the parser for the DODS constraint expression grammar. The parser
  calls various `helper' functions defined by the DAP classes which either
  implement the operations (in the case of relational ops) or store
  information (in the case of selection operations). 

  jhrg 9/5/95
*/

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: expr.y,v 1.45 2003/02/21 00:14:25 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <strstream>

#include "debug.h"
#include "escaping.h"

#include "DDS.h"

#include "Connect.h"

#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"
#include "Error.h"

#include "util.h"
#include "parser.h"
#include "expr.h"
#include "RValue.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::cerr;
using std::endl;
using std::ends;
using std::ostrstream;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed into the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro does not explicitly casts OBJ to an
// ERROR *.

#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

int exprlex(void);		/* the scanner; see expr.lex */

void exprerror(const char *s);	/* easier to overload than to use stdarg... */
void exprerror(const char *s, const char *s2);
void no_such_func(void *arg, char *name);
void no_such_ident(void *arg, char *name, char *word);

void exprerror(const string &s); 
void exprerror(const string &s, const string &s2);
void no_such_func(void *arg, const string &name);
void no_such_ident(void *arg, const string &name, const string &word);

int_list *make_array_index(value &i1, value &i2, value &i3);
int_list *make_array_index(value &i1, value &i2);
int_list *make_array_index(value &i1);
int_list_list *make_array_indices(int_list *index);
int_list_list *append_array_index(int_list_list *indices, int_list *index);

void delete_array_indices(int_list_list *indices);
bool bracket_projection(DDS &table, const char *name, 
			int_list_list *indices);

bool process_array_indices(BaseType *variable, int_list_list *indices); 
bool process_grid_indices(BaseType *variable, int_list_list *indices); 
bool process_sequence_indices(BaseType *variable, int_list_list *indices);

bool is_array_t(BaseType *variable);
bool is_grid_t(BaseType *variable);
bool is_sequence_t(BaseType *variable);

rvalue_list *make_rvalue_list(rvalue *rv);
rvalue_list *append_rvalue_list(rvalue_list *rvals, rvalue *rv);

BaseType *make_variable(DDS &table, const value &val);

rvalue *dereference_variable(rvalue *rv, DDS &dds);
rvalue *dereference_url(value &val);

bool_func get_function(const DDS &table, const char *name);
btp_func get_btp_function(const DDS &table, const char *name);
proj_func get_proj_function(const DDS &table, const char *name);

%}

%union {
    bool boolean;
    int op;
    char id[ID_MAX];

    value val;

    bool_func b_func;
    btp_func bt_func;

    int_list *int_l_ptr;
    int_list_list *int_ll_ptr;
    
    rvalue *rval_ptr;
    rvalue_list *r_val_l_ptr;
}

%token <val> SCAN_STR

%token <id> SCAN_WORD

%token <op> SCAN_EQUAL
%token <op> SCAN_NOT_EQUAL
%token <op> SCAN_GREATER
%token <op> SCAN_GREATER_EQL
%token <op> SCAN_LESS
%token <op> SCAN_LESS_EQL
%token <op> SCAN_REGEXP

%type <boolean> constraint_expr projection proj_clause proj_function array_proj
%type <boolean> selection clause bool_function
%type <op> rel_op
%type <int_l_ptr> array_index
%type <int_ll_ptr> array_indices
%type <rval_ptr> r_value id_or_const
%type <r_val_l_ptr> r_value_list arg_list

%%

constraint_expr: /* empty constraint --> send all */
                 {
		     (*DDS_OBJ(arg)).mark_all(true);
		     $$ = true;
		 }
                 /* projection only */
                 | projection
		 /* selection only --> project everything */
                 | '&' { (*DDS_OBJ(arg)).mark_all(true); } selection
                 { 
		     $$ = $3;
		 }
                 | projection '&' selection
                 {
		     $$ = $1 && $3;
		 }
;

projection:     proj_clause
                | proj_clause ',' projection
                {
		    $$ = $1 && $3;
		}
;

proj_clause:	SCAN_WORD 
                { 
		    BaseType *var = (*DDS_OBJ(arg)).var($1);
		    if (var) {
			DBG(cerr << "Marking " << $1 << endl);
			$$ = (*DDS_OBJ(arg)).mark($1, true);
			DBG(cerr << "result: " << $$ << endl);
		    }
		    else {
			no_such_ident(arg, $1, "identifier");
		    }
		}
                | proj_function
                {
		    $$ = $1;
		}
		| array_proj
                {
		    $$ = $1;
		}
;

proj_function:  SCAN_WORD '(' arg_list ')'
	        {
		    proj_func p_f = 0;
		    btp_func f = 0;

		    if ((f = get_btp_function(*(DDS_OBJ(arg)), $1))) {
			(*DDS_OBJ(arg)).append_clause(f, $3);
			$$ = true;
		    }
		    else if ((p_f = get_proj_function(*(DDS_OBJ(arg)), $1))) {
			BaseType **args = build_btp_args($3, *(DDS_OBJ(arg)));
			(*p_f)(($3) ? $3->size():0, args, *(DDS_OBJ(arg)));
			$$ = true;
		    }
		    else {
			no_such_func(arg, $1);
		    }
		}
;

selection:	clause
		| selection '&' clause
                {
		    $$ = $1 && $3;
		}
;

clause:		r_value rel_op '{' r_value_list '}'
                {
		    if ($1) {
			(*DDS_OBJ(arg)).append_clause($2, $1, $4);
			$$ = true;
		    }
		    else
			$$ = false;
		}
		| r_value rel_op r_value
                {
		    if ($1) {
			rvalue_list *rv = new rvalue_list;
			rv->push_back($3);
			(*DDS_OBJ(arg)).append_clause($2, $1, rv);
			$$ = true;
		    }
		    else
			$$ = false;
		}
		| bool_function
                {
		    $$ = $1;
		}
;

bool_function: SCAN_WORD '(' arg_list ')'
	       {
		   bool_func b_func = get_function((*DDS_OBJ(arg)), $1);
		   if (!b_func) {
		       no_such_func(arg, $1);
		   }
		   else {
		       (*DDS_OBJ(arg)).append_clause(b_func, $3);
		       $$ = true;
		   }
	       }
;

r_value:        id_or_const
		| '*' id_or_const
		{
		    $$ = dereference_variable($2, *DDS_OBJ(arg));
		    if (!$$) {
			exprerror("Could not dereference the URL", 
				  ($2)->value_name());
		    }
		}
		| SCAN_WORD '(' arg_list ')'
		{
		    btp_func func = get_btp_function((*DDS_OBJ(arg)), $1);
		    if (func) {
			$$ = new rvalue(func, $3);
		    } 
		    else {  		
			no_such_func(arg, $1);
		    }
		}
;

r_value_list:	r_value
		{
		    if ($1)
			$$ = make_rvalue_list($1);
		    else
			$$ = 0;
		}
		| r_value_list ',' r_value
                {
		    if ($1 && $3)
			$$ = append_rvalue_list($1, $3);
		    else
			$$ = 0;
		}
;

arg_list:     r_value_list
              {  
		  $$ = $1;
	      }
              | /* Null, argument lists may be empty */
              { 
		  $$ = 0; 
	      }
;

id_or_const:    SCAN_WORD
		{ 
		    BaseType *btp = (*DDS_OBJ(arg)).var(www2id(string($1)));
		    if (!btp) {
			value new_val;
			if (check_int32($1)) {
			    new_val.type = dods_int32_c;
			    new_val.v.i = atoi($1);
			}
			else if (check_uint32($1)) {
			    new_val.type = dods_uint32_c;
			    new_val.v.ui = atoi($1);
			}
			else if (check_float64($1)) {
			    new_val.type = dods_float64_c;
			    new_val.v.f = atof($1);
			}
			else {
			    new_val.type = dods_str_c;
			    new_val.v.s = new string($1);
			}
			BaseType *btp = make_variable((*DDS_OBJ(arg)), new_val); 
			// *** test for btp == null
			// delete new_val.v.s; // Str::val2buf copies the value.
			$$ = new rvalue(btp);
		    }
		    else
			$$ = new rvalue(btp);
		}
		| SCAN_STR
                { 
		    BaseType *btp = make_variable((*DDS_OBJ(arg)), $1); 
		    $$ = new rvalue(btp);
		}
;

array_proj:	SCAN_WORD array_indices 
                {
		  if (!bracket_projection((*DDS_OBJ(arg)), $1, $2))
		    // no_such_ident throws an exception.
		    no_such_ident(arg, $1, "array, grid or sequence");
		  else
		    $$ = true;
		}
;

array_indices:  array_index
                {
		    $$ = make_array_indices($1);
		}
                | array_indices array_index
                {
		    $$ = append_array_index($1, $2);
		}
;

array_index: 	'[' SCAN_WORD ']'
                {
		    if (!check_uint32($2)) {
			string msg = "The word `";
			msg += string($2) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    value i;
		    i.type = dods_uint32_c;
		    i.v.i = atoi($2);
		    $$ = make_array_index(i);
		}
		|'[' SCAN_WORD ':' SCAN_WORD ']'
                {
		    if (!check_uint32($2)) {
			string msg = "The word `";
			msg += string($2) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    if (!check_uint32($4)) {
			string msg = "The word `";
			msg += string($4) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    value i,j;
		    i.type = j.type = dods_uint32_c;
		    i.v.i = atoi($2);
		    j.v.i = atoi($4);
		    $$ = make_array_index(i, j);
		}
		| '[' SCAN_WORD ':' SCAN_WORD ':' SCAN_WORD ']'
                {
		    if (!check_uint32($2)) {
			string msg = "The word `";
			msg += string($2) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    if (!check_uint32($4)) {
			string msg = "The word `";
			msg += string($4) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    if (!check_uint32($6)) {
			string msg = "The word `";
			msg += string($6) + "' is not a valid array index.";
			throw Error(malformed_expr, msg);
		    }
		    value i, j, k;
		    i.type = j.type = k.type = dods_uint32_c;
		    i.v.i = atoi($2);
		    j.v.i = atoi($4);
		    k.v.i = atoi($6);
		    $$ = make_array_index(i, j, k);
		}
;

rel_op:		SCAN_EQUAL
		| SCAN_NOT_EQUAL
		| SCAN_GREATER
		| SCAN_GREATER_EQL
		| SCAN_LESS
		| SCAN_LESS_EQL
		| SCAN_REGEXP
;

%%

// All these error reporting function now throw instnaces of Error. The expr
// parser no longer returns an error code to indicate and error. 2/16/2000
// jhrg.

void
exprerror(const string &s)
{ 
    exprerror(s.c_str());
}

void
exprerror(const char *s)
{
    // cerr << "Expression parse error: " << s << endl;
    string msg = "Constraint expression parse error: " + (string)s;
    throw Error(malformed_expr, msg);
}

void
exprerror(const string &s, const string &s2)
{
    exprerror(s.c_str(), s2.c_str());
}

void
exprerror(const char *s, const char *s2)
{
    string msg = "Constraint expression parse error: " + (string)s + ": " 
	+ (string)s2;
    throw Error(malformed_expr, msg);
}

void
no_such_ident(void *arg, const string &name, const string &word)
{
    string msg = "No such " + word + " in dataset";
    exprerror(msg.c_str(), name);
    //    no_such_ident(arg, name.c_str(), word.c_str());
}

void
no_such_ident(void *arg, char *name, char *word)
{
    string msg = "No such " + (string)word + " in dataset";
    exprerror(msg.c_str(), name);
}

void
no_such_func(void *arg, const string &name)
{
    no_such_func(arg, name.c_str());
}

void
no_such_func(void *arg, char *name)
{
    exprerror("Not a registered function", name);
}

bool
bracket_projection(DDS &table, const char *name, int_list_list *indices)
{
    bool status = true;
    BaseType *var = table.var(name);

    if (var && is_array_t(var)) {
	/* calls to set_send_p should be replaced with
	   calls to DDS::mark so that arrays of Structures,
	   etc. will be processed correctly when individual
	   elements are projected using short names (Whew!)
	   9/1/98 jhrg */
	/* var->set_send_p(true); */
	table.mark(name, true);
	status = process_array_indices(var, indices);
	if (!status) {
	    string msg = "The indices given for `";
	    msg += (string)name + (string)"' are out of range.";
	    throw Error(malformed_expr, msg);
	}
	delete_array_indices(indices);
    }
    else if (var && is_grid_t(var)) {
	table.mark(name, true);
	/* var->set_send_p(true); */
	status = process_grid_indices(var, indices);
	if (!status) {
	    string msg = "The indices given for `";
	    msg += (string)name + (string)"' are out of range.";
	    throw Error(malformed_expr, msg);
	}
	delete_array_indices(indices);
    }
    else if (var && is_sequence_t(var)) {
	table.mark(name, true);
	status = process_sequence_indices(var, indices);
	if (!status) {
	    string msg = "The indices given for `";
	    msg += (string)name + (string)"' are out of range.";
	    throw Error(malformed_expr, msg);
	}
	delete_array_indices(indices);
    }
    else {
	status = false;
    }
  
    return status;
}

// Given three values (I1, I2, I3), all of which must be integers, build an
// int_list which contains those values.
//
// Returns: A pointer to an int_list of three integers or NULL if any of the
// values are not integers.

int_list *
make_array_index(value &i1, value &i2, value &i3)
{
    int_list *index = new int_list;

    if (i1.type != dods_uint32_c
	|| i2.type != dods_uint32_c
	|| i3.type != dods_uint32_c)
	return (int_list *)0;

    index->push_back((int)i1.v.i);
    index->push_back((int)i2.v.i);
    index->push_back((int)i3.v.i);

    DBG(cout << "index: ";\
	for (int_iter dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    return index;
}

int_list *
make_array_index(value &i1, value &i2)
{
    int_list *index = new int_list;

    if (i1.type != dods_uint32_c || i2.type != dods_uint32_c)
	return (int_list *)0;

    index->push_back((int)i1.v.i);
    index->push_back(1);
    index->push_back((int)i2.v.i);

    DBG(cout << "index: ";\
	for (int_citer dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    return index;
}

int_list *
make_array_index(value &i1)
{
    int_list *index = new int_list;

    if (i1.type != dods_uint32_c)
	return (int_list *)0;

    index->push_back((int)i1.v.i);
    index->push_back(1);
    index->push_back((int)i1.v.i);

    DBG(cout << "index: ";\
	for (int_citer dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    return index;
}

int_list_list *
make_array_indices(int_list *index)
{
    int_list_list *indices = new int_list_list;

    DBG(cout << "index: ";\
	for (int_citer dp = index->begin(); dp != index->end(); dp++)\
	cout << (*dp) << " ";\
	cout << endl);

    assert(index);
    indices->push_back(index);

    return indices;
}

int_list_list *
append_array_index(int_list_list *indices, int_list *index)
{
    assert(indices);
    assert(index);

    indices->push_back(index);

    return indices;
}

// Delete an array indices list. 

void
delete_array_indices(int_list_list *indices)
{
    assert(indices);

    for (int_list_citer i = indices->begin(); i != indices->end(); i++) {
	int_list *il = *i ;
	assert(il);
	delete il;
    }

    delete indices;
}

bool
is_array_t(BaseType *variable)
{
    assert(variable);

    if (variable->type() != dods_array_c)
	return false;
    else
	return true;
}

bool
is_grid_t(BaseType *variable)
{
    assert(variable);

    if (variable->type() != dods_grid_c)
	return false;
    else
	return true;
}

bool
is_sequence_t(BaseType *variable)
{
    assert(variable);

    if (variable->type() != dods_sequence_c)
	return false;
    else
	return true;
}

bool
process_array_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);

    Array *a = dynamic_cast<Array *>(variable); // replace with dynamic cast
    if (!a)
	throw Error(malformed_expr, 
	   string("The constraint expression evaluator expected an array; ")
		    + variable->name() + " is not an array.");
		   
    if (a->dimensions(true) != (unsigned)indices->size())
	throw Error(malformed_expr, 
	   string("Error: The number of dimenstions in the constraint for ")
		    + variable->name() 
		    + " must match the number in the array.");
		   
    DBG(cerr << "Before clear_costraint:" << endl);
    DBG(a->print_decl(stderr, "", true, false, true));

    a->clear_constraint();	// each projection erases the previous one

    DBG(cerr << "After clear_costraint:" << endl);
    DBG(a->print_decl(stderr, "", true, false, true));

    assert(indices);
    int_list_citer p = indices->begin() ;
    Array::Dim_iter r = a->dim_begin() ;
    for (; p != indices->end() && r != a->dim_end(); p++, r++) {
	int_list *index = *p;
	assert(index);

	int_citer q = index->begin(); 
	assert(q!=index->end());
	int start = *q;

	q++;
	int stride = *q;
	
	q++;
	int stop = *q;

	q++;
	if (q != index->end()) {
	    throw Error(malformed_expr,
			string("Too many values in index list for ")
			+ a->name() + ".");
	}

	DBG(cerr << "process_array_indices: Setting constraint on "\
	    << a->name() << "[" << start << ":" << stop << "]" << endl);

	a->add_constraint(r, start, stride, stop);

	DBG(cerr << "Set Constraint: " << a->dimension_size(r, true) << endl);
    }

    DBG(cerr << "After processing loop:" << endl);
    DBG(a->print_decl(stderr, "", true, false, true));

    DBG(cout << "Array Constraint: ";\
	for (Array::Dim_iter dp = a->dim_begin(); dp != a->dim_end(); dp++)\
	    cout << a->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p != indices->end() && r == a->dim_end()) {
	throw Error(malformed_expr,
		    string("Too many indices in constraint for ")
		    + a->name() + ".");
    }

    return status;
}

bool
process_grid_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);
    assert(variable->type() == dods_grid_c);
    Grid *g = dynamic_cast<Grid *>(variable);
    if (!g)
	throw Error(unknown_error, "Expected a Grid variable");

    Array *a = dynamic_cast<Array *>(g->array_var());
    if (!a)
	throw InternalErr(__FILE__, __LINE__, "Malformed Grid variable");
    if (a->dimensions(true) != (unsigned)indices->size())
	throw Error(malformed_expr, 
	   string("Error: The number of dimenstions in the constraint for ")
		    + variable->name() 
		    + " must match the number in the grid.");
		   
    // First do the constraints on the ARRAY in the grid.
    process_array_indices(g->array_var(), indices);

    // Now process the maps.
    Grid::Map_iter r = g->map_begin() ;

    // Supress all maps by default.
    for (; r != g->map_end(); r++)
    {
	(*r)->set_send_p(false);
    }

    // Add specified maps to the current projection.
    assert(indices);
    int_list_citer p = indices->begin();
    r = g->map_begin(); 
    for (; p != indices->end() && r != g->map_end(); p++, r++)
    {
	int_list *index = *p;
	assert(index);

	int_citer q = index->begin(); 
	assert(q != index->end());
	int start = *q;

	q++;
	int stride = *q;
	
	q++;
	int stop = *q;

	BaseType *btp = *r;
	assert(btp);
	assert(btp->type() == dods_array_c);
	Array *a = (Array *)btp;
	a->set_send_p(true);
	a->clear_constraint();

	q++;
	if (q!=index->end()) {
	    throw Error(malformed_expr,
			string("Too many values in index list for ")
			+ a->name() + ".");
	}

	DBG(cerr << "process_grid_indices: Setting constraint on "\
	    << a->name() << "[" << start << ":" << stop << "]" << endl);

	Array::Dim_iter si = a->dim_begin() ;
	a->add_constraint(si, start, stride, stop);

	DBG(Array::Dim_iter aiter = a->dim_begin() ; \
	    cerr << "Set Constraint: " \
	    << a->dimension_size(aiter, true) << endl);
    }

    DBG(cout << "Grid Constraint: ";\
	for (Array::Dim_iter dp = ((Array *)g->array_var())->dim_begin();
	     dp != ((Array *)g->array_var())->dim_end(); \
	     dp++)\
	   cout << ((Array *)g->array_var())->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p!=indices->end() && r==g->map_end()) {
	throw Error(malformed_expr,
		    string("Too many indices in constraint for ")
		    + (*r)->name() + ".");
    }

    return status;
}

bool
process_sequence_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);
    assert(variable->type() == dods_sequence_c);
    Sequence *s = dynamic_cast<Sequence *>(variable);
    if (!s)
	throw Error(malformed_expr, "Expected a Sequence variable");

    // Add specified maps to the current projection.
    assert(indices);
    for (int_list_citer p = indices->begin(); p != indices->end(); p++)
    {
	int_list *index = *p;
	assert(index);

	int_citer q = index->begin(); 
	assert(q!=index->end());
	int start = *q;

	q++;
	int stride = *q;
	
	q++;
	int stop = *q;

	q++;
	if (q!=index->end()) {
	  throw Error(malformed_expr, 
		      string("Too many values in index list for ")
		      + s->name() + ".");
	}

	s->set_row_number_constraint(start, stop, stride);
    }

    return status;
}

// Create a list of r values and add VAL to the list.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
make_rvalue_list(rvalue *rv)
{
    assert(rv);

    rvalue_list *rvals = new rvalue_list;

    return append_rvalue_list(rvals, rv);
}

// Given a rvalue_list pointer RVALS and a value pointer VAL, make a variable
// to hold VAL and append that variable to the list RVALS.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
append_rvalue_list(rvalue_list *rvals, rvalue *rv)
{
    assert(rvals);
    assert(rv);

    rvals->push_back(rv);

    return rvals;
}

// Given a string which is a URL, dereference it and return the data it
// points to.

static rvalue *
dereference_string(string &s)
{
    // FIX Once Connect/HTTPConnect settle down. ***
    unsigned int qpos = s.find('?');
    string url = s.substr(0, qpos);	// strip off CE
    string ce = s.substr(qpos+1);	// yes, get the CE

    // I don't think that the `false' is really necessary, but g++ seems to
    // want it. jhrg 2/10/97
    Connect c(url, false); // make the virtual connection

    // the initial URL must be a complete reference to data; thus no
    // additional CE is needed. 
    DDS *d = c.request_data(ce, false, false); 

    // By definition, the DDS `D' can have only one variable, so make sure
    // that is true.
    if (d->num_var() != 1) {
	throw Error (malformed_expr,
		     string("Too many variables in URL; use only single variable projections"));
    }

    // OK, we're here. The first_var() must be the only var, return it bound
    // up in an rvalue struct. NB: the *object* must be copied since the one
    // within DDS `D' will be deleted by D's dtor.
    BaseType *btp = (*(d->var_begin()))->ptr_duplicate();
    rvalue *rv = new rvalue(btp);

    delete d;

    return rv;
}

rvalue *
dereference_url(value &val)
{
    if (val.type != dods_str_c)
	return 0;

    return dereference_string(*val.v.s);
}

// Given a rvalue, get the BaseType that encapsulates its value, make sure it
// is a string and, if all that works, dereference it.

rvalue *
dereference_variable(rvalue *rv, DDS &dds)
{
    assert(rv);
    // the value will be read over the net
    BaseType *btp = rv->bvalue("dummy", dds); 
    if (btp->type() != dods_str_c && btp->type() != dods_url_c) {
	throw Error(malformed_expr, string("The variable `") + btp->name() 
		    + "' must be either a string or a url");
    }

    string s;
    string  *sp = &s;
    btp->buf2val((void **)&sp);
    
    return dereference_string(s);
}

// Given a value, wrap it up in a BaseType and return a pointer to the same.

BaseType *
make_variable(DDS &table, const value &val)
{
    BaseType *var;
    switch (val.type) {
      case dods_int32_c: {
	var = (BaseType *)NewInt32("dummy");
	var->val2buf((void *)&val.v.i);
	break;
      }

      case dods_float64_c: {
	var = (BaseType *)NewFloat64("dummy");
	var->val2buf((void *)&val.v.f);
	break;
      }

      case dods_str_c: {
	var = (BaseType *)NewStr("dummy");
	var->val2buf((void *)val.v.s);
	break;
      }

      default:
	var = (BaseType *)0;
	return var;
    }

    var->set_read_p(true);	// ...so the evaluator will know it has data
    table.append_constant(var);

    return var;
}

// Given a string (passed in VAL), consult the DDS CE function lookup table
// to see if a function by that name exists. 
// NB: function arguments are type-checked at run-time.
//
// Returns: A poitner to the function or NULL if not such function exists.

bool_func
get_function(const DDS &table, const char *name)
{
    bool_func f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}

btp_func
get_btp_function(const DDS &table, const char *name)
{
    btp_func f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}

proj_func
get_proj_function(const DDS &table, const char *name)
{
    proj_func f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}

/*
 * $Log: expr.y,v $
 * Revision 1.45  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.44  2003/01/23 00:22:25  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.43  2003/01/10 19:46:41  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.39.4.16  2002/12/17 22:35:03  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.39.4.15  2002/11/05 00:53:52  jimg
 * Removed 'identifier.' It was flagged as unneeded by bison.
 *
 * Revision 1.39.4.14  2002/10/28 21:17:44  pwest
 * Converted all return values and method parameters to use non-const iterator.
 * Added operator== and operator!= methods to IteratorAdapter to handle Pix
 * problems.
 *
 * Revision 1.39.4.13  2002/09/05 22:52:55  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.39.4.12  2002/07/06 20:56:06  jimg
 * Somehow the code added on 6.11.2002 that checked to ensure that the number of
 * dimensions in an array's constraint matched the number of dimensions in the
 * array was LOST. Hmmm... I added it back again. Looks like it was a casualty
 * of the libcurl migration.
 *
 * Revision 1.39.4.11  2002/06/18 23:05:33  jimg
 * Updated grammar files when replacing libwww.
 *
 * Revision 1.42  2002/06/03 22:21:16  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.39.4.8  2002/03/01 21:03:08  jimg
 * Significant changes to the var(...) methods. These now take a btp_stack
 * pointer and are used by DDS::mark(...). The exact_match methods have also
 * been updated so that leaf variables which contain dots in their names
 * will be found. Note that constructor variables with dots in their names
 * will break the lookup routines unless the ctor is the last field in the
 * constraint expression. These changes were made to fix bug 330.
 *
 * Revision 1.39.4.7  2002/02/20 19:16:27  jimg
 * Changed the expression parser so that variable names may contain only
 * digits.
 *
 * Revision 1.39.4.6  2001/11/01 00:43:51  jimg
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
 * Revision 1.41  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.39.4.5  2001/09/25 20:24:28  jimg
 * Added some debugging stuff to process_array_indices (and _grid_) to help
 * debug the grid() server function.
 *
 * Revision 1.39.4.4  2001/09/19 21:57:26  jimg
 * Changed no_such_ident(void *, const string &, const string &) so that it
 * calls exprerror(...) directly. The call to it's other overloaded version
 * was not working and resulted in an infinite loop.
 *
 * Revision 1.39.4.3  2001/09/06 22:04:03  jimg
 * Fixed the error message for `No such X in dataset.' I removed an extra
 * colon (Ouch...).
 *
 * Revision 1.40  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.39.4.2  2001/07/28 01:10:42  jimg
 * Some of the numeric type classes did not have copy ctors or operator=.
 * I added those where they were needed.
 * In every place where delete (or delete []) was called, I set the pointer
 * just deleted to zero. Thus if for some reason delete is called again
 * before new memory is allocated there won't be a mysterious crash. This is
 * just good form when using delete.
 * I added calls to www2id and id2www where appropriate. The DAP now handles
 * making sure that names are escaped and unescaped as needed. Connect is
 * set to handle CEs that contain names as they are in the dataset (see the
 * comments/Log there). Servers should not handle escaping or unescaping
 * characters on their own.
 *
 * Revision 1.39.4.1  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.39  2000/09/22 02:17:23  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.38  2000/09/21 16:22:10  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.37  2000/09/14 10:30:20  rmorris
 * Added usage of ends and ostrstream elements in the std namespace for win32.
 *
 * Revision 1.36  2000/09/11 16:17:47  jimg
 * Added Sequence selection using row numbers. This `selection' operation
 * uses the brackets a la arrays and grids.
 *
 * Revision 1.35  2000/07/09 21:43:30  rmorris
 * Mods to increase portability, minimize ifdef's for win32
 *
 * Revision 1.34  2000/06/07 18:07:00  jimg
 * Merged the pc port branch
 *
 * Revision 1.33.14.1  2000/06/02 18:36:39  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.33.8.1  2000/02/17 05:03:17  jimg
 * Added file and line number information to calls to InternalErr.
 * Resolved compile-time problems with read due to a change in its
 * parameter list given that errors are now reported using exceptions.
 *
 * Revision 1.33  1999/07/22 17:11:52  jimg
 * Merged changes from the release-3-0-2 branch
 *
 * Revision 1.32.6.1  1999/06/07 20:03:25  edavis
 * Changed all string class usage of 'data()' to 'c_str()'.
 *
 * Revision 1.32  1999/05/21 17:20:08  jimg
 * Made the parser error messages a bit easier to decode by adding `Expression'
 * to them. Still, these are pretty lame messages...
 *
 * Revision 1.31  1999/05/04 19:47:24  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.30  1999/04/29 02:29:36  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.29  1999/04/22 22:30:52  jimg
 * Uses dynamic_cast
 *
 * Revision 1.28  1998/11/10 00:48:54  jimg
 * Changed no_such_id() to no_such_ident() (the former is used in bastring.h).
 *
 * Revision 1.27  1998/11/05 23:41:20  jimg
 * Made error message for errant CEs involving arrays better.
 * DDS::mark() now used for array variables --- this should fix a potential
 * problem with structures of arrays.
 *
 * Revision 1.26  1998/10/21 16:55:15  jimg
 * Single array element may now be refd as [<int>]. So element seven of the
 * array `a' can be referenced as a[7]. The old syntax, a[7:7], will still
 * work. Projection functions are now supported. Functions listed in the
 * projection part of a CE are evaluated (executed after parsing) as they are
 * found (before the parse of the rest of the projections or the start of the
 * parse of the selections. These functions take the same three arguments as
 * the boll and BaseType * functions (int argc, BaseType *argv[], DDS &dds)
 * but they return void. They can do whatever they like, but the use I
 * foresee is adding new (synthesized - see BaseType.cc/h) variables to the
 * DDS.
 *
 * Revision 1.25  1998/09/17 16:56:50  jimg
 * Made the error messages more verbose (that is, the text in the Error objects
 * sent back to the client).
 * Fixed a bug where non-existent fields could be accessed - with predictably
 * bad results.
 *
 * Revision 1.24.6.2  1999/02/05 09:32:36  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.24.6.1  1999/02/02 21:57:07  jimg
 * String to string version
 *
 * Revision 1.24  1998/03/19 23:22:38  jimg
 * Fixed the error messages so they use `' instead of :
 * Added Error objects for array index errors.
 * Removed old code (that was surrounded by #if 0 ... #endif).
 *
 * Revision 1.23  1998/02/05 20:14:03  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.22  1997/10/09 22:19:31  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.21  1997/10/04 00:33:05  jimg
 * Release 2.14c fixes
 *
 * Revision 1.20.6.1  1997/09/23 15:45:09  jimg
 * Fixed nasty comment bug with CVS 1.9
 *
 * Revision 1.20  1997/02/17 20:27:19  jimg
 * Fixed silly spelling errors.
 *
 * Revision 1.19  1997/02/12 19:46:33  jimg
 * Fixed bad asserts in process_array_indices and process_grid_indices.
 *
 * Revision 1.18  1997/02/10 02:32:46  jimg
 * Added assert statements for pointers
 *
 * Revision 1.17  1996/12/18 18:47:24  jimg
 * Modified the parser so that it returns Error objects for certain types of
 * errors. In order to take advantage of this, callers must examine the
 * returned object and process it as an Error object if status is false.
 *
 * Revision 1.16  1996/11/27 22:40:26  jimg
 * Added DDS as third parameter to function in the CE evaluator
 *
 * Revision 1.15  1996/10/18 16:55:15  jimg
 * Fixed the fix for bison 1.25...
 *
 * Revision 1.14  1996/10/08 17:04:43  jimg
 * Added a fix for Bison 1.25 so that PARSE_PARAM will still work
 *
 * Revision 1.13  1996/08/13 19:00:21  jimg
 * Added not_used to definition of char rcsid[].
 * Switched to the parser_arg object for communication with callers. Removed
 * unused parameters from dereference_{url, variable}, make_rvalue_list and
 * append_rvalue_list.
 *
 * Revision 1.12  1996/06/18 23:54:31  jimg
 * Fixes for Grid constraints. These include not deleting the array indices
 * lists after processing the Array component of a grid (but before processing
 * the Maps...).
 *
 * Revision 1.11  1996/06/11 17:27:11  jimg
 * Moved debug.h in front of all the other DODS includes - this ensures that
 * the debug.h included in this file is the one in effect (as opposed to a copy
 * included by some other include file). We should banish nested includes...
 * Added support for `Grid constraints'. These are like the Array constraints -
 * projections where the start, stop and stride of each dimension may be
 * specified. The new feature required a grammar change (so the parser would
 * accept grids with the array bracket notation) and two new functions:
 * is_grid_t() and process_grid_indices(). The actual projection information is
 * stored in the array members of the Grid.
 *
 * Revision 1.10  1996/05/31 23:31:04  jimg
 * Updated copyright notice.
 *
 * Revision 1.9  1996/05/29 22:08:57  jimg
 * Made changes necessary to support CEs that return the value of a function
 * instead of the value of a variable. This was done so that it would be
 * possible to translate Sequences into Arrays without first reading the
 * entire sequence over the network.
 *
 * Revision 1.8  1996/05/14 15:39:02  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.7  1996/04/05 00:22:21  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.6  1996/03/02 01:17:09  jimg
 * Added support for the complete CE spec.
 *
 * Revision 1.5  1996/02/01 17:43:18  jimg
 * Added support for lists as operands in constraint expressions.
 *
 * Revision 1.4  1995/12/09  01:07:41  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.3  1995/12/06  18:42:44  jimg
 * Added array constraints to the parser.
 * Added functions for the actions of those new rules.
 * Changed/added rule's return types.
 * Changed the types in the %union {}.
 *
 * Revision 1.2  1995/10/23  23:10:38  jimg
 * Added includes for various classes.
 * Aded rules, actions and functions for evaluation of projections.
 * Changed the value of YYSTYPE so that bison's %union feature is used -
 * rules now return several different types.
 *
 * Revision 1.1  1995/10/13  03:04:08  jimg
 * First version. Incorporates Glenn's suggestions. 
 */

