
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

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

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: dds.y,v 1.43 2003/05/23 03:24:57 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <stack>
#include <sstream>

#include "DDS.h"
#include "Array.h"
#include "Error.h"
#include "parser.h"
#include "util.h"

 using namespace std;

// These macros are used to access the `arguments' passed to the parser. A
// pointer to an error object and a pointer to an integer status variable are
// passed in to the parser within a structure (which itself is passed as a
// pointer). Note that the ERROR macro explicitly casts OBJ to an ERROR *. 
// ERROR is no longer used. These parsers now signal problems by throwing
// exceptions. 5/22/2002 jhrg
#define DDS_OBJ(arg) ((DDS *)((parser_arg *)(arg))->_object)

#define YYPARSE_PARAM arg

extern int dds_line_num;	/* defined in dds.lex */

// No global static objects in the dap library! 1/24/2000 jhrg
static stack<BaseType *> *ctor;
static BaseType *current;
static string *id;
static Part part = nil;		/* Part is defined in BaseType */

static char *NO_DDS_MSG =
"The descriptor object returned from the dataset was null.\n\
Check that the URL is correct.";

static char *BAD_DECLARATION =
"In the dataset descriptor object: Expected a variable declaration\n\
(e.g., Int32 i;). Make sure that the variable name is not the name\n\
of a datatype and that the Array: and Maps: sections of a Grid are\n\
labeled properly.";
 
int ddslex();
void ddserror(char *s);

void add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, 
	       Part p);
void invalid_declaration(parser_arg *arg, string semantic_err_msg, 
			 char *type, char *name);

%}

%expect 52

%union {
    bool boolean;
    char word[ID_MAX];
}

%token <word> SCAN_WORD
%token <word> SCAN_DATASET
%token <word> SCAN_LIST
%token <word> SCAN_SEQUENCE
%token <word> SCAN_STRUCTURE
%token <word> SCAN_FUNCTION
%token <word> SCAN_GRID
%token <word> SCAN_BYTE
%token <word> SCAN_INT16
%token <word> SCAN_UINT16
%token <word> SCAN_INT32
%token <word> SCAN_UINT32
%token <word> SCAN_FLOAT32
%token <word> SCAN_FLOAT64
%token <word> SCAN_STRING
%token <word> SCAN_URL 

%type <boolean> datasets dataset declarations array_decl

%type <word> declaration base_type structure sequence grid var var_name name

%%

start:
                {
		    /* On entry to the parser, make the BaseType stack. */
		    ctor = new stack<BaseType *>;
                }
                datasets
                {
		    delete ctor;
		}
;

datasets:	dataset
		| datasets dataset
;

dataset:	SCAN_DATASET '{' declarations '}' name ';'
                {
		    $$ = $3 && $5;
		}
                | error
                {
		    parse_error((parser_arg *)arg, NO_DDS_MSG,
 				dds_line_num, $<word>1);
		    YYABORT;
		}
;

declarations:	/* empty */
                {
		    $$ = true;
		}
                | declaration { $$ = true; }
                | declarations declaration { $$ = true; }
;

/* This non-terminal is here only to keep types like `List List Int32' from
   parsing. DODS does not allow Lists of Lists. Those types make translation
   to/from arrays too hard. */

declaration:  base_type var ';' 
                { 
		    string smsg;
		    if (current->check_semantics(smsg)) {
			/* BaseType *current_save = current ; */
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
			/* FIX
			if( current_save == current )
			{
			    delete current ;
			    current = 0 ;
			}
			*/
		    } else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $2);
		      YYABORT;
		    }
                    strcpy($$,$2);
		}

		| structure  '{' declarations '}' 
		{ 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		} 
                var ';' 
                { 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $6);
		      YYABORT;
		    }
                    strcpy($$,$6);
		}

		| sequence '{' declarations '}' 
                { 
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		} 
                var ';' 
                { 
		    string smsg;
		    if (current->check_semantics(smsg))
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $6);
		      YYABORT;
		    }
                    strcpy($$,$6);
		}

		| grid '{' SCAN_WORD ':'
		{ 
		    if (is_keyword(string($3), "array"))
			part = array; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, $3);
			YYABORT;
		    }
                }
                declaration SCAN_WORD ':'
		{ 
		    if (is_keyword(string($7), "maps"))
			part = maps; 
		    else {
			ostringstream msg;
			msg << BAD_DECLARATION;
			parse_error((parser_arg *)arg, msg.str().c_str(),
				    dds_line_num, $7);
			YYABORT;
		    }
                }
                declarations '}' 
		{
		    if( current ) delete current ;
		    current = ctor->top(); 
		    ctor->pop();
		}
                var ';' 
                {
		    string smsg;
		    if (current->check_semantics(smsg)) {
			part = nil; 
			add_entry(*DDS_OBJ(arg), &ctor, &current, part); 
		    }
		    else {
		      invalid_declaration((parser_arg *)arg, smsg, $1, $13);
		      YYABORT;
		    }
                    strcpy($$,$13);
		}

                | error 
                {
		    ostringstream msg;
		    msg << BAD_DECLARATION;
		    parse_error((parser_arg *)arg, msg.str().c_str(),
				dds_line_num, $<word>1);
		    YYABORT;
		}
