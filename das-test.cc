
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Test the DAS class.
// Read attributes from one or more files, printing the resulting table to
// stdout. If a file is named `-' read from stdin for that file. The option
// `-d' causes new/delete run-time debugging to be turned on.
//
// jhrg 7/25/94

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: das-test.cc,v 1.29 2002/06/03 22:21:15 jimg Exp $"};

#include <iostream>
#include <string>
#include <Pix.h>
#include <GetOpt.h>

#define YYSTYPE char *

#include "DAS.h"
#include "das.tab.h"
#include "Error.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::cerr;
using std::endl;
using std::flush;

void plain_driver(DAS &das, bool deref_alias);
void load_attr_table(AttrTable at);
void load_attr_table_ptr(AttrTable *atp);
void parser_driver(DAS &das, bool deref_alias);
void test_scanner();

int daslex();

extern int dasdebug;
const char *prompt = "das-test: ";
const char *version = "version 1.18";

void
usage(string name)
{
    cerr << "usage: " << name 
	 << " [-v] [-s] [-d] [-c] [-p] {< in-file > out-file}" << endl
	 << " s: Test the DAS scanner." << endl
	 << " p: Scan and parse from <in-file>; print to <out-file>." << endl
	 << " c: Test building the DAS from C++ code." << endl
	 << " v: Print the version of das-test and exit." << endl
	 << " d: Print parser debugging information." << endl
	 << " r: Print the DAS with aliases deReferenced." << endl;
}

#ifdef WIN32
void
#else
int
#endif
main(int argc, char *argv[])
{

    GetOpt getopt (argc, argv, "scpvdr");
    int option_char;
    bool parser_test = false;
    bool scanner_test = false;
    bool code_test = false;
    bool deref_alias = false;
    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'p':
	      parser_test = true;
	      break;
	    case 's':
	      scanner_test = true;
	      break;
	    case 'c':
	      code_test = true;
	      break;
	    case 'v':
	      cerr << argv[0] << ": " << version << endl;
	      exit(0);
	    case 'd':
	      dasdebug = 1;
	      break;
	    case 'r':
	      deref_alias = true;
	      break;
	    case '?': 
	    default:
	      usage(argv[0]);
	      exit(1);
	  }

    DAS das;

    if (!parser_test && !scanner_test && !code_test) {
	usage(argv[0]);
	exit(1);
    }
	
    try {
      if (parser_test)
	parser_driver(das, deref_alias);

      if (scanner_test)
	test_scanner();

      if (code_test)
	plain_driver(das, deref_alias);
    }
    catch (Error &e) {
      e.display_message();
    }

#ifdef WIN32
	exit(0); //  DejaGnu/Cygwin based test suite requires this.
	return;  //  Visual C++ requests this.
#endif

}

void
test_scanner()
{
    int tok;

    cout << prompt << flush;		// first prompt
    while ((tok = daslex())) {
	switch (tok) {
	  case SCAN_ATTR:
	    cout << "ATTR" << endl;
	    break;
	  case SCAN_ALIAS:
	    cout << "ALIAS" << endl;
	    break;
	  case SCAN_WORD:
	    cout << "WORD=" << daslval << endl;
	    break;

	  case SCAN_BYTE:
	    cout << "BYTE" << endl;
	    break;
	  case SCAN_INT16:
	    cout << "INT16" << endl;
	    break;
	  case SCAN_UINT16:
	    cout << "UINT16" << endl;
	    break;
	  case SCAN_INT32:
	    cout << "INT32" << endl;
	    break;
	  case SCAN_UINT32:
	    cout << "UINT32" << endl;
	    break;
	  case SCAN_FLOAT32:
	    cout << "FLOAT32" << endl;
	    break;
	  case SCAN_FLOAT64:
	    cout << "FLOAT64" << endl;
	    break;
	  case SCAN_STRING:
	    cout << "STRING" << endl;
	    break;
	  case SCAN_URL:
	    cout << "URL" << endl;
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
	  case ',':
	    cout << "Comma" << endl;
	    break;

	  default:
	    cout << "Error: Unrecognized input" << endl;
	}
	cout << prompt << flush;		// print prompt after output
    }
}


void
parser_driver(DAS &das, bool deref_alias)
{
    das.parse();

    das.print(cout, deref_alias);
}

// Given a DAS, add some stuff to it.

