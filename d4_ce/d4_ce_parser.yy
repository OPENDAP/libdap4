
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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


// The d4ce_parser.tab.cc and .hh files define and declare this class
%define parser_class_name {D4CEParser}
// D4CEParser is in this namespace
%define api.namespace {libdap}

%define parse.trace
%define parse.error verbose
%define parse.assert

// Could not get this to work with a C++ scanner built by flex. 8/10/13 jhrg
// %define api.token.constructor
%define api.value.type variant

// Because the code uses the C++ mode of flex, we don't use this. 8/8/13 jhrg
// %define api.prefix { d4_ce }

%code requires {
namespace libdap {
    class D4CEDriver;
    class D4CEScanner;
}

}

// Pass both the scanner and parser objects to both the automatically generated
// parser and scanner.
%lex-param   { D4CEScanner  &scanner  }
%parse-param { D4CEScanner  &scanner  }

%lex-param   { D4CEDriver  &driver  }
%parse-param { D4CEDriver  &driver  }

%locations
%initial-action
{
    // Initialize the initial location. 'expression' is a field in D4CEDriver
    // and it is set by D4CEDriver::parse(const std::string &expr)
    // Normally this would be the name of a file...
    @$.begin.filename = @$.end.filename = &driver.expression;
};

%code {
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   /* include for all driver functions */
   #include "D4CEDriver.h"

   /* this is silly, but I can't figure out a way around */
   static int yylex(libdap::D4CEParser::semantic_type *yylval,
                    libdap::location *loc,
                    libdap::D4CEScanner  &scanner,
                    libdap::D4CEDriver   &driver);

}

// The strings used in the token definitions are used for error messages
%token <std::string> WORD "word"
%token <std::string> STRING "string"

%type <bool> predicate filter fields indexes index subset projection projections

%token 
    END  0  "end of file"
    
    SEMICOLON ";"
    PIPE "|"

    LBRACKET "["
    RBRACKET "]"
    COLON ":"

    LBRACE "{"
    RBRACE "}"

    COMMA ","

    ND "ND"
    ASSIGN "="

    LESS "<"
    GREATER ">"
    LESS_EQUAL "<="
    GREATER_EQUAL ">="
    EQUAL "=="
    NOT_EQUAL "!="
    REGEX_MATCH "~="

    LESS_BBOX "<<"
    GREATER_BBOX ">>"

    MASK "@="

    GROUP_SEP "/"
    PATH_SEP "."
;

%%

%start expression;

expression : projections { driver.set_result($1); }
;

projections : projection
           | projections ";" projection
;
                    
projection : subset
          | subset "|" filter
;

// FIXME push id's BaseType on a stack here. We'll need to access it 
// for indexes and filters. For 'fields indexes' maybe use a set
// so maybe push a set of BaseTypes?
subset : id { driver.dmr()->root()->find_var($1)->set_send_p(true); $$ = true; }
       | id indexes { $$ = true; }
       | id fields { $$ = true; }
       | id indexes fields { $$ = true; }
       | fields indexes { $$ = true; }
;
    
indexes : index
        | index indexes
;
   
index   : "[" "]" { $$ = true; }
        | "[" WORD "]" { $$ = true; }
        | "[" WORD ":" WORD "]" { $$ = true; }
        | "[" WORD ":" WORD ":" WORD "]" { $$ = true; }
        | "[" WORD ":" "]" { $$ = true; }
        | "[" WORD ":" WORD ":" "]" { $$ = true; }
;
        
fields : "{" projections "}" { $$ = true; }
;

filter : predicate 
       | filter "," predicate
;

// Here we use a grammar that is overly general: id op id is not really
// supported by the CE evaluator. However, id op constant, which captures
// the intent of the evaluator design introduces a number of reduce/reduce
// conflicts because any sensible definition of 'constant' will be the
// same as the definition of 'name'. This happens because we must make 'name'
// far more general than ideal (it must include tokens that start with digits
// odd characters that clash with the operators, et cetera). Note that the
// actions here must test for id == "ND" and op == "=", along with a host
// of other checks.

predicate : id op id { $$ = true; }
          | id op id op id { $$ = true; }
;

//           | "ND" "=" id { $$ = true; }

op : "<"
   | ">"
   | "<="
   | ">="
   | "=="
   | "!="
   | "~="

   | "<<"
   | ">>"

   | "@="
   
   | "="
;

id : path
   | group "/" path
;

group : "/" name
      | group "/" name
;

path : name 
     | path "." name
;

// Because some formats/datasets allow 'any' name for a variable, it's possible
// that a variable name will be a number, etc. The grammar also allows STRING
// to support "name"."name with spaces and dots (.)".x
name : WORD | STRING
;

%%

// Forward the error to the driver for handling. The location parameter
// provides the line number and character position of the error.
void
libdap::D4CEParser::error(const location_type &l, const std::string &m)
{
    driver.error(l, m);
}

/* include for access to scanner.yylex */
#include "D4CEScanner.h"

static int yylex(libdap::D4CEParser::semantic_type *yylval,
                 libdap::location *loc,
                 libdap::D4CEScanner &scanner,
                 libdap::D4CEDriver &driver)
{
    if (driver.trace_scanning())
        scanner.set_debug(true);
    
    return( scanner.yylex(yylval, loc) );
}
