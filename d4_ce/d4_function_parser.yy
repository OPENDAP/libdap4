
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
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

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "2.5"
%defines


// The d4_function_parser.tab.cc and .hh files define and declare this class
%define parser_class_name {D4FunctionParser}
// D4FunctionParser is in this namespace
%define api.namespace {libdap}

%define parse.trace
%define parse.error verbose
%define parse.assert

// Could not get this to work with a C++ scanner built by flex. 8/10/13 jhrg
// %define api.token.constructor
%define api.value.type variant

// Because the code uses the C++ mode of flex, we don't use this. 8/8/13 jhrg
// %define api.prefix { d4_function_ }

%code requires {

#include "D4FunctionDriver.h"
namespace libdap {
    class D4FunctionScanner;
}

}

// Pass both the scanner and parser objects to both the automatically generated
// parser and scanner.
%lex-param   { D4FunctionScanner  &scanner  }
%parse-param { D4FunctionScanner  &scanner  }

%lex-param   { D4FunctionDriver  &driver  }
%parse-param { D4FunctionDriver  &driver  }

%locations
%initial-action
{
    // Initialize the initial location. This is printed when the parser builds
    // its own error messages - when the parse fails as opposed to when the 
    // function(s) name(s) a missing variables, ...

    @$.initialize (driver.expression());
};

%code {
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   #include "BaseType.h"
   #include "DMR.h"

   /* include for all driver functions */
   #include "D4FunctionsDriver.h"

   /* this is silly, but I can't figure out a way around */
   static int yylex(libdap::D4FunctionParser::semantic_type *yylval,
                    libdap::location *loc,
                    libdap::D4FunctionScanner  &scanner,
                    libdap::D4FunctionDriver   &driver);

}

%type <RValueList*> functions "functions"
%type <RValueList*> args "arguments"

%type <RValue*> function "function"
%type <RValue*> arg "argument"

%type <D4Function> fname "function name"
%type <RValue*> variable_or_constant "variable or constant"

%type <std::string> id path group name

// The strings used in the token definitions are used for error messages
%token <std::string> WORD "word"
%token <std::string> STRING "string"

%token 
    END  0  "end of file"
    
    SEMICOLON ";"

    LPAREN "("
    RPAREN ")"
    
    COMMA ","

    GROUP_SEP "/"
    PATH_SEP "."
;

%%

%start program;

program : functions { driver.set_result($1); }
;

functions : function { } // build a program object (holds a list of functions)
| functions ";" function { } // ditto
;
                    
function : fname "(" args ")" { $$ = new D4RValue($1, $3); } // Build a RValue: function pointer and RValuesList
;

fname: WORD { } // lookup the name and return a pointer
;

args: arg { } // build a RValueList from the RValue
| args "," arg { } // Append the RValue ($3) to the RValueList ($1), then return
;

arg: function { } // Build a RValue
| variable_or_constant { }
// | array_constant { } // FIXME
;

variable_or_constant : id
{
    $$ = driver.build_rvalue($1)
}
;
        
id : path
{
    $$ = $1;
}
| "/" path
{
    $$.append("/");
    $$.append($2);
}
| group "/" path
{
    $1.append("/");
    $1.append($3);
    $$ = $1;
}
;

group : "/" name
{
    $$.append("/");
    $$.append($2);
}
| group "/" name
{
    $1.append(".");
    $1.append($3);
    $$ = $1;
}
;

path : name 
{
    $$ = $1;
}
| path "." name
{
    $1.append(".");
    $1.append($3);
    $$ = $1;
}
;

// Because some formats/datasets allow 'any' name for a variable, it's possible
// that a variable name will be a number, etc. The grammar also allows STRING
// to support "name"."name with spaces and dots (.)".x
name : WORD 
{
    $$=$1;
}
| STRING 
{
    $$=$1;
}
;

%%

// Forward the error to the driver for handling. The location parameter
// provides the line number and character position of the error.
void
libdap::D4CFunctionParser::error(const location_type &l, const std::string &m)
{
    driver.error(l, m);
}

/* include for access to scanner.yylex */
#include "D4FunctionScanner.h"

static int yylex(libdap::D4FunctionParser::semantic_type *yylval,
                 libdap::location *loc,
                 libdap::D4FunctionScanner &scanner,
                 libdap::D4FunctionDriver &driver)
{
    if (driver.trace_scanning())
        scanner.set_debug(true);
    
    return( scanner.yylex(yylval, loc) );
}
