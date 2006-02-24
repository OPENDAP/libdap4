
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

#include "config.h"

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
        fprintf(stderr, "Caught exception: %s\n", e.what());
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
        cerr << e.get_error_message() << endl;
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
        cerr << e.get_error_message() << endl;
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

        cerr << e.get_error_message() << endl;
	return false;
    }
	
    delete ttf; ttf = 0;
    delete factory; factory = 0;

    return true;
}

