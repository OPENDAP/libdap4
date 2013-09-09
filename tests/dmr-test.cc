// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2013 OPeNDAP, Inc.
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

// Test the DMR parser

#include "config.h"

#include <fstream>
#include <tr1/memory>

#include <GetOpt.h>

#include "DMR.h"
#include "util.h"
#include "Error.h"

#include "ResponseBuilder.h"
#include "ConstraintEvaluator.h"

#include "D4ParserSax2.h"
#include "D4TestTypeFactory.h"

#include "util.h"

int test_variable_sleep_interval = 0;   // Used in Test* classes for testing timeouts.

using namespace libdap;

/**
 * Open the named XML file and parse it, assuming that it contains a DMR.
 * @param name The name of the DMR XML file (or '-' for stdin)
 * @param debug True if the debug mode of the parse should be used
 * @param print Once parsed, should the DMR object be printed?
 * @return true if the parse worked, false otherwise
 */
DMR *
test_dap4_parser(const string &name, bool debug, bool print)
{
    D4TestTypeFactory *factory = new D4TestTypeFactory;
    DMR *table = new DMR(factory);

    try {
        D4ParserSax2 parser;
        if (name == "-") {
            parser.intern(cin, table, debug);
        }
        else {
            fstream in(name.c_str(), ios_base::in);
            parser.intern(in, table, debug);
        }
    }
    catch(Error &e) {
        cerr << "Parse error: " << e.get_error_message() << endl;
        delete factory;
        delete table;
        return 0;
    }

    cerr << "Parse successful" << endl;

    if (print) {
        XMLWriter xml("    ");
        table->print_dap4(xml, false);
        cout << xml.get_doc() << endl;
    }

    return table;
}

/**
 * Should the changing values - meant to mimic the DTS - be used?
 * @param dmr Set for this DMR
 * @param state True to use the DTS-like values, false otherwise
 */
void
set_series_values(DMR *dmr, bool state)
{
	TestCommon *tc = dynamic_cast<TestCommon*>(dmr->root());
	if (tc)
		tc->set_series_values(state);
	else
		cerr << "Could not cast root group to TestCommon (" << dmr->root()->type_name() << ", " << dmr->root()->name() << ")" << endl;
}

/**
 * Call the parser and then serialize the resulting DMR after applying the
 * constraint. The persistent representation is written to a file. The file
 * is name '<name>_data.bin'.
 *
 * @param name The name of the XML file that holds the dataset DMR
 * @param debug Turn on parser debugging
 * @param print Use libdap to print the in-memory DMR/DDS object
 * @param constraint The constraint expression to apply.
 * @param series_values Use the Test* classes' series values?
 */
void
send_data(const string &name, bool debug, bool print, const string &constraint, bool series_values)
{
    DMR *server = test_dap4_parser(name, debug, print);

    set_series_values(server, series_values);

    ConstraintEvaluator eval;	// This is a place holder. jhrg 9/6/13
    ResponseBuilder rb;
    rb.set_ce(constraint);
    rb.set_dataset_name(name);

    // TODO Remove once real CE evaluator is written. jhrg 9/6/13
    // Mark all variables to be sent in their entirety.
    server->root()->set_send_p(true);

    string file_name = path_to_filename(name) + "_data.bin";
    ofstream out(file_name.c_str(), ios::out|ios::trunc|ios::binary);
    rb.send_data_dmr(out, *server, eval, "start", "boundary", true);
    out.close();

#if 0
    // Now do what Connect::request_data() does:
    FILE *fp = fopen("expr-test-data.bin", "r");

    Response r(fp, 400);
    Connect c("http://dummy_argument");

    BaseTypeFactory factory;
    DataDDS dds(&factory, "Test_data", "DAP/3.2");      // Must use DataDDS on receiving end

    c.read_data(dds, &r);

    cout << "The data:" << endl;
    for (DDS::Vars_iter q = dds.var_begin(); q != dds.var_end(); q++) {
        (*q)->print_val(cout);
    }
#endif
}

void usage()
{
    cerr << "Usage: dmr-test -p|s <file> [-d -x]" << endl
            << "p: parse a file (use "-" for stdin)" << endl
            << "s: parse and 'send' a file (or stdin)" << endl
            << "d: turn on detailed debugging" << endl
            << "x: print the binary object(s) built by the parse." << endl;
}

int
main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "p:s:xd");
    int option_char;
    bool parse = false;
    bool debug = false;
    bool print = false;
    bool send = false;
    string name = "";

    // process options

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = true;
            break;

        case 'p':
            parse = true;
            name = getopt.optarg;
            break;

        case 'x':
            print = true;
            break;

        case 's':
        	send = true;
        	name = getopt.optarg;
        	break;

        case '?':
        case 'h':
            usage();
            return 0;

        default:
            cerr << "Error: ";
            usage();
            return 1;
        }

    if (! (parse || send)) {
        cerr << "Error: ";
        usage();
        return 1;
    }

    try {
        if (parse)
            (void)test_dap4_parser(name, debug, print);	// ignore the return value

        // Add constraint and series values when ready
        if (send)
        	send_data(name, debug, print, "", false);
    }
    catch (Error &e) {
        cerr << e.get_error_message() << endl;
        return 1;
    }

    return 0;
}

