
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1997
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

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
 * Revision 1.24  1999/03/24 23:32:33  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.23  1998/08/13 22:12:44  jimg
 * Fixed error messages.
 *
 * Revision 1.22  1997/11/20 20:14:10  jimg
 * Added to the name rule so that it recognizes both the ID and NAME lexeme
 * as valid when parsing the dataset name. NAME (see dds.lex) is just like ID
 * except that it includes `.'. Thus datasets with names like sst.reynolds.nc
 * now parse correctly.
 *
 * Revision 1.21  1997/02/28 01:31:22  jimg
 * Added error messages.
 *
 * Revision 1.20  1996/10/28 23:44:16  jimg
 * Added unsigned int to set of possible datatypes.
 *
 * Revision 1.19  1996/10/16 22:35:31  jimg
 * Fixed bad operator in DODS_BISON_VER preprocessor statement.
 *
 * Revision 1.18  1996/10/08 17:04:42  jimg
 * Added a fix for Bison 1.25 so that PARSE_PARAM will still work
 *
 * Revision 1.17  1996/08/13 20:54:45  jimg
 * Generated files.
 *
 * Revision 1.16  1996/05/31 23:27:17  jimg
 * Removed {YYACCEPT;} from rule 2 (dataset: DATASET ...).
 *
 * Revision 1.15  1996/05/29 21:59:51  jimg
 * *** empty log message ***
 *
 * Revision 1.14  1996/05/14 15:38:54  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.13  1996/04/05 21:59:38  jimg
 * Misc Changes for release 2.0.1 of the core software - for developers.
 *
 * Revision 1.12  1996/04/05 00:06:45  jimg
 * Merged changes from version 1.1.1.
 * Eliminated the static global CTOR.
 *
 * Revision 1.11  1995/12/06 19:45:08  jimg
 * Changed grammar so that List List ... <type> is no longer possible. This
 * fixed some hard problems in the serailize/deserailize mfuncs.
 *
 * Revision 1.10  1995/10/23  22:59:41  jimg
 * Modified some rules so that they use the functions defined in
 * parser_util.cc instead of local definitions.
 *
 * Revision 1.9  1995/08/23  00:27:47  jimg
 * Uses new member functions.
 * Added copyright notice.
 * Switched from String to enum type representation.
 *
 * Revision 1.8.2.1  1996/04/04 23:24:44  jimg
 * Removed static global CTOR from the dds parser. The stack for constructor
 * variable is now managed via a pointer. The stack is allocated when first
 * used by add_entry().
 *
 * Revision 1.8  1995/01/19  20:13:04  jimg
 * The parser now uses the new utility functions to create new instances
 * of the variable objects (Byte, ..., Grid).
 * Fixed the number of shift/reduce conflicts expected (now at 60).
 *
 * Revision 1.7  1994/12/22  04:30:57  reza
 * Made save_str static to avoid linking conflict.
 *
 * Revision 1.6  1994/12/16  22:24:23  jimg
 * Switched from a CtorType stack to BaseType stack.
 * Fixed an error in save_str() (see das.y).
 * Fixed a bug in the use of append_dim - it was called with $4 when it
 * should have been called with $5.
 *
 * Revision 1.5  1994/12/09  21:42:41  jimg
 * Added to array: so that an array decl can contain: an int or an id=int.
 * This is for the named dimensions (see Array.{cc,h}).
 *
 * Revision 1.4  1994/11/10  19:50:54  jimg
 * In the past it was possible to have a null file correctly parse as a
 * DAS or DDS. However, now that is not possible. It is possible to have
 * a file that contains no variables parse, but the keyword `Attribute'
 * or `Dataset' *must* be present. This was changed so that errors from
 * the CGIs could be detected (since they return nothing in the case of
 * a error).
 *
 * Revision 1.3  1994/09/23  14:56:19  jimg
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

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: dds.y,v 1.24 1999/03/24 23:32:33 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream.h>
#include <strstream.h>

#include "DDS.h"
#include "Array.h"
#include "Error.h"
#include "BTXPStack.h"
#include "parser.h"
#include "dds.tab.h"
#include "util.h"

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

extern int dds_line_num;	/* defined in dds.lex */

static BaseTypePtrXPStack *ctor; /* stack for ctor types */
static BaseType *current;
static Part part = nil;		/* Part is defined in BaseType */
static char id[ID_MAX];

static char *NO_DDS_MSG =
"The descriptor object returned from the dataset was null.\n\
Check that the URL is correct.";

int ddslex();
void ddserror(char *s);

void add_entry(DDS &table, BaseTypePtrXPStack **ctor, BaseType **current, 
	       Part p);

%}

%expect 66

%token ID
%token NAME
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
%token INT16
%token UINT16
%token INT32
%token UINT32
%token FLOAT32
%token FLOAT64
%token STRING
%token URL 

%%

datasets:	dataset
		| datasets dataset
;

dataset:	DATASET '{' declarations '}' name ';'
                | error
                {
		    parse_error((parser_arg *)arg, NO_DDS_MSG);
		    YYABORT;
		}
;

declarations:	/* empty */
		| declaration
		| declarations declaration
;

declaration: 	list non_list_decl
                { 
		    String smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
			ostrstream msg;
			msg << "In the dataset descriptor object:" << endl
			    << "`" << $1 << " " << $2 
			    << "' is not a valid declaration" << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
                | non_list_decl
;

