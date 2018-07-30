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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

#include "config.h"

#include <fstream>

#include <GetOpt.h>

#include "parser.h"
#include "dds.tab.hh"
#include "BaseType.h"
#include "Int32.h"
#include "DDS.h"
#include "util.h"
#include "Error.h"

using namespace libdap;

void test_scanner();
void test_parser(const string &name);
void test_class();

int ddslex();

extern YYSTYPE ddslval;
extern int ddsdebug;
static bool print_ddx = false;

const char *prompt = "dds-test: ";

void usage(string name)
{
    cerr << "Usage: " << name << "[s] [pd] [c]" << endl << "s: Test the scanner." << endl << "p: Test the parser; reads from stdin and prints the"
        << endl << "   internal structure to stdout." << endl << "d: Turn on parser debugging. (only for the hard core.)" << endl
        << "c: Test the C++ code for manipulating DDS objects." << endl << "   Reads from stdin, parses and writes the modified DDS" << endl
        << "   to stdout." << endl;
}

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "spP:dcx"); // remove fF:
    int option_char;
    int scanner_test = 0, parser_test = 0, class_test = 0;
    string name = "";

    // process options
    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            ddsdebug = 1;
            break;
        case 's':
            scanner_test = 1;
            break;
        case 'p':
            parser_test = 1;
            break;
        case 'P':
            parser_test = 1;
            name = getopt.optarg;
            break;
        case 'x':
            print_ddx = true;
            break;
        case 'c':
            class_test = 1;
            break;
        case '?':
        default:
            usage(argv[0]);
            return 1;
        }

    if (!scanner_test && !parser_test && !class_test/* && !dap4_parser_test*/) {
        usage(argv[0]);
        return 1;
    }

    try {
        if (scanner_test) test_scanner();

        if (parser_test) test_parser(name);
        if (class_test) test_class();
    }
    catch (Error &e) {
        cerr << e.get_error_message() << endl;
    }
}

void test_scanner(void)
{
    int tok;

    cout << prompt << flush; // first prompt

    while ((tok = ddslex())) {
        switch (tok) {
        case SCAN_DATASET:
            cout << "DATASET" << endl;
            break;
        case SCAN_LIST:
            cout << "LIST" << endl;
            break;
        case SCAN_SEQUENCE:
            cout << "SEQUENCE" << endl;
            break;
        case SCAN_STRUCTURE:
            cout << "STRUCTURE" << endl;
            break;
        case SCAN_FUNCTION:
            cout << "FUNCTION" << endl;
            break;
        case SCAN_GRID:
            cout << "GRID" << endl;
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
            cout << "Url" << endl;
            break;
        case SCAN_WORD:
            cout << "WORD: " << ddslval.word << endl;
            break;
        case '{':
            cout << "Left Brace" << endl;
            break;
        case '}':
            cout << "Right Brace" << endl;
            break;
        case '[':
            cout << "Left Bracket" << endl;
            break;
        case ']':
            cout << "Right Bracket" << endl;
            break;
        case ';':
            cout << "Semicolon" << endl;
            break;
        case ':':
            cout << "Colon" << endl;
            break;
        case '=':
            cout << "Assignment" << endl;
            break;
        default:
            cout << "Error: Unrecognized input" << endl;
            break;
        }
        cout << prompt << flush; // print prompt after output
    }
}

void test_parser(const string &name)
{
    // Remove the dynamic allocation (and delete at the end)
    // Possible because DDS does not manage the storage and the
    // lifetime of 'table' is limited to this function.
#if 0
    BaseTypeFactory *factory = new BaseTypeFactory;
    DDS table(factory);
#endif

    BaseTypeFactory factory;
    DDS table(&factory);

    if (name.empty())
        table.parse();
    else
        table.parse(name);

    if (table.check_semantics())
        cout << "DDS past semantic check" << endl;
    else
        cout << "DDS failed semantic check" << endl;

    if (table.check_semantics(true))
        cout << "DDS past full semantic check" << endl;
    else
        cout << "DDS failed full semantic check" << endl;

    if (print_ddx)
        table.print_xml_writer(cout, false, "");
    else
        table.print(cout);

#if 0
    delete factory;
    factory = 0;
#endif
}

void test_class(void)
{
#if 0
    BaseTypeFactory *factory = new BaseTypeFactory;
#endif
    BaseTypeFactory factory;
    DDS table(&factory);
    table.parse();

    if (table.check_semantics())
        cout << "DDS past semantic check" << endl;
    else
        cout << "DDS filed semantic check" << endl;

    if (table.check_semantics(true))
        cout << "DDS past full semantic check" << endl;
    else
        cout << "DDS filed full semantic check" << endl;

    table.print(cout);

    DDS table2 = table; // test copy ctor;
    table2.print(cout);

#if 0
    BaseTypeFactory *factory2 = new BaseTypeFactory;
#endif
    BaseTypeFactory factory2;
    DDS table3(&factory2);
    table3 = table; // test operator=

    cout << "Dataset name: " << table.get_dataset_name() << endl;

    string name = "goofy";
    table.add_var(table.get_factory()->NewInt32(name)); // table dtor should delete this object

    table.print(cout);

    BaseType *btp = table.var(name);

    btp->print_decl(cout, "", true); // print out goofy w/semicolon

    table.del_var(name);

    table.print(cout);

    table.add_var(table.get_factory()->NewInt32("goofy"));

    table.print(cout);

    btp = table.var("goofy");

    btp->print_decl(cout, "", true); // print out goofy w/semicolon

    table.del_var("goofy");

    table.print(cout);

    for (DDS::Vars_iter p = table.var_begin(); p != table.var_end(); p++)
        (*p)->print_decl(cout, "", true); // print them all w/semicolons

#if 0
    delete factory;
    factory = 0;
    delete factory2;
    factory2 = 0;
#endif
}

