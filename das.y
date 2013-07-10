
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
   Grammar for the DAS. This grammar can be used with the bison parser
   generator to build a parser for the DAS. It assumes that a scanner called
   `daslex()' exists and that the objects DAS and AttrTable also exist.

   jhrg 7/12/94 
*/

%{

#define YYSTYPE char *
#define ATTR_STRING_QUOTE_FIX

#include "config.h"

#include <string>

#include <vector>

#include "DAS.h"
#include "Error.h"
#include "util.h"
#include "escaping.h"
#include "debug.h"
#include "parser.h"
#include "util.h"
#include "das.tab.hh"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#define yylex daslex
#define yyerror daserror 

using namespace std;
using namespace libdap ;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 
// The parser now throws an exception when it encounters an error. 5/23/2002
// jhrg 

#define DAS_OBJ(arg) ((DAS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

extern int das_line_num;	/* defined in das.lex */

// No global static objects. We go through this every so often, I guess I
// should learn... 1/24/2000 jhrg
static string *name;	/* holds name in attr_pair rule */
static string *type;	/* holds type in attr_pair rule */

static vector<AttrTable *> *attr_tab_stack;

// I use a vector of AttrTable pointers for a stack

#define TOP_OF_STACK (attr_tab_stack->back())
#define PUSH(x) (attr_tab_stack->push_back((x)))
#define POP (attr_tab_stack->pop_back())
#define STACK_LENGTH (attr_tab_stack->size())
#define OUTER_TABLE_ONLY (attr_tab_stack->size() == 1)
#define STACK_EMPTY (attr_tab_stack->empty())

#define TYPE_NAME_VALUE(x) *type << " " << *name << " " << (x)

static const char *ATTR_TUPLE_MSG = 
"Expected an attribute type (Byte, Int16, UInt16, Int32, UInt32, Float32,\n\
Float64, String or Url) followed by a name and value.";
static const char *NO_DAS_MSG =
"The attribute object returned from the dataset was null\n\
Check that the URL is correct.";

typedef int checker(const char *);

int daslex(void);
static void daserror(const string &s /*char *s*/);
static void add_attribute(const string &type, const string &name, 
			  const string &value, checker *chk) throw (Error);
static void add_alias(AttrTable *das, AttrTable *current, const string &name, 
		      const string &src) throw (Error);
static void add_bad_attribute(AttrTable *attr, const string &type,
			      const string &name, const string &value,
			      const string &msg);

%}

%expect 26

%token SCAN_ATTR

%token SCAN_WORD

%token SCAN_ALIAS

%token SCAN_BYTE
%token SCAN_INT16
%token SCAN_UINT16
%token SCAN_INT32
%token SCAN_UINT32
%token SCAN_FLOAT32
%token SCAN_FLOAT64
%token SCAN_STRING
%token SCAN_URL
%token SCAN_XML

%%

/*
  Parser algorithm: 

  Look for a `variable' name (this can be any identifier, but by convention
  it is either the name of a variable in a dataset or the name of a grouping
  of global attributes). Create a new attribute table for this identifier and
  push the new attribute table onto a stack. If attribute tuples
  (type-name-value tuples) are found, intern them in the attribute table
  found on the top of the stack. If the start of a new attribute table if
  found (before the current table is closed), create the new table and push
  *it* on the stack. As attribute tables are closed, pop them off the stack.
  This algorithm ensures that we can nest attribute tables to an arbitrary
  depth.

  Aliases are handled using mfuncs of both the DAS and AttrTable objects. This
  is necessary because the first level of a DAS object can contain only
  AttrTables, not attribute tuples. Whereas, the subsequent levels can
  contain both. Thus the compete definition is split into two objects. In
  part this is also a hold over from an older design which did not
  have the recursive properties of the current design.

  Aliases can be made between attributes within a given lexical level, from
  one level to the next within a sub-hierarchy or across hierarchies.

  Tokens:

  BYTE, INT32, UINT32, FLOAT64, STRING and URL are tokens for the type
  keywords. The tokens INT, FLOAT, STR and ID are returned by the scanner to
  indicate the type of the value represented by the string contained in the
  global DASLVAL. These two types of tokens are used to implement type
  checking for the attributes. See the rules `bytes', etc. Additional tokens:
  ATTR (indicates the start of an attribute object) and ALIAS (indicates an
  alias). */

/* This rule makes sure the objects needed by this parser are built. Because
   the DODS DAP library is often used with linkers that are not C++-aware, we
   cannot use global objects (because their constructors might never be
   called). I had thought this was going to go away... 1/24/2000 jhrg */

attr_start:
	{
		name = new string();
		type = new string();
		attr_tab_stack = new vector<AttrTable *>;

		// push outermost AttrTable
		PUSH(DAS_OBJ(arg)->get_top_level_attributes());
	}
        attributes
        {
		POP;	// pop the DAS/AttrTable before stack's dtor
		delete name;
		delete type;
		delete attr_tab_stack;
	}
;

attributes:     attribute
    	    	| attributes attribute

;
    	    	
attribute:    	SCAN_ATTR '{' attr_list '}'
                | error
                {
		    parse_error((parser_arg *)arg, NO_DAS_MSG, das_line_num);
		}
;

attr_list:  	/* empty */
    	    	| attr_tuple
    	    	| attr_list attr_tuple
;

attr_tuple:	alias

                | SCAN_BYTE { save_str(*type, "Byte", das_line_num); }
                name { save_str(*name, $3, das_line_num); } 
		bytes ';'

		| SCAN_INT16 { save_str(*type, "Int16", das_line_num); } 
                name { save_str(*name, $3, das_line_num); } 
		int16 ';'

		| SCAN_UINT16 { save_str(*type, "UInt16", das_line_num); } 
                name { save_str(*name, $3, das_line_num); } 
		uint16 ';'

		| SCAN_INT32 { save_str(*type, "Int32", das_line_num); } 
                name { save_str(*name, $3, das_line_num); } 
		int32 ';'

		| SCAN_UINT32 { save_str(*type, "UInt32", das_line_num); } 
                name { save_str(*name, $3, das_line_num); } 
		uint32 ';'

		| SCAN_FLOAT32 { save_str(*type, "Float32", das_line_num); } 
                name { save_str(*name, $3, das_line_num); } 
		float32 ';'

		| SCAN_FLOAT64 { save_str(*type, "Float64", das_line_num); } 
                name { save_str(*name, $3, das_line_num); } 
		float64 ';'

		| SCAN_STRING { *type = "String"; } 
                name { *name = $3; } 
		strs ';'

                | SCAN_URL { *type = "Url"; } 
                name { *name = $3; } 
                urls ';'

                | SCAN_XML { *type = "OtherXML"; } 
                name { *name = $3; } 
                xml ';'

		| SCAN_WORD
                {
		    DBG(cerr << "Processing ID: " << $1 << endl);
		    
		    AttrTable *at = TOP_OF_STACK->get_attr_table($1);
		    if (!at) {
			try {
			    at = TOP_OF_STACK->append_container($1);
			}
			catch (Error &e) {
			    // re-throw with line number info
			    parse_error(e.get_error_message().c_str(), 
					das_line_num);
			}
		    }
		    PUSH(at);

		    DBG(cerr << " Pushed attr_tab: " << at << endl);

		}
		'{' attr_list 
                {
		    /* pop top of stack; store in attr_tab */
		    DBG(cerr << " Popped attr_tab: " << TOP_OF_STACK << endl);
		    POP;
		}
		'}'

		| error 
                { 
		    parse_error(ATTR_TUPLE_MSG, das_line_num, $1);
		} ';'
;

bytes:		SCAN_WORD
		{
		    add_attribute(*type, *name, $1, &check_byte);
		}
		| bytes ',' SCAN_WORD
		{
		    add_attribute(*type, *name, $3, &check_byte);
		}
;

int16:		SCAN_WORD
		{
		    add_attribute(*type, *name, $1, &check_int16);
		}
		| int16 ',' SCAN_WORD
		{
		    add_attribute(*type, *name, $3, &check_int16);
		}
;

uint16:		SCAN_WORD
		{
		    add_attribute(*type, *name, $1, &check_uint16);
		}
		| uint16 ',' SCAN_WORD
		{
		    add_attribute(*type, *name, $3, &check_uint16);
		}
;

int32:		SCAN_WORD
		{
		    add_attribute(*type, *name, $1, &check_int32);
		}
		| int32 ',' SCAN_WORD
		{
		    add_attribute(*type, *name, $3, &check_int32);
		}
;

uint32:		SCAN_WORD
		{
		    add_attribute(*type, *name, $1, &check_uint32);
		}
		| uint32 ',' SCAN_WORD
		{
		    add_attribute(*type, *name, $3, &check_uint32);
		}
;

float32:	float_or_int
		{
		    add_attribute(*type, *name, $1, &check_float32);
		}
		| float32 ',' float_or_int
		{
		    add_attribute(*type, *name, $3, &check_float32);
		}
;

float64:	float_or_int
		{
		    add_attribute(*type, *name, $1, &check_float64);
		}
		| float64 ',' float_or_int
		{
		    add_attribute(*type, *name, $3, &check_float64);
		}
;

strs:		str_or_id
		{
		    string attr = remove_quotes($1);
		    add_attribute(*type, *name, attr, 0);
		}
		| strs ',' str_or_id
		{
		    string attr = remove_quotes($3);
		    add_attribute(*type, *name, attr, 0);
		}
;

urls:           url
                {
                    add_attribute(*type, *name, $1, &check_url);
                }
                | urls ',' url
                {
                    add_attribute(*type, *name, $3, &check_url);
                }
;

xml:            SCAN_WORD
                {
                    // XML must be quoted in the DAS but the quotes are an
                    // artifact of the DAS syntax so they are not part of the
                    // value.
                    cerr << "Attr value as read: " << $1 << endl;
                    string xml = unescape_double_quotes($1);
                    cerr << "w/o quotes: " << remove_quotes(xml) << endl;
                    
                    if (is_quoted(xml))
                        add_attribute(*type, *name, remove_quotes(xml), 0);
                    else
                        add_attribute(*type, *name, xml, 0);
                }
;

url:		SCAN_WORD
;

str_or_id:	SCAN_WORD
;

float_or_int:   SCAN_WORD
;

name:           SCAN_WORD | SCAN_ATTR | SCAN_ALIAS | SCAN_BYTE | SCAN_INT16 
                | SCAN_UINT16 | SCAN_INT32 | SCAN_UINT32 | SCAN_FLOAT32 
                | SCAN_FLOAT64 | SCAN_STRING | SCAN_URL | SCAN_XML
;

alias:          SCAN_ALIAS SCAN_WORD
                { 
		    *name = $2;
		} 
                SCAN_WORD
                {
		    add_alias( DAS_OBJ(arg)->get_top_level_attributes(),
		               TOP_OF_STACK, *name, string($4) ) ;
                }
                ';'
;

%%

// This function is required for linking, but DODS uses its own error
// reporting mechanism.

static void
daserror(const string &/*char */)
{
}

static string
a_or_an(const string &subject)
{
    string first_char(1, subject[0]);
    string::size_type pos = first_char.find_first_of("aeiouAEIOUyY");
    
    if (pos == string::npos)
	return "a";
    else
	return "an";
}

// This code used to throw an exception when a bad attribute value came
// along; now it dumps the errant value(s) into a sub container called *_DODS
// and stores the parser's error message in a string attribute named
// `explanation.' 
static void
add_attribute(const string &type, const string &name, const string &value,
	      checker *chk) throw (Error)
{
    DBG(cerr << "Adding: " << type << " " << name << " " << value \
	<< " to Attrtable: " << TOP_OF_STACK << endl);

    if (chk && !(*chk)(value.c_str())) {
	string msg = "`";
	msg += value + "' is not " + a_or_an(type) + " " + type + " value.";
	add_bad_attribute(TOP_OF_STACK, type, name, value, msg);
	return;
    }
    
    if (STACK_EMPTY) {
	string msg = "Whoa! Attribute table stack empty when adding `" ;
	msg += name + ".' ";
	parse_error(msg, das_line_num);
    }
    
    try {
#if 0
        // Special treatment for XML: remove the double quotes that were 
        // included in the value by this parser.
        if (type == OtherXML && is_quoted(value))
            TOP_OF_STACK->append_attr(name, type, value.substr(1, value.size()-2));
        else    
#endif
	    TOP_OF_STACK->append_attr(name, type, value);
    }
    catch (Error &e) {
	// re-throw with line number
	parse_error(e.get_error_message().c_str(), das_line_num);
    }
}

