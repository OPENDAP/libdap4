
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1997
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

/*
   Grammar for the DAS. This grammar can be used with the bison parser
   generator to build a parser for the DAS. It assumes that a scanner called
   `daslex()' exists and that the objects DAS and AttrTable also exist.

   jhrg 7/12/94 
*/

/* 
 * $Log: das.y,v $
 * Revision 1.30  1997/05/13 23:32:19  jimg
 * Added changes to handle the new Alias and lexical scoping rules.
 *
 * Revision 1.29  1997/05/06 22:09:57  jimg
 * Added aliases to the grammar. An alias can appear in place of an attribute
 * and uses the syntax `alias <var1> <var2>'. If var1 exists, var2 becomes an
 * alias to it and vice versa. If neither var1 nor var2 exists or if they both
 * exist, and error is reported and parsing stops.
 *
 * Revision 1.28  1997/02/28 01:01:07  jimg
 * Tweaked error messages so that they no longer mumble about parse errors.
 *
 * Revision 1.27  1997/02/10 02:36:57  jimg
 * Fixed bug where attribute type of int32 was broken on 64bit machines.
 *
 * Revision 1.26  1996/10/28 23:04:46  jimg
 * Added unsigned int to set of possible attribute value types.
 *
 * Revision 1.25  1996/10/11 00:11:03  jimg
 * Fixed DODS_BISON_VER preprocessor statement. >= apparently is not recognized
 * by g++'s preprocessor.
 *
 * Revision 1.24  1996/10/08 17:04:40  jimg
 * Added a fix for Bison 1.25 so that PARSE_PARAM will still work
 *
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

static char rcsid[] __unused__ = {"$Id: das.y,v 1.30 1997/05/13 23:32:19 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <strstream.h>
#include <SLList.h>

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

#if DODS_BISON_VER > 124
#define YYPARSE_PARAM arg
#else
#define YYPARSE_PARAM void *arg
#endif

extern int das_line_num;	/* defined in das.lex */

static char name[ID_MAX];	/* holds name in attr_pair rule */
static char type[ID_MAX];	/* holds type in attr_pair rule */

static SLList<AttrTablePtr> *attr_tab_stack;

// I use a SLList of AttrTable pointers for a stack

#define TOP_OF_STACK (attr_tab_stack->front())
#define PUSH(x) (attr_tab_stack->prepend((x)))
#define POP (attr_tab_stack->remove_front())
#define STACK_LENGTH (attr_tab_stack->length())
#define STACK_EMPTY (attr_tab_stack->empty())

#define TYPE_NAME_VALUE << type << " " << name << " " << $1

static char *ATTR_TUPLE_MSG = 
"Expected an attribute type (Byte, Int32, UInt32, Float64, String or Url)\n\
followed by a name and value.";
static char *NO_DAS_MSG =
"The attribute object returned from the dataset was null\n\
Check that the URL is correct.";

void mem_list_report();
int daslex(void);
void daserror(char *s);
String attr_name(String name);

%}

%expect 18

%token ATTR

%token ID
%token INT
%token FLOAT
%token STR
%token ALIAS

%token BYTE
%token INT32
%token UINT32
%token FLOAT64
%token STRING
%token URL

%%

/*
  Parser algorithm: 

  Look for a `variable' name (this can be any identifier, but by convention
  it is either the name of a variable in a dataset or the name of a grouping
  of global attributes). Create a new attribute table for this identifier and
  push the new attribute table onto a stack. If attribute tuples
  (type-name-value tuples) are found, intern them in the attribute table
  found on the top of the stack. If the start attribute table of a new
  attribute table if found (before the current table is closed), create the
  new table and push *it* on the stack. As attribute tables are closed, pop
  them off the stack.

  This algorithm ensures that we can nest attribute tables to an arbitrary
  depth).

  Alias are handled using mfuncs of both the DAS and AttrTable objects. This
  is necessary because the first level of a DAS object can contain only
  AttrTables, not attribute tuples. Whereas, the subsequent levels can
  contain both. Thus the compete definition is split into two objects. In
  part this is also a hold over from an older design which did not
  have the recursive properties of the current design.

  Alias can be made between attributes within a given lexical level, from one
  level to the next within a sub-hierarchy or across hierarchies.

  Tokens:

  BYTE, INT32, UINT32, FLOAT64, STRING and URL are tokens for the type
  keywords. The tokens INT, FLOAT, STR and ID are returned by the scanner to
  indicate the type of the value represented by the string contained in the
  global DASLVAL. These two types of tokens are used to implement type
  checking for the attributes. See the rules `bytes', etc. Additional tokens:
  ATTR (indicates the start of an attribute object) and ALIAS (indicates an
  alias). 
*/

attributes:    	attribute
    	    	| attributes attribute
;
    	    	
attribute:    	ATTR 
                /* Create the AttrTable stack if necessary */
                {
		    if (!attr_tab_stack)
			attr_tab_stack = new SLList<AttrTablePtr>;
		}
                '{' attr_list '}'
                | error
                {
		    parse_error((parser_arg *)arg, NO_DAS_MSG);
		    YYABORT;
		}
;

attr_list:  	/* empty */
    	    	| attr_tuple
    	    	| attr_list attr_tuple
;

