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
  This is the parser for the DODS constraint expression grammar. The parser
  calls various `helper' functions defined by the DAP classes which either
  implement the operations (in the case of relational ops) or store
  information (in the case of selection operations). 

  jhrg 9/5/95
*/

/* $Log: expr.y,v $
/* Revision 1.3  1995/12/06 18:42:44  jimg
/* Added array constraints to the parser.
/* Added functions for the actions of those new rules.
/* Changed/added rule's return types.
/* Changed the types in the %union {}.
/*
 * Revision 1.2  1995/10/23  23:10:38  jimg
 * Added includes for various classes.
 * Aded rules, actions and functions for evaluation of projections.
 * Changed the value of YYSTYPE so that bison's %union feature is used -
 * rules now return several different types.
 *
 * Revision 1.1  1995/10/13  03:04:08  jimg
 * First version. Incorporates Glenn's suggestions.
 */

%{

static char rcsid[]={"$Id: expr.y,v 1.3 1995/12/06 18:42:44 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <String.h>
#include <SLList.h>

#include "DDS.h"

#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Function.h"
#include "Grid.h"

#include "config_dap.h"
#define DEBUG 1
#include "debug.h"
#include "parser.h"
#include "expr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

int exprlex(void);		/* the scanner; see expr.lex */
int exprerror(char *s);

bool mark_id(DDS &table, value &val);
IntList *make_array_index(value &i1, value &i2, value &i3);
IntListList *make_array_indeces(IntList *index);
IntListList *append_array_index(IntListList *indeces, IntList *index);
bool process_array_indeces(DDS &dds, value &id, IntListList *indeces);
value dereference_url(DDS &table, value &val);
bool relational_op(value &val1, value &val2, int op);
bool int32_op(int i1, int i2, int op);
bool float64_op(double d1, double d2, int op);
bool str_op(String s1, String s2, int op);

/* 
  The parser recieives a DDS &table as a formal argument. TABLE is the DDS
  of the entire dataset; each variable in the constraint expression must be
  in this DDS and their datatypes must match the use in the constraint
  expression.
*/

%}

%expect 6

%token <val> INT
%token <val> FLOAT

%token <val> ID
%token <val> FIELD
%token <val> STR

%token <op> EQUAL
%token <op> NOT_EQUAL
%token <op> GREATER
%token <op> GREATER_EQL
%token <op> LESS
%token <op> LESS_EQL
%token <op> REGEXP

%union {
    bool boolean;
    int op;

    value val;

    IntList *int_l_ptr;
    IntListList *int_ll_ptr;
}

%type <boolean> constraint_expr projection selection clause array_sel

%type <val> operand

%type <op> rel_op

%type <int_l_ptr> array_index

%type <int_ll_ptr> array_indeces

%%

constraint_expr: /* empty */ 
                 {
		     table.mark_all(true); /* empty constraint --> send all */
		     $$ = true;
		 }
                 | projection
                 | '&' { table.mark_all(true); } selection 
                   { 
		       $$ = $3;
		   }
                 | projection '&' selection
                   {
		       $$ = $1 && $3;
		   }
;

projection:	ID 
                  { 
		      $$ = mark_id(table, $1); 
		  }
                | FIELD
                  { 
		      $$ = mark_id(table, $1); 
		  }

                | projection ',' ID
                  { 
		      $$ = $1 && mark_id(table, $3); 
		  }
                | projection ',' FIELD
                  { 
		      $$ = $1 && mark_id(table, $3); 
		  }
;

selection:	clause
		| selection '&' clause
                  {
		      $$ = $1 && $3;
		  }
;

clause:		/* operand rel_op '{' operand_list '}'
		| */ operand rel_op operand 
                  {
		      $$ = relational_op($1, $3, $2);
		  }
                | array_sel
;

operand:	ID
		| INT
		| FLOAT
		| FIELD
		| '*' STR { $$ = dereference_url(table, $2); }
;

array_sel:	ID array_indeces 
                  {
		      $$ = process_array_indeces(table, $1, $2);
		  }
	        | FIELD array_indeces 
                  {
		      $$ = process_array_indeces(table, $1, $2);
		  }
;

array_indeces:  array_index
                  {
		      $$ = make_array_indeces($1);
		  }
                | array_indeces array_index
                  {
		      $$ = append_array_index($1, $2);
		  }
;

array_index: 	'[' INT ':' INT ']'
                  {
		      value val;
		      val.type = Int32;
		      val.v.int32 = 1;
		      $$ = make_array_index($2, val, $4);
		  }
		| '[' INT ':' INT ':' INT ']'
                  {
		      $$ = make_array_index($2, $4, $6);
		  }
;
/*
operand_list:	list_str
		| list_int
		| list_float
;

list_str:	STR
		| ID 
		| list_str ',' STR
		| list_str ',' ID
;

list_int:	INT
		| list_int ',' INT
;

list_float:	FLOAT
		| list_float ',' FLOAT
;
*/
rel_op:		EQUAL
		| NOT_EQUAL
		| GREATER
		| GREATER_EQL
		| LESS
		| LESS_EQL
		| REGEXP
;

%%

int 
exprerror(char *s)
{
    fprintf(stderr, "%s\n");
}

bool
mark_id(DDS &table, struct value &val)
{
    if (!val.type == Id)
	return false;
    
    return table.mark(&val.v.id[0], true);
}

IntList *
make_array_index(value &i1, value &i2, value &i3)
{
    IntList *index = new IntList;

    if (i1.type != Int32
	|| i2.type != Int32
	|| i3.type != Int32)
	return (void *)0;

    index->append(i1.v.int32);
    index->append(i2.v.int32);
    index->append(i3.v.int32);

    DBG(Pix dp;\
	cout << "index: ";\
	for (dp = index->first(); dp; index->next(dp))\
	cout << (*index)(dp) << " ";\
	cout << endl);

    return index;
}

IntListList *
make_array_indeces(IntList *index)
{
    IntListList *indeces = new IntListList;

    DBG(Pix dp;\
	cout << "index: ";\
	for (dp = index->first(); dp; index->next(dp))\
	cout << (*index)(dp) << " ";\
	cout << endl);

    indeces->append(index);

    return indeces;
}

IntListList *
append_array_index(IntListList *indeces, IntList *index)
{
    indeces->append(index);

    return indeces;
}

bool
process_array_indeces(DDS &dds, value &id, IntListList *indeces)
{
    if (!id.type == Id)
	return false;

    BaseType *variable = dds.var(id.v.id);

    if (!variable) {
	cerr << "Variable " << id.v.id << " does not exist." << endl;
	return false;
    }

    if (variable->type() != array_t) {
	cerr << "Variable " << id.v.id << " is not an array." << endl;
	return false;
    }
    
    Array *a = (Array *)variable;

    a->clear_constraint();
    
    Pix p, r;
    for (p = indeces->first(), r = a->first_dim(); 
	 p && r; 
	 indeces->next(p), a->next_dim(r)) {

	IntList *index = (*indeces)(p);

	Pix q = index->first(); 
	int start = (*index)(q);

	index->next(q);
	int stride = (*index)(q);
	
	index->next(q);
	int stop = (*index)(q);

	index->next(q);
	if (q) {
	    cerr << "Too many values in index list for " << id.v.id << "." 
		 << endl;
	    return false;
	}
	
	a->add_constraint(r, start, stride, stop);
	DBG(cerr << "Set Constraint: " << a->dimension_size(r, true) << endl);
    }

    DBG(Pix dp;\
	cout << "Array Constraint: ";\
	for (dp = a->first_dim(); dp; a->next_dim(dp))\
	    cout << a->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p && !r) {
	cerr << "Too many indeces in constraint for " << id.v.id << "." 
	     << endl;
	return false;
    }

    return true;
}

// Given a string which is a URL, dereference it and return the data it
// points to.

value
dereference_url(DDS &table, value &val)
{
    value result_val;
    cerr << "Urls are not currently supported" << endl;
    result_val.type = Int32;
    result_val.v.int32 = 1023;
    return result_val;
}

// Given two values and an operator, perform the operation on the values.
// Returns: true if the expression is true, false otherwise.

bool
relational_op(value &val1, value &val2, int op)
{
    if (val1.type != val2.type) {
	cerr << "Operand types in a constraint expression must match" << endl;
	return false;
    }

    switch (val1.type) {
      case Int32:
	return int32_op(val1.v.int32, val2.v.int32, op);
      case Float64:
	return float64_op(val1.v.float64, val2.v.float64, op);
      case Str:
	return str_op(val1.v.str, val2.v.str, op);
      default:
	cerr << "No such datatype" << endl;
	return false;
    }
}

bool
int32_op(int i1, int i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == i2;
      case NOT_EQUAL:
	return i1 != i2;
      case GREATER:
	return i1 > i2;
      case GREATER_EQL:
	return i1 >= i2;
      case LESS:
	return i1 < i2;
      case LESS_EQL:
	return i1 <= i2;
      case REGEXP:
	cerr << "Regexp not valid for integers" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
float64_op(double d1, double d2, int op)
{
    switch (op) {
      case EQUAL:
	return d1 == d2;
      case NOT_EQUAL:
	return d1 != d2;
      case GREATER:
	return d1 > d2;
      case GREATER_EQL:
	return d1 >= d2;
      case LESS:
	return d1 < d2;
      case LESS_EQL:
	return d1 <= d2;
      case REGEXP:
	cerr << "Regexp not valid for floating point types" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
str_op(String s1, String s2, int op)
{
    switch (op) {
      case EQUAL:
	return s1 == s2;
      case NOT_EQUAL:
	return s1 != s2;
      case GREATER:
	return s1 > s2;
      case GREATER_EQL:
	return s1 >= s2;
      case LESS:
	return s1 < s2;
      case LESS_EQL:
	return s1 <= s2;
      case REGEXP: 
	  {
	      Regex r = s2;
	      return s1.matches(r);
	  }
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

#ifdef NEVER
static void
shorthand_error_message(String &id)
{
    cerr << "Shorthand field names must be unique;" << endl
	 << "`" << id << "' appears more than once" << endl;
}