/* This non-terminal is here only to keep types like `List List Int32' from
   parsing. DODS does not allow Lists of Lists. Those types make translation
   to/from arrays too hard. */

non_list_decl:  base_type var ';' 
                { 
		    String smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
			ostrstream msg;
			msg << "In the dataset descriptor object:" << endl
			    << "`" << $1 << " " << $2 
			    << "' is not a valid declaration" << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}

		| structure  '{' declarations '}' 
		{ 
		    current = ctor->pop(); 
		} 
                var ';' 
                { 
		    String smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
			ostrstream msg;
			msg << "In the dataset descriptor object:" << endl
			    << "`" << $1 << "'" << endl
			    << "is not a valid declaration." << endl
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}

		| sequence '{' declarations '}' 
                { 
		    current = ctor->pop(); 
		} 
                var ';' 
                { 
		    String smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
			ostrstream msg;
			msg << "In the dataset descriptor object:" << endl
			    << "`" << $1 << "'" << endl
			    << "is not a valid declaration." << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}

		| function '{' INDEPENDENT ':'
		{ part = independent; }
                declarations DEPENDENT ':' 
		{ part = dependent;} 
                declarations '}' 
                { 
		    current = ctor->pop(); 
		}
                var ';'
                { 
		    String smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
			ostrstream msg;
			msg << "In the dataset descriptor object:" << endl
			    << "`" << $1 << "'" << endl
			    << "is not a valid declaration." << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}

		| grid '{' ARRAY ':' 
		{ part = array; }
                declaration MAPS ':' 
		{ part = maps; }
                declarations '}' 
		{
		    current = ctor->pop(); 
		}
                var ';' 
                {
		    String smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
			ostrstream msg;
			msg << "In the dataset descriptor object:" << endl
			    << "`" << $1 << "'" << endl
			    << "is not a valid declaration." << endl 
			    << smsg << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}

                | error
                {
		    ostrstream msg;
		    msg << "In the dataset descriptor object:" << endl
			<< "Expected a varaible declaration" << endl 
			<< "(e.g., Int32 i;). Make sure that the" << endl
			<< "variable name is not a reserved word" << endl
			<< "(Byte, Int32, Float64, String, Url" <<endl
			<< "Structure, Sequence or Grid - all" << endl
			<< "forms, byte, Byte and BYTE, are the same)" << endl
			<< ends;
		    parse_error((parser_arg *)arg, msg.str());
		    msg.freeze(0);
		    YYABORT;
		}
;
 

list:		LIST 
		{ 
		    if (!ctor) 
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewList()); 
		}
;

structure:	STRUCTURE
		{ 
		    if (!ctor)
	                ctor = new BaseTypePtrXPStack;
		    ctor->push(NewStructure()); 
		}
;

sequence:	SEQUENCE 
		{ 
		    if (!ctor)
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewSequence()); 
		}
;

function:	FUNCTION 
		{ 
		    if (!ctor)
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewFunction()); 
		}
;

grid:		GRID 
		{ 
		    if (!ctor)
			ctor = new BaseTypePtrXPStack;
		    ctor->push(NewGrid()); 
		}
;

base_type:	BYTE { current = NewByte(); }
		| INT16 { current = NewInt16(); }
		| UINT16 { current = NewUInt16(); }
		| INT32 { current = NewInt32(); }
		| UINT32 { current = NewUInt32(); }
		| FLOAT32 { current = NewFloat32(); }
		| FLOAT64 { current = NewFloat64(); }
		| STRING { current = NewStr(); }
		| URL { current = NewUrl(); }
;

var:		ID { current->set_name($1); }
 		| var array_decl
;

array_decl:	'[' INTEGER ']'
                 { 
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi($2));
		     }
		     else {
			 Array *a = NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi($2));
			 current = a;
		     }
		 }

		 | '[' ID 
		 {
		     save_str(id, $2, dds_line_num);
		 } 
                 '=' INTEGER 
                 { 
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi($5), id);
		     }
		     else {
			 Array *a = NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi($5), id);
			 current = a;
		     }
		 }
		 ']'

		 | error
                 {
		     ostrstream msg;
		     msg << "In the dataset descriptor object:" << endl
			 << "Expected an array subscript." << endl << ends;
		     parse_error((parser_arg *)arg, msg.str());
		     msg.freeze(0);
		     YYABORT;
		 }
;

name:		NAME { (*DDS_OBJ(arg)).set_dataset_name($1); }
		| ID { (*DDS_OBJ(arg)).set_dataset_name($1); }
                | error 
                {
		     ostrstream msg;
		     msg << "Error parsing the dataset name." << endl
			 << "The name may be missing or may contain an illegal character." << endl << ends;
		     parse_error((parser_arg *)arg, msg.str());
		     msg.freeze(0);
		     YYABORT;
		}
;

%%

/* 
   This function must be defined. However, use the error reporting code in
   parser-utils.cc.
*/

void 
ddserror(char *)
{
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
add_entry(DDS &table, BaseTypePtrXPStack **ctor, BaseType **current, Part part)
{ 
    if (!*ctor)
	*ctor = new BaseTypePtrXPStack;

    if (!(*ctor)->empty()) { /* must be parsing a ctor type */
	(*ctor)->top()->add_var(*current, part);

 	const Type &ctor_type = (*ctor)->top()->type();

	if (ctor_type == dods_list_c || ctor_type == dods_array_c)
	    *current = (*ctor)->pop();
	else
	    return;
    }
    else
	table.add_var(*current);
}
