
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

/*
  This is the parser for the DODS constraint expression grammar. The parser
  calls various `helper' functions defined by the DAP classes which either
  implement the operations (in the case of relational ops) or store
  information (in the case of selection operations). 

  jhrg 9/5/95
*/

/*
 * $Log: expr.y,v $
 * Revision 1.25  1998/09/17 16:56:50  jimg
 * Made the error messages more verbose (that is, the text in the Error objects
 * sent back to the client).
 * Fixed a bug where non-existent fields could be accessed - with predictably
 * bad results.
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
 * Added __unused__ to definition of char rcsid[].
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

%{

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: expr.y,v 1.25 1998/09/17 16:56:50 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <String.h>
#include <SLList.h>

#include "debug.h"

#include "DDS.h"

#include "Connect.h"

#include "BaseType.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Function.h"
#include "Grid.h"
#include "Error.h"

#include "util.h"
#include "parser.h"
#include "expr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)
#define ERROR_OBJ(arg) ((parser_arg *)(arg))->_error
#define STATUS(arg) ((parser_arg *)(arg))->_status
#if DODS_BISON_VER > 124
#define YYPARSE_PARAM arg
#else
#define YYPARSE_PARAM void *arg
#endif

int exprlex(void);		/* the scanner; see expr.lex */

void exprerror(const char *s);	/* easier to overload than use stdarg... */
void exprerror(const char *s, const char *s2);

int_list *make_array_index(value &i1, value &i2, value &i3);
int_list_list *make_array_indices(int_list *index);
int_list_list *append_array_index(int_list_list *indices, int_list *index);
void delete_array_indices(int_list_list *indices);
bool process_array_indices(BaseType *variable, int_list_list *indices); 
bool process_grid_indices(BaseType *variable, int_list_list *indices); 

bool is_array_t(BaseType *variable);
bool is_grid_t(BaseType *variable);

rvalue_list *make_rvalue_list(rvalue *rv);
rvalue_list *append_rvalue_list(rvalue_list *rvals, rvalue *rv);

BaseType *make_variable(DDS &table, const value &val);

rvalue *dereference_variable(rvalue *rv, DDS &dds);
rvalue *dereference_url(value &val);

bool_func get_function(const DDS &table, const char *name);
btp_func get_btp_function(const DDS &table, const char *name);

%}

%expect 6

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
%type <int_ll_ptr> array_indices
%type <rval_ptr> r_value constant identifier
%type <r_val_l_ptr> r_value_list 

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
                 | ID '(' r_value_list ')'
		   {
		       btp_func func = get_btp_function(*(DDS_OBJ(arg)), $1);
		       if (!func) {
			   exprerror("Not a BaseType pointer function", $1);
			   String msg = "The function `";
			   msg += (String)$1 + "' is not defined on this server.";
			   ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			   STATUS(arg) = false;
			   $$ = false;
		       }
		       else {
			   (*DDS_OBJ(arg)).append_clause(func, $3);
			   $$ = true;
		       }
		   }
;

