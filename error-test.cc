
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Test the Error object scanner, parser and class.
//
// jhrg 4/25/96

// $Log: error-test.cc,v $
// Revision 1.2  1996/08/13 18:52:52  jimg
// Added __unused__ to definition of char rcsid[].
// Now tests the Gui.
//
// Revision 1.1  1996/05/31 23:28:16  jimg
// Added.
//

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: error-test.cc,v 1.2 1996/08/13 18:52:52 jimg Exp $"};

#include <assert.h>

#include <iostream.h>
#include <GetOpt.h>

#include "Error.h"
#include "parser.h"
#include "Error.tab.h"

void test_scanner();
void test_parser(Error &err);
void test_object(Error &err);
void usage();

int Errorlex();
int Errorparse(void *);

extern YYSTYPE Errorlval;
extern int Errordebug;
const char *prompt = "error-test: ";

int
main(int argc, char *argv[])
{
    GetOpt getopt (argc, argv, "spdo");
    int option_char;
    bool scanner_test = false, parser_test = false, object_test = false;

    // process options

    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'd': 
	      Errordebug = 1;
	      break;
	    case 's':
	      scanner_test = true;
	      break;
	    case 'p':
	      parser_test = true;
	      break;
	    case 'o':
	      parser_test = object_test = true;
	      break;
	    case '?': 
	    default:
	      usage();
	  }

    if (!(scanner_test || parser_test || object_test))
	usage();

    if (scanner_test)
	test_scanner();

    Error err;
    if (parser_test)
	test_parser(err);

    if (object_test)
	test_object(err);
}

void
usage(void)
{
    cerr << "usage: " << "error-test: [d][spo] <  filename ..."  << endl;
    cerr << "       " << "d: extra parser debugging information" << endl;
    cerr << "       " << "s: run the scanner" << endl;
    cerr << "       " << "p: run the parser" << endl;
    cerr << "       " << "o: evaluate the object, runs the parser" << endl;
}

void
test_scanner(void)
{
    int tok;

    cout << prompt;		// first prompt
    while ((tok = Errorlex())) {
	switch (tok) {
	  case ERROR:
	    cout << "ERROR" << endl;
	    break;
	  case CODE:
	    cout << "CODE" << endl;
	    break;
	  case PTYPE:
	    cout << "PTYPE" << endl;
	    break;
	  case MSG:
	    cout << "MSG" << endl;
	    break;
	  case PROGRAM:
	    cout << "PROGRAM" << endl;
	    break;
	  case STR:
	    cout << Errorlval.string << endl;
	    break;
	  case INT:
	    cout << Errorlval.integer << endl;
	    break;
	  case '{':
	    cout << "Left Brace" << endl;
	    break;
	  case '}':
	    cout << "Right Brace" << endl;
	    break;
	  case ';':
	    cout << "Semicolon" << endl;
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
test_parser(Error &err)
{
    int status = err.parse(stdin);
    cout << "Status from parser: " << status << endl;
    
    if (err.OK())
	cout << "Error passed OK check" << endl;
    else 
	cout << "Error failed OK check" << endl;

    err.print();
}

void
test_object(Error &err)
{
    Gui g;

    String response = err.correct_error(&g);
    
    cout << "Response: " << response << endl;
}
