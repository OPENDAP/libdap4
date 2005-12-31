
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Test the CE scanner and parser.
//
// jhrg 9/12/95

#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#ifdef WIN32
#include <rpc.h>
#include <winsock2.h>
#include <xdr.h>
#include <io.h>
#include <fcntl.h>
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include <GetOpt.h>

#include <string>

#include "DDS.h"
#include "DataDDS.h"
#include "BaseType.h"
#include "TestSequence.h"
#include "TestCommon.h"
#include "TestTypeFactory.h"

#include "parser.h"
#include "expr.h"
#include "expr.tab.h"
#include "util.h"
#include "debug.h"

using namespace std;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
				      // timeouts. 

#define CRLF "\r\n"                   // Change this here and in cgi_util.cc
#define DODS_DDS_PRX "dods_dds"
#define YY_BUFFER_STATE (void *)

void test_scanner(const string &str);
void test_scanner(bool show_prompt);
void test_parser(DDS &table, const string &dds_name, const string &constraint);
bool read_table(DDS &table, const string &name, bool print);
void evaluate_dds(DDS &table, bool print_constrained);
bool loopback_pipe(FILE **pout, FILE **pin);
bool constrained_trans(const string &dds_name, const bool constraint_expr,
		       const string &ce, const bool series_values);

int exprlex();			// exprlex() uses the global exprlval
int exprparse(void *arg);
void exprrestart(FILE *in);

// Glue routines declared in expr.lex
void expr_switch_to_buffer(void *new_buffer);
void expr_delete_buffer(void * buffer);
void *expr_string(const char *yy_str);

extern int exprdebug;

static int keep_temps = 0;	// MT-safe; test code.

const string version = "version 1.12";
const string prompt = "expr-test: ";
const string options = "sS:bdecvp:w:f:k:v";
const string usage = "\
\nexpr-test [-s [-S string] -d -c -v [-p dds-file]\
\n[-e expr] [-w dds-file] [-f data-file] [-k expr]]\
\nTest the expression evaluation software.\
\nOptions:\
\n	-s: Feed the input stream directly into the expression scanner, does\
\n	    not parse.\
\n      -S: <string> Scan the string as if it was standard input.\
\n	-d: Turn on expression parser debugging.\
\n	-c: Print the constrained DDS (the one that will be returned\
\n	    prepended to a data transmission. Must also supply -p and -e \
\n      -v: Verbose output\
\n      -V: Print the version of expr-test\
\n  	-p: DDS-file: Read the DDS from DDS-file and create a DDS object,\
\n	    then prompt for an expression and parse that expression, given\
\n	    the DDS object.\
\n	-e: Evaluate the constraint expression. Must be used with -p.\
\n	-w: Do the whole enchilada. You don't need to supply -p, -e, ...\
\n          This prompts for the constraint expression and the optional\
\n          data file name. NOTE: The CE parser Error objects do not print\
\n          with this option.\
\n      -b: Use periodic/cyclic/chaning values. For testing Sequence CEs.\
\n      -f: A file to use for data. Currently only used by -w for sequences.\
\n      -k: A constraint expression to use with the data. Works with -p,\
\n          -e, -t and -w";

