
/* -*- C++ -*- */

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
/* Revision 1.8  1996/05/14 15:39:02  jimg
/* These changes have already been checked in once before. However, I
/* corrupted the source repository and restored it from a 5/9/96 backup
/* tape. The previous version's log entry should cover the changes.
/*
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

%{

static char rcsid[]={"$Id: expr.y,v 1.8 1996/05/14 15:39:02 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <String.h>
#include <SLList.h>

#include "DDS.h"

#include "Connect.h"

#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Function.h"
#include "Grid.h"

#include "config_dap.h"
#include "util.h"
#include "debug.h"
#include "parser.h"
#include "expr.h"		/* the types IntList, IntListList and value */

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

int exprlex(void);		/* the scanner; see expr.lex */

int exprerror(const char *s);	/* easier to overload than use stdarg... */
int exprerror(const char *s, const char *s2);

IntList *make_array_index(value &i1, value &i2, value &i3);
IntListList *make_array_indeces(IntList *index);
IntListList *append_array_index(IntListList *indeces, IntList *index);
void delete_array_indeces(IntListList *indeces);
bool process_array_indeces(BaseType *variable, IntListList *indeces);

bool is_array_t(BaseType *variable);

rvalue_list *make_rvalue_list(DDS &table, rvalue *rv);
rvalue_list *append_rvalue_list(DDS &table, rvalue_list *rvals, rvalue *rv);

BaseType *make_variable(DDS &table, const value &val);

rvalue *dereference_variable(DDS &table, rvalue *rv);
rvalue *dereference_url(DDS &table, value &val);

bool_func_ptr get_function(const DDS &table, const char *name);
btp_func_ptr get_btp_function(const DDS &table, const char *name);

/* 
  The parser receives a DDS &table as a formal argument. TABLE is the DDS
  of the entire dataset; each variable in the constraint expression must be
  in this DDS and their data types must match the use in the constraint
  expression.
*/

%}

%expect 6

%union {
    bool boolean;
    int op;
    char id[ID_MAX];

    value val;

    bool_func_ptr bool_func;
    btp_func_ptr btp_func;

    IntList *int_l_ptr;
    IntListList *int_ll_ptr;
    
    rvalue *rval_ptr;
    rvalue_list *r_val_l_ptr;
}

%token <val> INT
%token <val> FLOAT
%token <val> STR

%token <id> ID
%token <id> FIELD

%token <op> EQUAL
%token <op> NOT_EQUAL
%token <op> GREATER
%token <op> GREATER_EQL
%token <op> LESS
%token <op> LESS_EQL
%token <op> REGEXP

%type <boolean> constraint_expr projection selection clause array_sel
%type <op> rel_op
%type <int_l_ptr> array_index
%type <int_ll_ptr> array_indeces
%type <rval_ptr> r_value constant identifier
%type <r_val_l_ptr> r_value_list 

%%

constraint_expr: /* empty constraint --> send all */
                 {
		     table.mark_all(true);
		     $$ = true;
		 }
                 /* projection only */
                 | projection
		 /* selection only --> project everything */
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
		      BaseType *var = table.var($1);
		      if (var) {
			  var->set_send_p(true); // add to projection
			  $$ = true;
		      }
		      else {
			  $$ = false;
			  exprerror("No such identifier in dataset", $1);
		      }
		  }
                | FIELD
                  { 
		      BaseType *var = table.var($1);
		      if (var)
			  $$ = table.mark($1, true); // must add parents, too
		      else {
			  $$ = false;
			  exprerror("No such field in dataset", $1);
		      }
		  }
		| array_sel
		  {
		      $$ = $1;
		  }
                | projection ',' ID
                  { 
		      BaseType *var = table.var($3);
		      if (var) {
			  var->set_send_p(true);
			  $$ = true;
		      }
		      else {
			  $$ = false;
			  exprerror("No such identifier in dataset", $3);
		      }

		  }
                | projection ',' FIELD
                  { 
		      BaseType *var = table.var($3);
		      if (var)
			  $$ = table.mark($3, true);
		      else {
			  $$ = false;
			  exprerror("No such field in dataset", $3);
		      }
		  }
                | projection ',' array_sel
                  {
		      $$ = $1 && $3;
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
		      assert(($1));
		      table.append_clause($2, $1, $4);
		      $$ = true;
		  }
		| r_value rel_op r_value
                  {
		      assert(($1));

		      rvalue_list *rv = new rvalue_list;
		      rv->append($3);
		      table.append_clause($2, $1, rv);
		      $$ = true;
		  }
		| ID '(' r_value_list ')'
		  {
		      bool_func_ptr b_f_ptr = get_function(table, $1);
		      if (!b_f_ptr) {
  			  exprerror("Not a boolean function", $1);
			  $$ = false;
		      }
		      else {
			  table.append_clause(b_f_ptr, $3);
			  $$ = true;
		      }
		  }
