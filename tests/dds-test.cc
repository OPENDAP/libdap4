
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

// Test the DDS scanner, parser and DDS class.
//
// jhrg 8/29/94

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: dds-test.cc,v 1.30 2005/03/30 21:36:33 jimg Exp $"};

#include <GetOpt.h>

#include "parser.h"
#include "dds.tab.h"
#include "BaseType.h"
#include "Int32.h"
#include "DDS.h"
#include "util.h"
#include "Error.h"

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
    fprintf( stderr, "usage: %s %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n",
		     name.c_str(),
		     "[s] [pd] [c]",
		     "s: Test the scanner.",
		     "p: Test the parser; reads from stdin and prints the",
		     "   internal structure to stdout.",
		     "d: Turn on parser debugging. (only for the hard core.)",
		     "c: Test the C++ code for manipulating DDS objects.",
		     "   Reads from stdin, parses and writes the modified DDS",
		     "   to stdout." ) ;
}

#ifdef WIN32
void
#else
int
#endif
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

    try {
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
    catch (Error &e) {
      e.display_message();
    }

#ifdef WIN32
    exit(0); //  DejaGnu/Cygwin based test suite requires this.
    return;  //  Visual C++ requests this.
#endif
}

void
test_scanner(void)
{
    int tok;

    fprintf( stdout, "%s", prompt ) ; // first prompt
    fflush( stdout ) ;
    while ((tok = ddslex())) {
	switch (tok) {
	  case SCAN_DATASET:
	    fprintf( stdout, "DATASET\n" ) ;
	    break;
	  case SCAN_LIST:
	    fprintf( stdout, "LIST\n" ) ;
	    break;
	  case SCAN_SEQUENCE:
	    fprintf( stdout, "SEQUENCE\n" ) ;
	    break;
	  case SCAN_STRUCTURE:
	    fprintf( stdout, "STRUCTURE\n" ) ;
	    break;
	  case SCAN_FUNCTION:
	    fprintf( stdout, "FUNCTION\n" ) ;
	    break;
	  case SCAN_GRID:
	    fprintf( stdout, "GRID\n" ) ;
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
	    fprintf( stdout, "Url\n" ) ;
	    break;
	  case SCAN_WORD:
	    fprintf( stdout, "WORD: %s\n", ddslval.word ) ;
	    break;
	  case '{':
	    fprintf( stdout, "Left Brace\n" ) ;
	    break;
	  case '}':
	    fprintf( stdout, "Right Brace\n" ) ;
	    break;
	  case '[':
	    fprintf( stdout, "Left Bracket\n" ) ;
	    break;
	  case ']':
	    fprintf( stdout, "Right Bracket\n" ) ;
	    break;
	  case ';':
	    fprintf( stdout, "Semicolon\n" ) ;
	    break;
	  case ':':
	    fprintf( stdout, "Colon\n" ) ;
	    break;
	  case '=':
	    fprintf( stdout, "Assignment\n" ) ;
	    break;
	  default:
	    fprintf( stdout, "Error: Unrecognized input\n" ) ;
	}
	fprintf( stdout, "%s", prompt ) ; // print prompt after output
	fflush( stdout ) ;
    }
}

void
test_parser(void)
{
    BaseTypeFactory *factory = new BaseTypeFactory;
    DDS table(factory);
    table.parse();
    
    if (table.check_semantics())
	fprintf( stdout, "DDS past semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS failed semantic check\n" ) ;

    if (table.check_semantics(true))
	fprintf( stdout, "DDS past full semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS failed full semantic check\n" ) ;

    table.print( stdout );

    delete factory; factory = 0;
}

void
test_class(void)
{
    BaseTypeFactory *factory = new BaseTypeFactory;
    DDS table(factory);
    table.parse();
    
    if (table.check_semantics())
	fprintf( stdout, "DDS past semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS filed semantic check\n" ) ;

    if (table.check_semantics(true))
	fprintf( stdout, "DDS past full semantic check\n" ) ;
    else 
	fprintf( stdout, "DDS filed full semantic check\n" ) ;

    table.print( stdout );

    DDS table2 = table;		// test copy ctor;
    table2.print( stdout );

    BaseTypeFactory *factory2 = new BaseTypeFactory;
    DDS table3(factory2);
    table3 = table;		// test operator=

    fprintf( stdout, "Dataset name: %s\n", table.get_dataset_name().c_str()) ;

    string name = "goofy";
    table.add_var(table.get_factory()->NewInt32(name)); // table dtor should delete this object

    table.print( stdout );

    BaseType *btp = table.var(name);

    btp->print_decl(stdout, "", true); // print out goofy w/semicolon

    table.del_var(name);

    table.print( stdout );

    table.add_var(table.get_factory()->NewInt32("goofy"));

    table.print( stdout );

    btp = table.var("goofy");

    btp->print_decl(stdout, "", true); // print out goofy w/semicolon

    table.del_var("goofy");

    table.print( stdout );

    for (DDS::Vars_iter p = table.var_begin(); p != table.var_end(); p++)
	(*p)->print_decl(stdout, "", true);	// print them all w/semicolons

    delete factory; factory = 0;
    delete factory2; factory2 = 0;
}

// $Log: dds-test.cc,v $
// Revision 1.30  2005/03/30 21:36:33  jimg
// Now uses the BaseTypeFactory class.
//
// Revision 1.29  2003/12/08 18:02:30  edavis
// Merge release-3-4 into trunk
//
// Revision 1.28  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.27  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.26.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.26  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.25  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.21.4.7  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.21.4.6  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.21.4.5  2002/09/05 22:52:55  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.24  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.21.4.4  2001/11/01 00:43:51  jimg
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
// Revision 1.23  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.21.4.3  2001/08/18 00:04:23  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.21.4.2  2001/06/23 00:52:08  jimg
// Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
// that they are (more or less) the same in all the scanners. There are
// one or two characters that differ (for example das.lex allows ( and )
// in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
// are essentially the same across the board.
// Added `#' to the set of characeters allowed in an ID (bug 179).
//
// Revision 1.22  2001/06/15 23:49:03  jimg
// Merged with release-3-2-4.
//
// Revision 1.21.4.1  2001/05/08 19:10:47  jimg
// Expanded the set of names that the dds.y parser will recognize to
// include integers (for files named like 990412.nc). Also removed the
// unused keywords Dependent and Independent from both the DDS scanner
// and parser.
// Added other reserved words to the set of possible Dataset names.
//
// Revision 1.21  2000/09/22 02:52:58  jimg
// Fixes to the tests to recognize some of the new error messages. Also,
// the test drivers were modified to catch the exceptions now thrown by
// some of the parsers.
//
// Revision 1.20  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.19  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.18  2000/07/19 22:51:40  rmorris
// Call and return from main in a manner Visual C++ likes and
// exit the program with exit(0) so that DejaGnu/Cygwin based
// testsuite can succeed for win32.
//
// Revision 1.17  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.16  2000/06/07 18:07:00  jimg
// Merged the pc port branch
//
// Revision 1.15.20.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.15.14.1  2000/02/17 05:03:17  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
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

