/*
   Grammar for the DAS. This grammar can be used with the bison parser
   generator to build a parser for the DAS. It assumes that a scanner called
   `daslex()' exists and returns one of three token types (ID, ATTR, and VAL)
   in addition to several single character token types. The matched lexeme
   for an ID or VAL is stored by the scanner in a global char * `daslval'.

   Notes:
   1) the rule for var_attr has a mid-rule action used to insert a new ID
   into the symbol table.

   jhrg 7/12/94 
*/

/* $Log: das.y,v $
/* Revision 1.1  1994/07/25 14:23:00  jimg
/* First attempt at the DAS parsing software.
/*
 */

%{
#define YYSTYPE char *
#define YYDEBUG 1

static char rcsid[]={"$Id: das.y,v 1.1 1994/07/25 14:23:00 jimg Exp $"};

extern int line_num;

#include <stdio.h>
#include "das.tab.h"
%}

%expect 3

%token ID
%token ATTR
%token VAL

%%

attributes:	/* empty */
    	    	| attribute
    	    	| attributes attribute
    	    	
attribute:    	ATTR '{' var_attr_list '}'

var_attr_list: 	/* empty */
    	    	| var_attr
    	    	| var_attr_list var_attr

var_attr:   	ID { printf("Var: %s\n", $1); } '{' attr_list '}'

attr_list:  	/* empty */
    	    	|attr_pair
    	    	| attr_list attr_pair

attr_pair:	attr_name attr_val ';' 

attr_name:  	ID   { printf("\tName: %s\n", $$); }

attr_val:   	VAL  { printf("\tValue: %s\n", $$); }
    	    	| ID { printf("\tValue2: %s\n", $$); }

%%

int daserror(char *s)
{
    fprintf(stderr, "%s line: %d\n", s, line_num);
}

	
