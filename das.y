/*
   Grammar for the DAS. This grammar can be used with the bison parser
   generator to build a parser for the DAS. It assumes that a scanner called
   `daslex()' exists and returns one of three token types (ID, ATTR, and VAL)
   in addition to several single character token types. The matched lexeme
   for an ID or VAL is stored by the scanner in a global char * `daslval'.
   Because the scanner returns a value via this global and because the parser
   stores daslval (not the information pointed to), the values of rule
   components must be stored as they are parsed and used once accumulated at
   or near the end of a rule. If daslval returned a value (instead of a
   pointer to a value) this would not be necessary.

   Notes:
   1) the rule for var_attr has a mid-rule action used to insert a new ID
   into the symbol table.
   2) the rule for attr_pair uses two mid-rule actions - one to store the
   name of an attribute (attr_name) to a temporary char * array and one to
   insert the resulting name-value pair into the AttrVHMap `var'. 

   jhrg 7/12/94 
*/

/* 
 * $Log: das.y,v $
 * Revision 1.3  1994/09/09 16:16:38  jimg
 * Changed the include name to correspond with the class name changes (Var*
 * to DAS*).
 *
 * Revision 1.2  1994/08/02  18:54:15  jimg
 * Added C++ statements to grammar to generate a table of parsed attributes.
 * Added a single parameter to dasparse - an object of class DAS.
 * Solved strange `string accumulation' bug with $1 %2 ... by copying
 * token's semantic values to temps using mid rule actions.
 * Added code to create new attribute tables as each variable is parsed (unless
 * a table has already been allocated, in which case that one is used).
 *
 * Revision 1.2  1994/07/25  19:01:21  jimg
 * Modified scanner and parser so that they can be compiled with g++ and
 * so that they can be linked using g++. They will be combined with a C++
 * method using a global instance variable.
 * Changed the name of line_num in the scanner to das_line_num so that
 * global symbol won't conflict in executables/libraries with multiple
 * scanners.
 *
 * Revision 1.1  1994/07/25  14:26:45  jimg
 * Test files for the DAS/DDS parsers and symbol table software.
 */

%{
#define YYSTYPE char *
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define ID_MAX 256

static char rcsid[]={"$Id: das.y,v 1.3 1994/09/09 16:16:38 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "das.tab.h"
#include "DASVHMap.h"

extern int das_line_num;

static char name[ID_MAX];	/* holds name in attr_pair rule */
static AttrTablePtr attr_tab_ptr;

int daslex(void);
int daserror(char *s);
%}

%expect 3

%token ID
%token ATTR
%token VAL

%%

attributes:	/* empty */
    	    	| attribute
    	    	| attributes attribute
;
    	    	
attribute:    	ATTR '{' var_attr_list '}'
;

var_attr_list: 	/* empty */
    	    	| var_attr
    	    	| var_attr_list var_attr
;

var_attr:   	ID 
		{ 
		  if (!table[$1]) /* new variable or adding to existing one */
		    table[$1] = new AttrTable;
		  attr_tab_ptr = table[$1];
		} 
		'{' attr_list '}'
;

attr_list:  	/* empty */
    	    	| attr_pair
    	    	| attr_list attr_pair
;

attr_pair:	attr_name 
		{ 
		    strncpy(name, $1, ID_MAX);
		    name[ID_MAX-1] = '\0';		/* in case ... */
		    if (strlen($1) >= ID_MAX) 
			fprintf(stderr, "line: %d `%s' truncated to `%s'\n", 
				das_line_num, $1, name);
		} 
		attr_val 
                { 
		  (*attr_tab_ptr)[name] = $3;
		} 
                ';' 
;

attr_name:  	ID
;

attr_val:   	VAL
                | ID
;

%%

int daserror(char *s)
{
    fprintf(stderr, "%s line: %d\n", s, das_line_num);
}

	
