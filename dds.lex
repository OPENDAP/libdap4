
/*
 -*- mode: c++; c-basic-offset:4 -*-

 This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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
   Scanner for the DDS. This file works with gnu's flex scanner generator. It
   returns either DATASET, INDEPENDENT, DEPENDENT, ARRAY, MAPS, LIST,
   SEQUENCE, STRUCTURE, FUNCTION, GRID, BYTE, INT32, FLOAT64, STRING, URL, ID
   or one of the single character tokens `{', `}', `;', `=' or `\n' as
   integers. In the case of an ID, the scanner stores a pointer to the lexeme
   in yylval (whose type is char *).

   The scanner discards all comment text.

   The scanner is not reentrant, but can share name spaces with other
   scanners.
   
   Note:
   1) The `defines' file dds.tab.h is built using `bison -d'.
   2) Define YY_DECL such that the scanner is called `ddslex'.
   3) When bison builds the dds.tab.h file, it uses `dds' instead of `yy' for
   variable name prefixes (e.g., yylval --> ddslval).
   
   jhrg 8/29/94 
*/

%{

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: dds.lex,v 1.35 2003/01/23 00:22:24 jimg Exp $"};

#include <string.h>

// #include "Error.h"
#include "parser.h"
#include "dds.tab.h"
#include "escaping.h"

#define YY_DECL int ddslex YY_PROTO(( void ))
#define YY_INPUT(buf,result,max_size) { \
    fgets((buf), (max_size), yyin); \
    result = (feof(yyin) || strcmp(buf, "Data:\n") == 0) \
             ? YY_NULL : strlen(buf); \
}

int dds_line_num = 1;

static void store_word();

%}
    
%option noyywrap
%x comment

DATASET 	DATASET|Dataset|dataset 
LIST 		LIST|List|list
SEQUENCE 	SEQUENCE|Sequence|sequence
STRUCTURE 	STRUCTURE|Structure|structure
GRID 		GRID|Grid|grid
BYTE 		BYTE|Byte|byte
INT16 		INT16|Int16|int16
UINT16 		UINT16|UInt16|uint16
INT32 		INT32|Int32|int32
UINT32 		UINT32|UInt32|uint32
FLOAT32 	FLOAT32|Float32|float32
FLOAT64 	FLOAT64|Float64|float64
STRING 		STRING|String|string
URL 		URL|Url|url

/* See das.lex for comments about the characters allowed in a WORD.
   10/31/2001 jhrg */

