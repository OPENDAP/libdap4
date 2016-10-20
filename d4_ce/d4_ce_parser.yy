
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

// A minor edit

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
#include "D4ConstraintEvaluator.h"
#include "escaping.h" // for www2id() used with WORD and STRING
namespace libdap {
    class D4CEScanner;
}

}

// Pass both the scanner and parser objects to both the automatically generated
// parser and scanner. Note that in the actions bound to the rules, 'driver' 
// means use the 'D4ConstraintEvaluator' instance.
%lex-param   { D4CEScanner  &scanner  }
%parse-param { D4CEScanner  &scanner  }

%lex-param   { D4ConstraintEvaluator  &driver  }
%parse-param { D4ConstraintEvaluator  &driver  }

%locations
%initial-action
{
    // Initialize the initial location. This is printed when the parser builds
    // its own error messages - when the parse fails as opposed to when the 
    // CE names a missing variables, ...

    @$.initialize (driver.expression());
};

%code {
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   #include "BaseType.h"
   #include "DMR.h"
   #include "D4Group.h"

   /* include for all driver functions */
   #include "D4ConstraintEvaluator.h"

   /* this is silly, but I can't figure out a way around it */
   static int yylex(libdap::D4CEParser::semantic_type *yylval,
                    libdap::location *loc,
                    libdap::D4CEScanner  &scanner,
                    libdap::D4ConstraintEvaluator   &driver);

}

// The strings used in the token definitions are used for error messages
%token <std::string> WORD "word"
%token <std::string> STRING "string"

// %type is used to set the return type of non-terminals; %token sets the
// return type for terminals.
%type <bool> filter predicate fields indexes subset clause clauses dimension dimensions
%type <std::string> id path group name op

%type <libdap::D4ConstraintEvaluator::index> index
// %type <libdap::D4FilterClause> predicate

%token
    END  0  "end of file"
  
%token 
    SEMICOLON ";"
    PIPE "|"

    LBRACKET "["
    RBRACKET "]"
    COLON ":"

    LBRACE "{"
    RBRACE "}"

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
    ND "ND"

    COMMA ","

    ASSIGN "="

    GROUP_SEP "/"
    PATH_SEP "."
    
%%

%start expression;

expression : clauses { driver.set_result($1); }
| dimensions ";" clauses { driver.set_result($1 && $3); }
;

dimensions : dimension { $$ = $1; }
| dimensions ";" dimension { $$ = $1 && $3; }
;

dimension : id "=" index
{
    $$ = driver.slice_dimension($1, $3);
}
;

clauses : clause { $$ = $1; }
| clauses ";" clause { $$ = $1 && $3; }
;
    
// Change: I moved the pop_basetype() call out of the 'fields'
// actions in 'subset' so that I could push the basetype for
// all of the cases. That way I'm sure to have a top_baseype()
// when processing the 'filter' part of the grammar. I need the
// the top basetype so that I know which Sequence to use.
// jhrg 4/23/16
               
clause : subset { $$ = $1; driver.pop_basetype(); }

// For the DAP4 at this time (3/18/15) filters apply only to D4Sequences 

| subset "|" filter { driver.pop_basetype(); $$ = $1 && $3; }
;

// mark_variable returns a BaseType* or throws Error
// Note that this is a fairly long production rule with a number
// of different right hand sides spanning about 110 lines.
// jhrg 4/8/16
subset : id 
{
    BaseType *btp = 0;
    if (driver.top_basetype()) {
        btp = driver.top_basetype()->var($1);
    }
    else {
        btp = driver.dmr()->root()->find_var($1);
    }
    
    if (!btp)
        driver.throw_not_found($1, "id");

    $$ = driver.mark_variable(btp);
    
    // push the basetype so that it is
    // accessible if/while filters are parsed
    driver.push_basetype(btp);
}

| id indexes 
{
    BaseType *btp = 0;
    if (driver.top_basetype()) {
        btp = driver.top_basetype()->var($1);
    }
    else {
        btp = driver.dmr()->root()->find_var($1);
    }
    
    if (!btp)
        driver.throw_not_found($1, "id indexes");
        
    if (btp->type() != dods_array_c)
        driver.throw_not_array($1, "id indexes");
        
    $$ = driver.mark_variable(btp);
    
    // push the basetype so that it is
    // accessible if/while filters are parsed
    driver.push_basetype(btp);
}

