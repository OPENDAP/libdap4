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
 * Revision 1.12  1995/02/10 02:56:21  jimg
 * Added type checking.
 *
 * Revision 1.11  1994/12/22  04:30:56  reza
 * Made save_str static to avoid linking conflict.
 *
 * Revision 1.10  1994/12/16  22:06:23  jimg
 * Fixed a bug in save_str() where the global NAME was used instead of the
 * parameter DST.
 *
 * Revision 1.9  1994/12/07  21:19:45  jimg
 * Added a new rule (var) and modified attr_val to handle attribute vectors.
 * Each element in the vector is seaprated by a comma.
 * Replaces some old instrumentation code with newer code using the DGB
 * macros.
 *
 * Revision 1.8  1994/11/10  19:50:55  jimg
 * In the past it was possible to have a null file correctly parse as a
 * DAS or DDS. However, now that is not possible. It is possible to have
 * a file that contains no variables parse, but the keyword `Attribute'
 * or `Dataset' *must* be present. This was changed so that errors from
 * the CGIs could be detected (since they return nothing in the case of
 * a error).
 *
 * Revision 1.7  1994/10/18  00:23:18  jimg
 * Added debugging statements.
 *
 * Revision 1.6  1994/10/05  16:46:51  jimg
 * Modified the DAS grammar so that TYPE tokens (from the scanner) were
 * parsed correcly and added to the new AttrTable class.
 * Changed the code used to add entries based on changes to AttrTable.
 * Consoladated error reporting code.
 *
 * Revision 1.5  1994/09/27  23:00:39  jimg
 * Modified to use the new DAS class and new AttrTable class.
 *
 * Revision 1.4  1994/09/15  21:10:56  jimg
 * Added commentary to das.y -- how does it work.
 *
 * Revision 1.3  1994/09/09  16:16:38  jimg
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

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static char rcsid[]={"$Id: das.y,v 1.12 1995/02/10 02:56:21 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "debug.h"
#include "das.tab.h"
#include "DAS.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

extern int das_line_num;

static char name[ID_MAX];	/* holds name in attr_pair rule */
static char type[ID_MAX];	/* holds type in attr_pair rule */
static AttrTablePtr attr_tab_ptr;

void mem_list_report();
int daslex(void);
int daserror(char *s);
static void not_a_datatype(char *s);
static void save_str(char *dst, char *src);
static int check_byte(char *val);
static int check_int(char *val);
static int check_float(char *val);
static int check_url(char *val);

%}

%expect 6

%token ATTR

%token ID
%token INT
%token FLOAT
%token STR

%token BYTE
%token INT32
%token FLOAT64
%token STRING
%token URL

%%

/*
  The parser takes a single argument, a reference to an object of class
  DAS. The reference is called `table'.

  Parser algorithm: 

  When a variable is found (rule: var_attr) check the table to see if some
  attributes for that var have already been parsed - if so the var must have
  a table entry alread allocated; get that entry and use it. Otherwise,
  allocate a new table entry.  

  Store the table entry for the current variable in attr_tab_ptr.

  For every attribute name-value pair (rule: attr_pair) enter the name and
  value in the table entry for the current variable.

  Tokens:

  BYTE, INT32, FLOAT64, STRING and URL are tokens for the type keywords.
  The tokens INT, FLOAT, STR and ID are returned by the scanner to indicate
  the type of the value represented by the string contained in the global
  daslval. These two types of tokens are used to implement type checking for
  the atributes. See the rules `bytes', ...
*/

attributes:    	attribute
    	    	| attributes attribute
;
    	    	
attribute:    	ATTR { parse_ok = TRUE; } '{' var_attr_list '}'
;

var_attr_list: 	/* empty */
    	    	| var_attr
    	    	| var_attr_list var_attr
;

var_attr:   	ID 
		{ 
		    DBG2(mem_list_report());
		    attr_tab_ptr = table.get_table($1);
		    DBG2(mem_list_report());
		    if (!attr_tab_ptr) { /* is this a new var? */
			attr_tab_ptr = table.add_table($1, new AttrTable);
			DBG(cerr << "attr_tab_ptr: " << attr_tab_ptr << endl);
		    }
		    DBG2(mem_list_report());
		} 
		'{' attr_list '}'
		| error { parse_ok = FALSE; }
