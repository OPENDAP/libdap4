
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

 (c) COPYRIGHT URI/MIT 1994-1996,1998,1999
*/ 

/*
  Scanner for the Error object. It recognizes the five keywords in the
  persistent representation of the Error object plus some syntactic sugar
  (`=', `{', ...). The object's persistent representation uses a keyword =
  value notation, where the values are quoted strings or integers.

  The scanner is not reentrant, but can share name spaces with other
  scanners. It must be processed by GNU's flex scanner generator.
*/

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <cstdlib>
#include <cassert>
#include <cstring>

#include <string>

#include "parser.h"
#include "Error.tab.hh"

using namespace libdap;

#ifndef YY_PROTO
#define YY_PROTO(proto) proto
#endif

//#define YY_NO_UNPUT
#define YY_DECL int Errorlex YY_PROTO(( void ))

#define YY_FATAL_ERROR(msg) {\
    throw(Error(string("Error scanning the error response: ") + string(msg))); \
    yy_fatal_error(msg); /* see das.lex */ \
}

int error_line_num = 1;
static int start_line;		/* used in quote and comment error handlers */

void store_integer();
void store_string();

%}
    
%option noyywrap
%option nounput
%option noinput
%option 8bit
%option prefix="Error"
%option outfile="lex.Error.cc"
%x quote
%x comment

SCAN_INT		[0-9]+

SCAN_ERROR		error|Error|ERROR
SCAN_CODE		code|Code|CODE
SCAN_MSG		message|Message|MESSAGE
SCAN_PTYPE		program_type|ProgramType|PROGRAM_TYPE|Program_Type
SCAN_PROGRAM	program|Program|PROGRAM

NEVER   [^a-zA-Z0-9_/.+\-{}:;,]

%%


{SCAN_ERROR}	store_string(); return SCAN_ERROR;

{SCAN_CODE}	store_string(); return SCAN_CODE;
{SCAN_MSG}	store_string(); return SCAN_MSG;

{SCAN_INT}	store_integer(); return SCAN_INT;

"{" 	    	return (int)*yytext;
"}" 	    	return (int)*yytext;
";" 	    	return (int)*yytext;
"="		return (int)*yytext;

[ \t]+
\n	    	    	++error_line_num;
<INITIAL><<EOF>>    	yy_init = 1; error_line_num = 1; yyterminate();

"#"	    	    	BEGIN(comment);
<comment>[^\n]*
<comment>\n		++error_line_num; BEGIN(INITIAL);
<comment><<EOF>>        yy_init = 1; error_line_num = 1; yyterminate();

\"			BEGIN(quote); start_line = error_line_num; yymore();
<quote>[^"\n\\]*	yymore();
<quote>[^"\n\\]*\n	yymore(); ++error_line_num;
<quote>\\.		yymore();
<quote>\"		{ 
    			  BEGIN(INITIAL); 
			  store_string();
			  return SCAN_STR;
                        }
<quote><<EOF>>		{
                          char msg[256];
			  snprintf(msg, 255,
				  "Unterminated quote (starts on line %d)\n",
				  start_line);
			  YY_FATAL_ERROR(msg);
                        }

{NEVER}                 {
                          if (yytext) {	/* suppress msgs about `' chars */
                            fprintf(stderr, "Character `%c' is not", *yytext);
                            fprintf(stderr, " allowed (except within");
			    fprintf(stderr, " quotes) and has been ignored\n");
			  }
			}
%%

// These three glue routines enable DDS to reclaim the memory used to parse a
// DDS off the wire. They are here because this file can see the YY_*
// symbols; the file DDS.cc cannot.

void *
Error_buffer(FILE *fp)
{
    return (void *)Error_create_buffer(fp, YY_BUF_SIZE);
}

void
Error_switch_to_buffer(void *buf)
{
    Error_switch_to_buffer((YY_BUFFER_STATE)buf);
}

void
Error_delete_buffer(void *buf)
{
    Error_delete_buffer((YY_BUFFER_STATE)buf);
}

void
store_integer()
{
    Errorlval.integer = atoi(yytext);
}

void
store_string()
{
    Errorlval.string = yytext;
}