;
 

structure:	SCAN_STRUCTURE
		{ 
		    ctor->push(NewStructure()); 
		}
;

sequence:	SCAN_SEQUENCE 
		{ 
		    ctor->push(NewSequence()); 
		}
;

grid:		SCAN_GRID 
		{ 
		    ctor->push(NewGrid()); 
		}
;

base_type:	SCAN_BYTE { if( current ) delete current ;current = NewByte(); }
		| SCAN_INT16 { if( current ) delete current ;current = NewInt16(); }
		| SCAN_UINT16 { if( current ) delete current ;current = NewUInt16(); }
		| SCAN_INT32 { if( current ) delete current ;current = NewInt32(); }
		| SCAN_UINT32 { if( current ) delete current ;current = NewUInt32(); }
		| SCAN_FLOAT32 { if( current ) delete current ;current = NewFloat32(); }
		| SCAN_FLOAT64 { if( current ) delete current ;current = NewFloat64(); }
		| SCAN_STRING { if( current ) delete current ;current = NewStr(); }
		| SCAN_URL { if( current ) delete current ;current = NewUrl(); }
;

var:		var_name { current->set_name($1); }
 		| var array_decl
;

var_name:       SCAN_WORD | SCAN_BYTE | SCAN_INT16 | SCAN_INT32 | SCAN_UINT16
                | SCAN_UINT32 | SCAN_FLOAT32 | SCAN_FLOAT64 | SCAN_STRING
                | SCAN_URL | SCAN_STRUCTURE | SCAN_SEQUENCE | SCAN_GRID
                | SCAN_LIST
;

array_decl:	'[' SCAN_WORD ']'
                 { 
		     if (!check_int32($2)) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, $2);
		     }
		     if (current->type() == dods_array_c
			 && check_int32($2)) {
			 ((Array *)current)->append_dim(atoi($2));
		     }
		     else {
			 Array *a = NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi($2));
			 if( current ) delete current ;
			 current = a;
		     }
		 }

		 | '[' SCAN_WORD 
		 {
		     id = new string($2);
		 } 
                 '=' SCAN_WORD 
                 { 
		     if (!check_int32($5)) {
			 string msg = "In the dataset descriptor object:\n";
			 msg += "Expected an array subscript.\n";
			 parse_error((parser_arg *)arg, msg.c_str(), 
				 dds_line_num, $5);
		     }
		     if (current->type() == dods_array_c) {
			 ((Array *)current)->append_dim(atoi($5), *id);
		     }
		     else {
			 Array *a = NewArray(); 
			 a->add_var(current); 
			 a->append_dim(atoi($5), *id);
			 if( current ) delete current ;
			 current = a;
		     }

		     delete id;
		 }
		 ']'

		 | error
                 {
		     ostringstream msg;
		     msg << "In the dataset descriptor object:" << endl
			 << "Expected an array subscript." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(), 
				 dds_line_num, $<word>1);
		     YYABORT;
		 }
;

name:		var_name { (*DDS_OBJ(arg)).set_dataset_name($1); }
		| SCAN_DATASET { (*DDS_OBJ(arg)).set_dataset_name($1); }
                | error 
                {
		  ostringstream msg;
		  msg << "Error parsing the dataset name." << endl
		      << "The name may be missing or may contain an illegal character." << endl;
		     parse_error((parser_arg *)arg, msg.str().c_str(),
				 dds_line_num, $<word>1);
		     YYABORT;
		}
;

%%

/* 
   This function must be defined. However, use the error reporting code in
   parser-utils.cc.
*/

void 
ddserror(char *)
{
}

/*
  Invalid declaration message.
*/

void
invalid_declaration(parser_arg *arg, string semantic_err_msg, char *type, 
		    char *name)
{
  ostringstream msg;
  msg << "In the dataset descriptor object: `" << type << " " << name 
      << "'" << endl << "is not a valid declaration." << endl 
      << semantic_err_msg;
  parse_error((parser_arg *)arg, msg.str().c_str(), dds_line_num);
}

