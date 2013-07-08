
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

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

%{

#include "config_dap.h"

#include <cstring>
#include <cassert>
#include <iostream>
#include <stack>
#include <sstream>

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "DDS.h"
#include "Error.h"
#include "parser.h"
#include "util.h"

using namespace std;
using namespace libdap;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 
// ERROR is no longer used. These parsers now signal problems by throwing
// exceptions. 5/22/2002 jhrg
#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

extern int dds_line_num;	/* defined in dds.lex */

// No global static objects in the dap library! 1/24/2000 jhrg
static stack<BaseType *> *ctor;
static BaseType *current;
static string *id;
static Part part = nil;		/* Part is defined in BaseType */

static const char *NO_DDS_MSG =
"The descriptor object returned from the dataset was null.\n\
Check that the URL is correct.";

static const char *BAD_DECLARATION =
"In the dataset descriptor object: Expected a variable declaration\n\
(e.g., Int32 i;). Make sure that the variable name is not the name\n\
of a datatype and that the Array: and Maps: sections of a Grid are\n\
labeled properly.";
 
int ddslex();
void ddserror(const string &s /*char *s*/);
void error_exit_cleanup();
void add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, 
	       Part p);
void invalid_declaration(parser_arg *arg, string semantic_err_msg, 
			 char *type, char *name);

%}

%expect 52

%union {
    bool boolean;
    char word[ID_MAX];
}

%token <word> SCAN_WORD
%token <word> SCAN_DATASET
%token <word> SCAN_LIST
%token <word> SCAN_SEQUENCE
%token <word> SCAN_STRUCTURE
%token <word> SCAN_FUNCTION
%token <word> SCAN_GRID
%token <word> SCAN_BYTE
%token <word> SCAN_INT16
%token <word> SCAN_UINT16
%token <word> SCAN_INT32
%token <word> SCAN_UINT32
%token <word> SCAN_FLOAT32
%token <word> SCAN_FLOAT64
%token <word> SCAN_STRING
%token <word> SCAN_URL 

%type <boolean> datasets dataset declarations array_decl

%type <word> declaration base_type structure sequence grid var var_name name

%%

start:
                {
		    /* On entry to the parser, make the BaseType stack. */
		    ctor = new stack<BaseType *>;
                }
                datasets
                {
		    delete ctor; ctor = 0;
		}
;

datasets:	dataset
		| datasets dataset
;

dataset:	SCAN_DATASET '{' declarations '}' name ';'
                {
		    $$ = $3 && $5;
		}
                | error
                {
		    parse_error((parser_arg *)arg, NO_DDS_MSG,
 				dds_line_num, $<word>1);
		    error_exit_cleanup();
		    YYABORT;
		}
;

declarations:	/* empty */
                {
		    $$ = true;
		}

                | declaration { $$ = true; }
                | declarations declaration { $$ = true; }
;

/* This non-terminal is here only to keep types like `List List Int32' from
   parsing. DODS does not allow Lists of Lists. Those types make translation
   to/from arrays too hard. */

declaration:  base_type var ';' 
                { 
		    string smsg;
		    if (current->check_semantics(smsg)) {
			/* BaseType *current_save = current ; */
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
			/* FIX
			if( current_save == current )
			{
			    delete current ;
			    current = 0 ;
			}
			*/
		    } else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $2);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy($$,$2,ID_MAX);
                    $$[ID_MAX-1] = '\0';
		}

		| structure  '{' declarations '}' 
		{ 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		} 
                var ';' 
                { 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $6);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy($$,$6,ID_MAX);
                    $$[ID_MAX-1] = '\0';
		}

		| sequence '{' declarations '}' 
                { 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		} 
                var ';' 
                { 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $6);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy($$,$6,ID_MAX);
                    $$[ID_MAX-1] = '\0';
		}

		| grid '{' SCAN_WORD ':'
		{ 
		    if (is_keyword(string($3), "array"))
			part = array; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, $3);
			YYABORT;
		    }
                }
                declaration SCAN_WORD ':'
		{ 
		    if (is_keyword(string($7), "maps"))
			part = maps; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, $7);
			YYABORT;
		    }
                }
                declarations '}' 
		{
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		}
                var ';' 
                {
		    string smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $13);
		      error_exit_cleanup();
		      YYABORT;
		    }
                    strncpy($$,$13,ID_MAX);
                    $$[ID_MAX-1] = '\0';
		}

                | error 
                {
		    ostringstream msg;
		    msg << BAD_DECLARATION;
		    parse_error((parser_arg *)arg, msg.str().c_str(),
				dds_line_num, $<word>1);
		    YYABORT;
		}
;
 

structure:	SCAN_STRUCTURE
		{ 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewStructure()); 
		}
;

sequence:	SCAN_SEQUENCE 
		{ 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewSequence()); 
		}
