
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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
 
// (c) COPYRIGHT URI/MIT 1997,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Error.y,v 1.16 2004/02/19 19:42:52 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>

#include "Error.h"

#include "parser.h"
#include "debug.h"
#include "util.h"

using namespace std;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define ERROR_OBJ(arg) ((Error *)((parser_arg *)(arg))->_object)
#define STATUS(arg) ((parser_arg *)(arg))->_status

#define YYPARSE_PARAM arg

extern int error_line_num;	// defined in Error.lex

int Errorlex();			// the scanner
void Errorerror(char *s);	// gotta love automatically generated names...

#define	yymaxdepth error_maxdepth
#define	yyparse	error_parse
#define	yylex	error_lex
#define	yyerror	error_error
#define	yylval	error_lval
#define	yychar	error_char
#define	yydebug	error_debug
#define	yypact	error_pact
#define	yyr1	error_r1
#define	yyr2	error_r2
#define	yydef	error_def
#define	yychk	error_chk
#define	yypgo	error_pgo
#define	yyact	error_act
#define	yyexca	error_exca
#define yyerrflag error_errflag
#define yynerrs	error_nerrs
#define	yyps	error_ps
#define	yypv	error_pv
#define	yys	error_s
#define	yy_yys	error_yys
#define	yystate	error_state
#define	yytmp	error_tmp
#define	yyv	error_v
#define	yy_yyv	error_yyv
#define	yyval	error_val
#define	yylloc	error_lloc
#define yyreds	error_reds
#define yytoks	error_toks
#define yylhs	error_yylhs
#define yylen	error_yylen
#define yydefred error_yydefred
#define yydgoto	error_yydgoto
#define yysindex error_yysindex
#define yyrindex error_yyrindex
#define yygindex error_yygindex
#define yytable	 error_yytable
#define yycheck	 error_yycheck
#define yyname   error_yyname
#define yyrule   error_yyrule

%}

%union {
#ifdef __SUNPRO_CC
    int boolean;
#else
    bool boolean;
#endif
    int integer;
    char *string;
}

%token <integer>	SCAN_INT
%token <string>		SCAN_STR

%token <integer>	SCAN_ERROR
%token <integer>	SCAN_CODE
%token <integer>	SCAN_PTYPE
%token <string>		SCAN_MSG
%token <string>		SCAN_PROGRAM

%type <boolean> error_object contents description program
%type <boolean> code program_type message program_code

%%

// The parser is called through a function named ERRORPARSE which takes a
// pointer to a structure and returns a boolean. The structure contains a
// pointer to an Error object which is empty and an integer which contains
// status information. In addition the parser_arg strucuture contains a
// pointer to an error object. However, the `error' member of parser_arg is
// not yet used here.

error_object:	SCAN_ERROR '{' contents '}' ';' { $$ = $3; STATUS(arg) = $3; }
;

contents:	description program { $$ = $1 && $2; }
                | description { $$ = $1; }
;

description:	code message { $$ = $1 && $2; }
                | code { $$ = $1; }
;

program:	program_type program_code { $$ = $1 && $2; }
;

code:		SCAN_CODE '=' SCAN_INT ';' 
		{ 
		    ERROR_OBJ(arg)->set_error_code((ErrorCode)$3);
		    $$ = true; 
		}
;

message:	SCAN_MSG '=' SCAN_STR 
		{ 
		    ERROR_OBJ(arg)->set_error_message($3);
		    $$ = true; 
		} 
		';' 
;

program_type:	SCAN_PTYPE '=' SCAN_INT ';'
		{
		    ERROR_OBJ(arg)->set_program_type((ProgramType)$3);
		    $$ = true; 
		}
;

program_code:	SCAN_PROGRAM '=' SCAN_STR
		{
		    DBG(cerr << "Program: " << $3 << endl);
		    ERROR_OBJ(arg)->set_program($3);
		    $$ = true; 
		}
		';' 
;

%%

void
Errorerror(char *s)
{
  string msg = s;
  msg += " line: ";
  append_long_to_string(error_line_num, 10, msg);
  msg += "\n";

  throw Error(unknown_error, msg);
}


// $Log: Error.y,v $
// Revision 1.16  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.14.2.1  2004/01/22 17:09:52  jimg
// Added std namespace declarations since the DBG() macro uses cerr.
//
// Revision 1.15  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.14  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.13  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.12.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.12  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.11  2000/10/02 18:49:26  jimg
// The Error class now has const accessors
//
// Revision 1.10  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.9  2000/07/09 21:43:29  rmorris
// Mods to increase portability, minimize ifdef's for win32
//
// Revision 1.8  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.7.20.1  2000/06/02 18:21:27  rmorris
// Mod's for port to Win32.
//
// Revision 1.7  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.6.4.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code.
//
// Revision 1.6.4.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.6  1997/08/23 00:17:19  jimg
// Added to the `description' rule so that Error objects with only a code
// will parse.
//
// Revision 1.5  1997/02/14 23:56:11  jimg
// Changed grammar to allow Error objects which have no `program' component.
// That is, the program part is absent. Previously the program part had to be
// there even if the fields had null values.
//
// Revision 1.4  1996/10/16 22:35:56  jimg
// Fixed bad operator in DODS_BISON_VER preprocessor statement.
//
// Revision 1.3  1996/10/08 17:04:39  jimg
// Added a fix for Bison 1.25 so that PARSE_PARAM will still work
//
// Revision 1.2  1996/08/13 18:21:19  jimg
// Switched to parser_arg object for communication with caller.
// Fixed bogus declaration of Errorerror() (from int to void).
//
// Revision 1.1  1996/05/31 23:18:17  jimg
// Added.

