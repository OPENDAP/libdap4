
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

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Test the CE scanner and parser.
//
// jhrg 9/12/95

#include "config.h"

//#define DODS_DEBUG

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <string>

#include "GetOpt.h"

#include "BaseType.h"
#include "DDS.h"
#include "DataDDS.h"
#include "ConstraintEvaluator.h"
#include "ServerFunctionsList.h"
#include "XDRStreamUnMarshaller.h"
#include "XDRStreamMarshaller.h"
#include "ResponseBuilder.h"
#include "Response.h"
#include "Connect.h"
#include "Error.h"

#include "TestSequence.h"
#include "TestCommon.h"
#include "TestTypeFactory.h"

#include "TestFunction.h"

#include "parser.h"
#include "expr.h"
#include "ce_expr.tab.hh"
#include "util.h"

#include "debug.h"

using namespace std;

int test_variable_sleep_interval = 0;   // Used in Test* classes for testing
                                      // timeouts.

#define CRLF "\r\n"             // Change this here and in mime_util.cc
#define DODS_DDS_PRX "dods_dds"
#define YY_BUFFER_STATE (void *)

void test_scanner(const string & str);
void test_scanner(bool show_prompt);
void test_parser(ConstraintEvaluator & eval, DDS & table,
                 const string & dds_name, string constraint);
bool read_table(DDS & table, const string & name, bool print);
void evaluate_dds(DDS & table, bool print_constrained, bool xml_syntax);
void constrained_trans(const string & dds_name, const bool constraint_expr,
                       const string & ce, const bool series_values);
void intern_data_test(const string & dds_name, const bool constraint_expr,
                 const string & ce, const bool series_values);

int ce_exprlex();               // exprlex() uses the global ce_exprlval
// int ce_exprparse(void *arg);
void ce_exprrestart(FILE * in);

// Glue routines declared in expr.lex
void ce_expr_switch_to_buffer(void *new_buffer);
void ce_expr_delete_buffer(void *buffer);
void *ce_expr_string(const char *yy_str);

extern int ce_exprdebug;

#if 0
static int keep_temps = 0;      // MT-safe; test code.
#endif

