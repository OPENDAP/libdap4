
/*
  Test the DDS scanner, parser and DDS class.

  jhrg 8/29/94
*/

#include <iostream.h>
#include <GetOpt.h>

#define YYSTYPE char *

#include "dds.tab.h"
#include "BaseType.h"
#include "Int32.h"
#include "DDS.h"

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
    table.add_var(new Int32(name)); // table dtor should delete this object

    table.print();

    BaseType *btp = table.var(name);

    btp->print_decl(cout, true); // print out goofy w/semicolon

    table.del_var(name);

    table.print();

    table.add_var(new Int32("goofy"));

    table.print();

    btp = table.var("goofy");

    btp->print_decl(cout, true); // print out goofy w/semicolon

    table.del_var("goofy");

    table.print();

    for (Pix p = table.first_var(); p; table.next_var(p))
	table.var(p)->print_decl(cout, true);	// print them all w/semicolons
}