projection:	ID 
                  { 
		      BaseType *var = (*DDS_OBJ(arg)).var($1);
		      if (var) {
			  $$ = (*DDS_OBJ(arg)).mark($1, true);
		      }
		      else {
			  exprerror("No such identifier in dataset", $1);
			  String msg = "The identifier `";
			  msg += (String)$1 + "' is not in the dataset.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = false;
		      }
		  }
                | FIELD
                  { 
		      BaseType *var = (*DDS_OBJ(arg)).var($1);
		      if (var)
			  $$ = (*DDS_OBJ(arg)).mark($1, true); // must add parents, too
		      else {
			  exprerror("No such field in dataset", $1);
			  String msg = "The field `";
			  msg += (String)$1 + "' is not in this dataset.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = false;
		      }
		  }
		| array_sel	/* Array *Selection* is a misnomer... */
		  {
		      $$ = $1;
		  }
                | projection ',' ID
                  { 
		      BaseType *var = (*DDS_OBJ(arg)).var($3);
		      if (var) {
			  $$ = (*DDS_OBJ(arg)).mark($3, true);
		      }
		      else {
			  exprerror("No such identifier in dataset", $3);
			  String msg = "The identifier `";
			  msg += (String)$1 + "' is not in this dataset.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = false;
		      }

		  }
                | projection ',' FIELD
                  { 
		      BaseType *var = (*DDS_OBJ(arg)).var($3);
		      if (var)
			  $$ = (*DDS_OBJ(arg)).mark($3, true);
		      else {
			  exprerror("No such field in dataset", $3);
			  String msg = "The field `";
			  msg += (String)$1 + "' is not in this dataset.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = false;
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
			  rv->append($3);
			  (*DDS_OBJ(arg)).append_clause($2, $1, rv);
			  $$ = true;
		      }
		      else
			  $$ = false;
		  }
		| ID '(' r_value_list ')'
		  {
		      bool_func b_func = get_function((*DDS_OBJ(arg)), $1);
		      if (!b_func) {
  			  exprerror("Not a boolean function", $1);
			  String msg = "The function `";
			  msg += (String)$1 + "' is not defined on this server.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = false;
		      }
		      else {
			  (*DDS_OBJ(arg)).append_clause(b_func, $3);
			  $$ = true;
		      }
		  }
;

