
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Test the Error object scanner, parser and class.
//
// jhrg 4/25/96

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: error-test.cc,v 1.8 2000/09/22 02:17:22 jimg Exp $"};

#include <assert.h>

#include <iostream>
#include <GetOpt.h>

#include "Error.h"
#include "parser.h"
#include "Error.tab.h"

#ifdef WIN32
using std::cerr;
using std::endl;
using std::flush;
#endif

void test_scanner();
void test_parser(Error &err);
#ifdef GUI
void test_object(Error &err);
#endif
void usage();

int Errorlex();
int Errorparse(void *);

extern YYSTYPE Errorlval;
extern int Errordebug;
const char *prompt = "error-test: ";

#ifdef WIN32
void
#else
int
#endif
main(int argc, char *argv[])
{
#ifdef WIN32
    GetOpt getopt (argc, argv, "spd");
#else
    GetOpt getopt (argc, argv, "spdo");
#endif
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
#ifndef WIN32
	    case 'o':
	      parser_test = object_test = true;
	      break;
#endif
	    case '?': 
	    default:
	      usage();
	  }

#ifdef WIN32
	if (!(scanner_test || parser_test))
#else
    if (!(scanner_test || parser_test || object_test))
#endif
	usage();

    if (scanner_test)
	test_scanner();

    Error err;
    if (parser_test)
	test_parser(err);

#ifdef GUI
    if (object_test)
	test_object(err);
#endif

#ifdef WIN32
    exit(0);  //  Cygwin/Dejagu test suites require this to succeed.
    return;   //  Visual C++ requires this.
#endif
}

void
usage()
{
#ifdef WIN32
    cerr << "usage: " << "error-test: [d][sp] <  filename ..."  << endl;
#else
    cerr << "usage: " << "error-test: [d][spo] <  filename ..."  << endl;
#endif
    cerr << "       " << "d: extra parser debugging information" << endl;
    cerr << "       " << "s: run the scanner" << endl;
    cerr << "       " << "p: run the parser" << endl;
#ifdef WIN32
    cerr << "       " << "o: evaluate the object, runs the parser" << endl;
#endif
}

void
test_scanner()
{
    int tok;

    cout << prompt << flush;		// first prompt
    while ((tok = Errorlex())) {
	switch (tok) {
	  case SCAN_ERROR:
	    cout << "ERROR" << endl;
	    break;
	  case SCAN_CODE:
	    cout << "CODE" << endl;
	    break;
	  case SCAN_PTYPE:
	    cout << "PTYPE" << endl;
	    break;
	  case SCAN_MSG:
	    cout << "MSG" << endl;
	    break;
	  case SCAN_PROGRAM:
	    cout << "PROGRAM" << endl;
	    break;
	  case SCAN_STR:
	    cout << Errorlval.string << endl;
	    break;
	  case SCAN_INT:
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
	cout << prompt << flush;		// print prompt after output
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

#ifndef WIN32
void
test_object(Error &err)
{
    Gui g;

    string response = err.correct_error(&g);
    
    cout << "Response: " << response << endl;
}
#endif

// $Log: error-test.cc,v $
// Revision 1.8  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.7  2000/07/19 22:51:40  rmorris
// Call and return from main in a manner Visual C++ likes and
// exit the program with exit(0) so that DejaGnu/Cygwin based
// testsuite can succeed for win32.
//
// Revision 1.6  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.5  2000/06/07 18:07:00  jimg
// Merged the pc port branch
//
// Revision 1.4.20.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.4  1999/04/29 02:29:36  jimg
// Merge of no-gnu branch
//
// Revision 1.3.14.2  1999/02/05 09:32:36  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.3.14.1  1999/02/02 21:57:07  jimg
// String to string version
//
// Revision 1.3  1997/02/19 04:53:40  jimg
// Changed (void) to ().
//
// Revision 1.2  1996/08/13 18:52:52  jimg
// Added not_used to definition of char rcsid[].
// Now tests the Gui.
//
// Revision 1.1  1996/05/31 23:28:16  jimg
// Added.
//