int
main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, options.c_str());
    int option_char;
    bool scanner_test = false, parser_test = false, evaluate_test = false;
    bool print_constrained = false;
    bool whole_enchalada = false, constraint_expr = false;
    bool scan_string = false;
    bool verbose = false;
    bool series_values = false;
    string dds_file_name;
    string dataset = "";
    string constraint = "";
    TestTypeFactory *ttf = new TestTypeFactory();
    DDS table(ttf);

    // process options

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'b':
            series_values = true;
            break;
        case 'd':
            exprdebug = true;
            break;
        case 's':
            scanner_test = true;
            break;
        case 'S':
            scanner_test = true;
            scan_string = true;
            constraint = getopt.optarg;
            break;
        case 'p':
            parser_test = true;
            dds_file_name = getopt.optarg;
            break;
        case 'e':
            evaluate_test = true;
            break;
        case 'c':
            print_constrained = true;
            break;
        case 'w':
            whole_enchalada = true;
            dds_file_name = getopt.optarg;
            break;
        case 'k':
            constraint_expr = true;
            constraint = getopt.optarg;
            break;
        case 'f':
            dataset = getopt.optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case 'V':
            fprintf(stderr, "%s: %s\n", argv[0], version.c_str());
            exit(0);
        case '?':
        default:
            fprintf(stderr, "%s\n", usage.c_str());
            exit(1);
            break;
        }

    try {
        if (!scanner_test && !parser_test && !evaluate_test
            && !whole_enchalada) {
            fprintf(stderr, "%s\n", usage.c_str());
            delete ttf;
            ttf = 0;
            exit(1);
        }
        // run selected tests

        if (scanner_test) {
            if (scan_string)
                test_scanner(constraint);
            else
                test_scanner(true);

            delete ttf;
            ttf = 0;
            exit(0);
        }

        if (parser_test) {
            test_parser(table, dds_file_name, constraint);
        }

        if (evaluate_test) {
            evaluate_dds(table, print_constrained);
        }

        if (whole_enchalada) {
            constrained_trans(dds_file_name, constraint_expr, constraint,
                              series_values);
        }

    }
    catch(exception & e) {
        cerr << "Caught exception: " << e.what << endl;
    }

    delete ttf;
    ttf = 0;

    exit(0);
}

// Instead of reading the tokens from stdin, read them from a string.


void
test_scanner(const string &str)
{
    exprrestart(0);
    void *buffer = expr_string(str.c_str());
    expr_switch_to_buffer(buffer);

    test_scanner(false);

    expr_delete_buffer(buffer);
}

void
test_scanner(bool show_prompt)
{
    if (show_prompt) 
	fprintf( stdout, "%s", prompt.c_str() )	; // first prompt

    int tok;
    while ((tok = exprlex())) {
	switch (tok) {
	  case SCAN_WORD:
	    fprintf( stdout, "WORD: %s\n", exprlval.id ) ;
	    break;
	  case SCAN_STR:
	    fprintf( stdout, "STR: %s\n", exprlval.val.v.s->c_str() ) ;
	    break;
#if 0
	  case SCAN_INT:
	    fprintf( stdout, "INT: %d\n", exprlval.val.v.i ) ;
	    break;
	  case SCAN_FLOAT:
	    fprintf( stdout, "FLOAT: %f\n", exprlval.val.v.f ) ;
	    break;
#endif
	  case SCAN_EQUAL:
	    fprintf( stdout, "EQUAL: %d\n", exprlval.op ) ;
	    break;
	  case SCAN_NOT_EQUAL:
	    fprintf( stdout, "NOT_EQUAL: %d\n", exprlval.op ) ;
	    break;
	  case SCAN_GREATER:
	    fprintf( stdout, "GREATER: %d\n", exprlval.op ) ;
	    break;
	  case SCAN_GREATER_EQL:
	    fprintf( stdout, "GREATER_EQL: %d\n", exprlval.op ) ;
	    break;
	  case SCAN_LESS:
	    fprintf( stdout, "LESS: %d\n", exprlval.op ) ;
	    break;
	  case SCAN_LESS_EQL:
	    fprintf( stdout, "LESS_EQL: %d\n", exprlval.op ) ;
	    break;
	  case SCAN_REGEXP:
	    fprintf( stdout, "REGEXP: %d\n", exprlval.op ) ;
	    break;
	  case '*':
	    fprintf( stdout, "Dereference\n" ) ;
	    break;
	  case '.':
	    fprintf( stdout, "Field Selector\n" ) ;
	    break;
	  case ',':
	    fprintf( stdout, "List Element Separator\n" ) ;
	    break;
	  case '[':
	    fprintf( stdout, "Left Bracket\n" ) ;
	    break;
	  case ']':
	    fprintf( stdout, "Right Bracket\n" ) ;
	    break;
	  case '(':
	    fprintf( stdout, "Left Paren\n" ) ;
	    break;
	  case ')':
	    fprintf( stdout, "Right Paren\n" ) ;
	    break;
	  case '{':
	    fprintf( stdout, "Left Brace\n" ) ;
	    break;
	  case '}':
	    fprintf( stdout, "Right Brace\n" ) ;
	    break;
	  case ':':
	    fprintf( stdout, "Colon\n" ) ;
	    break;
	  case '&':
	    fprintf( stdout, "Ampersand\n" ) ;
	    break;
	  default:
	    fprintf( stdout, "Error: Unrecognized input\n" ) ;
	}
	fprintf( stdout, "%s", prompt.c_str() ) ; // print prompt after output
	fflush( stdout ) ;
    }
}

