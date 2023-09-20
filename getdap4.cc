
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

// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// This is the source to `getdap'; a simple tool to exercise the Connect
// class. It can be used to get naked URLs as well as the DAP2 DAS and DDS
// objects.  jhrg.

#include "config.h"

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h>     // getopt

#include "DMR.h"
#include "XMLWriter.h"
#include "D4BaseTypeFactory.h"
#include "D4Group.h"
#include "D4Sequence.h"
#include "D4Connect.h"
#include "StdinResponse.h"
#include "HTTPConnect.h"
#include "RCReader.h"

using namespace std;
using namespace libdap;

const char *version = CVER " (" DVR " DAP/" DAP_PROTOCOL_VERSION ")";

static void usage(const string &)
{
    const char *message = R"(
    Usage: getdap4 [dD vVmzsM][-c <expr>][-m <num>] <url> [<url> ...]
           getdap4 [dD vVmzsM][-c <expr>][-m <num>] <file> [<file> ...]

    In the first form of the command, dereference the URL and perform
    the requested operations. This includes routing the returned
    information through the DAP processing library (parsing the
    returned objects, et c.). If none of d, or D are used with a URL,
    then the DAP library routines are NOT used and the URLs contents
    are dumped to standard output.

    Note: If the URL contains a query string the query string will be
    preserved in the request. However, if the query string contains
    DAP4 keys they may interfere with the operation of getdap4. A
    warning will be written to stderr when getdap4 identifies the
    presence of a DAP4 query key in the submitted URL's query string.

    In the second form of the command, assume the files are DAP4 data
    responses (stored in files or read from pipes)

    Options: 
            d: For each URL, get the (DAP4) DMR object. Does not get data. 
            D: For each URL, get the DAP4 Data response. 

            v: Verbose output. 
            V: Version of this client 
            i: For each URL, get the server version. 
            m: Request the same URL <num> times. 
            z: Ask the server to compress data. 
            s: Print Sequences using numbered rows. 
            M: Assume data read from a file has no MIME headers; use only 
               with files 

            c: <expr> is a constraint expression. Used with -d/D 
               NB: You can use a `?' for the CE also. 
            S: Used in conjunction with -d and will report the total size 
               of the data referenced in the DMR.)";

    cerr << message << endl;
}

// Used for raw http access/transfer
bool read_data(FILE *fp)
{
    if (!fp) {
        fprintf(stderr, "getdap4: Whoa!!! Null stream pointer.\n");
        return false;
    }
    // Changed from a loop that used getc() to one that uses fread(). getc()
    // worked fine for transfers of text information, but *not* for binary
    // transfers. fread() will handle both.
    char c = 0;
    while (fp && !feof(fp) && fread(&c, 1, 1, fp))
        printf("%c", c);        // stick with stdio

    return true;
}

static void
read_response_from_file(D4Connect *url, DMR &dmr, Response &r, bool mime_headers, bool get_dap4_data, bool get_dmr)
{
    if (mime_headers) {
        if (get_dap4_data)
            url->read_data(dmr, r);
        else if (get_dmr)
            url->read_dmr(dmr, r);
        else
            throw Error("Only supports Data or DMR responses");
    }
    else {
        if (get_dap4_data)
            url->read_data_no_mime(dmr, r);
        else if (get_dmr)
            url->read_dmr_no_mime(dmr, r);
        else
            throw Error("Only supports Data or DMR responses");
    }
}

static void print_group_data(D4Group *g, bool print_rows = false)
{
    for (Constructor::Vars_iter i = g->var_begin(), e = g->var_end(); i != e; i++) {
        if (print_rows && (*i)->type() == dods_sequence_c)
            dynamic_cast<D4Sequence &>(**i).print_val_by_rows(cout);
        else
            (*i)->print_val(cout);
    }

    for (D4Group::groupsIter gi = g->grp_begin(), ge = g->grp_end(); gi != ge; ++gi) {
        print_group_data(*gi, print_rows);
    }
}

static void print_data(DMR &dmr, bool print_rows = false)
{
    cout << "The data:" << endl;

    D4Group *g = dmr.root();

    print_group_data(g, print_rows);

    cout << endl << flush;
}

/** Get the size of a response. This method looks at the variables in the DDS
 *  a computes the number of bytes in the response.
 *
 *  @note This version of the method does a poor job with Sequences. A better
 *  implementation would look at row-constraint-based limitations and use them
 *  for size computations. If a row-constraint is missing, return an error.
 *
 *  @param constrained Should the size of the whole DDS be used or should the
 *  current constraint be taken into account?
 */
unsigned long long get_size(D4Group *grp, bool constrained = false)
{
    unsigned long long w = 0;

    for (auto var_itr = grp->var_begin(); var_itr != grp->var_end(); var_itr++) {
        if (constrained) {
            if ((*var_itr)->send_p())
                w += (*var_itr)->width(constrained);
        }
        else {
            w += (*var_itr)->width(constrained);
        }
    }
    for (auto grp_itr = grp->grp_begin(); grp_itr != grp->grp_end(); grp_itr++) {
        w += get_size(*grp_itr, constrained);
    }

    return w;
}

unsigned long long get_size(DMR &dmr, bool constrained = false)
{
    return get_size(dmr.root(), constrained);
}


int main(int argc, char *argv[])
{
    int option_char;

    bool get_dmr = false;
    bool get_dap4_data = false;
    bool verbose = false;
    bool accept_deflate = false;
    bool print_rows = false;
    bool mime_headers = true;
    bool report_errors = false;
    int times = 1;
    int dap_client_major = 4;
    int dap_client_minor = 0;
    string expr;
    bool compute_size = false;

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    while ((option_char = getopt(argc, argv, "dDvVrm:Mzsc:S")) != -1) {
        switch (option_char) {
            case 'd':
                get_dmr = true;
                break;
            case 'D':
                get_dap4_data = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'V':
                cerr << "getdap4 version: " << version << endl;
                exit(0);
            case 'S':
                compute_size = true;
                break;
            case 'r':
                report_errors = true;
                break;
            case 'm':
                times = atoi(optarg);
                break;
            case 'z':
                accept_deflate = true;
                break;
            case 's':
                print_rows = true;
                break;
            case 'M':
                mime_headers = false;
                break;
            case 'c':
                expr = optarg;
                break;
            case 'h':
            case '?':
            default:
                usage(argv[0]);
                exit(1);
        }
    }

    D4Connect *url = nullptr;
    try {
        // If after processing all the command line options there is nothing
        // left (no URL or file) assume that we should read from stdin.
        for (int i = optind; i < argc; ++i) {
            if (verbose)
                cerr << "Fetching: " << argv[i] << endl;

            string name = argv[i];
            url = new D4Connect(name);

            // This overrides the value set in the .dodsrc file.
            if (accept_deflate)
                url->set_accept_deflate(accept_deflate);

            if (dap_client_major > 2)
                url->set_xdap_protocol(dap_client_major, dap_client_minor);

            if (url->is_local()) {
                if (verbose)
                    cerr << "Assuming " << argv[i] << " is a file that contains a response object; decoding." << endl;

                try {
                    D4BaseTypeFactory factory;
                    DMR dmr(&factory);

                    if (strcmp(argv[i], "-") == 0) {
                        StdinResponse r(cin);

                        if (!r.get_cpp_stream())
                            throw Error("Could not open standard input.");

                        read_response_from_file(url, dmr, r, mime_headers, get_dap4_data, get_dmr);
                    }
                    else {
                        fstream f(argv[i], std::ios_base::in);
                        if (!f.is_open() || f.bad() || f.eof())
                            throw Error((string) "Could not open: " + argv[i]);

                        Response r(&f, 0);

                        read_response_from_file(url, dmr, r, mime_headers, get_dap4_data, get_dmr);
                    }

                    if (verbose)
                        cerr << "DAP version: " << url->get_protocol().c_str() << " Server version: "
                             << url->get_version().c_str() << endl;

                    // Always write the DMR
                    XMLWriter xml;
                    dmr.print_dap4(xml);
                    cout << xml.get_doc() << endl;

                    if (get_dap4_data)
                        print_data(dmr, print_rows);
                }
                catch (Error &e) {
                    cerr << "Error: " << e.get_error_message() << endl;
                    delete url;
                    url = nullptr;
                    if (report_errors)
                        return EXIT_FAILURE;
                }
            }
            else if (get_dmr) {
                for (int j = 0; j < times; ++j) {
                    D4BaseTypeFactory factory;
                    DMR dmr(&factory);
                    try {
                        url->request_dmr(dmr, expr);

                        if (verbose) {
                            cout << "DAP version: " << url->get_protocol() << ", Server version: " << url->get_version()
                                 << endl;
                            cout << "DMR:" << endl;
                        }

                        XMLWriter xml;
                        dmr.print_dap4(xml);
                        cout << xml.get_doc() << endl;
                        if (compute_size) {
                            cout << "DMR References " << get_size(dmr) << " bytes of data," << endl;
                        }
                    }
                    catch (Error &e) {
                        cerr << e.get_error_message() << endl;
                        if (report_errors)
                            return EXIT_FAILURE;
                        continue;       // Goto the next URL or exit the loop.
                    }
                }
            }
            else if (get_dap4_data) {
                for (int j = 0; j < times; ++j) {
                    D4BaseTypeFactory factory;
                    DMR dmr(&factory);
                    try {
                        url->request_dap4_data(dmr, expr);

                        if (verbose) {
                            cout << "DAP version: " << url->get_protocol() << ", Server version: " << url->get_version()
                                 << endl;
                            cout << "DMR:" << endl;
                        }

                        XMLWriter xml;
                        dmr.print_dap4(xml);
                        cout << xml.get_doc() << endl;

                        print_data(dmr, print_rows);
                    }
                    catch (Error &e) {
                        cerr << e.get_error_message() << endl;
                        if (report_errors)
                            return EXIT_FAILURE;
                        continue;       // Goto the next URL or exit the loop.
                    }
                }
            }
            else {
                HTTPConnect http(RCReader::instance());

                // This overrides the value set in the .dodsrc file.
                if (accept_deflate)
                    http.set_accept_deflate(accept_deflate);

                if (dap_client_major > 2)
                    url->set_xdap_protocol(dap_client_major, dap_client_minor);

                string url_string = argv[i];
                for (int j = 0; j < times; ++j) {
                    try {
                        HTTPResponse *r = http.fetch_url(url_string);
                        if (verbose) {
                            vector <string> *headers = r->get_headers();
                            copy(headers->begin(), headers->end(), ostream_iterator<string>(cout, "\n"));
                        }
                        if (!read_data(r->get_stream())) {
                            continue;
                        }
                        delete r;
                        r = 0;
                    }
                    catch (Error &e) {
                        cerr << e.get_error_message() << endl;
                        if (report_errors)
                            return EXIT_FAILURE;
                        continue;
                    }
                }
            }

            delete url;
            url = nullptr;
        }
    }
    catch (Error &e) {
        delete url;
        if (e.get_error_code() == malformed_expr) {
            cerr << e.get_error_message() << endl;
            usage(argv[0]);
        }
        else {
            cerr << e.get_error_message() << endl;

        }

        cerr << "Exiting." << endl;
        return EXIT_FAILURE;
    }
    catch (exception &e) {
        delete url;
        cerr << "C++ library exception: " << e.what() << endl;
        cerr << "Exiting." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