const string version = "version 1.12";
const string prompt = "expr-test: ";
const string options = "sS:bdecvp:w:W:f:k:vx?";
const string usage = "\
\nexpr-test [-s [-S string] -d -c -v [-p dds-file]\
\n[-e expr] [-w|-W dds-file] [-f data-file] [-k expr]]\
\nTest the expression evaluation software.\
\nOptions:\
\n  -s: Feed the input stream directly into the expression scanner, does\
\n      not parse.\
\n  -S: <string> Scan the string as if it was standard input.\
\n  -d: Turn on expression parser debugging.\
\n  -c: Print the constrained DDS (the one that will be returned\
\n      prepended to a data transmission. Must also supply -p and -e \
\n  -v: Verbose output\
\n  -V: Print the version of expr-test\
\n  -p: DDS-file: Read the DDS from DDS-file and create a DDS object,\
\n      then prompt for an expression and parse that expression, given\
\n      the DDS object.\
\n  -e: Evaluate the constraint expression. Must be used with -p.\
\n  -w: Do the whole enchilada. You don't need to supply -p, -e, ...\
\n      This prompts for the constraint expression and the optional\
\n      data file name. NOTE: The CE parser Error objects do not print\
\n      with this option.\
\n  -W: Similar to -w but uses the new (11/2007) intern_data() methods\
\n      in place of the serialize()/deserialize() combination.\
\n  -b: Use periodic/cyclic/changing values. For testing Sequence CEs.\
\n  -f: A file to use for data. Currently only used by -w for sequences.\
\n  -k: A constraint expression to use with the data. Works with -p,\
\n      -e, -t and -w\
\n  -x: Print declarations using the XML syntax. Does not work with the\
\n      data printouts.\
\n  -?: Print usage information";

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, options.c_str());

    int option_char;
    bool scanner_test = false, parser_test = false, evaluate_test = false;
    bool print_constrained = false;
    bool whole_enchalada = false, constraint_expr = false;
    bool whole_intern_enchalada = false;
    bool scan_string = false;
    bool verbose = false;
    bool series_values = false;
    bool xml_syntax = false;
    string dds_file_name;
    string dataset = "";
    string constraint = "";
    TestTypeFactory ttf;
    DDS table(&ttf);

    // Load our one server function...
    ServerFunction *scale = new TestFunction;
    ServerFunctionsList::TheList()->add_function(scale);

    // process options

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'b':
            series_values = true;
            break;
        case 'd':
            ce_exprdebug = true;
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
        case 'W':
            whole_intern_enchalada = true;
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
            cerr << argv[0] << ": " <<  version << endl;
            exit(0);
        case 'x':
        	xml_syntax = true;
        	break;
        case '?':
        default:
            cerr << usage << endl;
            exit(1);
            break;
        }

    try {
        if (!scanner_test && !parser_test && !evaluate_test
            && !whole_enchalada && !whole_intern_enchalada) {
            cerr << usage << endl;
            exit(1);
        }
        // run selected tests

        if (scanner_test) {
            if (scan_string)
                test_scanner(constraint);
            else
                test_scanner(true);

            exit(0);
        }

        if (parser_test) {
            ConstraintEvaluator eval;

            test_parser(eval, table, dds_file_name, constraint);
        }

        if (evaluate_test) {
            evaluate_dds(table, print_constrained, xml_syntax);
        }

        if (whole_enchalada) {
            constrained_trans(dds_file_name, constraint_expr, constraint, series_values);
        }
        if (whole_intern_enchalada) {
            intern_data_test(dds_file_name, constraint_expr, constraint, series_values);
        }
    }
    catch(Error & e) {
        cerr <<e.get_error_message() << endl;
        exit(1);
    }
    catch(exception & e) {
        cerr << "Caught exception: " << e.what() << endl;
        exit(1);
    }

    exit(0);
}

// Instead of reading the tokens from stdin, read them from a string.

void test_scanner(const string & str)
{
    ce_exprrestart(0);
    void *buffer = ce_expr_string(str.c_str());
    ce_expr_switch_to_buffer(buffer);

    test_scanner(false);

    ce_expr_delete_buffer(buffer);
}

void test_scanner(bool show_prompt)
{
    if (show_prompt)
        cout << prompt;

    int tok;
    while ((tok = ce_exprlex())) {
        switch (tok) {
        case SCAN_WORD:
            cout << "WORD: " << ce_exprlval.id << endl;
            break;
        case SCAN_STR:
            cout << "STR: " << *ce_exprlval.val.v.s << endl;
            break;
        case SCAN_EQUAL:
            cout << "EQUAL: " << ce_exprlval.op << endl;
            break;
        case SCAN_NOT_EQUAL:
            cout << "NOT_EQUAL: " << ce_exprlval.op << endl;
            break;
        case SCAN_GREATER:
            cout << "GREATER: " << ce_exprlval.op << endl;
            break;
        case SCAN_GREATER_EQL:
            cout << "GREATER_EQL: " << ce_exprlval.op << endl;
            break;
        case SCAN_LESS:
            cout << "LESS: " << ce_exprlval.op << endl;
            break;
        case SCAN_LESS_EQL:
            cout << "LESS_EQL: " << ce_exprlval.op << endl;
            break;
        case SCAN_REGEXP:
            cout << "REGEXP: " << ce_exprlval.op << endl;
            break;
        case SCAN_STAR:
            cout << "STAR: " << ce_exprlval.op << endl;
            break;
        case '.':
            cout << "Field Selector" << endl;
            break;
        case ',':
            cout << "List Element Separator" << endl;
            break;
        case '[':
            cout << "Left Bracket" << endl;
            break;
        case ']':
            cout << "Right Bracket" << endl;
            break;
        case '(':
            cout << "Left Paren" << endl;
            break;
        case ')':
            cout << "Right Paren" << endl;
            break;
        case '{':
            cout << "Left Brace" << endl;
            break;
        case '}':
            cout << "Right Brace" << endl;
            break;
        case ':':
            cout << "Colon" << endl;
            break;
        case '&':
            cout << "Ampersand" << endl;
            break;
        case SCAN_HASH_INT32:
            cout << "Hash Int32" << endl;
            break;
        default:
            cout << "Error: Unrecognized input" << endl;
            break;
        }

        cout << prompt << flush;  // print prompt after output
    }
}