;

r_value:        identifier
                | constant
		| '*' identifier
		  {
		      $$ = dereference_variable(table, $2);
		      if (!$$)
			  exprerror("Could not dereference variable", 
				    ($2)->btp->name());
		  }
		| '*' STR
		  {
		      $$ = dereference_url(table, $2);
		      if (!$$)
			  exprerror("Could not dereference URL", *($2).v.s);
		  }
		| ID '(' r_value_list ')'
		  {
		      btp_func_ptr btp_f_ptr = get_btp_function(table, $1);
		      if (!btp_f_ptr) {
  			  exprerror("Not a BaseType * function", $1);
			  $$ = 0;
		      }
		      $$ = new rvalue(new btp_func_rvalue(btp_f_ptr, $3));
		  }
;

r_value_list:	r_value
		{
		    $$ = make_rvalue_list(table, $1);
		}
		| r_value_list ',' r_value
                {
		    $$ = append_rvalue_list(table, $1, $3);
		}
;

identifier:	ID 
                  { 
		      BaseType *btp = table.var($1);
		      if (!btp)
			  exprerror("No such identifier in dataset", $1);
		      $$ = new rvalue(btp);
		  }
		| FIELD 
                  { 
		      BaseType *btp = table.var($1);
		      if (!btp)
			  exprerror("No such field in dataset", $1);
		      $$ = new rvalue(btp);
		  }
;

constant:       INT
                  {
		      BaseType *btp = make_variable(table, $1);
		      $$ = new rvalue(btp);
		  }
		| FLOAT
                  {
		      BaseType *btp = make_variable(table, $1);
		      $$ = new rvalue(btp);
		  }
		| STR
                  { 
		      BaseType *btp = make_variable(table, $1); 
		      $$ = new rvalue(btp);
		  }
;

array_sel:	ID array_indeces 
                  {
		      BaseType *var = table.var($1);
		      if (var && is_array_t(var)) {
			  var->set_send_p(true);
			  $$ = process_array_indeces(var, $2);
		      }
		      else
			  $$ = false;
		  }
	        | FIELD array_indeces 
                  {
		      BaseType *var = table.var($1);
		      if (var && is_array_t(var))
			  $$ = table.mark($1, true) // set all the parents, too
			  && process_array_indeces(var, $2);
		      else
			  $$ = false;
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
		      val.type = dods_int32_c;
		      val.v.i = 1;
		      $$ = make_array_index($2, val, $4);
		  }
		| '[' INT ':' INT ':' INT ']'
                  {
		      $$ = make_array_index($2, $4, $6);
		  }
;

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
exprerror(const char *s)
{
    cerr << "Parse error: " << s << endl;
}

int 
exprerror(const char *s, const char *s2)
{
    cerr << "Parse error: " << s << ": " << s2 << endl;
}

// Given three values (I1, I2, I3), all of which must be integers, build an
// IntList which contains those values.
//
// Returns: A pointer to an IntList of three integers or NULL if any of the
// values are not integers.

