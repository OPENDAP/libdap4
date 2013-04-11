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

#include <GetOpt.h>

#include "DMR.h"
#include "util.h"
#include "Error.h"

#include "D4ParserSax2.h"
#include "D4BaseTypeFactory.h"

using namespace libdap;

void test_dap4_parser(const string &name, bool debug, bool print);

void usage(string name) {
    cerr << "Usage: " << name << "-p | -P <file> [-d -x]" << endl
            << "where p or P parse stdin or a file" << endl
            << "d: turn on detailed debugging" << endl
            << "x: print the binary object(s) built by the parse.";
}

int main(int argc, char *argv[]) {
    GetOpt getopt(argc, argv, "pP:x");
    int option_char;
    int dap4_parser_test = 0;
    bool debug = false, print = false;
    string name = "";

    // process options

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = 1;
            break;

        case 'p':
            dap4_parser_test = 1;
            break;

        case 'P':
            dap4_parser_test = 1;
            name = getopt.optarg;
            break;

        case 'x':
            print = true;
            break;

        case '?':
        case 'h':
            usage(argv[0]);
            return 0;

        default:
            cerr << "Error: ";
            usage(argv[0]);
            return 1;
        }

    if (!dap4_parser_test) {
        cerr << "Error: ";
        usage(argv[0]);
        return 1;
    }

    try {
        if (dap4_parser_test)
            test_dap4_parser(name, debug, print);
    }
    catch (Error &e) {
        cerr << e.get_error_message() << endl;
        return 1;
    }

    return 0;
}

void test_dap4_parser(const string &name, bool /*debug*/, bool print)
{
    D4BaseTypeFactory factory;
    DMR table(&factory);

    try {
        D4ParserSax2 parser;
        if (name.empty()) {
            parser.intern(cin, &table);
        }
        else {
            fstream in(name.c_str(), ios_base::in);
            parser.intern(in, &table);
        }
    }
    catch(D4ParseError &e) {
        cerr << "Parse error: " << e.get_error_message() << endl;
        return;
    }

    cerr << "Parse successful" << endl;

    if (print) {
        XMLWriter xml("    ");
        table.print_dap4(xml, false);
        cout << xml.get_doc() << endl;
    }
}