/*
  Add the variable pointed to by CURRENT to either the topmost ctor object on
  the stack CTOR or to the dataset variable table TABLE if CTOR is empty.  If
  it exists, the current ctor object is popped off the stack and assigned to
  CURRENT.

  NB: the ctor stack is popped for arrays because they are ctors which
  contain only a single variable. For other ctor types, several variables may
  be members and the parse rule (see `declaration' above) determines when to
  pop the stack.

  Returns: void 
*/

void	
add_entry(DDS &table, stack<BaseType *> **ctor, BaseType **current, Part part)
{ 
    if (!*ctor)
	*ctor = new stack<BaseType *>;

    if (!(*ctor)->empty()) { /* must be parsing a ctor type */
	(*ctor)->top()->add_var(*current, part);

 	const Type &ctor_type = (*ctor)->top()->type();

	if (ctor_type == dods_array_c) {
	    if( *current ) delete *current ;
	    *current = (*ctor)->top();
	    (*ctor)->pop();
	}
	else
	    return;
    }
    else
	table.add_var(*current);
}

/* 
 * $Log: dds.y,v $
 * Revision 1.43  2003/05/23 03:24:57  jimg
 * Changes that add support for the DDX response. I've based this on Nathan
 * Potter's work in the Java DAP software. At this point the code can
 * produce a DDX from a DDS and it can merge attributes from a DAS into a
 * DDS to produce a DDX fully loaded with attributes. Attribute aliases
 * are not supported yet. I've also removed all traces of strstream in
 * favor of stringstream. This code should no longer generate warnings
 * about the use of deprecated headers.
 *
 * Revision 1.42  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.41  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.40.2.1  2003/02/21 00:10:08  jimg
 * Repaired copyright.
 *
 * Revision 1.40  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.39  2003/01/10 19:46:41  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.33.4.7  2002/12/24 00:24:44  jimg
 * I removed a variable, current_save, that was unused after Patrick fixed a bug.
 *
 * Revision 1.33.4.6  2002/11/21 21:24:17  pwest
 * memory leak cleanup and file descriptor cleanup
 *
 * Revision 1.38  2002/06/03 22:21:16  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.37  2002/05/22 21:52:31  jimg
 * I added a new rule called start. This rule is used to initialize objects used
 * by the parser (like the stack of ctors). The logic of the parser have not
 * been changed. This just localizes the code to init this object.
 *
 * Revision 1.33.4.5  2001/11/03 10:08:07  rmorris
 * Fixed four lines that were using assignment "=" on addresses of strings.
 * Assumed a string copy was what was meant.  "$$ = $2" to "strcpy($$,$2)"
 * where the $$ and $2 generate vars that are string addresses.  Left note
 * to James to make sure what I assumed he meant was what he actually meant.
 *
 * Revision 1.33.4.4  2001/11/01 00:43:51  jimg
 * Fixes to the scanners and parsers so that dataset variable names may
 * start with digits. I've expanded the set of characters that may appear
 * in a variable name and made it so that all except `#' may appear at
 * the start. Some characters are not allowed in variables that appear in
 * a DDS or CE while they are allowed in the DAS. This makes it possible
 * to define containers with names like `COARDS:long_name.' Putting a colon
 * in a variable name makes the CE parser much more complex. Since the set
 * of characters that people want seems pretty limited (compared to the
 * complete ASCII set) I think this is an OK approach. If we have to open
 * up the expr.lex scanner completely, then we can but not without adding
 * lots of action clauses to teh parser. Note that colon is just an example,
 * there's a host of characters that are used in CEs that are not allowed
 * in IDs.
 *
 * Revision 1.36  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.33.4.3  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.35  2001/06/15 23:49:03  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.33.4.2  2001/05/08 19:10:47  jimg
 * Expanded the set of names that the dds.y parser will recognize to
 * include integers (for files named like 990412.nc). Also removed the
 * unused keywords Dependent and Independent from both the DDS scanner
 * and parser.
 * Added other reserved words to the set of possible Dataset names.
 *
 * Revision 1.33.4.1  2001/05/04 00:12:10  jimg
 * Added a rule that allows variable names to be the names of datatypes.
 *
 * Revision 1.33  2000/09/22 02:17:22  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.32  2000/09/21 16:22:10  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.31  2000/08/16 18:29:02  jimg
 * Added dot (.) to the set of characters allowed in a variable name
 *
 * Revision 1.30  2000/07/09 21:43:29  rmorris
 * Mods to increase portability, minimize ifdef's for win32
 *
 * Revision 1.29  2000/06/07 18:07:00  jimg
 * Merged the pc port branch
 *
 * Revision 1.28.6.1  2000/06/02 18:36:38  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.27.8.1  2000/02/17 05:03:17  jimg
 * Added file and line number information to calls to InternalErr.
 * Resolved compile-time problems with read due to a change in its
 * parameter list given that errors are now reported using exceptions.
 *
 * Revision 1.28  2000/01/27 06:30:00  jimg
 * Resolved conflicts from merge with release-3-1-4
 *
 * Revision 1.27.2.1  2000/01/24 22:25:10  jimg
 * Removed static global objects
 *
 * Revision 1.27  1999/07/22 17:07:47  jimg
 * Fixed a bug found by Peter Fox. Array index names were not handled properly
 * after the String to string conversion.
 *
 * Revision 1.26  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.25  1999/04/29 02:29:36  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.24  1999/03/24 23:32:33  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.23  1998/08/13 22:12:44  jimg
 * Fixed error messages.
 *
 * Revision 1.22.6.2  1999/02/05 09:32:36  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.22.6.1  1999/02/02 21:57:06  jimg
 * String to string version
 *
 * Revision 1.22  1997/11/20 20:14:10  jimg
 * Added to the name rule so that it recognizes both the ID and NAME lexeme
 * as valid when parsing the dataset name. NAME (see dds.lex) is just like ID
 * except that it includes `.'. Thus datasets with names like sst.reynolds.nc
 * now parse correctly.
 *
 * Revision 1.21  1997/02/28 01:31:22  jimg
 * Added error messages.
 *
 * Revision 1.20  1996/10/28 23:44:16  jimg
 * Added unsigned int to set of possible datatypes.
 *
 * Revision 1.19  1996/10/16 22:35:31  jimg
 * Fixed bad operator in DODS_BISON_VER preprocessor statement.
 *
 * Revision 1.18  1996/10/08 17:04:42  jimg
 * Added a fix for Bison 1.25 so that PARSE_PARAM will still work
 *
 * Revision 1.17  1996/08/13 20:54:45  jimg
 * Generated files.
 *
 * Revision 1.16  1996/05/31 23:27:17  jimg
 * Removed {YYACCEPT;} from rule 2 (dataset: DATASET ...).
 *
 * Revision 1.15  1996/05/29 21:59:51  jimg
 * *** empty log message ***
 *
 * Revision 1.14  1996/05/14 15:38:54  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.13  1996/04/05 21:59:38  jimg
 * Misc Changes for release 2.0.1 of the core software - for developers.
 *
 * Revision 1.12  1996/04/05 00:06:45  jimg
 * Merged changes from version 1.1.1.
 * Eliminated the static global CTOR.
 *
 * Revision 1.11  1995/12/06 19:45:08  jimg
 * Changed grammar so that List List ... <type> is no longer possible. This
 * fixed some hard problems in the serailize/deserailize mfuncs.
 *
 * Revision 1.10  1995/10/23  22:59:41  jimg
 * Modified some rules so that they use the functions defined in
 * parser_util.cc instead of local definitions.
 *
 * Revision 1.9  1995/08/23  00:27:47  jimg
 * Uses new member functions.
 * Added copyright notice.
 * Switched from String to enum type representation.
 *
 * Revision 1.8.2.1  1996/04/04 23:24:44  jimg
 * Removed static global CTOR from the dds parser. The stack for constructor
 * variable is now managed via a pointer. The stack is allocated when first
 * used by add_entry().
 *
 * Revision 1.8  1995/01/19  20:13:04  jimg
 * The parser now uses the new utility functions to create new instances
 * of the variable objects (Byte, ..., Grid).
 * Fixed the number of shift/reduce conflicts expected (now at 60).
 *
 * Revision 1.7  1994/12/22  04:30:57  reza
 * Made save_str static to avoid linking conflict.
 *
 * Revision 1.6  1994/12/16  22:24:23  jimg
 * Switched from a CtorType stack to BaseType stack.
 * Fixed an error in save_str() (see das.y).
 * Fixed a bug in the use of append_dim - it was called with $4 when it
 * should have been called with $5.
 *
 * Revision 1.5  1994/12/09  21:42:41  jimg
 * Added to array: so that an array decl can contain: an int or an id=int.
 * This is for the named dimensions (see Array.{cc,h}).
 *
 * Revision 1.4  1994/11/10  19:50:54  jimg
 * In the past it was possible to have a null file correctly parse as a
 * DAS or DDS. However, now that is not possible. It is possible to have
 * a file that contains no variables parse, but the keyword `Attribute'
 * or `Dataset' *must* be present. This was changed so that errors from
 * the CGIs could be detected (since they return nothing in the case of
 * a error).
 *
 * Revision 1.3  1994/09/23  14:56:19  jimg
 * Added code to build in-memory DDS during parse.
 *
 * Revision 1.2  1994/09/15  21:11:56  jimg
 * Modified dds.y so that it can parse all the DDS types.
 * Still no error checking beyond what bison gives you.
 *
 * Revision 1.1  1994/09/08  21:10:45  jimg
 * DDS Class test driver and parser and scanner.
 */

