/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

/*
  This is the parser for the DODS constraint expression grammar. The parser
  calls various `helper' functions defined by the DAP classes which either
  implement the operations (in the case of relational ops) or store
  information (in the case of selection operations). 

  jhrg 9/5/95
 */

/* $Log: expr.y,v $
/* Revision 1.1  1995/10/13 03:04:08  jimg
/* First version. Incorporates Glenn's suggestions.
/*
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

static char rcsid[]={"$Id: expr.y,v 1.1 1995/10/13 03:04:08 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config_dap.h"
#include "debug.h"
#include "expr.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

static char operand1[ID_MAX];	/* holds operand 1's text */
static char operand2[ID_MAX];

void mem_list_report();
int exprlex(void);		/* the scanner; see expr.lex */
int exprerror(char *s);

%}

%expect 6

%token ID
%token INT
%token FLOAT
%token STR

%token EQUAL
%token NOT_EQUAL
%token GREATER
%token GREATER_EQL
%token LESS
%token LESS_EQL
%token REGEXP

%%

constraint_expr: /* empty */
		| selection
		| projection
		| projection '&' selection
;

projection:	ID
		| field_sel
		| projection ',' ID
		| projection ',' field_sel
;

selection:	clause
		| selection '&' clause
;

clause:		operand rel_op '{' list '}'
		| operand rel_op operand
		| operand
;

operand:	ID
		| INT
		| FLOAT
		| field_sel
		| array_sel
		| '*' STR
;

field_sel:	ID '.' ID
		| field_sel '.' ID
;

array_sel:	ID array_index
		| array_sel array_index
;

array_index:	'[' INT ':' INT ':' INT ']'
		| '[' INT ':' INT ']'
;

list:		list_str
		| list_int
		| list_float
;

list_str:	STR
		| ID 
		| list_str ',' STR
		| list_str ',' ID
;

list_int:	INT
		| list_int ',' INT
;

list_float:	FLOAT
		| list_float ',' FLOAT
;

rel_op:		EQUAL
		| NOT_EQUAL
		| GREATER
		| GREATER_EQL
		| LESS
		| LESS_EQL
		| REGEXP
;

%%

int 
exprerror(char *s)
{
    fprintf(stderr, "%s\n");
}

