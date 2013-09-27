
/*
 -*- mode: c++; c-basic-offset:4 -*-

 This file is part of libdap, A C++ implementation of the OPeNDAP Data
 Access Protocol.

 Copyright (c) 2002,2003 OPeNDAP, Inc.
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

 (c) COPYRIGHT URI/MIT 1994-1999
*/ 

/*
  Scanner for constraint expressions. The scanner returns tokens for each of
  the relational and selection operators. It requires GNU flex version 2.5.2
  or newer.

  The scanner is not reentrant, but can share a name space with other
  scanners. 

   Note:
   1) The `defines' file expr.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `exprlex'.
   3) When bison builds the expr.tab.h file, it uses `expr' instead of `yy'
   for variable name prefixes (e.g., yylval --> exprlval).

  jhrg 9/5/95
*/

%{

#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <cstdio>
#include <string>
#include <cstring>

#ifndef YY_PROTO
#define YY_PROTO(proto) proto
#endif

#define YY_DECL int ce_exprlex YY_PROTO(( void ))
#define YY_FATAL_ERROR(msg) {\
    throw(Error(string("Error scanning constraint expression text: ") + string(msg))); \
    yy_fatal_error(msg); /* see das.lex */ \
}

#include "Error.h"
#include "parser.h"
#include "expr.h"
#include "RValue.h"
#include "ce_expr.tab.hh"
#include "escaping.h"

using namespace libdap ;

static void store_id();
static void store_str();
static void store_op(int op);

%}

%option noyywrap
%option nounput
%option noinput
%option 8bit
%option prefix="ce_expr"
%option outfile="lex.ce_expr.cc"

%x quote
    
/* In the DAS and DDS parsers I removed the INT and FLOAT lexemes. However,
   not having them here complicates parsing since you must check to see if a
   word is a number (like 2.3) or a variable called `2.3.' I'm assuming that
   people will always put some characters in variable names (e.g., they'll
   use `2300.7%20MHz' and not just `2300.7'). If that turns out to be a bad
   assumption, the we'll have to put more code in the parser to figure out
   what exactly each word is; is it a constant or a variable name. Time will
   tell. 10/31/2001 jhrg */

NAN		[Nn][Aa][Nn]
INF		[Ii][Nn][Ff]
/* See das.lex for comments about the characters allowed in a WORD.
   10/31/2001 jhrg 

   I've added '*' to the set of characters in a WORD for both the DDS and DAS
   scanners, but not here because it'll conflict with the url dereference
   operator. 6/10/2002 jhrg
*/

SCAN_WORD       [-+a-zA-Z0-9_/%.\\][-+a-zA-Z0-9_/%.\\#]*

SCAN_EQUAL	    =
SCAN_NOT_EQUAL	!=
SCAN_GREATER	>
SCAN_GREATER_EQL >=
SCAN_LESS	    <
SCAN_LESS_EQL	<=
SCAN_REGEXP	    =~

SCAN_HASH_BYTE    $Byte
SCAN_HASH_INT16   $Int16
SCAN_HASH_UINT16  $UInt16
SCAN_HASH_INT32   $Int32
SCAN_HASH_UINT32  $UInt32
SCAN_HASH_FLOAT32 $Float32
SCAN_HASH_FLOAT64 $Float64

SCAN_STAR       \*

NEVER		[^\-+a-zA-Z0-9_/%.\\:,(){}[\]&<>=~]

%%

"["    	return (int)*yytext;
"]"    	return (int)*yytext;
":"    	return (int)*yytext;
","		return (int)*yytext;
"&"		return (int)*yytext;
"("		return (int)*yytext;
")"		return (int)*yytext;
"{"		return (int)*yytext;
"}"		return (int)*yytext;

{SCAN_WORD}	        store_id(); return SCAN_WORD;

{SCAN_EQUAL}	    store_op(SCAN_EQUAL); return SCAN_EQUAL;
{SCAN_NOT_EQUAL}    store_op(SCAN_NOT_EQUAL); return SCAN_NOT_EQUAL;
{SCAN_GREATER}	    store_op(SCAN_GREATER); return SCAN_GREATER;
{SCAN_GREATER_EQL}  store_op(SCAN_GREATER_EQL); return SCAN_GREATER_EQL;
{SCAN_LESS}	        store_op(SCAN_LESS); return SCAN_LESS;
{SCAN_LESS_EQL}	    store_op(SCAN_LESS_EQL); return SCAN_LESS_EQL;
{SCAN_REGEXP}	    store_op(SCAN_REGEXP); return SCAN_REGEXP;

{SCAN_STAR}         store_op(SCAN_STAR); return SCAN_STAR;

{SCAN_HASH_BYTE}      return SCAN_HASH_BYTE;
{SCAN_HASH_INT16}     return SCAN_HASH_INT16;
{SCAN_HASH_UINT16}    return SCAN_HASH_UINT16;
{SCAN_HASH_INT32}     return SCAN_HASH_INT32;
{SCAN_HASH_UINT32}    return SCAN_HASH_UINT32;
{SCAN_HASH_FLOAT32}   return SCAN_HASH_FLOAT32;
{SCAN_HASH_FLOAT64}   return SCAN_HASH_FLOAT64;

[ \t\r\n]+
<INITIAL><<EOF>> yy_init = 1; yyterminate();

\"		BEGIN(quote); yymore();

<quote>[^"\\]*  yymore(); /*"*/

<quote>\\.	yymore();

<quote>\"	{ 
    		  BEGIN(INITIAL); 
              store_str();
              return SCAN_STR;
            }

<quote><<EOF>>	{
                  BEGIN(INITIAL);   /* resetting the state is needed for reentrant parsers */
                  char msg[256];
                  sprintf(msg, "Unterminated quote\n");
                  YY_FATAL_ERROR(msg);
                }

{NEVER}         {
                  if (yytext) {	/* suppress msgs about `' chars */
                    fprintf(stderr, "Character `%c' is not", *yytext);
                    fprintf(stderr, " allowed and has been ignored\n");
                  }
		        }
%%

// Three glue routines for string scanning. These are not declared in the
// header expr.tab.h nor is YY_BUFFER_STATE. Including these here allows them
// to see the type definitions in lex.expr.c (where YY_BUFFER_STATE is
// defined) and allows callers to declare them (since callers outside of this
// file cannot declare the YY_BUFFER_STATE variable). Note that I changed the
// name of the expr_scan_string function to expr_string because C++ cannot
// distinguish by return type. 1/12/99 jhrg

void *
ce_expr_string(const char *str)
{
    return (void *)ce_expr_scan_string(str);
}

void
ce_expr_switch_to_buffer(void *buf)
{
    ce_expr_switch_to_buffer((YY_BUFFER_STATE)buf);
}

void
ce_expr_delete_buffer(void *buf)
{
    ce_expr_delete_buffer((YY_BUFFER_STATE)buf);
}

static void
store_id()
{
    strncpy(ce_exprlval.id, yytext, ID_MAX-1);
    ce_exprlval.id[ID_MAX-1] = '\0';
}

static void
store_str()
{
    // transform %20 to a space. 7/11/2001 jhrg
    string *s = new string(yytext); // move all calls of www2id into the parser. jhrg 7/5/13 www2id(string(yytext)));

    if (*s->begin() == '\"' && *(s->end()-1) == '\"') {
	s->erase(s->begin());
	s->erase(s->end()-1);
    }

    ce_exprlval.val.type = dods_str_c;
    ce_exprlval.val.v.s = s;
}

static void
store_op(int op)
{
    ce_exprlval.op = op;
}

