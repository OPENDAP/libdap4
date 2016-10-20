
/*
 -*- mode: c++; c-basic-offset:4 -*-

 This file is part of libdap, A C++ implementation of the OPeNDAP Data
 Access Protocol.

 Copyright (c) 2013 OPeNDAP, Inc.
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

#include "D4CEScanner.h"

/* typedef to make the returns for the tokens shorter */
typedef libdap::D4CEParser::token token;

/* This was added because of some notes on the net about compiler version
   issues. I don't know if it's needed when using the C++ mode of flex. */
#undef yywrap
#define yywrap() 1

/* define yyterminate as this instead of NULL */
#define yyterminate() return(token::END)

#define YY_FATAL_ERROR(msg) {\
    throw(libdap::Error(malformed_expr, std::string("Error scanning constraint expression text: ") + std::string(msg))); \
}

%}

%option c++
%option yyclass="D4CEScanner"

/* Use this if several scanners are needed. This will cause flex to
   #define yyFlexLexer to be <prefix>FlexLexer (the yyFlexLexer is defined
   in lex.<prefix>.cc. jhrg 8/8/13 */
%option prefix="d4_ce"

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

/* This pattern just ensures that a word does not start with '#' which
   is the DAP2 comment character. 

   Having the characters !, ~, and @ in the second set of the chars allowed
   in a WORD token meant that 'var!=' parsed as WORD == 'var!' and '=' ane not
   'var' and '!='. I see that in DAP2 I did not include these in the definition
   of a WORD.
   jhrg 4/29/16 */
   
WORD    [-+a-zA-Z0-9_%*\\!~@][-+a-zA-Z0-9_%*\\#]* 

%{
// Code run each time a pattern is matched
#define YY_USER_ACTION loc->columns(yyleng);
%}

%%

%{
// Code run each time yylex is called
loc->step();
%}

"["     return token::LBRACKET;
"]"     return token::RBRACKET;
":"     return token::COLON;
","		return token::COMMA;
";"		return token::SEMICOLON;
"|"     return token::PIPE;
"{"		return token::LBRACE;
"}"		return token::RBRACE;
"/"     return token::GROUP_SEP;
"."     return token::PATH_SEP;
"="     return token::ASSIGN;

"=="    return token::EQUAL;
"!="    return token::NOT_EQUAL;
">"	    return token::GREATER;
">="    return token::GREATER_EQUAL;
"<"     return token::LESS;
"<="    return token::LESS_EQUAL;
"~="    return token::REGEX_MATCH;
"<<"    return token::LESS_BBOX;
">>"    return token::GREATER_BBOX;
"@="    return token::MASK;

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