// Mark a field.

static bool
mark_field(DDS &table, char *fields)
{
    DBG(cout << "FIELD: " << fields << endl);
    String f = fields;

    if (!f.contains(".")) {
	return false;
    }


	String aggregate = f.before(".");
	String field = f.from(".");
	field = field.after(".");

	BaseType *agg_ptr = dds.var(aggregate);
	if (agg_ptr)
	    return agg_ptr->var(field);	// recurse
	else
	    return 0;		// qualified names must be *fully* qualified
    }
    BaseType *bt;
    if ((bt = table.var(fields))) {
	DBG(cout << "Found " << fields << endl);
	bt->set_send_p(true);
	return true;
    }
    else {
	DBG(cout << "No " << fields << " in the DDS" << endl);
	return false;
    }
}

// When no projection is given, the entire dataset is marked.

static void
mark_all(DDS &table)
{
    for (Pix p = table.first_var(); p; table.next_var(p))
	table.var(p)->set_send_p(true);
}

// Mark an identifier. When marked a variable is read and serialized by the
// server. 

static bool
mark_id(DDS &table, char *id)
{
    DBG(cout << "ID: " <<  id << endl);

    BaseType *bt;
    if ((bt = table.var(id))) {
	DBG(cout << "Found " << id << endl);
	bt->set_send_p(true);
	return true;
    }
    else
	return false;
}

