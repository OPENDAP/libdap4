
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


// Test the CE scanner and parser.
//
// jhrg 9/12/95

// $Log: expr-test.cc,v $
// Revision 1.2  1995/10/23 23:08:17  jimg
// Fixed scanner display code to match current scanner.
// Added code to test simple evaluator.
// Fixed type declarations (YYSTYPE, ...).
//
// Revision 1.1  1995/10/13  03:02:26  jimg
// First version. Runs scanner and parser.
//

static char rcsid[]= {"$Id: expr-test.cc,v 1.2 1995/10/23 23:08:17 jimg Exp $"};

#include <stdio.h>
#include <iostream.h>
#include <GetOpt.h>

#include <String.h>
#include <SLList.h>

#include "DDS.h"
#include "BaseType.h"
#include "expr.tab.h"

void test_scanner();
void test_parser(DDS &table, String &dds_name);
bool read_table(DDS &table, String &name, bool print);
void evaluate_dds(DDS &table);

int exprlex();			// exprlex() uses the global exprlval
int exprparse(DDS &table);

int exprrestart(FILE *in);

extern YYSTYPE exprlval;
extern int exprdebug;
const char *prompt = "expr-test: ";
const char *options = "sp:de";

int
main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, options);
    int option_char;
    int scanner_test = 0, parser_test = 0, evaluate_test = 0;
    String dds_file_name;
    DDS table;

    // process options

    while ((option_char = getopt()) != EOF)
	switch (option_char)
	  {
	    case 'd': 
	      exprdebug = 1;
	      break;
	    case 's':
	      scanner_test = 1;
	      break;
	    case 'p':
	      parser_test = 1;
	      dds_file_name = getopt.optarg;
	      break;
	    case 'e':
	      evaluate_test = 1;
	      break;
	    case '?': 
	      cerr << "usage: " << argv[0] << " " << options 
		   << " [filename]" << endl; 
	  }

    // run selected tests

    if (scanner_test) {
	test_scanner();
    }

    if (parser_test) {
	test_parser(table, dds_file_name);
    }

    if (evaluate_test) {
	evaluate_dds(table);
    }
}

void
test_scanner()
{
    int tok;

    cout << prompt;		// first prompt

    while ((tok = exprlex())) {
	switch (tok) {
	  case ID:
	    cout << "ID: " << exprlval.char_ptr << endl;
	    break;
	  case STR:
	    cout << "STR: " << exprlval.char_ptr << endl;
	    break;
	  case FIELD:
	    cout << "FIELD: " << exprlval.char_ptr << endl;
	    break;
	  case INT:
	    cout << "INT: " << exprlval.char_ptr << endl;
	    break;
	  case FLOAT:
	    cout << "FLOAT: " << exprlval.char_ptr << endl;
	    break;
	  case EQUAL:
	    cout << "EQUAL: " << exprlval.char_ptr << endl;
	    break;
	  case NOT_EQUAL:
	    cout << "NOT_EQUAL: " << exprlval.char_ptr << endl;
	    break;
	  case GREATER:
	    cout << "GREATER: " << exprlval.char_ptr << endl;
	    break;
	  case GREATER_EQL:
	    cout << "GREATER_EQL: " << exprlval.char_ptr << endl;
	    break;
	  case LESS:
	    cout << "LESS: " << exprlval.char_ptr << endl;
	    break;
	  case LESS_EQL:
	    cout << "LESS_EQL: " << exprlval.char_ptr << endl;
	    break;
	  case REGEXP:
	    cout << "REGEXP: " << exprlval.char_ptr << endl;
	    break;
	  case '*':
	    cout << "Dereference" << endl;
	    break;
	  case '.':
	    cout << "Field Selector" << endl;
	    break;
	  case ',':
	    cout << "List Element Separator" << endl;
	    break;
	  case '[':
	    cout << "Left Bracket" << endl;
	    break;
	  case ']':
	    cout << "Right Bracket" << endl;
	    break;
	  case '(':
	    cout << "Left Paren" << endl;
	    break;
	  case ')':
	    cout << "Right Paren" << endl;
	    break;
	  case '{':
	    cout << "Left Brace" << endl;
	    break;
	  case '}':
	    cout << "Right Brace" << endl;
	    break;
	  case ':':
	    cout << "Colon" << endl;
	    break;
	  case '&':
	    cout << "Ampersand" << endl;
	    break;
	  default:
	    cout << "Error: Unrecognized input" << endl;
	}
	cout << prompt;		// print prompt after output
    }
}

// NB: The DDS is read in via a file because reading from stdin must be
// terminated by EOF. However, the EOF used to terminate the DDS also closes
// stdin and thus the expr scanner exits immediately.

void
test_parser(DDS &table, String &dds_name)
{
    cout << "Enter a DDS:" << endl;

    read_table(table, dds_name, true);

    exprrestart(stdin);

    cout << prompt;

    if (exprparse(table) == 0)
	cout << "Input parsed" << endl;
    else
	cout << "Input did not parse" << endl;
}

// Read a DDS from stdin and build the cooresponding DDS. IF PRINT is true,
// print the text reprsentation of that DDS on the stdout. The DDS TABLE is
// modified as a side effect.
//
// Returns: true iff that DDS pasted the semantic_check() mfunc, otherwise
// false.

bool
read_table(DDS &table, String &name, bool print)
{
    int parse = table.parse(name);
    
    if (!parse) {
	cout << "Input did not parse" << endl;
	return false;
    }
    
    if (print)
	table.print();

    if (table.check_semantics(true))
	return true;
    else {
	cout << "Input did not pass semantic checks" << endl;
	return false;
    }
}

void
evaluate_dds(DDS &table)
{
    for (Pix p = table.first_var(); p; table.next_var(p))
	table.var(p)->print_decl(cout, "", true, true);
}
