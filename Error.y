
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


// $Log: Error.y,v $
// Revision 1.1  1996/05/31 23:18:17  jimg
// Added.
//

%{

static char rcsid[]={"$Id: Error.y,v 1.1 1996/05/31 23:18:17 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream.h>

#include "parser.h"
#define DEBUG 1
#include "debug.h"

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define ERROR_OBJ(arg) ((parser_arg *)(arg))->error()
#define STATUS(arg) ((parser_arg *)(arg))->status()
#define YYPARSE_PARAM void *arg

extern int error_line_num;	// defined in Error.lex

int Errorlex();			// the scanner
int Errorerror(char *s);	// gotta love automatically generated names...

%}

%union {
    bool boolean;
    int integer;
    char *string;
}

%token <integer> INT
%token <string> STR

%token <integer> ERROR
%token <integer> CODE
%token <integer> PTYPE
%token <string> MSG
%token <string> PROGRAM

%type <boolean> error_object contents description program
%type <boolean> code program_type message program_code

%%

// The parser is called through a function named ERRORPARSE which takes a
// pointer to a structure and returns a boolean. The structure contains a
// pointer to an Error object which is empty and an integer which contains
// status information.

error_object:	ERROR '{' contents '}' ';' { $$ = $3; STATUS(arg) = $3; }
;

contents:	description program { $$ = $1 && $2; }
;

description:	code message { $$ = $1 && $2; }
;

program:	program_type program_code { $$ = $1 && $2; }
;

code:		CODE '=' INT ';' 
		{ 
		    ERROR_OBJ(arg).error_code((ErrorCode)$3);
		    $$ = true; 
		}
;

message:	MSG '=' STR 
		{ 
		    ERROR_OBJ(arg).error_message($3);
		    $$ = true; 
		} 
		';' 
;

program_type:	PTYPE '=' INT ';'
		{
		    ERROR_OBJ(arg).program_type((ProgramType)$3);
		    $$ = true; 
		}
;

program_code:	PROGRAM '=' STR
		{
		    DBG(cerr << "Program: " << $3 << endl);
		    ERROR_OBJ(arg).program($3);
		    $$ = true; 
		}
		';' 
;

%%

int 
Errorerror(char *s)
{
#ifdef NEVER
    fprintf(stderr, "%s line: %d\n", s, dds_line_num);
#endif
    cerr << s << " line: " << error_line_num << endl;
}
