
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Test the DDS scanner, parser and DDS class.
//
// jhrg 8/29/94

// $Log: dds-test.cc,v $
// Revision 1.15  1999/04/29 02:29:35  jimg
// Merge of no-gnu branch
//
// Revision 1.14  1999/03/24 23:33:11  jimg
// Added support for the new Int16, UInt16 and Float32 types.
//
// Revision 1.13.4.2  1999/02/05 09:32:36  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.13.4.1  1999/02/02 21:57:06  jimg
// String to string version
//
// Revision 1.13  1997/12/16 00:45:41  jimg
// Added code for NAME lexeme.
//
// Revision 1.12  1996/08/13 18:49:55  jimg
// Added on-line help.
// Now, always returns an exit code.
//
// Revision 1.11  1996/05/31 23:30:53  jimg
// Updated copyright notice.
//
// Revision 1.10  1996/04/05 00:22:14  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.9  1995/10/23  22:56:32  jimg
// Added Log and RCSID.
//

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: dds-test.cc,v 1.15 1999/04/29 02:29:35 jimg Exp $"};

#include <iostream>
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

void
usage(string name)
{
    cerr << "usage: " << name
	 << " [s] [pd] [c]" << endl
	 << " s: Test the scanner." << endl
	 << " p: Test the parser; reads from stdin and prints the" << endl
	 << "    internal structure to stdout." << endl
	 << " d: Turn on parser debugging. (only for the hard core.)" << endl
	 << " c: Test the C++ code for manipulating DDS objects." << endl
	 << "    Reads from stdin, parses and writes the modified DDS" << endl
	 << "    to stdout." << endl;
}

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
	    default:
	      usage(argv[0]);
	      exit(1);
	  }

    if (!scanner_test && !parser_test && !class_test) {
	usage(argv[0]);
	exit(1);
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

    exit(0);
}

void
test_scanner(void)
{
    int tok;

    cout << prompt << flush;		// first prompt
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
	  case INT16:
	    cout << "INT16" << endl;
	    break;
	  case UINT16:
	    cout << "UINT16" << endl;
	    break;
	  case INT32:
	    cout << "INT32" << endl;
	    break;
	  case UINT32:
	    cout << "UINT32" << endl;
	    break;
	  case FLOAT32:
	    cout << "FLOAT32" << endl;
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
	  case NAME:
	    cout << "NAME: " << ddslval << endl;
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
	cout << prompt << flush;		// print prompt after output
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

    string name = "goofy";
    table.add_var(NewInt32(name)); // table dtor should delete this object

    table.print();

    BaseType *btp = table.var(name);

    btp->print_decl(cout, "", true); // print out goofy w/semicolon

    table.del_var(name);

    table.print();

    table.add_var(NewInt32("goofy"));

    table.print();

    btp = table.var("goofy");

    btp->print_decl(cout, "", true); // print out goofy w/semicolon

    table.del_var("goofy");

    table.print();

    for (Pix p = table.first_var(); p; table.next_var(p))
	table.var(p)->print_decl(cout, "", true);	// print them all w/semicolons
}
