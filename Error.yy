
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
 
// (c) COPYRIGHT URI/MIT 1997,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

%code requires {

#include "config_dap.h"

#include <iostream>

#include "Error.h"

#include "parser.h"
#include "debug.h"
#include "util.h"

using namespace std;
using namespace libdap;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define ERROR_OBJ(arg) ((Error *)((parser_arg *)(arg))->_object)
#define STATUS(arg) ((parser_arg *)(arg))->_status

//#define YYPARSE_PARAM arg

extern int error_line_num;	// defined in Error.lex

}

%code {

int Errorlex();			// the scanner
void Errorerror(parser_arg *arg, const string &s);	// gotta love automatically generated names...

}

%require "2.4"
%parse-param {parser_arg *arg}
%name-prefix "Error"
%defines
%debug
%verbose

%union {
#ifdef __SUNPRO_CC
    int boolean;
#else
    bool boolean;
#endif
    int integer;
    char *string;
}

%token <integer>	SCAN_INT
%token <string>		SCAN_STR

%token <integer>	SCAN_ERROR
%token <integer>	SCAN_CODE
%token <string>		SCAN_MSG

%type <boolean> error_object contents description code message

%%

// The parser is called through a function named ERRORPARSE which takes a
// pointer to a structure and returns a boolean. The structure contains a
// pointer to an Error object which is empty and an integer which contains
// status information. In addition the parser_arg structure contains a
// pointer to an error object. However, the `error' member of parser_arg is
// not yet used here.

error_object:	SCAN_ERROR '{' contents '}' ';' { $$ = $3; STATUS(arg) = $3; }
;

contents:	description { $$ = $1; }
;

description:	code message { $$ = $1 && $2; }
                | code { $$ = $1; }
;

code:		SCAN_CODE '=' SCAN_INT ';' 
		{ 
		    ERROR_OBJ(arg)->set_error_code((ErrorCode)$3);
		    $$ = true; 
		}
;

message:	SCAN_MSG '=' SCAN_STR 
		{ 
		    ERROR_OBJ(arg)->set_error_message($3);
		} 
		';' 
                {
		    $$ = true;
		}
;

%%

void
Errorerror(parser_arg *, const string &s)
{
  string msg = s;
  msg += " line: ";
  append_long_to_string(error_line_num, 10, msg);
  msg += "\n";

  throw Error(unknown_error, msg);
}