attr_tuple:	alias

                | BYTE { save_str(type, $1, das_line_num); } 
                ID { save_str(name, $3, das_line_num); } 
		bytes ';'

		| INT32 { save_str(type, $1, das_line_num); } 
                ID { save_str(name, $3, das_line_num); } 
		ints ';'

		| UINT32 { save_str(type, $1, das_line_num); } 
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

		| ID 
                {
		    AttrTable *at;
		    DBG(cerr << "Processing ID: " << $1 << endl);
		    /* If we are at the outer most level of attributes, make
		       sure to use the AttrTable in the DAS. */
		    if (STACK_EMPTY) {
			at = DAS_OBJ(arg)->get_table($1);
			if (!at)
			    at = DAS_OBJ(arg)->add_table((String)$1, 
							 new AttrTable);
		    }
		    else {
			at = TOP_OF_STACK->get_attr_table((String)$1);
			if (!at)
			    at = TOP_OF_STACK->append_container((String)$1);
		    }

		    PUSH(at);
		    DBG(cerr << " Pushed attr_tab: " << at << endl);
		}
		'{' attr_list 
                {
		    /* pop top of stack; store in attr_tab */
		    DBG(cerr << " Poped attr_tab: " << TOP_OF_STACK << endl);
		    POP;
		}
		'}'

		| error 
                { 
		    parse_error((parser_arg *)arg, ATTR_TUPLE_MSG);
		    YYABORT;
		} ';'
;

bytes:		INT
		{
		    DBG(cerr << "Adding byte: " TYPE_NAME_VALUE_1 << endl);
		    if (!check_byte($1, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Byte value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
		| bytes ',' INT
		{
		    DBG(cerr << "Adding INT: " << TYPE_NAME_VALUE_3 << endl);
		    if (!check_byte($3, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Byte value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
;

ints:		INT
		{
		    /* NB: On the Sun (SunOS 4) strtol does not check for */
		    /* overflow. Thus it will never figure out that 4 */
		    /* billion is way to large to fit in a 32 bit signed */
		    /* integer. What's worse, long is 64  bits on Alpha and */
		    /* SGI/IRIX 6.1... jhrg 10/27/96 */
		    DBG(cerr << "Adding INT: " << TYPE_NAME_VALUE_1 << endl);
		    DBG(cerr << " to AttrTable: " << TOP_OF_STACK << endl);
		    if (!(check_int($1, das_line_num) 
			  || check_uint($1, das_line_num))) {
			ostrstream msg;
			msg << "`" << $1 << "' is not an Int32 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
		| ints ',' INT
		{
		    DBG(cerr << "Adding INT: " << TYPE_NAME_VALUE_3 << endl);
		    if (!(check_int($3, das_line_num)
			  || check_uint($1, das_line_num))) {
			ostrstream msg;
			msg << "`" << $1 << "' is not an Int32 value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
;

floats:		float_or_int
		{
		    DBG(cerr << "Adding FLOAT: " << TYPE_NAME_VALUE_1 << endl);
		    if (!check_float($1, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." 
			    << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
		| floats ',' float_or_int
		{
		    DBG(cerr << "Adding FLOAT: " << TYPE_NAME_VALUE_3 << endl);
		    if (!check_float($3, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a Float64 value." 
			    << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
;

strs:		str_or_id
		{
		    DBG(cerr << "Adding STR: " << TYPE_NAME_VALUE_1 << endl);
		    /* Assume a string that parses is vaild. */
		    if (TOP_OF_STACK->append_attr(name, type, $1) == 0) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0); 
			YYABORT;
		    }
		}
		| strs ',' str_or_id
		{
		    DBG(cerr << "Adding STR: " << TYPE_NAME_VALUE_3 << endl);
		    if (TOP_OF_STACK->append_attr(name, type, $3) == 0) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
;

urls:		url
		{
		    DBG(cerr << "Adding STR: " << TYPE_NAME_VALUE_1 << endl);
		    if (!check_url($1, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a String value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $1)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
		| strs ',' url
		{
		    DBG(cerr << "Adding STR: " << TYPE_NAME_VALUE_3 << endl);
		    if (!check_url($3, das_line_num)) {
			ostrstream msg;
			msg << "`" << $1 << "' is not a String value." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		    else if (!TOP_OF_STACK->append_attr(name, type, $3)) {
			ostrstream msg;
			msg << "`" << name << "' previously defined." << ends;
			parse_error((parser_arg *)arg, msg.str());
			msg.freeze(0);
			YYABORT;
		    }
		}
;

url:		ID | STR
;

str_or_id:	STR | ID | INT | FLOAT
;

float_or_int:   FLOAT | INT
;

alias:          ALIAS ID 
                { 
		    save_str(name, $2, das_line_num); 
		} 
                ID
                {
		    // First try to alias within current lexical scope. If
		    // that fails then look in the complete environment for
		    // the AttrTable containing the source for the alias. In
		    // that case be sure to strip off the hierarchy
		    // information from the source's name (since TABLE is
		    // the AttrTable that contains the attribute named by the
		    // rightmost part of the source.
		    if (!TOP_OF_STACK->attr_alias(name, $4)) {
			AttrTable *table = DAS_OBJ(arg)->get_table($4);
			if (!TOP_OF_STACK->attr_alias(name, table, 
						      attr_name((String)$4))) {
			    ostrstream msg;
			    msg << "Could not alias `" << $4 << "' and `" 
				<< name << "'." << ends;
			    parse_error((parser_arg *)arg, msg.str());
			    msg.freeze(0);
			    YYABORT;
			}
		    }
		}
                ';'
;
%%

// This function is required for linking, but DODS uses its own error
// reporting mechanism.

void
daserror(char *)
{
}

// Return the rightmost component of name (where each component is separated
// by `.'.

String
attr_name(String name)
{
    String n = name.after(".");
    if (n.contains("."))
	return attr_name(n);
    else
	return n;
}
