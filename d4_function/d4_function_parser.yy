
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

#include "D4FunctionEvaluator.h"
#include "D4RValue.h"
#include "dods-datatypes.h"

namespace libdap {
    class D4FunctionScanner;
}

}

// Pass both the scanner and parser objects to both the automatically generated
// parser and scanner.
%lex-param   { D4FunctionScanner  &scanner  }
%parse-param { D4FunctionScanner  &scanner  }

%lex-param   { D4FunctionEvaluator  &evaluator  }
%parse-param { D4FunctionEvaluator  &evaluator  }

%locations
%initial-action
{
    // Initialize the initial location. This is printed when the parser builds
    // its own error messages - when the parse fails as opposed to when the 
    // function(s) name(s) a missing variable, ...

    @$.initialize (evaluator.expression());
};

%code {
    #include "BaseType.h"
    #include "DMR.h"
    #include "D4RValue.h"
    #include "ServerFunctionsList.h"
   
    #include "parser-util.h"

    /* include for all driver functions */
    #include "D4FunctionEvaluator.h"

    using namespace libdap ;
    
    /* this is silly, but I can't figure out a way around it */
    static int yylex(libdap::D4FunctionParser::semantic_type *yylval,
                     libdap::location *loc,
                     libdap::D4FunctionScanner  &scanner,
                     libdap::D4FunctionEvaluator   &evaluator);
}

%type <D4RValueList*> functions "functions"
%type <D4RValueList*> args "arguments"

%type <D4RValue*> arg "argument"
%type <D4RValue*> function "function"

%type <D4Function> fname "function name"
%type <D4RValue*> variable_or_constant "variable or constant"
%type <D4RValue*> array_constant "array constant"

%type <std::vector<dods_byte>*> fast_byte_arg_list "fast byte arg list"
%type <std::vector<dods_int8>*> fast_int8_arg_list "fast int8 arg list"
%type <std::vector<dods_uint16>*> fast_uint16_arg_list "fast uint16 arg list"
%type <std::vector<dods_int16>*> fast_int16_arg_list "fast int16 arg list"
%type <std::vector<dods_uint32>*> fast_uint32_arg_list "fast uint32 arg list"
%type <std::vector<dods_int32>*> fast_int32_arg_list "fast int32 arg list"
%type <std::vector<dods_uint64>*> fast_uint64_arg_list "fast uint64 arg list"
%type <std::vector<dods_int64>*> fast_int64_arg_list "fast int64 arg list"
%type <std::vector<dods_float32>*> fast_float32_arg_list "fast float32 arg list"
%type <std::vector<dods_float64>*> fast_float64_arg_list "fast float64 arg list"

%type <std::string> id path group name

// The strings used in the token definitions are used for error messages
%token <std::string> WORD "word"
%token <std::string> STRING "string"

%token 
    END  0  "end of file"
    
    SEMICOLON ";"
    COLON ":"

    LPAREN "("
    RPAREN ")"
    
    COMMA ","

    GROUP_SEP "/"
    PATH_SEP "."
    
    DOLLAR_BYTE "$Byte"
    DOLLAR_UINT8 "$UInt8"
    DOLLAR_INT8 "$Int8"
    DOLLAR_UINT16 "$UInt16"
    DOLLAR_INT16 "$Int16"
    DOLLAR_UINT32 "$UInt32"
    DOLLAR_INT32 "$Int32"
    DOLLAR_UINT64 "$UInt64"
    DOLLAR_INT64 "$Int64"
    DOLLAR_FLOAT32 "$Float32"
    DOLLAR_FLOAT64 "$Float64"
;

%%

%start program;

program : functions 
{ 
    evaluator.set_result($1); 
}
;

functions : function 
{
    $$ = new D4RValueList($1); 
}
| functions ";" function 
{ 
    $1->add_rvalue($3);
    $$ = $1; 
}
;
                    
function : fname "(" args ")" 
{ 
    $$ = new D4RValue($1, $3); // Build a D4RValue from a D4Function pointer and a D4RValueList 
} 
;

fname: WORD 
{ 
    D4Function f;
    if (!evaluator.sf_list()->find_function($1, f)) {
        // ...cloud use @1.{first,last}_column in these error messages.
        throw Error(malformed_expr, "'" + $1 + "' is not a registered DAP4 server function.");
    }

    $$ = f;
}        
;

args: arg
{ 
    $$ = new D4RValueList($1); // build a D4RValueList from the D4RValue
} 
| args "," arg 
{ 
    $1->add_rvalue($3);
    $$ = $1; // Append the D4RValue ($3) to the D4RValueList ($1), then return
} 
;

arg: function
{
    $$ = $1;
}
| variable_or_constant
{
    $$ = $1;
}
| array_constant 
{
    $$ = $1;
}
;