// NB: The DDS is read in via a file because reading from stdin must be
// terminated by EOF. However, the EOF used to terminate the DDS also closes
// stdin and thus the expr scanner exits immediately.

void
test_parser(ConstraintEvaluator & eval, DDS & dds, const string & dds_name,
            string constraint)
{
    try {
        read_table(dds, dds_name, true);

        if (constraint.empty()) {
            cout << "Constraint:";
            char c[256];
            cin.getline(c, 256);
            if (!cin)
                throw InternalErr(__FILE__, __LINE__,
                                  "Could not read the constraint expression\n");
            constraint = c;
        }

        eval.parse_constraint(constraint, dds);
        fprintf(stdout, "Input parsed\n");      // Parser throws on failure.
    }
    catch(Error & e) {
        cerr << e.get_error_message() << endl;
    }
}

// Read a DDS from stdin and build the corresponding DDS. IF PRINT is true,
// print the text representation of that DDS on the stdout. The DDS TABLE is
// modified as a side effect.
//
// Returns: true iff that DDS pasted the semantic_check() mfunc, otherwise
// false.

bool read_table(DDS & table, const string & name, bool print)
{
    table.parse(name);

    if (print)
        table.print(cout);

    if (table.check_semantics(true))
        return true;
    else {
        fprintf(stdout, "Input did not pass semantic checks\n");
        return false;
    }
}

void evaluate_dds(DDS & table, bool print_constrained, bool xml_syntax)
{
    if (print_constrained) {
    	if (xml_syntax)
    		table.print_xml(cout, print_constrained, "");
    	else
    		table.print_constrained(cout);
    }
    else {
        for (DDS::Vars_iter p = table.var_begin(); p != table.var_end(); p++) {
        	if (xml_syntax)
        		(*p)->print_decl(cout, "", print_constrained);
        	else
        		(*p)->print_decl(cout, "", true, true);
        }
    }
}

// Gobble up the MIME header. At one time the MIME Headers were output from
// the server filter programs (not the core software) so we could call
// DDS::send() from this test code and not have to parse the MIME header. But
// in order to get errors to work more reliably the header generation was
// moved `closer to the send'. That is, we put off determining whether to
// send a DDS or an Error object until later. That trade off is that the
// header generation is not buried in the core software. This code simply
// reads until the end of the header is found. 3/25/98 jhrg

void parse_mime(FILE * data_source)
{
    char line[256];

    fgets(line, 256, data_source);

    while (strncmp(line, CRLF, 2) != 0)
        fgets(line, 256, data_source);
}