void
plain_driver(DAS &das, bool deref_alias)
{
    AttrTable *atp;
    AttrTable *dummy;

    string name = "test";
    atp = new AttrTable;
    load_attr_table_ptr(atp);
    dummy = das.get_table(name);
    das.add_table(name, atp);

    name = "test2";
    atp = new AttrTable;
    load_attr_table_ptr(atp);
    das.add_table(name, atp);

    das.print(cout, deref_alias);
}

// stuff an AttrTable full of values. Also, print it out.

void
load_attr_table(AttrTable at)
{
    at.append_attr("month", "String", "Feb");
    at.append_attr("month", "String", "Feb");

    at.append_attr("month_a", "String", "Jan");
    at.append_attr("month_a", "String", "Feb");
    at.append_attr("month_a", "String", "Mar");

    at.append_attr("Date", "Int32", "12345");
    at.append_attr("day", "Int32", "01");
    at.append_attr("Time", "Float64", "3.1415");

    cout << "Using the Pix:" << endl;
    Pix p;
    for (p = at.first_attr(); p; at.next_attr(p)) {
	cout << at.get_name(p) << " " << at.get_type(p) << " ";
	for (unsigned i = 0; i < at.get_attr_num(p); ++i)
	     cout << at.get_attr(p, i) << " ";
	cout << endl;
    }

    string name = "month";
    cout << "Using String: " << at.get_type(name) << " " 
	 << at.get_attr(name, 0) << " " << at.get_attr(name, 1) << endl;
    cout << "Using char *: " << at.get_type("month") << " " 
	 << at.get_attr("month", 0) << " " << at.get_attr("month", 1) << endl;

    at.del_attr("month");

    cout << "After deletion:" << endl;
    for (p = at.first_attr(); p; at.next_attr(p)) {
	cout << at.get_name(p) << " " << at.get_type(p) << " ";
	for (unsigned i = 0; i < at.get_attr_num(p); ++i)
	     cout << at.get_attr(p, i) << " ";
	cout << endl;
    }

    at.print(cout);

    cout << "After print:" << endl;
    for (p = at.first_attr(); p; at.next_attr(p)) {
	cout << at.get_name(p) << " " << at.get_type(p) << " ";
	for (unsigned i = 0; i < at.get_attr_num(p); ++i)
	     cout << at.get_attr(p, i) << " ";
	cout << endl;
    }
}

// OK, now try it with a dymanic AttrTable

void
load_attr_table_ptr(AttrTable *at)
{
    at->append_attr("month", "String", "Feb");
    at->append_attr("month", "String", "Feb");

    at->append_attr("month_a", "String", "Jan");
    at->append_attr("month_a", "String", "Feb");
    at->append_attr("month_a", "String", "Mar");

    at->append_attr("Date", "Int32", "12345");
    at->append_attr("day", "Int32", "01");
    at->append_attr("Time", "Float64", "3.1415");

    cout << "Using the Pix:" << endl;
    Pix p;
    for (p = at->first_attr(); p; at->next_attr(p)) {
	cout << at->get_name(p) << " " << at->get_type(p) << " ";
	for (unsigned i = 0; i < at->get_attr_num(p); ++i)
	     cout << at->get_attr(p, i) << " ";
	cout << endl;
    }

    string name = "month";
    cout << "Using String: " << at->get_type(name) << " " 
	 << at->get_attr(name, 0) << " " << at->get_attr(name, 1) << endl;
    cout << "Using char *: " << at->get_type("month") << " " 
	 << at->get_attr("month", 0) << " " << at->get_attr("month", 1) << endl;

    at->del_attr("month");

    cout << "After deletion:" << endl;
    for (p = at->first_attr(); p; at->next_attr(p)) {
	cout << at->get_name(p) << " " << at->get_type(p) << " ";
	for (unsigned i = 0; i < at->get_attr_num(p); ++i)
	     cout << at->get_attr(p, i) << " ";
	cout << endl;
    }

    at->print(cout);

    cout << "After print:" << endl;
    for (p = at->first_attr(); p; at->next_attr(p)) {
	cout << at->get_name(p) << " " << at->get_type(p) << " ";
	for (unsigned i = 0; i < at->get_attr_num(p); ++i)
	     cout << at->get_attr(p, i) << " ";
	cout << endl;
    }
}