// NB: The DDS is read in via a file because reading from stdin must be
// terminated by EOF. However, the EOF used to terminate the DDS also closes
// stdin and thus the expr scanner exits immediately.

void
test_parser(DDS &table, const string &dds_name, const string &constraint)
{
    try {
	read_table(table, dds_name, true);

	if (constraint != "") {
	    table.parse_constraint(constraint);
	}
	else {
	    exprrestart(stdin);
	    fprintf( stdout, "%s", prompt.c_str() ) ;
	    parser_arg arg(&table);
	    exprparse((void *)&arg);
	}

	fprintf( stdout, "Input parsed\n" ) ;	// Parser throws on failure.
    }
    catch (Error &e) {
	e.display_message();
    }
}

// Read a DDS from stdin and build the cooresponding DDS. IF PRINT is true,
// print the text reprsentation of that DDS on the stdout. The DDS TABLE is
// modified as a side effect.
//
// Returns: true iff that DDS pasted the semantic_check() mfunc, otherwise
// false.

bool
read_table(DDS &table, const string &name, bool print)
{
    table.parse(name);
    
    if (print)
	table.print( stdout );

    if (table.check_semantics(true))
	return true;
    else {
	fprintf( stdout, "Input did not pass semantic checks\n" ) ;
	return false;
    }
}

void
evaluate_dds(DDS &table, bool print_constrained)
{
    if (print_constrained)
	table.print_constrained( stdout );
    else
	for (DDS::Vars_iter p = table.var_begin(); p != table.var_end(); p++)
	    (*p)->print_decl(stdout, "", true, true);
}

// create a pipe for the caller's process which can be used by the DODS
// software to write to and read from itself.

bool
loopback_pipe(FILE **pout, FILE **pin)
{
#ifdef WIN32
    int fd[2];
    if (_pipe(fd, 1024, _O_BINARY) < 0) {
	fprintf( stderr, "Could not open pipe\n" ) ;
	return false;
    }

    *pout = fdopen(fd[1], "w+b");
    *pin = fdopen(fd[0], "r+b");
#else
    int fd[2];
    if (pipe(fd) < 0) {
	fprintf( stderr, "Could not open pipe\n" ) ;
	return false;
    }

    *pout = fdopen(fd[1], "w");
    *pin = fdopen(fd[0], "r");
#endif

    return true;
}


// Originally in netexec.c (part of the netio library).
// Read the DDS from the data stream. Leave the binary information behind. The
// DDS is moved, without parsing it, into a file and a pointer to that FILE is
// returned. The argument IN (the input FILE stream) is positioned so that the
// next byte is the binary data.
//
// The binary data follows the text `Data:', which itself starts a line.
//
// Returns: a FILE * which contains the DDS describing the binary information
// in IF.
FILE *
move_dds(FILE *in)
{
    char c[] = {"dodsXXXXXX"};
#ifdef WIN32
    char *result = _mktemp(c);

    if (result == NULL) {
        fprintf( stderr, "Could not create unique tempoary file name\n");
        return NULL;
        }
        FILE *fp = fopen(_mktemp(c), "w+b");
#else
    int fd = mkstemp(c);
    if (fd == -1) {
        fprintf( stderr, "Could not create temporary file name %s\n",
                   strerror(errno) ) ;
        return NULL;
    }

    FILE *fp = fdopen(fd, "w+b");
#endif
    if (!keep_temps)
	unlink(c);
    if (!fp) {
	fprintf( stderr, "Could not open anonymous temporary file: %s\n",
			 strerror(errno) ) ;
	return NULL;
    }
	    
    int data = FALSE;
    char s[256], *sp;
    
    sp = &s[0];
    while (!feof(in) && !data) {
	sp = fgets(s, 255, in);
	if (strcmp(s, "Data:\n") == 0)
	    data = TRUE;
	else
	    fputs(s, fp);
    }

    fflush(fp);
    if (fseek(fp, 0L, 0) < 0) {
	fprintf( stderr, "Could not rewind data DDS stream: %s\n",
		 strerror(errno) ) ;
	return NULL;
    }

    return fp;
}