// Note this case is '| id fields'
| id 
{
    BaseType *btp = 0;
    if (driver.top_basetype()) {
        btp = driver.top_basetype()->var($1);
    }
    else {
        btp = driver.dmr()->root()->find_var($1);
    }

    if (!btp)
        driver.throw_not_found($1, "id fields");
    
    if (btp->type() == dods_array_c) {
        if (btp->var() && !btp->var()->is_constructor_type())
            throw Error(no_such_variable, "The variable " + $1 + " must be a Structure or Sequence to be used with {}.");
            
        // This call also tests the btp to make sure it's an array
        driver.mark_array_variable(btp);
    }
    else {
        // Don't mark the variable here because only some fields are to be sent and those
        // will be marked when the fields are parsed
        if (!btp->is_constructor_type())
            throw Error(no_such_variable, "The variable " + $1 + " must be a Structure or Sequence to be used with {}.");
    }
    
    // push the basetype so that it is
    // accessible when fields and if/while filters are parsed
    driver.push_basetype(btp);
} 
fields 
{ 
    //driver.pop_basetype(); 
    $$ = true; 
}

// Note this case is '| id indexes fields'

| id indexes
{
    BaseType *btp = 0;
    if (driver.top_basetype()) {
        btp = driver.top_basetype()->var($1);
    }
    else {
        btp = driver.dmr()->root()->find_var($1);
    }

    if (!btp)
        driver.throw_not_found($1, "id indexes fields");
    
    if (btp->type() != dods_array_c)
        driver.throw_not_array($1, "id indexes fields");

    // This call also tests the btp to make sure it's an array
    driver.mark_array_variable(btp);
    
    if (!btp->var()->is_constructor_type())
        throw Error(no_such_variable, "The variable " + $1 + " must be a Structure or Sequence to be used with {}.");
      
    driver.push_basetype(btp->var());       
} 
fields 
{ 
    //driver.pop_basetype();
    $$ = true; 
}

// The following has been removed from the syntax
// | fields indexes { $$ = true; }
;

// push_index stores the index in the D4ConstraintEvaluator
indexes : index 
{ 
    driver.push_index($1); 
    $$ = true; 
}
| index { driver.push_index($1); } indexes { $$ = $3; }
;

// Note that the index values are scanned as WORDs but the web escaping is
// not supported as it is for identifiers. jhrg 10/20/16
index   : "[" "]" { $$ = driver.make_index(); }
| "[" WORD "]" { $$ = driver.make_index($2); }
| "[" WORD ":" WORD "]" { $$ = driver.make_index($2, 1, $4); }
| "[" WORD ":" WORD ":" WORD "]" { $$ = driver.make_index($2, $4, $6); }
| "[" WORD ":" "]" { $$ = driver.make_index($2, 1); }
| "[" WORD ":" WORD ":" "]" { $$ = driver.make_index($2, $4); }
;
        
fields : "{" clauses "}" { $$ = $2; }
;

// A filter should return a FilterClauseList; a predicate should return a single
// FilterClause.

filter : predicate { $$ = true; }
| filter "," predicate { $$ = $1 && $3; }
;

// Here we use a grammar that is overly general: id op id is not really
// supported by the CE evaluator. However, id op constant, which captures
// the intent of the evaluator design introduces a number of reduce/reduce
// conflicts because any sensible definition of 'constant' will be the
// same as the definition of 'name'. This happens because we must make 'name'
// far more general than ideal (it must include tokens that start with digits,
// odd characters that clash with the operators, et cetera).

predicate : id op id
{ driver.add_filter_clause($2, $1, $3); $$ = true; }
          
| id op id op id 
{ 
    driver.add_filter_clause($2, $1, $3); 
    driver.add_filter_clause($4, $3, $5); 
    $$ = true; 

}
| "ND" "=" id { throw Error(malformed_expr, "The 'ND' operator is not currently supported."); }
;

// See http://docs.opendap.org/index.php/DAP4:_Constraint_Expressions,_v2
// for a discussion of filters that's quite a bit longer than the current
// draft spec. << and >> are the 'less than bbox' and '> bbox' operations
// that I'm not so sure about now; @= is the same as *= and is the mapping
// operation. jhrg 3/18/15 
op : "<" {$$ = "<";}
   | ">" {$$ = ">";}
   | "<=" {$$ = "<=";}
   | ">=" {$$ = ">=";}
   | "==" {$$ = "==";}
   | "!=" {$$ = "!=";}
   | "~=" {$$ = "~=";}

   | "<<" {$$ = "<<";}
   | ">>" {$$ = ">>";}

   | "@=" {$$ = "@=";}
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
    $1.append("/");
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
//
// I added calls here to remove the double quotes because they were breaking
// the parse for STRINGs and also added www2id() for WORDs (so that %20, etc.
// can be used for escaping stuff). However, the two cannot be mixed - if the
// parser id passed"Point%20Break" the %20 will remain as a literal in the STRING.
// jhrg 10/20/16
name : WORD 
{
    $$=www2id($1);
}
| STRING 
{
    $$=driver.remove_quotes($1);
}
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
                 libdap::D4ConstraintEvaluator &driver)
{
    if (driver.trace_scanning())
        scanner.set_debug(true);
    
    return( scanner.yylex(yylval, loc) );
}
