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
/* Revision 1.2  1995/10/23 23:10:38  jimg
/* Added includes for various classes.
/* Aded rules, actions and functions for evaluation of projections.
/* Changed the value of YYSTYPE so that bison's %union feature is used -
/* rules now return several different types.
/*
 * Revision 1.1  1995/10/13  03:04:08  jimg
 * First version. Incorporates Glenn's suggestions.
 */

%{

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

static char rcsid[]={"$Id: expr.y,v 1.2 1995/10/23 23:10:38 jimg Exp $"};

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

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void mem_list_report();
int exprlex(void);		/* the scanner; see expr.lex */
int exprerror(char *s);

static bool mark_id(DDS &table, char *id);
static bool mark_field(DDS &table, char *fields);
static void mark_all(DDS &table);
static bool mark_leaf(BaseType *bt, String &name);

/*
  The parser recieives DDS &table as a formal argument. TABLE is the DDS of
  the entire dataset; each variable in the constraint expression must be in
  this DDS and their datatypes must match the use in the constraint
  expression. 
*/

%}

%expect 6

%token <char_ptr> ID
%token <char_ptr> FIELD
%token <char_ptr> INT
%token <char_ptr> FLOAT
%token <char_ptr> STR

%token <char_ptr> EQUAL
%token <char_ptr> NOT_EQUAL
%token <char_ptr> GREATER
%token <char_ptr> GREATER_EQL
%token <char_ptr> LESS
%token <char_ptr> LESS_EQL
%token <char_ptr> REGEXP

%union {
    bool boolean;
    char *char_ptr;
    SLList<String> *str_list_ptr;
}

%type <boolean> constraint_expr projection

%%

constraint_expr: /* empty */ 
                 {
		     mark_all(table);
		     $$ = true;
		 }
                 | projection
;

/* | selection */
/* | projection '&' selection */


projection:	ID 
                  { 
		      DBG(cout << "PROJ:ID\n"); 
		      $$ = mark_id(table, $1); 
		  }
                | FIELD
                  { 
		      DBG(cout << "PROJ:FIELD\n"); 
		      $$ = mark_field(table, $1); 
		  }
                | projection ',' ID
                  { 
		      DBG(cout << "PROJ:PROJ,ID\n"); 
		      $$ = $1 && mark_id(table, $3); 
		  }
                | projection ',' FIELD
                  { 
		      DBG(cout << "PROJ:PROJ,FIELD\n"); 
		      $$ = $1 && mark_field(table, $3); 
		  }
;
/*
selection:	clause
		| selection '&' clause
;

clause:		operand rel_op '{' list '}'
		| operand rel_op operand
		| operand
;

operand:	ID
		| INT
		| FLOAT
		| field_sel
		| array_sel
		| '*' STR
;

array_sel:	ID array_index
		| array_sel array_index
;

array_index:	'[' INT ':' INT ':' INT ']'
		| '[' INT ':' INT ']'
;

list:		list_str
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

rel_op:		EQUAL
		| NOT_EQUAL
		| GREATER
		| GREATER_EQL
		| LESS
		| LESS_EQL
		| REGEXP
;
*/
%%

int 
exprerror(char *s)
{
    fprintf(stderr, "%s\n");
}

static void
shorthand_error_message(String &id)
{
    cerr << "Shorthand field names must be unique;" << endl
	 << "`" << id << "' appears more than once" << endl;
}

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

// Mark a field.

static bool
mark_field(DDS &table, char *fields)
{
    DBG(cout << "FIELD: " << fields << endl);

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

	    