// Gobble up the mime header. At one time the MIME Headers were output from
// the server filter programs (not the core software) so we could call
// DDS::send() from this test code and not have to parse the MIME header. But
// in order to get errors to work more reliably the header generation was
// moved `closer to the send'. That is, we put off determining whether to
// send a DDS or an Error object until later. That trade off is that the
// header generation is not buried in the core software. This code simply
// reads until the end of the header is found. 3/25/98 jhrg

void
parse_mime(FILE *data_source)
{
    char line[256];

    fgets(line, 256, data_source);
    
    while (strncmp(line, CRLF, 2) != 0)
	fgets(line, 256, data_source);
}

void
set_series_values(DDS &dds, bool state)
{
    for (DDS::Vars_iter q = dds.var_begin(); q != dds.var_end(); q++) {
#if 1
	dynamic_cast<TestCommon&>(**q).set_series_values(state);
#else
        TestCommon *tc = dynamic_cast<TestCommon*>(*q);
        if (tc)
            tc->set_series_values(state);
        else
            cerr << "TC is null" << endl;
#endif
    }
}

// Test the transmission of constrained datasets. Use read_table() to read
// the DDS from a file. Once done, prompt for the variable name and
// constraint expression. In a real client-server system the server would
// read the DDS for the entire dataset and send it to the client. The client
// would then respond to the server by asking for a variable given a
// constraint.
// 
// Once the constraint has been entered, it is evaluated in the context of
// the DDS using DDS:eval_constraint() (this would happen on the server-side
// in a real system). Once the evaluation is complete,
// DDS::print_constrained() is used to create a DDS describing only those
// parts of the dataset that are to be sent to the client process and written
// to the output stream. After that, the marker `Data:' is written to the
// output stream, followed by the binary data.