void set_series_values(DDS & dds, bool state)
{
    for (DDS::Vars_iter q = dds.var_begin(); q != dds.var_end(); q++) {
        dynamic_cast < TestCommon & >(**q).set_series_values(state);
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

void
constrained_trans(const string & dds_name, const bool constraint_expr,
                  const string & constraint, const bool series_values)
{
    // If the CE was not passed in, read it from the command line.
    string ce;
    if (!constraint_expr) {
        cout << "Constraint:";
        char c[256];
        cin.getline(c, 256);
        if (!cin) {
            throw InternalErr(__FILE__, __LINE__, "Could not read the constraint expression\n");
        }
        ce = c;
    }
    else {
        ce = constraint;
    }

    TestTypeFactory ttf;
    DDS server(&ttf);
    ConstraintEvaluator eval;

    cout << "The complete DDS:" << endl;
    read_table(server, dds_name, true);

    // by default this is false (to get the old-style values that are
    // constant); set series_values to true for testing Sequence constraints.
    // 01/14/05 jhrg And Array constraints, although it's of limited
    // versatility 02/05/07 jhrg
    set_series_values(server, series_values);

    ResponseBuilder df;
    df.set_ce(ce);
    df.set_dataset_name(dds_name);

    ofstream out("expr-test-data.bin", ios::out|ios::trunc|ios::binary);
    df.send_data(out, server, eval, true);
    out.close();

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
}

/** This function does what constrained_trans() does but does not use the
    serialize()/deserialize() methods. Instead it uses the new (11/2007)
    intern_data() methods.

    @param dds_name
    @param constraint_expr True is one was given, else false
    @param constraint The constraint expression if \c constraint_expr is
    true.
    @param series_values True if TestTypes should generate 'series values'
    like the DTS. False selects the old-style values. */
void
intern_data_test(const string & dds_name, const bool constraint_expr,
                 const string & constraint, const bool series_values)
{
    // If the CE was not passed in, read it from the command line.
    string ce;
    if (!constraint_expr) {
        cout << "Constraint: ";
        char c[256];
        cin.getline(c, 256);
        if (!cin) {
            throw InternalErr(__FILE__, __LINE__,
                              "Could not read the constraint expression\n");
        }
        ce = c;
    }
    else {
        ce = constraint;
    }

    TestTypeFactory ttf;
    DDS server(&ttf);
    ConstraintEvaluator eval;

    cout << "The complete DDS:\n";
    read_table(server, dds_name, true);

    // by default this is false (to get the old-style values that are
    // constant); set series_values to true for testing Sequence constraints.
    // 01/14/05 jhrg And Array constraints, although it's of limited
    // versatility 02/05/07 jhrg
    set_series_values(server, series_values);

    eval.parse_constraint(ce, server);  // Throws Error if the ce doesn't parse.

    server.tag_nested_sequences();      // Tag Sequences as Parent or Leaf node.

    if (eval.function_clauses()) {
        DDS *fdds = eval.eval_function_clauses(server);

        for (DDS::Vars_iter i = fdds->var_begin(); i != fdds->var_end(); i++)
            if ((*i)->send_p())
                (*i)->intern_data(eval, *fdds);

        cout << "The data:\n";

        // This code calls 'output_values()' because print_val() does not test
        // the value of send_p(). We need to wrap a method around the calls to
        // print_val() to ensure that only values for variables with send_p() set
        // are called. In the serialize/deserialize case, the 'client' DDS only
        // has variables sent by the 'server' but in the intern_data() case, the
        // whole DDS is still present and only variables selected in the CE have
        // values.
        for (DDS::Vars_iter q = fdds->var_begin(); q != fdds->var_end(); q++) {
            if ((*q)->send_p()) {
                (*q)->print_decl(cout, "", false, false, true);
                cout << " = ";
                dynamic_cast<TestCommon&>(**q).output_values(cout);
                cout << ";\n";
            }
        }

        delete fdds;
    }
    else {
        for (DDS::Vars_iter i = server.var_begin(); i != server.var_end(); i++)
            if ((*i)->send_p())
                (*i)->intern_data(eval, server);
        cout << "The data:\n";

        // This code calls 'output_values()' because print_val() does not test
        // the value of send_p(). We need to wrap a method around the calls to
        // print_val() to ensure that only values for variables with send_p() set
        // are called. In the serialize/deserialize case, the 'client' DDS only
        // has variables sent by the 'server' but in the intern_data() case, the
        // whole DDS is still present and only variables selected in the CE have
        // values.
        for (DDS::Vars_iter q = server.var_begin(); q != server.var_end(); q++) {
            if ((*q)->send_p()) {
                (*q)->print_decl(cout, "", false, false, true);
                cout << " = ";
                dynamic_cast<TestCommon&>(**q).output_values(cout);
                cout << ";\n";
            }
        }
    }
}
