
/*
 -*- mode: c++; c-basic-offset:4 -*-

 This file is part of libdap, A C++ implementation of the OPeNDAP Data
 Access Protocol.

 Copyright (c) 2002,2003 OPeNDAP, Inc.
 Author: James Gallagher <jgallagher@opendap.org>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

 (c) COPYRIGHT URI/MIT 1994-1999
*/ 

/*
  Scanner for constraint expressions. The scanner returns tokens for each of
  the relational and selection operators. It requires GNU flex version 2.5.2
  or newer.

  The scanner is not reentrant, but can share a name space with other
  scanners. 

   Note:
   1) The `defines' file expr.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `exprlex'.
   3) When bison builds the expr.tab.h file, it uses `expr' instead of `yy'
   for variable name prefixes (e.g., yylval --> exprlval).

  jhrg 9/5/95
*/

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <string.h>
#include <assert.h>

#include <string>

#define YY_DECL int exprlex YY_PROTO(( void ))
#define YY_FATAL_ERROR(msg) throw(Error(string("Error scanning DAS object text: ") + string(msg)))

#include "Error.h"
#include "parser.h"
#include "expr.h"
#include "RValue.h"
#include "expr.tab.h"
#include "escaping.h"

static void store_id();
static void store_str();
static void store_op(int op);

%}

%option noyywrap
%x quote
    
/* In the DAS and DDS parsers I removed the INT and FLOAT lexemes. However,
   not having them here complicates parsing since you must check to see if a
   word is a number (like 2.3) or a variable called `2.3.' I'm assuming that
   people will always put some characters in variable names (e.g., they'll
   use `2300.7%20MHz' and not just `2300.7'). If that turns out to be a bad
   assumption, the we'll have to put more code in the parser to figure out
   what exactly each word is; is it a constant or a variable name. Time will
   tell. 10/31/2001 jhrg */

NAN		[Nn][Aa][Nn]
INF		[Ii][Nn][Ff]
/* See das.lex for comments about the characters allowed in a WORD.
   10/31/2001 jhrg 

   I've added '*' to the set of characters in a WORD for both the DDS and DAS
   scanners, but not here because it'll conflict with the url dereference
   operator. 6/10/2002 jhrg
*/

