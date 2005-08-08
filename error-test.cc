
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
 
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Test the Error object scanner, parser and class.
//
// jhrg 4/25/96

#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <assert.h>

#include <GetOpt.h>

#include "Error.h"
#include "parser.h"
#include "Error.tab.h"

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
    fprintf( stderr, "usage: error-test: [d][sp] <  filename ...\n" ) ;
#else
    fprintf( stderr, "usage: error-test: [d][spo] <  filename ...\n" ) ;
#endif
    fprintf( stderr, "       d: extra parser debugging information\n" ) ;
    fprintf( stderr, "       s: run the scanner\n" ) ;
    fprintf( stderr, "       p: run the parser\n" ) ;
#ifdef WIN32
    fprintf( stderr, "       o: evaluate the object, runs the parser\n" ) ;
#endif
}

void
test_scanner()
{
    int tok;

    fprintf( stdout, "%s", prompt ) ; // first prompt
    fflush( stdout ) ;
    while ((tok = Errorlex())) {
	switch (tok) {
	  case SCAN_ERROR:
	    fprintf( stdout, "ERROR\n" ) ;
	    break;
	  case SCAN_CODE:
	    fprintf( stdout, "CODE\n" ) ;
	    break;
	  case SCAN_PTYPE:
	    fprintf( stdout, "PTYPE\n" ) ;
	    break;
	  case SCAN_MSG:
	    fprintf( stdout, "MSG\n" ) ;
	    break;
	  case SCAN_PROGRAM:
	    fprintf( stdout, "PROGRAM\n" ) ;
	    break;
	  case SCAN_STR:
	    fprintf( stdout, "%s\n", Errorlval.string ) ;
	    break;
	  case SCAN_INT:
	    fprintf( stdout, "%d\n", Errorlval.integer ) ;
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
test_parser(Error &err)
{
    int status = err.parse(stdin);
    fprintf( stdout, "Status from parser: %d\n", status ) ;
    
    if (err.OK())
	fprintf( stdout, "Error passed OK check\n" ) ;
    else 
	fprintf( stdout, "Error failed OK check\n" ) ;

    err.print(stdout);
}

#ifdef GUI
void
test_object(Error &err)
{
    Gui g;

    string response = err.correct_error(&g);
    
    fprintf( stdout, "Response: %s\n", response.c_str() ) ;
}
#endif

// $Log: error-test.cc,v $
// Revision 1.14  2003/12/08 18:02:30  edavis
// Merge release-3-4 into trunk
//
// Revision 1.13  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.12  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.11.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.11  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.10  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.8.4.2  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.9  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.8.4.1  2001/08/18 00:04:03  jimg
// Removed WIN32 compile guards from using statements.
//
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

