
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

// Test the DDS scanner, parser and DDS class.
//
// jhrg 8/29/94

// $Log: dds-test.cc,v $
// Revision 1.10  1996/04/05 00:22:14  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.9  1995/10/23  22:56:32  jimg
// Added Log and RCSID.
//

static char rcsid[]= {"$Id: dds-test.cc,v 1.10 1996/04/05 00:22:14 jimg Exp $"};

#include <iostream.h>
#include <GetOpt.h>

#define YYSTYPE char *

#include "dds.tab.h"
#include "BaseType.h"
#include "Int32.h"
#include "DDS.h"
#include "util.h"

void test_scanner();
void test_parser();
void test_class();

int ddslex();
int ddsparse(DDS &);

extern YYSTYPE ddslval;
extern int ddsdebug;
const char *prompt = "dds-test: ";

int
main(int argc, char *argv[])
{
    GetOpt getopt (argc, argv, "spdc");
    int option_char;
    int scanner_test = 0, parser_test = 0, class_test = 0;

    // process options

    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'd': 
	      ddsdebug = 1;
	      break;
	    case 's':
	      scanner_test = 1;
	      break;
	    case 'p':
	      parser_test = 1;
	      break;
	    case 'c':
	      class_test = 1;
	      break;
	    case '?': 
	      fprintf (stderr, "usage: %s [d] filename ...\n", argv[0]);
	  }

    if (scanner_test) {
	test_scanner();
    }

    if (parser_test) {
	test_parser();
    }

    if (class_test) {
	test_class();
    }
}

void
test_scanner(void)
{
    int tok;

    cout << prompt;		// first prompt
    while ((tok = ddslex())) {
	switch (tok) {
	  case DATASET:
	    cout << "DATASET" << endl;
	    break;
	  case DEPENDENT:
	    cout << "DEPENDENT" << endl;
	    break;
	  case INDEPENDENT:
	    cout << "INDEPENDENT" << endl;
	    break;
	  case ARRAY:
	    cout << "ARRAY" << endl;
	    break;
	  case MAPS:
	    cout << "MAPS" << endl;
	    break;
	  case LIST:
	    cout << "LIST" << endl;
	    break;
	  case SEQUENCE:
	    cout << "SEQUENCE" << endl;
	    break;
	  case STRUCTURE:
	    cout << "STRUCTURE" << endl;
	    break;
	  case FUNCTION:
	    cout << "FUNCTION" << endl;
	    break;
	  case GRID:
	    cout << "GRID" << endl;
	    break;
	  case BYTE:
	    cout << "BYTE" << endl;
	    break;
	  case INT32:
	    cout << "INT32" << endl;
	    break;
	  case FLOAT64:
	    cout << "FLOAT64" << endl;
	    break;
	  case STRING:
	    cout << "STRING" << endl;
	    break;
	  case URL:
	    cout << "Url" << endl;
	    break;
	  case ID:
	    cout << "ID: " << ddslval << endl;
	    break;
	  case INTEGER:
	    cout << "INTEGER: " << ddslval << endl;
	    break;
	  case '{':
	    cout << "Left Brace" << endl;
	    break;
	  case '}':
	    cout << "Right Brace" << endl;
	    break;
	  case '[':
	    cout << "Left Bracket" << endl;
	    break;
	  case ']':
	    cout << "Right Bracket" << endl;
	    break;
	  case ';':
	    cout << "Semicolon" << endl;
	    break;
	  case ':':
	    cout << "Colon" << endl;
	    break;
	  case '=':
	    cout << "Assignment" << endl;
	    break;
	  default:
	    cout << "Error: Unrecognized input" << endl;
	}
	cout << prompt;		// print prompt after output
    }
}

void
test_parser(void)
{
    DDS table;
    int status = table.parse();
    cout << "Status from parser: " << status << endl;
    
    if (table.check_semantics())
	cout << "DDS past semantic check" << endl;
    else 
	cout << "DDS failed semantic check" << endl;

    if (table.check_semantics(true))
	cout << "DDS past full semantic check" << endl;
    else 
	cout << "DDS failed full semantic check" << endl;

    table.print();
}

void
test_class(void)
{
    DDS table;
    int status = table.parse();
    cout << "Status from parser: " << status << endl;
    
    if (table.check_semantics())
	cout << "DDS past semantic check" << endl;
    else 
	cout << "DDS filed semantic check" << endl;

    if (table.check_semantics(true))
	cout << "DDS past full semantic check" << endl;
    else 
	cout << "DDS filed full semantic check" << endl;

    table.print();

    DDS table2 = table;		// test copy ctor;
    table2.print();

    DDS table3;
    table3 = table;		// test operator=

    cout << "Dataset name: " << table.get_dataset_name() << endl;

    String name = "goofy";
    table.add_var(NewInt32(name)); // table dtor should delete this object

    table.print();

    BaseType *btp = table.var(name);

    btp->print_decl(cout, true); // print out goofy w/semicolon

    table.del_var(name);

    table.print();

    table.add_var(NewInt32("goofy"));

    table.print();

    btp = table.var("goofy");

    btp->print_decl(cout, true); // print out goofy w/semicolon

    table.del_var("goofy");

    table.print();

    for (Pix p = table.first_var(); p; table.next_var(p))
	table.var(p)->print_decl(cout, true);	// print them all w/semicolons
}