SCAN_WORD       [-+a-zA-Z0-9_/%.\\][-+a-zA-Z0-9_/%.\\#]*

SCAN_EQUAL	=
SCAN_NOT_EQUAL	!=
SCAN_GREATER	>
SCAN_GREATER_EQL >=
SCAN_LESS	<
SCAN_LESS_EQL	<=
SCAN_REGEXP	=~

NEVER		[^\-+a-zA-Z0-9_/%.\\#:;,(){}[\]*&<>=~]

%%

"["    	    	return (int)*yytext;
"]"    	    	return (int)*yytext;
":"    	    	return (int)*yytext;
"*"		return (int)*yytext;
","		return (int)*yytext;
"&"		return (int)*yytext;
"("		return (int)*yytext;
")"		return (int)*yytext;
"{"		return (int)*yytext;
"}"		return (int)*yytext;

{SCAN_WORD}	store_id(); return SCAN_WORD;

{SCAN_EQUAL}	store_op(SCAN_EQUAL); return SCAN_EQUAL;
{SCAN_NOT_EQUAL} store_op(SCAN_NOT_EQUAL); return SCAN_NOT_EQUAL;
{SCAN_GREATER}	store_op(SCAN_GREATER); return SCAN_GREATER;
{SCAN_GREATER_EQL} store_op(SCAN_GREATER_EQL); return SCAN_GREATER_EQL;
{SCAN_LESS}	store_op(SCAN_LESS); return SCAN_LESS;
{SCAN_LESS_EQL}	store_op(SCAN_LESS_EQL); return SCAN_LESS_EQL;
{SCAN_REGEXP}	store_op(SCAN_REGEXP); return SCAN_REGEXP;

[ \t\r\n]+
<INITIAL><<EOF>> yy_init = 1; yyterminate();

\"		BEGIN(quote); yymore();
<quote>[^"\\]*  yymore(); /*"*/
<quote>\\.	yymore();
<quote>\"	{ 
    		  BEGIN(INITIAL); 
                  store_str();
		  return SCAN_STR;
                }
<quote><<EOF>>	{
                  char msg[256];
		  sprintf(msg, "Unterminated quote\n");
		  YY_FATAL_ERROR(msg);
                }

{NEVER}         {
                  if (yytext) {	/* suppress msgs about `' chars */
                    fprintf(stderr, "Character `%c' is not", *yytext);
                    fprintf(stderr, " allowed and has been ignored\n");
		  }
		}
%%

// Three glue routines for string scanning. These are not declared in the
// header expr.tab.h nor is YY_BUFFER_STATE. Including these here allows them
// to see the type definitions in lex.expr.c (where YY_BUFFER_STATE is
// defined) and allows callers to declare them (since callers outside of this
// file cannot declare the YY_BUFFER_STATE variable). Note that I changed the
// name of the expr_scan_string function to expr_string because C++ cannot
// distinguish by return type. 1/12/99 jhrg

void *
expr_string(const char *str)
{
    return (void *)expr_scan_string(str);
}

void
expr_switch_to_buffer(void *buf)
{
    expr_switch_to_buffer((YY_BUFFER_STATE)buf);
}

void
expr_delete_buffer(void *buf)
{
    expr_delete_buffer((YY_BUFFER_STATE)buf);
}

static void
store_id()
{
    strncpy(exprlval.id, dods2id(string(yytext)).c_str(), ID_MAX-1);
    exprlval.id[ID_MAX-1] = '\0';
}

static void
store_str()
{
    // transform %20 to a space. 7/11/2001 jhrg
    string *s = new string(dods2id(string(yytext)));  // XXX memory leak?

    if (*s->begin() == '\"' && *(s->end()-1) == '\"') {
	s->erase(s->begin());
	s->erase(s->end()-1);
    }

    exprlval.val.type = dods_str_c;
    exprlval.val.v.s = s;
}

static void
store_op(int op)
{
    exprlval.op = op;
}

/* 
 * $Log: expr.lex,v $
 * Revision 1.30  2003/12/08 18:02:30  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.29.2.1  2003/10/03 16:25:02  jimg
 * I changed the way the scanners handle errors. They were calling
 * YY_FATAL_ERROR and using the default value which prints a msg to stderr
 * and calls exit(1). I've changed that to a new sniplet that throws an
 * exception (Error). In addition, some of the scanners would ignore
 * illegal characters; they now treat those as fatal errors.
 *
 * Revision 1.29  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.28  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.27.2.1  2003/02/21 00:10:08  jimg
 * Repaired copyright.
 *
 * Revision 1.27  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.26  2003/01/10 19:46:41  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.23.4.7  2002/11/06 21:53:06  jimg
 * I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
 * make depend will include the header in the list of dependencies.
 *
 * Revision 1.23.4.6  2002/06/11 00:40:52  jimg
 * I added '*' to the set of characters allowed in a WORD in both the DAS
 * and DDS scanners. It's not allowed in the expr scanner because that
 * would cause conflicts with the URL dereference operator.
 *
 * Revision 1.25  2002/06/03 22:21:16  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.23.4.5  2002/02/20 19:16:27  jimg
 * Changed the expression parser so that variable names may contain only
 * digits.
 *
 * Revision 1.23.4.4  2001/11/01 00:43:51  jimg
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
 * Revision 1.24  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.23.4.3  2001/08/16 17:26:20  edavis
 * Use "%option noyywrap" instead of defining yywrap() to return 1.
 *
 * Revision 1.23.4.2  2001/07/28 01:10:42  jimg
 * Some of the numeric type classes did not have copy ctors or operator=.
 * I added those where they were needed.
 * In every place where delete (or delete []) was called, I set the pointer
 * just deleted to zero. Thus if for some reason delete is called again
 * before new memory is allocated there won't be a mysterious crash. This is
 * just good form when using delete.
 * I added calls to www2id and id2www where appropriate. The DAP now handles
 * making sure that names are escaped and unescaped as needed. Connect is
 * set to handle CEs that contain names as they are in the dataset (see the
 * comments/Log there). Servers should not handle escaping or unescaping
 * characters on their own.
 *
 * Revision 1.23.4.1  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.23  2000/08/31 23:44:16  jimg
 * Merged with 3.1.10
 *
 * Revision 1.21.6.1  2000/08/31 20:45:26  jimg
 * Added \r to the set of characters that are ignored. This is an untested fix
 * (?) for UNIX clients that read from servers run on win32 machines (e.g.,
 * the Java-SQL server can be run on a win32 box).
 *
 * Revision 1.21  1999/04/29 02:29:36  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.20  1999/03/24 23:30:55  jimg
 * Fixed some of the comments.
 *
 * Revision 1.19  1999/01/21 02:21:44  jimg
 * Made the store_op(), ... functions static.
 * Added glue routines for scanning strings.
 *
 * Revision 1.18  1998/10/23 00:09:03  jimg
 * Fixed an array write error where exprlval.id was over-written by writing to
 * element ID_MAX. The end of the array is ID_MAX-1.
 *
 * Revision 1.17  1998/10/21 16:45:50  jimg
 * Now includes RValue.h. Needed because expr.tab.h needs it.
 *
 * Revision 1.16.6.2  1999/02/05 09:32:36  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.16.6.1  1999/02/02 21:57:07  jimg
 * String to string version
 *
 * Revision 1.16  1998/03/26 00:26:23  jimg
 * Added % to the set of characters that can start and ID
 *
 * Revision 1.15  1997/08/11 18:19:36  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.14  1997/02/24 18:18:23  jimg
 * Removed `rule' for "." since it cannot be matched.
 *
 * Revision 1.13  1996/11/13 19:23:15  jimg
 * Fixed debugging.
 *
 * Revision 1.12  1996/10/08 17:10:52  jimg
 * Added % to the set of characters allowable in identifier names
 *
 * Revision 1.11  1996/08/26 21:13:17  jimg
 * Changes for version 2.07
 *
 * Revision 1.10  1996/08/13 18:56:24  jimg
 * Added not_used to definition of char rcsid[].
 *
 * Revision 1.9  1996/05/31 23:31:03  jimg
 * Updated copyright notice.
 *
 * Revision 1.8  1996/05/14 15:39:01  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.7  1996/04/05 00:22:19  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.6  1996/03/02 01:19:04  jimg
 * Fixed comments.
 * Fixed a bug in store_str(); leading and trailing double quotes are now
 * stripped from strings.
 *
 * Revision 1.5  1996/02/01 17:43:16  jimg
 * Added support for lists as operands in constraint expressions.
 *
 * Revision 1.4  1995/12/09  01:07:39  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.3  1995/12/06  18:57:37  jimg
 * Because the %union{} changed, the return types of some of the rules also
 * changed.
 * Returns integer codes for relops.
 * Returns a tagged union for most other values.
 *
 * Revision 1.2  1995/10/23  23:11:31  jimg
 * Fixed scanner to use the new definition of YYSTYPE.
 *
 * Revision 1.1  1995/10/13  03:03:17  jimg
 * Scanner. Incorporates Glenn's suggestions.
 *
 */