WORD            [-+a-zA-Z0-9_/%.\\*][-+a-zA-Z0-9_/%.\\#*]*

NEVER		[^\-+a-zA-Z0-9_/%.\\#;,(){}[\]]

%%

"{" 	    	    	return (int)*yytext;
"}" 	    	    	return (int)*yytext;
"["			return (int)*yytext;
"]"			return (int)*yytext;
":"			return (int)*yytext;
";" 	    	    	return (int)*yytext;
"="			return (int)*yytext;

{DATASET}		store_word(); return SCAN_DATASET;
{LIST}			store_word(); return SCAN_LIST;
{SEQUENCE}		store_word(); return SCAN_SEQUENCE;
{STRUCTURE}		store_word(); return SCAN_STRUCTURE;
{GRID}			store_word(); return SCAN_GRID;
{BYTE}			store_word(); return SCAN_BYTE;
{INT16}			store_word(); return SCAN_INT16;
{UINT16}		store_word(); return SCAN_UINT16;
{INT32}			store_word(); return SCAN_INT32;
{UINT32}		store_word(); return SCAN_UINT32;
{FLOAT32}		store_word(); return SCAN_FLOAT32;
{FLOAT64}		store_word(); return SCAN_FLOAT64;
{STRING}		store_word(); return SCAN_STRING;
{URL}			store_word(); return SCAN_URL;

{WORD}                  store_word(); return SCAN_WORD;

[ \t\r]+
\n	    	    	++dds_line_num;
<INITIAL><<EOF>>    	yy_init = 1; dds_line_num = 1; yyterminate();

"#"	    	    	BEGIN(comment);
<comment>[^\n]*
<comment>\n		++dds_line_num; BEGIN(INITIAL);
<comment><<EOF>>        yy_init = 1; dds_line_num = 1; yyterminate();

"Data:\n"		yyterminate();
"Data:\r\n"		yyterminate();

{NEVER}                 {
                          if (yytext) {	/* suppress msgs about `' chars */
                            fprintf(stderr, "Character `%c' is not", *yytext);
                            fprintf(stderr, " allowed and has been ignored\n");
			  }
			}
%%

static void
store_word()
{
    // dods2id(string(yytext)).c_str()
    strncpy(ddslval.word, yytext, ID_MAX-1);
    ddslval.word[ID_MAX-1] = '\0'; // for the paranoid...
}


/* 
 * $Log: dds.lex,v $
 * Revision 1.35  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.34  2003/01/10 19:46:41  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.30.2.5  2002/06/11 00:40:52  jimg
 * I added '*' to the set of characters allowed in a WORD in both the DAS
 * and DDS scanners. It's not allowed in the expr scanner because that
 * would cause conflicts with the URL dereference operator.
 *
 * Revision 1.33  2002/06/03 22:21:15  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.30.2.4  2001/11/01 00:43:51  jimg
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
 * Revision 1.32  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.30.2.3  2001/08/16 17:26:20  edavis
 * Use "%option noyywrap" instead of defining yywrap() to return 1.
 *
 * Revision 1.30.2.2  2001/06/23 00:52:08  jimg
 * Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
 * that they are (more or less) the same in all the scanners. There are
 * one or two characters that differ (for example das.lex allows ( and )
 * in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
 * are essentially the same across the board.
 * Added `#' to the set of characeters allowed in an ID (bug 179).
 *
 * Revision 1.31  2001/06/15 23:49:03  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.30.2.1  2001/05/08 19:10:47  jimg
 * Expanded the set of names that the dds.y parser will recognize to
 * include integers (for files named like 990412.nc). Also removed the
 * unused keywords Dependent and Independent from both the DDS scanner
 * and parser.
 * Added other reserved words to the set of possible Dataset names.
 *
 * Revision 1.30  2000/10/30 17:21:28  jimg
 * Added support for proxy servers (from cjm).
 *
 * Revision 1.29  2000/09/07 23:08:52  jimg
 * Fixed another goof from the last merge; I dropped the new feature that
 * identifiers can have dots i their names. This feature could really screw
 * things up, but it is needed for translation. It should be the case that the
 * CE evaluator never sees such a variable name (since the translation software
 * should get to it first and rearrange things).
 *
 * Revision 1.28  2000/09/07 16:17:20  jimg
 * Added SCAN_ prefix to returned constants. The prefix was lost during the
 * last merge.
 *
 * Revision 1.27  2000/08/31 23:44:16  jimg
 * Merged with 3.1.10
 *
 * Revision 1.24.6.1  2000/08/31 20:49:55  jimg
 * Added \r to the set of characters that are ignored. This is an untested fix
 * (?) for UNIX clients that read from servers run on win32 machines (e.g.,
 * the Java-SQL server can be run on a win32 box).
 *
 * Revision 1.24  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.23  1999/04/29 02:29:36  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.22  1999/03/24 23:32:49  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.21  1999/02/23 01:30:57  jimg
 * Added a YYINPUT define that looks for `Data:\n'. When seen it returns
 * YY_NULL which tells the scanner that EOF has been found. This hack keeps
 * flex from reading into the data while buffering from the input source. This
 * means that the calling code does not have to rewind the input to find the
 * start of the data. Of course, multi-part MIME docs would also solve this.
 * Replace this hack with MP/MIME code!
 *
 * Revision 1.20  1999/01/21 02:51:27  jimg
 * The dds scanner now recognizes the token `Data:' as an EOF marker. This
 * means that the data document can be scanned without splitting the DDS and
 * binary data parts into two files.
 *
 * Revision 1.19.6.1  1999/02/05 09:32:36  jimg
 * Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
 * math code. 
 *
 * Revision 1.19  1998/03/26 00:26:24  jimg
 * Added % to the set of characters that can start and ID
 *
 * Revision 1.18  1998/02/05 20:14:01  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.17  1997/12/16 00:46:29  jimg
 * Added `-' to characters allowed in NAME lexeme.
 *
 * Revision 1.16  1997/11/20 20:12:14  jimg
 * Added the NAME lexeme. This is explicitly for datasets with `.' in their
 * names. That character cannot be used in the name of a variable since in
 * DODS it separates parts of a constructor type. However, it can be part of
 * a dataset name. Using a separate lexeme is required by the parser.
 *
 * Revision 1.15  1997/08/11 18:19:34  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.14  1996/10/28 23:43:17  jimg
 * Added unsigned int to set of possible datatypes.
 *
 * Revision 1.13  1996/10/08 17:10:50  jimg
 * Added % to the set of characters allowable in identifier names
 *
 * Revision 1.12  1996/08/26 21:13:13  jimg
 * Changes for version 2.07
 *
 * Revision 1.11  1996/08/13 18:51:52  jimg
 * Added not_used to definition of char rcsid[].
 *
 * Revision 1.10  1996/05/31 23:30:55  jimg
 * Updated copyright notice.
 *
 * Revision 1.9  1996/04/05 00:22:16  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.8  1995/10/23  23:00:24  jimg
 * Removed code enclosed in #ifdef NEVER ... #endif
 *
 * Revision 1.7  1995/07/08  20:18:25  jimg
 * Removed declaration of yywrap().
 *
 * Revision 1.6  1995/02/10  02:57:40  jimg
 * Switched to sh style comments. C and C++ style comments are no longer
 * supported.
 *
 * Revision 1.5  1994/12/21  16:54:50  jimg
 * Repaired damage done to NEVER's definition in previous version.
 *
 * Revision 1.4  1994/12/16  22:22:14  jimg
 * Changed NEVER to be anything not caught by the earlier rules.
 * Fixed // style comments so that // ... <eof> works.
 *
 * Revision 1.3  1994/12/09  21:40:44  jimg
 * Added `=' to the set of recognized lexemes.
 * Added `[' and `]' to the set of rejected characters.
 *
 * Revision 1.2  1994/11/10  19:46:49  jimg
 * Added `/' to the set of characters that make up an identifier.
 *
 * Revision 1.1  1994/09/08  21:10:46  jimg
 * DDS Class test driver and parser and scanner.
 */