bool
constrained_trans(const string &dds_name, const bool constraint_expr, 
		  const string &constraint, const bool series_values) 
{
    bool status;
    FILE *pin, *pout;
    TestTypeFactory *ttf = new TestTypeFactory;
    DDS server(ttf);

    fprintf( stdout, "The complete DDS:\n" ) ;
    read_table(server, dds_name, true);

    status = loopback_pipe(&pout, &pin);
    if (!status) {
	fprintf( stderr, "Could not create the loopback streams\n" ) ;
	delete ttf; ttf = 0;
	return false;
    }

    // If the CE was not passed in, read it from the command line.
    string ce;
    if (!constraint_expr) {
	fprintf( stdout, "Constraint:" ) ;
	char c[256];
	cin.getline(c, 256);
	if (!cin) {
	    fprintf( stderr, "Could nore read the constraint expression\n" ) ;
	    delete ttf; ttf = 0;
	    exit(1);
	}
	ce = c;
    }
    else
	ce = constraint;

    string dataset = "";
    
    // by default this is false (to get the old-style values that are
    // constant); set series_values to true for testing Sequence constraints.
    // 01/14/05 jhrg
    set_series_values(server, series_values);

    try {
	// send the variable given the constraint; TRUE flushes the I/O
	// channel. Currently only Sequence uses the `dataset' parameter.
	//
	// We're at that awkward stage between two different error processing
	// techniques. 4/6/2000 jhrg
	// I think we've passed that stage... 08/08/05 jhrg
	if (!server.send(dataset, ce, pout, false)) {
	    fprintf( stderr, "Could not send the DDS\n" ) ;
	    delete ttf; ttf = 0;
	    return false;
	}
    }
    catch(Error &e) {
	e.display_message();
	fclose(pout);
	delete ttf; ttf = 0;
	return false;
    }

    fclose(pout);		// close pout to read from pin. Why?
    
    // Now do what Connect::request_data() does:

    // First read the DDS into a new object (using a file to store the DDS
    // temporarily - the parser/scanner won't stop reading until an EOF is
    // found, this fixes that problem).
    BaseTypeFactory *factory = new BaseTypeFactory;
    try {
	// I use the default BaseTypeFactory since we're just printing the
	// values here.
	DataDDS dds(factory, "Test_data", "DODS/3.2"); // Must use DataDDS on receving end
	FILE *dds_fp = move_dds(pin);
	DBG( fprintf( stderr, "Moved the DDS to a temp file\n" ) ) ;
	parse_mime(dds_fp);
	dds.parse(dds_fp);
	fclose(dds_fp);

	XDR *source = new_xdrstdio(pin, XDR_DECODE);

	// Back on the client side; deserialize the data *using the newly
	// generated DDS* (the one sent with the data).

	fprintf( stdout, "The data:\n" ) ;
	for (DDS::Vars_iter q = dds.var_begin(); q != dds.var_end(); q++)
	{
	    (*q)->deserialize(source, &dds);
	    (*q)->print_val(stdout);
	}

	delete_xdrstdio(source);
    }
    catch (Error &e) {
	delete factory; factory = 0;
	delete ttf; ttf = 0;

	e.display_message();
	return false;
    }
	
    delete ttf; ttf = 0;
    delete factory; factory = 0;

    return true;
}

