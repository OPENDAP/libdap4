/*
   Grammar for the DDS. This grammar can be used with the bison parser
   generator to build a parser for the DDS. It assumes that a scanner called
   `ddslex()' exists and returns several token types (see das.tab.h)
   in addition to several single character token types. The matched lexeme
   for an ID is stored by the scanner in a global char * `ddslval'.
   Because the scanner returns a value via this global and because the parser
   stores ddslval (not the information pointed to), the values of rule
   components must be stored as they are parsed and used once accumulated at
   or near the end of a rule. If ddslval returned a value (instead of a
   pointer to a value) this would not be necessary.

   jhrg 8/29/94 
*/

/* $Log: dds.y,v $
/* Revision 1.1  1994/09/08 21:10:45  jimg
/* DDS Class test driver and parser and scanner.
/*
 */

%{
#define YYSTYPE char *
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define ID_MAX 256

static char rcsid[]={"$Id: dds.y,v 1.1 1994/09/08 21:10:45 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dds.tab.h"
#include "DDS.h"

extern int dds_line_num;

static char name[ID_MAX];

int ddslex(void);
int ddserror(char *s);
%}

%expect 61

%token ID
%token INTEGER
%token DATASET
%token INDEPENDENT
%token DEPENDENT
%token ARRAY
%token MAPS
%token LIST
%token SEQUENCE
%token STRUCTURE
%token FUNCTION
%token GRID
%token BYTE
%token INT32
%token FLOAT64
%token STRING
%token URL 

%%

datasets:	/* empty */
		| dataset
		| datasets dataset
;

dataset:	DATASET '{' declarations '}' name ';'
;

declarations:	/* empty */
		| declaration
		| declarations declaration
;

declaration: 	LIST declaration
		| base_type var ';'
		| STRUCTURE  '{' declarations '}' var ';'
		| SEQUENCE '{' declarations '}' var ';'
		| FUNCTION '{' INDEPENDENT ':' declarations 
			       DEPENDENT ':' declarations '}' var ';'
		| GRID '{' ARRAY ':' declaration 
		               MAPS ':' declarations '}' var ';'
;

base_type:	BYTE
		| INT32
		| FLOAT64
		| STRING
		| URL
;

var:		ID
 		| var '[' INTEGER ']'
;

name:		ID
;

%%

int ddserror(char *s)
{
    fprintf(stderr, "%s line: %d\n", s, dds_line_num);
}

	