variable_or_constant : id
{
    D4RValue *rvalue = evaluator.build_rvalue($1);
    if (!rvalue) {
        throw Error(malformed_expr, "'" + $1 + "' is not a variable, number or string.");
    }
    
    $$ = rvalue;
}
;
  
array_constant : 
DOLLAR_BYTE "(" arg_length_hint ":" fast_byte_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_UINT8 "(" arg_length_hint ":" fast_byte_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_INT8 "(" arg_length_hint ":" fast_int8_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_UINT16 "(" arg_length_hint ":" fast_uint16_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_INT16 "(" arg_length_hint ":" fast_int16_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_UINT32 "(" arg_length_hint ":" fast_uint32_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_INT32 "(" arg_length_hint ":" fast_int32_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_UINT64 "(" arg_length_hint ":" fast_uint64_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_INT64 "(" arg_length_hint ":" fast_int64_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_FLOAT32 "(" arg_length_hint ":" fast_float32_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
|
DOLLAR_FLOAT64 "(" arg_length_hint ":" fast_float64_arg_list ")"
{
    $$ = new D4RValue(*($5));
    delete $5;
}
;

/* Here the arg length hint is stored in the eval class so it can be used by the 
   method that allocates the vector. The value is passed to vector::reserve().
   This rule is run for it's side-effect only. This is also used to track the 
   current arg number so that the hint can be used. */
   
arg_length_hint : WORD
{
    evaluator.set_arg_length_hint(get_ull($1.c_str()));
}
;

fast_byte_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_byte(strtol($1.c_str(), 0, 0)));
}
| fast_byte_arg_list "," WORD
{
    $1->push_back(strtol($3.c_str(), 0, 0));
    $$ = $1;
}
;

fast_int8_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_int8(strtol($1.c_str(), 0, 0)));
}
| fast_int8_arg_list "," WORD
{
    $1->push_back(strtol($3.c_str(), 0, 0));
    $$ = $1;
}
;

fast_uint16_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_uint16(strtol($1.c_str(), 0, 0)));
}
| fast_uint16_arg_list "," WORD
{
    $1->push_back(strtol($3.c_str(), 0, 0));
    $$ = $1;
}
;

fast_int16_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_int16(strtol($1.c_str(), 0, 0)));
}
| fast_int16_arg_list "," WORD
{
    $1->push_back(strtol($3.c_str(), 0, 0));
    $$ = $1;
}
;

fast_uint32_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_uint32(strtoul($1.c_str(), 0, 0)));
}
| fast_uint32_arg_list "," WORD
{
    $1->push_back(strtoul($3.c_str(), 0, 0));
    $$ = $1;
}
;
fast_int32_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_int32(strtol($1.c_str(), 0, 0)));
}
| fast_int32_arg_list "," WORD
{
    $1->push_back(strtol($3.c_str(), 0, 0));
    $$ = $1;
}
;

fast_uint64_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_uint64(strtoull($1.c_str(), 0, 0)));
}
| fast_uint64_arg_list "," WORD
{
    $1->push_back(strtoull($3.c_str(), 0, 0));
    $$ = $1;
}
;

fast_int64_arg_list: WORD
{
    $$ = evaluator.init_arg_list(dods_int64(strtoll($1.c_str(), 0, 0)));
}
| fast_int64_arg_list "," WORD
{
    $1->push_back(strtoll($3.c_str(), 0, 0));
    $$ = $1;
}
;

// I'm using path for the $Float constants so that tokens with dots will 
// parse. I could add a FLOAT token, and I might have to do that later on
// when filters are added to the CE, but this will work for now.
fast_float32_arg_list: path
{
    $$ = evaluator.init_arg_list(dods_float32(strtof($1.c_str(), 0)));
}
| fast_float32_arg_list "," path
{
    $1->push_back(strtof($3.c_str(), 0));
    $$ = $1;
}
;

fast_float64_arg_list: path
{
    $$ = evaluator.init_arg_list(dods_float64(strtod($1.c_str(), 0)));
}
| fast_float64_arg_list "," path
{
    $1->push_back(strtod($3.c_str(), 0));
    $$ = $1;
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
libdap::D4FunctionParser::error(const location_type &l, const std::string &m)
{
    evaluator.error(l, m);
}

/* include for access to scanner.yylex */
#include "D4FunctionScanner.h"

static int yylex(libdap::D4FunctionParser::semantic_type *yylval,
                 libdap::location *loc,
                 libdap::D4FunctionScanner &scanner,
                 libdap::D4FunctionEvaluator &evaluator)
{
    if (evaluator.trace_scanning())
        scanner.set_debug(true);
    
    return( scanner.yylex(yylval, loc) );
}
