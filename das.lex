
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

 (c) COPYRIGHT URI/MIT 1994-2000
*/ 

/*
   Scanner for the DAS. This file works with gnu's flex scanner generator. It
   returns either ATTR, ID, VAL, TYPE or one of the single character tokens
   `{', `}', `;', `,' or `\n' as integers. In the case of an ID or VAL, the
   scanner stores a pointer to the lexeme in yylval (whose type is char *).

   The scanner discards all comment text.

   The scanner returns quoted strings as VALs. Any characters may appear in a
   quoted string except backslash (\) and quote("). To include these escape
   them with a backslash.
   
   The scanner is not reentrant, but can share name spaces with other
   scanners.
   
   Note:
   1) The `defines' file das.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `daslex'.
   3) When bison builds the das.tab.h file, it uses `das' instead of `yy' for
   variable name prefixes (e.g., yylval --> daslval).
   4) The quote stuff is very complicated because we want backslash (\)
   escapes to work and because we want line counts to work too. In order to
   properly scan a quoted string two C functions are used: one to remove the
   escape characters from escape sequences and one to remove the trailing
   quote on the end of the string. 

   jhrg 7/12/94 

   NB: We don't remove the \'s or ending quotes any more -- that way the
   printed das can be re-parsed. 9/28/94. 
*/

%{
#include "config_dap.h"

#include <cstdio>

static char rcsid[] not_used ={"$Id$"};

#ifndef _MSC_VER
#include <string.h>
#else
#include <string>
#endif

using namespace std;

#include "debug.h"
#include "parser.h"

using namespace libdap ;

#ifndef YY_PROTO
#define YY_PROTO(proto) proto
#endif

/* These defines must precede the das.tab.h include. */
#define YYSTYPE char *
#define YY_DECL int daslex YY_PROTO(( void ))
#define YY_FATAL_ERROR(msg) {\
    throw(Error(string("Error scanning DAS object text: ") + string(msg))); \
    yy_fatal_error(msg); /* This will never be run but putting it here removes a warning that the funtion is never used. */ \
}

#include "das.tab.hh"

int das_line_num = 1;
static int start_line;		/* used in quote and comment error handlers */

%}
    
%option noyywrap
%option nounput
%option noinput
%option 8bit
%option prefix="das"
%option outfile="lex.das.cc"
 
%x quote
%x comment
%x xml

ATTR 	attributes|Attributes|ATTRIBUTES

ALIAS   ALIAS|Alias|alias
BYTE	BYTE|Byte|byte
INT16	INT16|Int16|int16
UINT16	UINT16|UInt16|Uint16|uint16
INT32	INT32|Int32|int32
UINT32	UINT32|UInt32|Uint32|uint32
FLOAT32 FLOAT32|Float32|float32
FLOAT64 FLOAT64|Float64|float64
STRING  STRING|String|string
URL	URL|Url|url
XML     OTHERXML|OtherXML|OtherXml|otherxml

/* Comment chars (#) are treated specially. Lets hope nobody wants to start
   A variable name with one... Note that the DAS allows Identifiers to have 
   parens and colons while the DDS and expr scanners don't. It's too hard to
   disambiguate functions when IDs have parens in them and adding colons
   makes parsing the array projections hard. 10/31/2001 jhrg */

WORD    [-+a-zA-Z0-9_/%.:\\()*][-+a-zA-Z0-9_/%.:\\()#*]*

NEVER   [^\-+a-zA-Z0-9_/%.:\\()#{};,[\]]

%%

{ATTR}	    	    	daslval = yytext; return SCAN_ATTR;

{ALIAS}                 daslval = yytext; return SCAN_ALIAS;
{BYTE}                  daslval = yytext; return SCAN_BYTE;
{INT16}                 daslval = yytext; return SCAN_INT16;
{UINT16}                daslval = yytext; return SCAN_UINT16;
{INT32}                 daslval = yytext; return SCAN_INT32;
{UINT32}                daslval = yytext; return SCAN_UINT32;
{FLOAT32}               daslval = yytext; return SCAN_FLOAT32;
{FLOAT64}               daslval = yytext; return SCAN_FLOAT64;
{STRING}                daslval = yytext; return SCAN_STRING;
{URL}                   daslval = yytext; return SCAN_URL;
{XML}                   daslval = yytext; return SCAN_XML;

{WORD}	    	    	{
			    daslval = yytext; 
			    DBG(cerr << "WORD: " << yytext << endl); 
			    return SCAN_WORD;
			}

"{" 	    	    	return (int)*yytext;
"}" 	    	    	return (int)*yytext;
";" 	    	    	return (int)*yytext;
","                     return (int)*yytext;

[ \t\r]+
\n	    	    	++das_line_num;
<INITIAL><<EOF>>    	yy_init = 1; das_line_num = 1; yyterminate();

"#"	    	    	BEGIN(comment);
<comment>[^\r\n]*
<comment>\n		++das_line_num; BEGIN(INITIAL);
<comment>\r\n		++das_line_num; BEGIN(INITIAL);
<comment><<EOF>>        yy_init = 1; das_line_num = 1; yyterminate();

\"                      BEGIN(quote); start_line = das_line_num; yymore();
<quote>[^"\r\n\\]*      yymore();
<quote>[^"\r\n\\]*\n    yymore(); ++das_line_num;
<quote>[^"\r\n\\]*\r\n  yymore(); ++das_line_num;
<quote>\\.              yymore();
<quote>\"               { 
                          BEGIN(INITIAL); 

                          daslval = yytext;

                          return SCAN_WORD;
                        }
<quote><<EOF>>          {
                          char msg[256];
                          sprintf(msg,
                                  "Unterminated quote (starts on line %d)\n",
                                  start_line);
                          YY_FATAL_ERROR(msg);
                        }

{NEVER}                 {
                          if (yytext) {
                            fprintf(stderr, "Character '%c' (%d) is not",
                            	    *yytext, *yytext);
                            fprintf(stderr, " allowed.");
			  }
			}
%%

// These three glue routines enable DDS to reclaim the memory used to parse a
// DDS off the wire. They are here because this file can see the YY_*
// symbols; the file DDS.cc cannot.

void *
das_buffer(FILE *fp)
{
    return (void *)das_create_buffer(fp, YY_BUF_SIZE);
}

void
das_switch_to_buffer(void *buf)
{
    das_switch_to_buffer((YY_BUFFER_STATE)buf);
}

void
das_delete_buffer(void *buf)
{
    das_delete_buffer((YY_BUFFER_STATE)buf);
}

