
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

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
 * Revision 1.23  1996/08/13 18:46:38  jimg
 * Added parser_arg object macros.
 * `Fixed' error messages.
 * Changed return typw of daserror() from int to void.
 *
 * Revision 1.22  1996/06/07 15:05:16  jimg
 * Removed old type checking code - use the type checkers in parser-util.cc.
 *
 * Revision 1.21  1996/05/31 23:30:52  jimg
 * Updated copyright notice.
 *
 * Revision 1.20  1996/04/05 00:22:13  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.19  1995/12/06  19:46:29  jimg
 * Added definition of YYSTYPE.
 *
 * Revision 1.18  1995/10/23  22:54:39  jimg
 * Removed code that was NEVER'd.
 * Changed some rules so that they call functions in parser_util.cc .
 *
 * Revision 1.17  1995/09/05  23:19:45  jimg
 * Fixed a bug in check_float where `=' was used where `==' should have been.
 *
 * Revision 1.16  1995/08/23  00:25:54  jimg
 * Added copyright notice.
 * Fixed some bogus comments.
 *
 * Revision 1.15  1995/07/08  18:32:10  jimg
 * Edited comments.
 * Removed unnecessary declarations.
 *
 * Revision 1.14  1995/05/10  13:45:43  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.13  1995/02/16  15:30:46  jimg
 * Fixed bug which caused Byte, ... values which were out of range to be
 * added to the attribute table anyway.
 * Corrected the number of expected shift-reduce conflicts.
 *
 * Revision 1.12  1995/02/10  02:56:21  jimg
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

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: das.y,v 1.23 1996/08/13 18:46:38 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <strstream.h>

#include "DAS.h"
#include "Error.h"
#include "debug.h"
#include "parser.h"
#include "das.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a strucutre (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 

#define DAS_OBJ(arg) ((DAS *)((parser_arg *)(arg))->_object)
#define ERROR_OBJ(arg) ((parser_arg *)(arg))->_error
#define STATUS(arg) ((parser_arg *)(arg))->_status
#define YYPARSE_PARAM void *arg

extern int das_line_num;	/* defined in das.lex */

static char name[ID_MAX];	/* holds name in attr_pair rule */
static char type[ID_MAX];	/* holds type in attr_pair rule */
static AttrTablePtr attr_tab;

static char *VAR_ATTR_MSG="Expected an identifier followed by a list of \
attributes.";
static char *ATTR_TUPLE_MSG="Expected an attribute type (Byte, Int32, \n\
Float64, String or Url) followed by a name and value.";

void mem_list_report();
int daslex(void);
void daserror(char *s);

%}

%expect 8

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
  Parser algorithm: 

  When a variable is found (rule: var_attr) check the table to see if some
  attributes for that var have already been parsed - if so the var must have
  a table entry alread allocated; get that entry and use it. Otherwise,
  allocate a new table entry.  

  Store the table entry for the current variable in attr_tab.

  For every attribute name-value pair (rule: attr_pair) enter the name and
  value in the table entry for the current variable.

  Tokens:

  BYTE, INT32, FLOAT64, STRING and URL are tokens for the type keywords.
  The tokens INT, FLOAT, STR and ID are returned by the scanner to indicate
  the type of the value represented by the string contained in the global
  DASLVAL. These two types of tokens are used to implement type checking for
  the atributes. See the rules `bytes', ...
*/

attributes:    	attribute
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
		    DBG2(mem_list_report()); /* mem_list_report is in */
					     /* libdbnew.a  */
		    attr_tab = DAS_OBJ(arg)->get_table($1);
		    DBG2(mem_list_report());
		    if (!attr_tab) { /* is this a new var? */
			attr_tab = DAS_OBJ(arg)->add_table($1, new AttrTable);
			DBG(cerr << "attr_tab: " << attr_tab << endl);
		    }
		    DBG2(mem_list_report());
		} 
		'{' attr_list '}'
		| error 
                { 
		    parse_error((parser_arg *)arg, VAR_ATTR_MSG, das_line_num);
		    YYABORT;
		}
;

attr_list:  	/* empty */
    	    	| attr_tuple
    	    	| attr_list attr_tuple
;

attr_tuple:	BYTE { save_str(type, $1, das_line_num); } 
                ID { save_str(name, $3, das_line_num); } 
		bytes ';'

		| INT32 { save_str(type, $1, das_line_num); } 
                ID { save_str(name, $3, das_line_num); } 
		ints ';'

		| FLOAT64 { save_str(type, $1, das_line_num); } 
                ID { save_str(name, $3, das_line_num); } 
		floats ';'

		| STRING { save_str(type, $1, das_line_num); } 
                ID { save_str(name, $3, das_line_num); } 
		strs ';'

		| URL { save_str(type, $1, das_line_num); } 
                ID { save_str(name, $3, das_line_num); } 
		urls ';'

		| error 
                { 
		    parse_error((parser_arg *)arg, ATTR_TUPLE_MSG, 
				das_line_num);
		    YYABORT;
		} ';'
;

bytes:		INT
		{
		    DBG(cerr << "Adding byte: " << name << " " << type << " "\
			<< $1 << endl);
		    if (!check_byte($1, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Byte value." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
		| bytes ',' INT
		{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< $3 << endl);
		    if (!check_byte($3, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Byte value." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
;

ints:		INT
		{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< $1 << endl);
		    if (!check_int($1, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not an Int32 value." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
		| ints ',' INT
		{
		    DBG(cerr << "Adding INT: " << name << " " << type << " "\
			<< $3 << endl);
		    if (!check_int($3, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not an Int32 value." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
;

floats:		float_or_int
		{
		    DBG(cerr << "Adding FLOAT: " << name << " " << type << " "\
			<< $1 << endl);
		    if (!check_float($1, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." 
			    << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
		| floats ',' float_or_int
		{
		    DBG(cerr << "Adding FLOAT: " << name << " " << type << " "\
			<< $3 << endl);
		    if (!check_float($3, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." 
			    << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
;

strs:		str_or_id
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $1 << endl);
		    /* Assume a string that parses is vaild. */
		    if (attr_tab->append_attr(name, type, $1) == 0) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0); 
			YYABORT;
		    }
		}
		| strs ',' str_or_id
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $3 << endl);
		    if (attr_tab->append_attr(name, type, $3) == 0) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
;

urls:		STR
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $1 << endl);
		    if (!check_url($1, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a String value." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
		| strs ',' STR
		{
		    DBG(cerr << "Adding STR: " << name << " " << type << " "\
			<< $3 << endl);
		    if (!check_url($3, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a String value." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!attr_tab->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str(), 
				    das_line_num);
			msg.freeze(0);
			YYABORT;
		    }
		}
;

str_or_id:	STR | ID | INT | FLOAT
;

float_or_int:   FLOAT | INT
;

%%

void
daserror(char */* s */)
{
#if 0
    cerr << " line: " << das_line_num << ": " << s << endl;
#endif
}