;

attr_list:  	/* empty */
    	    	| attr_tuple
    	    	| attr_list attr_tuple
;

attr_tuple:	BYTE { save_str(type, $1); } 
                ID { save_str(name, $3); } 
		bytes ';'

		| INT32 { save_str(type, $1); } 
                ID { save_str(name, $3); } 
		ints ';'

		| FLOAT64 { save_str(type, $1); } 
                ID { save_str(name, $3); } 
		floats ';'

		| STRING { save_str(type, $1); } 
                ID { save_str(name, $3); } 
		strs ';'

		| URL { save_str(type, $1); } 
                ID { save_str(name, $3); } 
		urls ';'

		| error { parse_ok = FALSE; } ';'
;

bytes:		INT
		{
		    DBG(cerr << "Adding byte: " << name << " " << type << " "\
			<< $1 << endl);
		    check_byte($1);
		    if (attr_tab_ptr->append_attr(name, type, $1) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
		| bytes ',' INT
		{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< $3 << endl);
		    check_byte($3);
		    if (attr_tab_ptr->append_attr(name, type, $3) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
;

ints:		INT
		{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< $1 << endl);
		    check_int($1);
		    if (attr_tab_ptr->append_attr(name, type, $1) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
		| ints ',' INT
		{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< $3 << endl);
		    check_int($3);
		    if (attr_tab_ptr->append_attr(name, type, $3) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
;

floats:		float_or_int
		{
		    DBG(cerr << "Adding FLOAT: " << name << " " << type << " "\
			<< $1 << endl);
		    check_float($1);
		    if (attr_tab_ptr->append_attr(name, type, $1) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
		| floats ',' float_or_int
		{
		    DBG(cerr << "Adding FLOAT: " << name << " " << type << " "\
			<< $3 << endl);
		    check_float($3);
		    if (attr_tab_ptr->append_attr(name, type, $3) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
;

strs:		str_or_id
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $1 << endl);
		    /* assume that a string that parsers is a vaild string */
		    if (attr_tab_ptr->append_attr(name, type, $1) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
		| strs ',' str_or_id
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $3 << endl);
		    if (attr_tab_ptr->append_attr(name, type, $3) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
;

urls:		STR
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $1 << endl);
		    check_url($1);
		    if (attr_tab_ptr->append_attr(name, type, $1) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
		| strs ',' STR
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $3 << endl);
		    check_url($3);
		    if (attr_tab_ptr->append_attr(name, type, $3) == 0) {
			daserror("Variable redefinition");
			parse_ok = 0;
		    }
		}
;

str_or_id:	STR | ID | INT | FLOAT
;

float_or_int:   FLOAT | INT
;

%%

static void
save_str(char *dst, char *src)
{
    strncpy(dst, src, ID_MAX);
    dst[ID_MAX-1] = '\0';		/* in case ... */
    if (strlen(src) >= ID_MAX) 
	cerr << "line: " << das_line_num << "`" << src << "' truncated to `"
             << dst << "'" << endl;
}

static void
not_a_datatype(char *s)
{
    fprintf(stderr, "`%s' is not a datatype; line %d\n", s, das_line_num);
}

int 
daserror(char *s)
{
    fprintf(stderr, "%s line: %d\n", s, das_line_num);
}

static int
check_byte(char *val)
{
    int v = atoi(val);

    if (abs(v) > 255) {
	daserror("Not a byte value");
	return FALSE;
    }

    return TRUE;
}

static int
check_int(char *val)
{
    int v = atoi(val);

    if (abs(v) > 2147483647) {	/* don't use the constant from limits.h */
	daserror("Not a 32-bit integer value");
	return FALSE;
    }

    return TRUE;
}

static int
check_float(char *val)
{
    double v = atof(val);

    if (v = 0.0) {
	daserror("Not decodable to a 64-bit float value");
	return FALSE;
    }

    return TRUE;
}

static int
check_url(char *val)
{
    return TRUE;
}