#endif
#ifdef NEVER
    else {
	for (Pix p = table.first_var(); p; table.next_var(p)) {
	    /* This call must check that the id is a *unique* field name,
	       thus the second call to mark_leaf with the remaining
	       DDS members which ensure that no other field (at this level in
	       the DDS) is named `id'. See mark_leaf() for more of this
	       kludge. */
	    if (mark_leaf(table.var(p), (String)id)) {
		table.next_var(p);
		while (p) {
		    if (mark_leaf(table.var(p), (String)id)) {
			shorthand_error_message((String)id);
			return false;
		    }
		    table.next_var(p);
		}
		return true;
	    }
	}

	cerr << "No `" << id << "' in the dataset" << endl;
	return false;
    }
}
#endif

/*
  Handle the special case where field names which are unique to a dataset do
  not need to be fully qualified.
*/

#ifdef NEVER
static bool
mark_leaf(BaseType *bt, String &name)
{
    if (bt->name() == name) {
	DBG(cout << "Found " << name << endl);
	bt->set_send_p(true);
	return true;
    }
    else
	switch(bt->type()) {
	    Pix p;
	  case array_t:
	    Array *a = (Array *)bt; /* nasty */
	    if (mark_leaf(a->var(a->name()), name)) /* kludge */
		return true;
	    break;

	  case list_t:
	    List *l = (List *)bt;
	    if (mark_leaf(l->var(l->name()), name))
		return true;
	    break;

	  case structure_t:
	    Structure *s = (Structure *)bt;
	    for (p = s->first_var(); p; s->next_var(p))
		if (mark_leaf(s->var(p), name)) {
		    s->next_var(p);
		    while (p) {
			if (mark_leaf(s->var(p), name)) {
			    shorthand_error_message(name);
			    return false;
			}
			s->next_var(p);
		    }
		    return true;
		}
	    break;
	    
	  case sequence_t:
	    Sequence *seq = (Sequence *)bt;
	    for (p = seq->first_var(); p; seq->next_var(p))
		if (mark_leaf(seq->var(p), name)) {
		    seq->next_var(p);
		    while (p) {
			if (mark_leaf(seq->var(p), name)) {
			    shorthand_error_message(name);
			    return false;
			}
			seq->next_var(p);
		    }
		    return true;
		}
	    break;

	  case function_t:
	    Function *f = (Function *)bt;
	    for (p = f->first_indep_var(); p; f->next_indep_var(p))
		if (mark_leaf(f->indep_var(p), name)) {
		    f->next_indep_var(p);
		    while (p) {
			if (mark_leaf(f->indep_var(p), name)) {
			    shorthand_error_message(name);
			    return false;
			}
			f->next_indep_var(p);
		    }
		    /* if an independent variable matches, the function must
		       check not only the remaining indep vars but also the
		       dependdetn variables. */
		    for (p = f->first_dep_var(); p; f->next_dep_var(p)) {
			if (mark_leaf(f->dep_var(p), name)) {
			    shorthand_error_message(name);
			    return false;
			}
		    }
		    return true;
		}

	    for (p = f->first_dep_var(); p; f->next_dep_var(p))
		if (mark_leaf(f->dep_var(p), name)) {
		    f->next_dep_var(p);
		    while (p) {
			if (mark_leaf(f->dep_var(p), name)) {
			    shorthand_error_message(name);
			    return false;
			}
			f->next_dep_var(p);
		    }
		    return true;
		}
	    break;
	    
	  case grid_t:
	    Grid *g = (Grid *)bt;
	    if (mark_leaf(g->array_var(), name)) {
		for (p = g->first_map_var(); p; g->next_map_var(p))
		    if (mark_leaf(g->map_var(p), name)) {
			shorthand_error_message(name);
			return false;
		    }
		return true;
	    }
	    for (p = g->first_map_var(); p; g->next_map_var(p))
		if (mark_leaf(g->map_var(p), name)) {
		    g->next_map_var(p);
		    while (p) {
			if (mark_leaf(g->map_var(p), name)) {
			    shorthand_error_message(name);
			    return false;
			}
			g->next_map_var(p);
		    }
		    return true;
		}
	    break;

	  default:
	    break;
	}

    return false;
}
#endif