;

grid:		SCAN_GRID 
		{ 
		    ctor->push(DDS_OBJ(arg)->get_factory()->NewGrid()); 
		}
;

base_type:	SCAN_BYTE { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewByte(); }
		| SCAN_INT16 { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewInt16(); }
		| SCAN_UINT16 { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUInt16(); }
		| SCAN_INT32 { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewInt32(); }
		| SCAN_UINT32 { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUInt32(); }
		| SCAN_FLOAT32 { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewFloat32(); }
		| SCAN_FLOAT64 { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewFloat64(); }
		| SCAN_STRING { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewStr(); }
		| SCAN_URL { if( current ) delete current ;current = DDS_OBJ(arg)->get_factory()->NewUrl(); }
;

var:		var_name { current->set_name($1); }
 		| var array_decl
;

var_name:       SCAN_WORD | SCAN_BYTE | SCAN_INT16 | SCAN_INT32 | SCAN_UINT16
                | SCAN_UINT32 | SCAN_FLOAT32 | SCAN_FLOAT64 | SCAN_STRING
                | SCAN_URL | SCAN_STRUCTURE | SCAN_SEQUENCE | SCAN_GRID
                | SCAN_LIST
;

array_decl:	'[' SCAN_WORD ']'
                 { 
		     if (!check_int32($2)) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, $2);
		     }
		     if (current->type() == dods_array_c
			 && check_int32($2)) {
			 ((Array *)current)->append_dim(atoi($2));
		     }
		     else {
			 Array *a = DDS_OBJ(arg)->get_factory()->NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi($2));
			 if( current ) delete current ;
			 current = a;
		     }

		     $$ = true;
		 }

		 | '[' SCAN_WORD 
		 {
		     id = new string($2);
		 } 
                 '=' SCAN_WORD 
                 { 
		     if (!check_int32($5)) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, $5);
			 error_exit_cleanup();
			 YYABORT;
		     }
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi($5), *id);
		     }
		     else {
			 Array *a = DDS_OBJ(arg)->get_factory()->NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi($5), *id);
			 if( current ) delete current ;
			 current = a;
		     }

		     delete id; id = 0;
		 }
		 ']'
                 {
		     $$ = true;
		 }

		 | error
                 {
		     ostringstream msg;
		     msg << "In the dataset descriptor object:" << endl
			 << "Expected an array subscript." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(), 
				 dds_line_num, $<word>1);
		     YYABORT;
		 }
;

name:		var_name { (*DDS_OBJ(arg)).set_dataset_name($1); }
		| SCAN_DATASET { (*DDS_OBJ(arg)).set_dataset_name($1); }
                | error 
                {
		  ostringstream msg;
		  msg << "Error parsing the dataset name." << endl
		      << "The name may be missing or may contain an illegal character." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(),
				 dds_line_num, $<word>1);
		     YYABORT;
		}
;

%%

/* 
 This function must be defined. However, use the error reporting code in
 parser-utils.cc.
 */

void
ddserror(const string & /*char * */)
{
}

/*
 Error clean up. Call this before calling YYBORT. Don't call this on a
 normal exit.
 */

void error_exit_cleanup()
{
    delete id;
    id = 0;
    delete current;
    current = 0;
    delete ctor;
    ctor = 0;
}

/*
 Invalid declaration message.
 */

void invalid_declaration(parser_arg *arg, string semantic_err_msg, char *type, char *name)
{
    ostringstream msg;
    msg << "In the dataset descriptor object: `" << type << " " << name << "'" << endl << "is not a valid declaration."
            << endl << semantic_err_msg;
    parse_error((parser_arg *) arg, msg.str().c_str(), dds_line_num);
}

/*
 Add the variable pointed to by CURRENT to either the topmost ctor object on
 the stack CTOR or to the dataset variable table TABLE if CTOR is empty.  If
 it exists, the current ctor object is popped off the stack and assigned to
 CURRENT.

 NB: the ctor stack is popped for arrays because they are ctors which
 contain only a single variable. For other ctor types, several variables may
 be members and the parse rule (see `declaration' above) determines when to
 pop the stack.

 Returns: void 
 */

void add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, Part part)
{
    if (!*ctor)
        *ctor = new stack<BaseType *> ;

    if (!(*ctor)->empty()) { /* must be parsing a ctor type */
        (*ctor)->top()->add_var(*current, part);

        const Type &ctor_type = (*ctor)->top()->type();

        if (ctor_type == dods_array_c) {
            if (*current)
                delete *current;
            *current = (*ctor)->top();
            (*ctor)->pop();

            // Return here to avoid deleting the new value of 'current.'
            return;
        }
    }
    else {
        table.add_var(*current);
    }

    if (*current)
        delete *current;
    *current = 0;
}

