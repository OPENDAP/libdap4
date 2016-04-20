
/*
 -*- mode: c++; c-basic-offset:4 -*-

 This file is part of libdap, A C++ implementation of the OPeNDAP Data
 Access Protocol.

 Copyright (c) 2014 OPeNDAP, Inc.
 Author: James Gallagher <jgallagher@opendap.org>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
*/ 

%{ /* -*- C++ -*- */

//#include "config.h"

#include <string>
#include "Error.h"

#include "D4FunctionScanner.h"

/* typedef to make the returns for the tokens shorter */

/* NB: It would be best to use the same scanner (and maybe parser) for
   both the D4 CE and Function parameters, but for the initial version 
   far less complexity is require by the Function expression scanner
   (since the initial version will just support variables, constants, 
   functions and the $<type> array special form) and not function arguments
   that are general expressions (like array slicing and/or filters).
   
   This comment is here because this is the first place where there is 
   coupling between the CE parser and its scanner. I'm not sure, however,
   if one string can be parsed by two parsers if they are using two scanners,
   so extending the Function parser to allow function args to be any CE 
   clause may mean some more serious work with the parsers.
   
   jhrg 3/10/14 */
typedef libdap::D4FunctionParser::token token;

/* This was added because of some notes on the net about compiler version
   issues. I don't know if it's needed when using the C++ mode of flex. */
#undef yywrap
#define yywrap() 1

/* define yyterminate as this instead of NULL */
#define yyterminate() return(token::END)

#define YY_FATAL_ERROR(msg) {\
    throw(libdap::Error(malformed_expr, std::string("Error scanning function expression text: ") + std::string(msg))); \
}

%}

%option c++
%option yyclass="D4FunctionScanner"

/* Use this if several scanners are needed. This will cause flex to
   #define yyFlexLexer to be <prefix>FlexLexer (the yyFlexLexer is defined
   in lex.<prefix>.cc. jhrg 8/8/13 */
%option prefix="d4_function"

/* These two options turn on line counting - useful for error messages - 
   and debugging, respectively. When debugging is on, it's possible to see
   which scanner rules are used at which points in the input. */
%option yylineno
%option debug

/* Do not output the default rule (where any unmatched input is echoed to 
   stdout). When set, nodefault will cause the scanner to exit on an error. */
%option nodefault
/* noyywrap makes the scanner assume that EOF/EOS is the end of the input.
   If this is not set, the scanner will assume there are more files to 
   scan. */ 
%option noyywrap
%option nounput
/* When set, warn prints a message when the default rule can be matched
   but nodefault is given (among other warnings). */
%option warn

%option batch

%x quote

/* This pattern is slightly different from the one used by the CE scanner
   because it allows a WORD to start with a '#' so that the #<type> 
   array constant syntax can be used in functions. Otherwise, a WORD must
   be able to contain this hideous mix of characters because a variable 
   can. jhrg 3/10/14 */
WORD    [-+a-zA-Z0-9_%*\\~@!#][-+a-zA-Z0-9_%*\\~@!#]* 

/* I added these tokens because floating point values may contain dots and
   added a '.' to WORD will break the parsing of paths (or make for some 
   fairly obscure code - where $Float32() takes tokens that match 'path'.
   Since we have a separate scanner for the function expressions, might as
   well add a FLOAT token... jhg 3/17/14 
FLOAT   [-+eE.0-9][-+eE.0-9]*
*/

%{
// Code run each time a pattern is matched
#define YY_USER_ACTION loc->columns(yyleng);
%}

%%

%{
// Code run each time yylex is called
loc->step();
%}

","		return token::COMMA;
";"		return token::SEMICOLON;
":"     return token::COLON;

"("     return token::LPAREN;
")"     return token::RPAREN;

"/"     return token::GROUP_SEP;
"."     return token::PATH_SEP;

"$Byte" return token::DOLLAR_BYTE;
"$UInt8" return token::DOLLAR_UINT8;
"$Int8" return token::DOLLAR_INT8;
"$UInt16" return token::DOLLAR_UINT16;
"$Int16" return token::DOLLAR_INT16;
"$UInt32" return token::DOLLAR_UINT32;
"$Int32" return token::DOLLAR_INT32;
"$UInt64" return token::DOLLAR_UINT64;
"$Int64" return token::DOLLAR_INT64;
"$Float32" return token::DOLLAR_FLOAT32;
"$Float64" return token::DOLLAR_FLOAT64;

[ \t]+  /* ignore these */

[\r\n]+ /* ignore these */

{WORD}  { yylval->build<std::string>(yytext); return token::WORD; }

<INITIAL><<EOF>> return token::END;

["]    { BEGIN(quote); yymore(); }

<quote>[^"\\]*  yymore(); /* Anything that's not a double quote or a backslash */

<quote>[\\]["]	yymore(); /* This matches the escaped double quote (\") */

<quote>[\\]{2}  yymore(); /* This matches an escaped escape (\\) */

<quote>[\\]{1}  {
                    BEGIN(INITIAL);
                    if (yytext) {
                        YY_FATAL_ERROR("Inside a string, backslash (\\) can escape a double quote or must itself be escaped (\\\\).");
                    }
                }

<quote>["]  { 
                /* An unescaped double quote in the 'quote' state indicates the end of the string */
                BEGIN(INITIAL); 
                yylval->build<std::string>(yytext); 
                return token::STRING;
            }

<quote><<EOF>>	{
                  BEGIN(INITIAL);   /* resetting the state is needed for reentrant parsers */
                  YY_FATAL_ERROR("Unterminated quote");
                }

.   {
        BEGIN(INITIAL);
        if (yytext) {
            YY_FATAL_ERROR("Characters found in the input were not recognized.");
        }
    }
%%
