
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Test the DAS class.
// Read attributes from one or more files, printing the resulting table to
// stdout. If a file is named `-' read from stdin for that file. The option
// `-d' causes new/delete run-time debugging to be turned on.
//
// jhrg 7/25/94

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: das-test.cc,v 1.33 2003/04/02 19:13:12 pwest Exp $"};

#include <string>
#include <GetOpt.h>

#define YYSTYPE char *

#include "DAS.h"
#include "das.tab.h"
#include "Error.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

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
    fprintf( stderr, "usage: %s %s\n %s\n %s\n %s\n %s\n %s\n %s\n",
		     name.c_str(),
		     "[-v] [-s] [-d] [-c] [-p] {< in-file > out-file}",
		     "s: Test the DAS scanner.",
		     "p: Scan and parse from <in-file>; print to <out-file>.",
		     "c: Test building the DAS from C++ code.",
		     "v: Print the version of das-test and exit.",
		     "d: Print parser debugging information.",
		     "r: Print the DAS with aliases deReferenced." ) ;
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
	      fprintf( stderr, "%s: %s\n", argv[0], version ) ;
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

    fprintf( stdout, "%s", prompt ) ; // first prompt
    fflush( stdout ) ;
    while ((tok = daslex())) {
	switch (tok) {
	  case SCAN_ATTR:
	    fprintf( stdout, "ATTR\n" ) ;
	    break;
	  case SCAN_ALIAS:
	    fprintf( stdout, "ALIAS\n" ) ;
	    break;
	  case SCAN_WORD:
	    fprintf( stdout, "WORD=%s\n", daslval ) ;
	    break;

	  case SCAN_BYTE:
	    fprintf( stdout, "BYTE\n" ) ;
	    break;
	  case SCAN_INT16:
	    fprintf( stdout, "INT16\n" ) ;
	    break;
	  case SCAN_UINT16:
	    fprintf( stdout, "UINT16\n" ) ;
	    break;
	  case SCAN_INT32:
	    fprintf( stdout, "INT32\n" ) ;
	    break;
	  case SCAN_UINT32:
	    fprintf( stdout, "UINT32\n" ) ;
	    break;
	  case SCAN_FLOAT32:
	    fprintf( stdout, "FLOAT32\n" ) ;
	    break;
	  case SCAN_FLOAT64:
	    fprintf( stdout, "FLOAT64\n" ) ;
	    break;
	  case SCAN_STRING:
	    fprintf( stdout, "STRING\n" ) ;
	    break;
	  case SCAN_URL:
	    fprintf( stdout, "URL\n" ) ;
	    break;

	  case '{':
	    fprintf( stdout, "Left Brace\n" ) ;
	    break;
	  case '}':
	    fprintf( stdout, "Right Brace\n" ) ;
	    break;
	  case ';':
	    fprintf( stdout, "Semicolon\n" ) ;
	    break;
	  case ',':
	    fprintf( stdout, "Comma\n" ) ;
	    break;

	  default:
	    fprintf( stdout, "Error: Unrecognized input\n" ) ;
	}
	fprintf( stdout, "%s", prompt ) ; // print prompt after output
	fflush( stdout ) ;
    }
}


