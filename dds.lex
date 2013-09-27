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
   Scanner for the DDS. This file works with gnu's flex scanner generator. It
   returns either DATASET, INDEPENDENT, DEPENDENT, ARRAY, MAPS, LIST,
   SEQUENCE, STRUCTURE, FUNCTION, GRID, BYTE, INT32, FLOAT64, STRING, URL, ID
   or one of the single character tokens `{', `}', `;', `=' or `\n' as
   integers. In the case of an ID, the scanner stores a pointer to the lexeme
   in yylval (whose type is char *).

   The scanner discards all comment text.

   The scanner is not reentrant, but can share name spaces with other
   scanners.

   Note:
   1) The `defines' file dds.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `ddslex'.
   3) When bison builds the dds.tab.h file, it uses `dds' instead of `yy' for
   variable name prefixes (e.g., yylval --> ddslval).

   jhrg 8/29/94
*/

%{

#include "config_dap.h"

#include <cstdio>
#include <cstring>

#include "parser.h"
#include "dds.tab.hh"
#include "escaping.h"

using namespace libdap ;

#ifndef YY_PROTO
#define YY_PROTO(proto) proto
#endif

#define YY_DECL int ddslex YY_PROTO(( void ))

#define YY_INPUT(buf,result,max_size) { \
    if (fgets((buf), (max_size), (ddsin)) == NULL) { \
      *buf = '\0'; \
    } \
    result = (feof(ddsin) || *buf == '\0' || strncmp(buf, "Data:\n", 6) == 0) \
             ? YY_NULL : strlen(buf); \
}

#define YY_FATAL_ERROR(msg) {\
    throw(Error(string("Error scanning DDS object text: ") + string(msg))); \
    yy_fatal_error(msg); /* see das.lex */ \
}

int dds_line_num = 1;

static void store_word();

%}

%option noyywrap
%option nounput
%option noinput
%option 8bit
%option prefix="dds"
%option outfile="lex.dds.cc"
%x comment

DATASET 	DATASET|Dataset|dataset
LIST 		LIST|List|list
SEQUENCE 	SEQUENCE|Sequence|sequence
STRUCTURE 	STRUCTURE|Structure|structure
GRID 		GRID|Grid|grid
BYTE 		BYTE|Byte|byte
INT16 		INT16|Int16|int16
UINT16 		UINT16|UInt16|uint16
INT32 		INT32|Int32|int32
UINT32 		UINT32|UInt32|uint32
FLOAT32 	FLOAT32|Float32|float32
FLOAT64 	FLOAT64|Float64|float64
STRING 		STRING|String|string
URL 		URL|Url|url

/* See das.lex for comments about the characters allowed in a WORD.
   10/31/2001 jhrg */

WORD        [-+a-zA-Z0-9_/%.\\*][-+a-zA-Z0-9_/%.\\#*]*

NEVER		[^\-+a-zA-Z0-9_/%.\\#,(){}[\]]

%%

"{"         return (int)*yytext;
"}" 	    return (int)*yytext;
"["			return (int)*yytext;
"]"			return (int)*yytext;
":"			return (int)*yytext;
";" 	    return (int)*yytext;
"="			return (int)*yytext;

{DATASET}		store_word(); return SCAN_DATASET;
{LIST}			store_word(); return SCAN_LIST;
{SEQUENCE}		store_word(); return SCAN_SEQUENCE;
{STRUCTURE}		store_word(); return SCAN_STRUCTURE;
{GRID}			store_word(); return SCAN_GRID;
{BYTE}			store_word(); return SCAN_BYTE;
{INT16}			store_word(); return SCAN_INT16;
{UINT16}		store_word(); return SCAN_UINT16;
{INT32}			store_word(); return SCAN_INT32;
{UINT32}		store_word(); return SCAN_UINT32;
{FLOAT32}		store_word(); return SCAN_FLOAT32;
{FLOAT64}		store_word(); return SCAN_FLOAT64;
{STRING}		store_word(); return SCAN_STRING;
{URL}			store_word(); return SCAN_URL;

{WORD}      store_word(); return SCAN_WORD;

[ \t\r]+
\n	    	++dds_line_num;
<INITIAL><<EOF>>    yy_init = 1; dds_line_num = 1; yyterminate();

"#"     BEGIN(comment);
<comment>[^\n]*
<comment>\n		++dds_line_num; BEGIN(INITIAL);
<comment><<EOF>>    yy_init = 1; dds_line_num = 1; yyterminate();

"Data:\n"		yyterminate();
"Data:\r\n"		yyterminate();

{NEVER} {
            if (yytext) {	/* suppress msgs about `' chars */
                fprintf(stderr, "Character `%c' is not", *yytext);
                fprintf(stderr, " allowed and has been ignored\n");
	    }
	}
%%

// These three glue routines enable DDS to reclaim the memory used to parse a
// DDS off the wire. They are here because this file can see the YY_*
// symbols; the file DDS.cc cannot.

void *
dds_buffer(FILE *fp)
{
    return (void *)dds_create_buffer(fp, YY_BUF_SIZE);
}

void
dds_switch_to_buffer(void *buf)
{
    dds_switch_to_buffer((YY_BUFFER_STATE)buf);
}

void
dds_delete_buffer(void *buf)
{
    dds_delete_buffer((YY_BUFFER_STATE)buf);
}

static void
store_word()
{
    // dods2id(string(yytext)).c_str()
    strncpy(ddslval.word, yytext, ID_MAX-1);
    ddslval.word[ID_MAX-1] = '\0'; // for the paranoid...
}

