
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

#include "D4CEScanner.h"

/* typedef to make the returns for the tokens shorter */
typedef libdap::D4CEParser::token token;

#undef yywrap
#define yywrap() 1

// The location of the current token
// static libdap::location loc;

/* define yyterminate as this instead of NULL */
#define yyterminate() return(token::END)

%}

%option c++
%option yyclass="D4CEScanner"

/* Use this if several scanners are needed. This will cause flex to
   #define yyFlexLexer to be <prefix>FlexLexer (the yyFlexLexer is defined
   in lex.<prefix>.cc. jhrg 8/8/13 */
%option prefix="d4_ce"

%option yylineno
%option debug

%option noyywrap
%option nounput
%option warn

%option batch

%x quote

NAN		[Nn][Aa][Nn]
INF		[Ii][Nn][Ff]

WORD    [-+a-zA-Z0-9_/%.*\\][-+a-zA-Z0-9_/%.*\\#]* 
 
EQUAL	    ==
NOT_EQUAL	!=
GREATER	    >
GREATER_EQL >=
LESS	    <
LESS_EQL	<=
REGEXP	    ~=
LESS_BBOX   <<
GREATER_BBOX  >>
MASK        @=

NEVER		[^\-+a-zA-Z0-9_/%.*\\:;,(){}[\]&<>=~@]

%{
// Code run each time a pattern is matched
// #define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
// Code run each time yylex is called
// loc.step();
%}

"["    	return token::LBRACKET;
"]"    	return token::RBRACKET;
":"    	return token::COLON;
","		return token::COMMA;
";"		return token::SEMICOLON;
"|"     return token::PIPE;
"{"		return token::LBRACE;
"}"		return token::RBRACE;
"/"     return token::GROUP_SEP;
"."     return token::PATH_SEP;

{WORD}          { yylval->build<std::string>(yytext); return token::WORD; }

{EQUAL}	        return token::EQUAL;
{NOT_EQUAL}     return token::NOT_EQUAL;
{GREATER}	    return token::GREATER;
{GREATER_EQL}   return token::GREATER_EQUAL;
{LESS}	        return token::LESS;
{LESS_EQL}	    return token::LESS_EQUAL;
{REGEXP}	    return token::REGEX_MATCH;
{LESS_BBOX}     return token::LESS_BBOX;
{GREATER_BBOX}  return token::GREATER_BBOX;
{MASK}          return token::MASK;

[ \t\r\n]+
<INITIAL><<EOF>> return token::END;

\"		BEGIN(quote); yymore();

<quote>[^"\\]*  yymore(); /*"*/

<quote>\\.	yymore();

<quote>\"	{ 
    		  BEGIN(INITIAL); 
              { yylval->build<std::string>(yytext); return token::STRING; }

            }

<quote><<EOF>>	{
                  BEGIN(INITIAL);   /* resetting the state is needed for reentrant parsers */
                  YY_FATAL_ERROR("Unterminated quote");
                }

{NEVER}         {
                  BEGIN(INITIAL);
                  if (yytext) {
                    YY_FATAL_ERROR("Characters found in the input were not recognized.");
                  }
		        }
%%
