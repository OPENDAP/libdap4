/*
   Grammar for the DDS. This grammar can be used with the bison parser
   generator to build a parser for the DDS. It assumes that a scanner called
   `ddslex()' exists and returns several token types (see das.tab.h)
   in addition to several single character token types. The matched lexeme
   for an ID is stored by the scanner in a global char * `ddslval'.
   Because the scanner returns a value via this global and because the parser
   stores ddslval (not the information pointed to), the values of rule
   components must be stored as they are parsed and used once accumulated at
   or near the end of a rule. If ddslval returned a value (instead of a
   pointer to a value) this would not be necessary.

   jhrg 8/29/94 
*/

/* 
 * $Log: dds.y,v $
 * Revision 1.3  1994/09/23 14:56:19  jimg
 * Added code to build in-memory DDS during parse.
 *
 * Revision 1.2  1994/09/15  21:11:56  jimg
 * Modified dds.y so that it can parse all the DDS types.
 * Still no error checking beyond what bison gives you.
 *
 * Revision 1.1  1994/09/08  21:10:45  jimg
 * DDS Class test driver and parser and scanner.
 */

%{
#define YYSTYPE char *
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

static char rcsid[]={"$Id: dds.y,v 1.3 1994/09/23 14:56:19 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream.h>

#include "dds.tab.h"
#include "DDS.h"

#include "Byte.h"
#include "Int32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Function.h"
#include "Grid.h"
#include "CTXPStack.h"

extern int dds_line_num;

static CtorTypePtrXPStack ctor;	/* stack for ctor types */
static BaseType *current;
static Part part = nil;		/* Part is defined in CtorType */

int ddslex();
int ddserror(char *s);
void add_entry(DDS &table, CtorTypePtrXPStack &ctor, BaseType **current, 
	       Part p);
%}

%expect 61

%token ID
%token INTEGER
%token DATASET
%token INDEPENDENT
%token DEPENDENT
%token ARRAY
%token MAPS
%token LIST
%token SEQUENCE
%token STRUCTURE
%token FUNCTION
%token GRID
%token BYTE
%token INT32
%token FLOAT64
%token STRING
%token URL 

%%

/* 
   The parser is called through a function named ddsparse(DDS &table). Once
   the parse has completed, table is an instance of class DDS which contains
   objects representing the DDS described by the input to this parser.

   Parser algorithm:
*/   

datasets:	/* empty */
		| dataset
		| datasets dataset
;

dataset:	DATASET '{' declarations '}' name ';' 
;

declarations:	/* empty */
		| declaration
		| declarations declaration
;

declaration: 	list declaration 
                    { if (current->check_semantics())
			add_entry(table, ctor, &current, part); }

                | base_type var ';' 
                    { if (current->check_semantics())
			add_entry(table, ctor, &current, part); }

		| structure  '{' declarations '}' 
		    { current = ctor.pop(); } 
                  var ';' 
                    { if (current->check_semantics())
			add_entry(table, ctor, &current, part); }

		| sequence '{' declarations '}' 
                    { current = ctor.pop(); } 
                  var ';' 
                    { if (current->check_semantics())
			add_entry(table, ctor, &current, part); }

		| function '{' INDEPENDENT ':'
		    { part = independent; }
                  declarations DEPENDENT ':' 
		    { part = dependent;} 
                  declarations '}' 
                    { current = ctor.pop(); }
                  var ';'
                    { if (current->check_semantics()) {
			part = nil; 
			add_entry(table, ctor, &current, part); 
		      }
                    }

		| grid '{' ARRAY ':' 
		    { part = array; }
                  declaration MAPS ':' 
		    { part = maps; }
                  declarations '}' 
		    { current = ctor.pop(); }
                  var ';' 
                    { if (current->check_semantics()) {
			part = nil; 
			add_entry(table, ctor, &current, part); 
		      }
                    }
;

list:		LIST { ctor.push(new List); }
;

structure:	STRUCTURE { ctor.push(new Structure); }
;

sequence:	SEQUENCE { ctor.push(new Sequence); }
;

function:	FUNCTION { ctor.push(new Function); }
;

grid:		GRID { ctor.push(new Grid); }
;

base_type:	BYTE { current = new Byte; }
		| INT32 { current = new Int32; }
		| FLOAT64 { current = new Float64; }
		| STRING { current = new Str; }
		| URL { current = new Url; }
;

var:		ID { current->set_var_name($1); }
 		| var array_decl
;

array_decl:	'[' INTEGER ']'
                 { 
		     if (current->get_var_type() == "Array") {
			 ((Array *)current)->append_dim(atoi($2));
		     }
		     else {
			 Array *a = new Array; 
			 a->add_var(current); 
			 a->append_dim(atoi($2));
			 current = a;
		     }
		 }
;

name:		ID { table.set_dataset_name($1); }
;

%%

int 
ddserror(char *s)
{
    fprintf(stderr, "%s line: %d\n", s, dds_line_num);
}

/*
  Add the variable pointed to by CURRENT to either the topmost ctor object on
  the stack CTOR or to the dataset variable table TABLE if CTOR is empty.  If
  it exists, the current ctor object is poped off the stack and assigned to
  CURRENT.

  NB: the ctor stack is poped for lists and arrays because they are ctors
  which contain only a single variable. For other ctor types, several
  varaiables may be members and the parse rule (see `declaration' above)
  determines when to pop the stack. 

  Returns: void 
*/

void	
add_entry(DDS &table, CtorTypePtrXPStack &ctor, BaseType **current, Part part)
{ 
    if (!ctor.empty()) { /* must be parsing a ctor type */
	ctor.top()->add_var(*current, part);
	const String &ctor_type = ctor.top()->get_var_type();
	if (ctor_type == "List" || ctor_type == "Array")
	    *current = ctor.pop();
	else
	    return;
    }
    else
	table.add_var(*current);
}

