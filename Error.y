
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997,1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Error.y,v 1.10 2000/09/22 02:17:20 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>

#include "Error.h"

#include "parser.h"
#include "debug.h"
#include "util.h"

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

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
		    ERROR_OBJ(arg)->error_code((ErrorCode)$3);
		    $$ = true; 
		}
;

message:	SCAN_MSG '=' SCAN_STR 
		{ 
		    ERROR_OBJ(arg)->error_message($3);
		    $$ = true; 
		} 
		';' 
;

program_type:	SCAN_PTYPE '=' SCAN_INT ';'
		{
		    ERROR_OBJ(arg)->program_type((ProgramType)$3);
		    $$ = true; 
		}
;

program_code:	SCAN_PROGRAM '=' SCAN_STR
		{
		    DBG(cerr << "Program: " << $3 << endl);
		    ERROR_OBJ(arg)->program($3);
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