r_value:        identifier
                | constant
		| '*' identifier
		  {
		      $$ = dereference_variable($2, *DDS_OBJ(arg));
		      if (!$$) {
			  exprerror("Could not dereference variable", 
				    ($2)->value->name());
			  String msg = "Could not dereference the URL: ";
			  msg += (String)($2)->value->name();
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
		      }
		  }
		| '*' STR
		  {
		      $$ = dereference_url($2);
		      if (!$$)
			  exprerror("Could not dereference URL", *($2).v.s);
		  }
		| ID '(' r_value_list ')'
		  {
		      btp_func bt_func = get_btp_function((*DDS_OBJ(arg)), $1);
		      if (!bt_func) {
  			  exprerror("Not a BaseType * function", $1);
			  String msg = "The function `";
			  msg += (String)$1 + "' is not defined on this server.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = 0;
		      }
		      $$ = new rvalue(new func_rvalue(bt_func, $3));
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

identifier:	ID 
                  { 
		      BaseType *btp = (*DDS_OBJ(arg)).var($1);
		      if (!btp) {
			  exprerror("No such identifier in dataset", $1);
			  String msg = "The identifier `";
			  msg += (String)$1 + "' is not in this dataset.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = 0;
		      }
		      else
			  $$ = new rvalue(btp);
		  }
		| FIELD 
                  { 
		      BaseType *btp = (*DDS_OBJ(arg)).var($1);
		      if (!btp) {
			  exprerror("No such field in dataset", $1);
			  String msg = "The field `";
			  msg += (String)$1 + "' is not in this dataset.";
			  ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			  STATUS(arg) = false;
			  $$ = 0;
		      }
		      else
			  $$ = new rvalue(btp);
		  }
;

constant:       INT
                  {
		      BaseType *btp = make_variable((*DDS_OBJ(arg)), $1);
		      $$ = new rvalue(btp);
		  }
		| FLOAT
                  {
		      BaseType *btp = make_variable((*DDS_OBJ(arg)), $1);
		      $$ = new rvalue(btp);
		  }
		| STR
                  { 
		      BaseType *btp = make_variable((*DDS_OBJ(arg)), $1); 
		      $$ = new rvalue(btp);
		  }
;

/* Array *selection* is a misnomer; it is really array *projection*. jhrg */
array_sel:	ID array_indices 
                  {
		      BaseType *var = (*DDS_OBJ(arg)).var($1);
		      if (var && is_array_t(var)) {
			  /* calls to set_send_p should be replaced with
			     calls to DDS::mark so that arrays of Structures,
			     etc. will be processed correctly when individual
			     elements are projected using short names (Whew!)
			     9/1/98 jhrg */
			  var->set_send_p(true);
			  $$ = process_array_indices(var, $2);
			  if (!$$) {
			      String msg = "The indices given for `";
			      msg += (String)$1 + "' are out of range.";
			      ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			      STATUS(arg) = false;
			  }
			  delete_array_indices($2);
		      }
		      else if (var && is_grid_t(var)) {
			  var->set_send_p(true);
			  $$ = process_grid_indices(var, $2);
			  if (!$$) {
			      String msg = "The indices given for `";
			      msg += (String)$1 + "' are out of range.";
			      ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			      STATUS(arg) = false;
			  }
			  delete_array_indices($2);
		      }
		      else
			  $$ = false;
		  }
	        | FIELD array_indices 
                  {
		      BaseType *var = (*DDS_OBJ(arg)).var($1);
		      if (var && is_array_t(var)) {
			  $$ = (*DDS_OBJ(arg)).mark($1, true) // set all the parents, too
			      && process_array_indices(var, $2);
			  if (!$$) {
			      String msg = "The indices given for `";
			      msg += (String)$1 + "' are out of range.";
			      ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			      STATUS(arg) = false;
			  }
			  delete_array_indices($2);
		      }
		      else if (var && is_grid_t(var)) {
			  $$ = (*DDS_OBJ(arg)).mark($1, true) // set all the parents, too
			       && process_grid_indices(var, $2);
			  if (!$$) {
			      String msg = "The indices given for `";
			      msg += (String)$1 + "' are out of range.";
			      ERROR_OBJ(arg) = new Error(malformed_expr, msg);
			      STATUS(arg) = false;
			  }
			  delete_array_indices($2);
		      }
		      else
			  $$ = false;
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

void
exprerror(const char *s)
{
    cerr << "Parse error: " << s << endl;
}

void
exprerror(const char *s, const char *s2)
{
    cerr << "Parse error: " << s << ": " << s2 << endl;
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

    if (i1.type != dods_int32_c
	|| i2.type != dods_int32_c
	|| i3.type != dods_int32_c)
	return (int_list *)0;

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

int_list_list *
make_array_indices(int_list *index)
{
    int_list_list *indices = new int_list_list;

    DBG(Pix dp;\
	cout << "index: ";\
	for (dp = index->first(); dp; index->next(dp))\
	cout << (*index)(dp) << " ";\
	cout << endl);

    assert(index);
    indices->append(index);

    return indices;
}

int_list_list *
append_array_index(int_list_list *indices, int_list *index)
{
    assert(indices);
    assert(index);

    indices->append(index);

    return indices;
}

// Delete an array indices list. 

void
delete_array_indices(int_list_list *indices)
{
    assert(indices);

    for (Pix p = indices->first(); p; indices->next(p)) {
	assert((*indices)(p));
	delete (*indices)(p);
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
process_array_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);
    assert(variable->type() == dods_array_c);
    Array *a = (Array *)variable; // replace with dynamic cast

    DBG(cerr << "Before clear_costraint:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    a->clear_constraint();	// each projection erases the previous one
    
    DBG(cerr << "After clear_costraint:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    Pix p, r;
    assert(indices);
    for (p = indices->first(), r = a->first_dim(); 
	 p && r; 
	 indices->next(p), a->next_dim(r)) {
	assert((*indices)(p));
	int_list *index = (*indices)(p);

	Pix q = index->first(); 
	assert(q);
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
	
	if (!a->add_constraint(r, start, stride, stop)) {
	    cerr << "Impossible index values in constraint for "
		 << a->name() << "." << endl;
	    status = false;
	    goto exit;
	}

	DBG(cerr << "Set Constraint: " << a->dimension_size(r, true) << endl);
    }

    DBG(cerr << "After processing loop:" << endl);
    DBG(a->print_decl(cerr, "", true, false, true));

    DBG(Pix dp;\
	cout << "Array Constraint: ";\
	for (dp = a->first_dim(); dp; a->next_dim(dp))\
	    cout << a->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p && !r) {
	cerr << "Too many indices in constraint for " << a->name() << "." 
	     << endl;
	status= false;
    }

exit:
    return status;
}

bool
process_grid_indices(BaseType *variable, int_list_list *indices)
{
    bool status = true;

    assert(variable);
    assert(variable->type() == dods_grid_c);
    Grid *g = (Grid *)variable; // Replace with dynamic cast.

    // First do the constraints on the ARRAY in the grid.
    status = process_array_indices(g->array_var(), indices);
    if (!status)
	goto exit;

    // Now process the maps.
    Pix p, r;

    // Supress all maps by default.
    for (r = g->first_map_var(); r; g->next_map_var(r))
	g->map_var(r)->set_send_p(false);

    // Add specified maps to the current projection.
    assert(indices);
    for (p = indices->first(), r = g->first_map_var(); 
	 p && r; 
	 indices->next(p), g->next_map_var(r)) {
	assert((*indices)(p));
	int_list *index = (*indices)(p);

	Pix q = index->first(); 
	assert(q);
	int start = (*index)(q);

	index->next(q);
	int stride = (*index)(q);
	
	index->next(q);
	int stop = (*index)(q);

	assert(g->map_var(r));
	assert(g->map_var(r)->type() == dods_array_c);
	Array *a = (Array *)g->map_var(r);
	a->set_send_p(true);
	a->clear_constraint();

	index->next(q);
	if (q) {
	    cerr << "Too many values in index list for " << a->name() << "." 
		 << endl;
	    status = false;
	    goto exit;
	}

	if (!a->add_constraint(a->first_dim(), start, stride, stop)) {
	    cerr << "Impossible index values in constraint for "
		 << a->name() << "." << endl;
	    status = false;
	    goto exit;
	}

	DBG(cerr << "Set Constraint: " \
	    << a->dimension_size(a->first_dim(), true) << endl);
    }

    DBG(Pix dp;\
	cout << "Grid Constraint: ";\
	for (dp = ((Array *)g->array_var())->first_dim(); dp; \
		 ((Array *)g->array_var())->next_dim(dp))\
	   cout << ((Array *)g->array_var())->dimension_size(dp, true) << " ";\
	cout << endl);
    
    if (p && !r) {
	cerr << "Too many indices in constraint for " 
	     << g->map_var(r)->name() << "." << endl;
	status= false;
    }

exit:
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

    rvals->append(rv);

    return rvals;
}

// Given a string which is a URL, dereference it and return the data it
// points to.

static rvalue *
dereference_string(String &s)
{
    String url = s.before("?");	// strip off CE
    String ce = s.after("?");	// yes, get the CE

    // I don't think that the `false' is really necessary, but g++ seems to
    // want it. jhrg 2/10/97
    Connect c = Connect(url, false); // make the virtual connection

    // the initial URL must be a complete reference to data; thus no
    // additional CE is needed. 
    DDS *d = c.request_data(ce, false, false); 

    // By definition, the DDS `D' can have only one variable, so make sure
    // that is true.
    if (d->num_var() != 1) {
	cerr << 
	    "Too many variables in URL; use only single variable projections"
	     << endl;
	return 0;
    }

    // OK, we're here. The first_var() must be the only var, return it bound
    // up in an rvalue struct. NB: the *object* must be copied since the one
    // within DDS `D' will be deleted by D's dtor.
    BaseType *btp = d->var(d->first_var())->ptr_duplicate();
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
	cerr << "Variable: " << btp->name() 
	    << " must be either a string or a url" 
	    << endl;
	return 0;
    }

    String s;
    String  *sp = &s;
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