static void
add_alias(AttrTable *das, AttrTable *current, const string &name, 
	  const string &src) throw (Error)
{
    DBG(cerr << "Adding an alias: " << name << ": " << src << endl);

    AttrTable *table = das->get_attr_table(src);
    if (table) {
	try {
	    current->add_container_alias(name, table);
	}
	catch (Error &e) {
	    parse_error(e.get_error_message().c_str(), das_line_num);
	}
    }
    else {
	try {
	    current->add_value_alias(das, name, src);
	}
	catch (Error &e) {
	    parse_error(e.get_error_message().c_str(), das_line_num);
	}
    }
}

static void
add_bad_attribute(AttrTable *attr, const string &type, const string &name,
		  const string &value, const string &msg)
{
    // First, if this bad value is already in a *_dods_errors container,
    // then just add it. This can happen when the server side processes a DAS
    // and then hands it off to a client which does the same.
    // Make a new container. Call it <attr's name>_errors. If that container
    // already exists, use it.
    // Add the attribute.
    // Add the error string to an attribute in the container called
    // `<name_explanation.'. 
    
    if (attr->get_name().find("_dods_errors") != string::npos) {
	attr->append_attr(name, type, value);
    }
    else {
	string error_cont_name = attr->get_name() + "_dods_errors";
	AttrTable *error_cont = attr->get_attr_table(error_cont_name);
	if (!error_cont)
	    error_cont = attr->append_container(error_cont_name);

	error_cont->append_attr(name, type, value);
#ifndef ATTR_STRING_QUOTE_FIX
    error_cont->append_attr(name + "_explanation", "String",
                "\"" + msg + "\"");
#else
       error_cont->append_attr(name + "_explanation", "String", msg);
#endif
    }
}