void
parser_driver(DAS &das, bool deref_alias)
{
    das.parse();

    das.print(stdout, deref_alias);
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

    das.print(stdout, deref_alias);
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

    fprintf( stdout, "Using the Pix:\n" ) ;
    for (Pix p1 = at.first_attr(); p1; at.next_attr(p1))
	{
		fprintf( stdout, "%s %s ", at.get_name(p1).c_str(),
				   at.get_type(p1).c_str() ) ;
		for (unsigned i = 0; i < at.get_attr_num(p1); ++i)
				fprintf( stdout, "%s ", at.get_attr(p1, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    fprintf( stdout, "Using the iterator:\n" ) ;
    for (AttrTable::Attr_iter p2 = at.attr_begin(); p2 != at.attr_end(); p2++)
    {
		fprintf( stdout, "%s %s ", at.get_name(p2).c_str(),
			at.get_type(p2).c_str() ) ;
		for (unsigned i = 0; i < at.get_attr_num(p2); ++i)
			fprintf( stdout, "%s ", at.get_attr(p2, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    string name = "month";
    fprintf( stdout, "Using String: %s %s %s\n",
		     at.get_type(name).c_str(),
		     at.get_attr(name, 0).c_str(),
		     at.get_attr(name, 1).c_str()) ;
    fprintf( stdout, "Using char *: %s %s %s\n",
		     at.get_type("month").c_str(),
		     at.get_attr("month", 0).c_str(),
		     at.get_attr("month", 1).c_str() ) ;

    at.del_attr("month");

    fprintf( stdout, "After deletion:\n" ) ;
    for (AttrTable::Attr_iter p3 = at.attr_begin(); p3 != at.attr_end(); p3++)
    {
		fprintf( stdout, "%s %s ", at.get_name(p3).c_str(),
			at.get_type(p3).c_str() ) ;
		for (unsigned i = 0; i < at.get_attr_num(p3); ++i)
			fprintf( stdout, "%s ", at.get_attr(p3, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    at.print(stdout);

    fprintf( stdout, "After print:\n" ) ;
    for (AttrTable::Attr_iter p4 = at.attr_begin(); p4 != at.attr_end(); p4++)
    {
	fprintf( stdout, "%s %s ", at.get_name(p4).c_str(),
		at.get_type(p4).c_str() ) ;
	for (unsigned i = 0; i < at.get_attr_num(p4); ++i)
	     fprintf( stdout, "%s ", at.get_attr(p4, i).c_str() ) ;
	fprintf( stdout, "\n" ) ;
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

    fprintf( stdout, "Using the Pix:\n" ) ;
    for (Pix p1 = at->first_attr(); p1; at->next_attr(p1))
	{
	fprintf( stdout, "%s %s ", at->get_name(p1).c_str(),
				   at->get_type(p1).c_str() ) ;
	for (unsigned i = 0; i < at->get_attr_num(p1); ++i)
	     fprintf( stdout, "%s ", at->get_attr(p1, i).c_str() ) ;
	fprintf( stdout, "\n" ) ;
    }

    fprintf( stdout, "Using the iterator:\n" ) ;
    for (AttrTable::Attr_iter p2 = at->attr_begin(); p2 != at->attr_end(); p2++)
    {
		fprintf( stdout, "%s %s ", at->get_name(p2).c_str(),
			at->get_type(p2).c_str() ) ;
		for (unsigned i = 0; i < at->get_attr_num(p2); ++i)
			fprintf( stdout, "%s ", at->get_attr(p2, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    string name = "month";
    fprintf( stdout, "Using String: %s %s %s\n",
		     at->get_type(name).c_str(),
		     at->get_attr(name, 0).c_str(),
		     at->get_attr(name, 1).c_str() ) ;
    fprintf( stdout, "Using char *: %s %s %s\n",
		     at->get_type("month").c_str(),
		     at->get_attr("month", 0).c_str(),
		     at->get_attr("month", 1).c_str() ) ;

    at->del_attr("month");

    fprintf( stdout, "After deletion:\n" ) ;
    for (AttrTable::Attr_iter p3 = at->attr_begin(); p3 != at->attr_end(); p3++)
    {
		fprintf( stdout, "%s %s ", at->get_name(p3).c_str(),
			at->get_type(p3).c_str() ) ;
		for (unsigned i = 0; i < at->get_attr_num(p3); ++i)
			fprintf( stdout, "%s ", at->get_attr(p3, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }

    at->print(stdout);

    fprintf( stdout, "After print:\n" ) ;
    for (AttrTable::Attr_iter p4 = at->attr_begin(); p4 !=at->attr_end(); p4++)
    {
		fprintf( stdout, "%s %s ", at->get_name(p4).c_str(), 
			at->get_type(p4).c_str() ) ;
		for (unsigned i = 0; i < at->get_attr_num(p4); ++i)
			fprintf( stdout, "%s ", at->get_attr(p4, i).c_str() ) ;
		fprintf( stdout, "\n" ) ;
    }
}

// $Log: das-test.cc,v $
// Revision 1.33  2003/04/02 19:13:12  pwest
// Fixed bug in parser-util that did not recognize illegal characters when
// checking float32 and float64 strings, added tests to parserUtilTest to
// test for illegal characters as in das-test, updated das-test as in 3.3
// version.
//
// Revision 1.31.2.2  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.31.2.1  2003/02/18 00:33:36  rmorris
// Fixed win32 incompatibility.  for(Pix p;something;something), but
// scope of Pix is wider under win32 than unix.  Using multiple vars
// of the same name in the same method is a problem under win32.
//
// Revision 1.31  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.30  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.26.4.7  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.26.4.6  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.26.4.5  2002/09/22 14:34:20  rmorris
// VC++ considers 'x' in 'for(int x,...)' to not be just for that scope of the
// block associated with that for.  When there are multiple of such type of
// thing - VC++ see redeclarations of the same var - moved to use different
// var names to prevent the error.
//
// Revision 1.26.4.4  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
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