// $Log: das-test.cc,v $
// Revision 1.29  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.26.4.3  2001/11/01 00:43:51  jimg
// Fixes to the scanners and parsers so that dataset variable names may
// start with digits. I've expanded the set of characters that may appear
// in a variable name and made it so that all except `#' may appear at
// the start. Some characters are not allowed in variables that appear in
// a DDS or CE while they are allowed in the DAS. This makes it possible
// to define containers with names like `COARDS:long_name.' Putting a colon
// in a variable name makes the CE parser much more complex. Since the set
// of characters that people want seems pretty limited (compared to the
// complete ASCII set) I think this is an OK approach. If we have to open
// up the expr.lex scanner completely, then we can but not without adding
// lots of action clauses to teh parser. Note that colon is just an example,
// there's a host of characters that are used in CEs that are not allowed
// in IDs.
//
// Revision 1.28  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.26.4.2  2001/08/18 00:05:13  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.27  2001/01/26 19:48:09  jimg
// Merged with release-3-2-3.
//
// Revision 1.26.4.1  2000/11/30 05:24:46  jimg
// Significant changes and improvements to the AttrTable and DAS classes. DAS
// now is a child of AttrTable, which makes attributes behave uniformly at
// all levels of the DAS object. Alias now work. I've added unit tests for
// several methods in AttrTable and some of the functions in parser-util.cc.
// In addition, all of the DAS tests now work.
//
// Revision 1.26  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.25  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.24  2000/07/19 22:51:40  rmorris
// Call and return from main in a manner Visual C++ likes and
// exit the program with exit(0) so that DejaGnu/Cygwin based
// testsuite can succeed for win32.
//
// Revision 1.23  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.22  2000/06/07 18:07:00  jimg
// Merged the pc port branch
//
// Revision 1.21.20.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.21.14.1  2000/02/17 05:03:17  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.21  1999/04/29 02:29:35  jimg
// Merge of no-gnu branch
//
// Revision 1.20  1999/03/24 23:34:15  jimg
// Added support for the new Int16, UInt16 and Float32 types.
//
// Revision 1.19.14.2  1999/02/05 09:32:35  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.19.14.1  1999/02/02 21:57:06  jimg
// String to string version
//
// Revision 1.19  1997/05/13 23:37:44  jimg
// Changed options and command `format' so that das-test is similar to
// dds-test.
//
// Revision 1.18  1996/07/16 17:49:29  jimg
// Added usage function.
// Added version option.
// Fixed calling logic - now a usage message is printed when no options are
// given.
// Fixed warnings about signed -vs- unsigned compares.
//
// Revision 1.17  1996/05/31 23:30:49  jimg
// Updated copyright notice.
//
// Revision 1.16  1996/04/05 21:59:35  jimg
// Misc Changes for release 2.0.1 of the core software - for developers.
//
// Revision 1.15  1996/04/05 00:22:12  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.14  1995/10/23  22:55:33  jimg
// Added RCSID.
//
// Revision 1.13  1995/08/26  00:32:04  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.12  1995/07/09  21:29:24  jimg
// Added copyright notice.
//
// Revision 1.11  1995/07/08  18:34:31  jimg
// Removed old code.
// Removed unnecessary declaration of dasparse().
//
// Revision 1.10  1995/05/10  15:34:07  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.9  1995/05/10  13:45:42  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.8  1995/03/16  17:36:15  jimg
// Added include config_dap.h to top of includes.
// Added TRACE_NEW switch dbnew debugging.
//
// Revision 1.7  1995/02/10  03:28:55  jimg
// Removed dummy_read.cc
// Updates das-test.cc so that type checking gets tested.
//
// Revision 1.6  1994/12/07  21:21:47  jimg
// Added code to test the scanner - it is selected using the -s option.
//
// Revision 1.5  1994/10/13  16:12:07  jimg
// Added -p to interface: run the parser. This option makes das-test
// behave as it used to, with the following exceptions:
// 1) if `none' is given as a filename, then the associated actions (reading
// or writing) is not performed. This lets you test the ctors and dtors
// w/o anything else getting in the way.
// 2) - means stdin or stdout
// Also added AttrTable test code that gets run if -p is not given.
//
// Revision 1.4  1994/10/05  16:48:52  jimg
// Added code to print the return value of the DAS::parse() mfunc.
//
// Revision 1.3  1994/09/27  22:58:59  jimg
// das-test no longer uses Space.cc for new/delete debugging - that system
// did not work on alphas).
//
// Revision 1.2  1994/09/09  16:13:16  jimg
// Added code to test the stdin, FILE * and file descriptor functions of
// class DAS.
//
// Revision 1.1  1994/08/02  18:08:38  jimg
// Test driver for DAS (and AttrTable) classes.
//