IntList *
make_array_index(value &i1, value &i2, value &i3)
{
    IntList *index = new IntList;

    if (i1.type != dods_int32_c
	|| i2.type != dods_int32_c
	|| i3.type != dods_int32_c)
	return (void *)0;

    index->append((int)i1.v.i);
    index->append((int)i2.v.i);
    index->append((int)i3.v.i);

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

// Delete an array indeces list. 

void
delete_array_indeces(IntListList *indeces)
{
    for (Pix p = indeces->first(); p; indeces->next(p))
	delete (*indeces)(p);

    delete indeces;
}

bool
is_array_t(BaseType *variable)
{
    if (variable->type() != dods_array_c) {
	cerr << "Variable " << variable->name() << " is not an array." << endl;
	return false;
    }
    else
	return true;
}

bool
process_array_indeces(BaseType *variable, IntListList *indeces)
{
    bool status = true;

    Array *a = (Array *)variable; // replace with dynamic cast

    a->clear_constraint();	// each projection erases the previous one
    
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
	    cerr << "Too many values in index list for " << a->name() << "." 
		 << endl;
	    status = false;
	    goto exit;
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
	cerr << "Too many indeces in constraint for " << a->name() << "." 
	     << endl;
	status= false;
    }

exit:
    delete_array_indeces(indeces);
    return status;
}

// Create a list of r values and add VAL to the list.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
make_rvalue_list(DDS &table, rvalue *rv)
{
    rvalue_list *rvals = new rvalue_list;

    return append_rvalue_list(table, rvals, rv);
}

// Given a rvalue_list pointer RVALS and a value pointer VAL, make a variable
// to hold VAL and append that variable to the list RVALS.
//
// Returns: A pointer to the updated rvalue_list.

rvalue_list *
append_rvalue_list(DDS &table, rvalue_list *rvals, rvalue *rv)
{
    rvals->append(rv);

    return rvals;
}

// Given a string which is a URL, dereference it and return the data it
// points to.

static rvalue *
dereference_string(DDS &table, String &s)
{
    String url = s.before("?");	// strip off CE
    String ce = s.after("?");	// yes, get the CE

    Connect c = Connect(url);	// make the virtual connection

    // the initial URL must be a complete reference to data; thus no
    // additional CE is needed. 
    DDS d = c.request_data(ce, false); 

    // By definition, the DDS `D' can have only one variable, so make sure
    // that is true.
    if (d.num_var() != 1) {
	cerr << 
	    "Too many variables in URL; use only single variable projections"
	     << endl;
	return 0;
    }

    // OK, we're here. The first_var() must be the only var, return it bound
    // up in an rvalue struct. NB: the *object* must be copied since the one
    // within DDS `D' will be deleted by D's dtor.
    BaseType *btp = d.var(d.first_var())->ptr_duplicate();
    rvalue *rv = new rvalue(btp);

    return rv;
}

rvalue *
dereference_url(DDS &table, value &val)
{
    if (val.type != dods_str_c)
	return 0;

    return dereference_string(table, *val.v.s);
}

// Given a rvalue, get the BaseType that encapsulates its value, make sure it
// is a string and, if all that works, dereference it.

rvalue *
dereference_variable(DDS &table, rvalue *rv)
{
    BaseType *btp = rv->bvalue("dummy"); // the value will be read over the net
    if (btp->type() != dods_str_c && btp->type() != dods_url_c) {
	cerr << "Variable: " << btp->name() 
	    << " must be either a string or a url" 
	    << endl;
	return 0;
    }

    String s;
    String  *sp = &s;
    btp->buf2val((void **)&sp);
    
    return dereference_string(table, s);
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
	cerr << "Unknow type constant value" << endl;
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

bool_func_ptr
get_function(const DDS &table, const char *name)
{
    bool_func_ptr f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}

btp_func_ptr
get_btp_function(const DDS &table, const char *name)
{
    btp_func_ptr f;

    if (table.find_function(name, &f))
	return f;
    else
	return 0;
}