// $Log: expr-test.cc,v $
// Revision 1.42  2005/03/30 21:55:34  jimg
// Now uses the BaseTypeFactory class.
//
// Revision 1.41  2005/02/08 21:31:57  jimg
// Merged with release-3-4-10.
//
// Revision 1.36.2.4  2005/02/03 01:26:11  jimg
// Fixed a conflict form the last merge.
//
// Revision 1.40  2005/01/28 17:25:13  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.36.2.3  2005/01/18 23:19:51  jimg
// Fixed the -k option. Added -b.
//
// Revision 1.39  2004/07/07 21:08:49  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.36.2.2  2004/05/02 00:07:20  rmorris
// Mod's to move to winsock2 under win32.  Necessary because of newer libcurl.
//
// Revision 1.38  2003/12/08 18:02:30  edavis
// Merge release-3-4 into trunk
//
// Revision 1.36.2.1  2003/07/24 00:45:06  jimg
// Added test_variable_sleep_interval. This is used to test timeouts by
// simulating very long read times.
//
// Revision 1.37  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.36  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.35  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.34.2.1  2003/02/21 00:10:08  jimg
// Repaired copyright.
//
// Revision 1.34  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.33  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.29.4.10  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.29.4.9  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.29.4.8  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.29.4.7  2002/09/05 22:52:55  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.29.4.6  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.32  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.29.4.5  2002/02/20 19:16:27  jimg
// Changed the expression parser so that variable names may contain only
// digits.
//
// Revision 1.29.4.4  2001/11/01 00:43:51  jimg
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
// Revision 1.31  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.29.4.3  2001/09/07 00:38:35  jimg
// Sequence::deserialize(...) now reads all the sequence values at once.
// Its call semantics are the same as the other classes' versions. Values
// are stored in the Sequence object using a vector<BaseType *> for each
// row (those are themselves held in a vector). Three new accessor methods
// have been added to Sequence (row_value() and two versions of var_value()).
// BaseType::deserialize(...) now always returns true. This matches with the
// expectations of most client code (the seqeunce version returned false
// when it was done reading, but all the calls for sequences must be changed
// anyway). If an XDR error is found, deserialize throws InternalErr.
//
// Revision 1.30  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.29.4.2  2001/08/18 00:02:57  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.29.4.1  2001/06/23 00:52:08  jimg
// Normalized the definitions of ID (SCAN_ID), INT, FLOAT and NEVER so
// that they are (more or less) the same in all the scanners. There are
// one or two characters that differ (for example das.lex allows ( and )
// in an ID while dds.lex, expr.lex and gse.lex don't) but the definitions
// are essentially the same across the board.
// Added `#' to the set of characeters allowed in an ID (bug 179).
//
// Revision 1.29  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.28  2000/09/21 16:22:10  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.27  2000/07/19 22:51:40  rmorris
// Call and return from main in a manner Visual C++ likes and
// exit the program with exit(0) so that DejaGnu/Cygwin based
// testsuite can succeed for win32.
//
// Revision 1.26  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.24  2000/06/07 18:07:00  jimg
// Merged the pc port branch
//
// Revision 1.23.4.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.23  2000/04/07 00:19:29  jimg
// Added exception handling
//
// Revision 1.22.14.1  2000/02/17 05:03:17  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.22  1999/05/04 19:47:24  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.21  1999/04/29 02:29:36  jimg
// Merge of no-gnu branch
//
// Revision 1.20  1999/03/24 23:32:05  jimg
// Added a verbose mode.
// Commented out the old transmit(...) function. Use constrained_trans(...)
// instead.
//
// Revision 1.19  1999/01/21 02:50:08  jimg
// Added code to test the expr scanner using strings and not files.
//
// Revision 1.18  1998/11/10 00:49:19  jimg
// Fixed up the online help.
//
// Revision 1.17  1998/09/17 17:00:02  jimg
// Added include files to get rid of compiler messages about missing
// prototypes.
//
// Revision 1.16.6.2  1999/02/05 09:32:36  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.16.6.1  1999/02/02 21:57:07  jimg
// String to string version
//
// Revision 1.16  1998/03/26 00:15:53  jimg
// Added keep_temps global for use with debuggers to keep those temp file
// around.
// Added parse_mime() to gobble up the mime header generated by DDS::send()
//
// Revision 1.15  1998/03/19 23:29:20  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.14  1997/09/22 22:33:14  jimg
// Added data file option. Now -f can be used to specify the name of a file
// from which to read data. This currently only works with Sequences, but
// in the future all test data could be read from a file.
// Added use of the DataDDS class (which is required by the new core
// software).
//
// Revision 1.13  1997/06/05 22:51:25  jimg
// Changed so that compression is not used.
//
// Revision 1.12  1996/08/13 18:55:20  jimg
// Added not_used to definition of char rcsid[].
// Uses the parser_arg object to communicate with the parser.
//
// Revision 1.11  1996/06/11 17:30:36  jimg
// Fixed -k (constraint expression) option when used with -p (parser) option.
//
// Revision 1.10  1996/06/04 21:34:00  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.9  1996/05/31 23:30:58  jimg
// Updated copyright notice.
//
// Revision 1.8  1996/05/29 22:04:13  jimg
// Removed old, useless, code.
//
// Revision 1.7  1996/05/22 18:05:35  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.6  1996/05/14 15:38:57  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.5  1996/03/05 00:57:19  jimg
// Fixed tests of constrained tranmission so CEs with spaces will be read
// properly.
// Added new option so that a CE may be given on the command line.
//
// Revision 1.4  1995/12/09  01:07:37  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.3  1995/12/06  19:43:09  jimg
// Added options for testing the constraint evaluator software.
// Added functions which test the constraint evaluator.
// Added function that simulates te complete client-server conversation which
// causes a variable to be sent after the evaluation of a CE. This manages
// multiple DDSs just as a real client would. This code is different than the
// simpler code run by evaluate_dds().
//
// Revision 1.2  1995/10/23  23:08:17  jimg
// Fixed scanner display code to match current scanner.
// Added code to test simple evaluator.
// Fixed type declarations (YYSTYPE, ...).
//
// Revision 1.1  1995/10/13  03:02:26  jimg
// First version. Runs scanner and parser.
//

