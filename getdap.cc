
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

#include "GetOpt.h"

#include "Sequence.h"
#include "Connect.h"
#include "Response.h"
#include "StdinResponse.h"

using std::cerr;
using std::endl;
using std::flush;

using namespace libdap ;

const char *version = CVER " (" DVR " DAP/" DAP_PROTOCOL_VERSION ")";

extern int libdap::dods_keep_temps;     // defined in HTTPResponse.h
extern int libdap::www_trace;

void usage(string name)
{
    cerr << "Usage: " << name << endl;
    cerr <<
    " [idDaxAVvks] [-B <db>][-c <expr>][-m <num>] <url> [<url> ...]" <<
    endl;
    cerr << " [VvksM] <file> [<file> ...]" << endl;
    cerr << endl;
    cerr << "In the first form of the command, dereference the URL and"
    << endl;
    cerr << "perform the requested operations. This includes routing" <<
    endl;
    cerr << "the returned information through the DAP processing" << endl;
    cerr << "library (parsing the returned objects, et c.). If none" <<
    endl;
    cerr << "of a, d, or D are used with a URL, then the DAP library" <<
    endl;
    cerr << "routines are NOT used and the URLs contents are dumped" <<
    endl;
    cerr << "to standard output." << endl;
    cerr << endl;
    cerr << "In the second form of the command, assume the files are" <<
    endl;
    cerr << "DataDDS objects (stored in files or read from pipes)" << endl;
    cerr << "and process them as if -D were given. In this case the" <<
    endl;
    cerr << "information *must* contain valid MIME header in order" <<
    endl;
    cerr << "to be processed." << endl;
    cerr << endl;
    cerr << "Options:" << endl;
    cerr << "        i: For each URL, get the server version." << endl;
    cerr << "        d: For each URL, get the the DDS." << endl;
    cerr << "        a: For each URL, get the the DAS." << endl;
    cerr << "        D: For each URL, get the the DataDDS." << endl;
    cerr <<
    "        x: For each URL, get the DDX object. Does not get data."
    << endl;
    cerr << "        X: Request a DataDDX from the server (the DAP4 data response" << endl;
    cerr << "        B: Build a DDX in getdap using the DDS and DAS." << endl;
    cerr << "        v: Verbose output." << endl;
    cerr << "        V: Version of this client; see 'i' for server version." << endl;
    cerr << "        c: <expr> is a constraint expression. Used with -D/X." <<
    endl;
    cerr << "           NB: You can use a `?' for the CE also." << endl;
    cerr << "        k: Keep temporary files created by libdap." << endl;
    cerr << "        m: Request the same URL <num> times." << endl;
    cerr << "        z: Ask the server to compress data." << endl;
    cerr << "        s: Print Sequences using numbered rows." << endl;
    cerr << "        M: Assume data read from a file has no MIME headers" << endl;
    cerr << "           (the default is to assume the headers are present)." << endl;
    cerr << "        p: Set DAP protocol to x.y" << endl;
}

bool read_data(FILE * fp)
{
    if (!fp) {
        fprintf(stderr, "getdap: Whoa!!! Null stream pointer.\n");
        return false;
    }
    // Changed from a loop that used getc() to one that uses fread(). getc()
    // worked fine for transfers of text information, but *not* for binary
    // transfers. fread() will handle both.
    char c;
    while (fp && !feof(fp) && fread(&c, 1, 1, fp))
        printf("%c", c);        // stick with stdio

    return true;
}

static void print_data(DDS & dds, bool print_rows = false)
{
    cout << "The data:" << endl;

    for (DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++) {
        BaseType *v = *i;
        if (print_rows && (*i)->type() == dods_sequence_c)
            dynamic_cast < Sequence * >(*i)->print_val_by_rows(cout);
        else
            v->print_val(cout);
    }

    cout << endl << flush;
}

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "idaDxXBVvkc:m:zshM?Hp:t");
    int option_char;

    bool get_das = false;
    bool get_dds = false;
    bool get_data = false;
    bool get_ddx = false;
    bool get_data_ddx = false;
    bool build_ddx = false;
    bool get_version = false;
    bool cexpr = false;
    bool verbose = false;
    bool multi = false;
    bool accept_deflate = false;
    bool print_rows = false;
    bool mime_headers = true;
    int times = 1;
    int dap_client_major = 2;
    int dap_client_minor = 0;
    string expr = "";

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            get_dds = true;
            break;
        case 'a':
            get_das = true;
            break;
        case 'D':
            get_data = true;
            break;
        case 'x':
            get_ddx = true;
            break;
        case 'X':
            get_data_ddx = true;
            break;
        case 'V':
            fprintf(stderr, "getdap version: %s\n", version);
            exit(0);
        case 'i':
            get_version = true;
            break;
        case 'v':
            verbose = true;
            break;
        case 'k':
            dods_keep_temps = 1;
            break;              // keep_temp is in Connect.cc
        case 'c':
            cexpr = true;
            expr = getopt.optarg;
            break;
        case 'm':
            multi = true;
            times = atoi(getopt.optarg);
            break;
        case 'B':
            build_ddx = true;
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
        case 'p': {
            istringstream iss(getopt.optarg);
            char dot;
            iss >> dap_client_major;
            iss >> dot;
            iss >> dap_client_minor;
            break;
        }
        case 't':
            www_trace = 1;
            break;
        case 'h':
        case '?':
        default:
            usage(argv[0]);
            exit(1);
            break;
        }

    try {
        // If after processing all the command line options there is nothing
        // left (no URL or file) assume that we should read from stdin.
        for (int i = getopt.optind; i < argc; ++i) {
            if (verbose)
                fprintf(stderr, "Fetching: %s\n", argv[i]);

            string name = argv[i];
            Connect *url = 0;

            url = new Connect(name);

            // This overrides the value set in the .dodsrc file.
            if (accept_deflate)
                url->set_accept_deflate(accept_deflate);

            if (dap_client_major > 2)
                url->set_xdap_protocol(dap_client_major, dap_client_minor);

            if (url->is_local()) {
                if (verbose) {
                    fprintf(stderr,
                            "Assuming that the argument %s is a file that contains a DAP2 data object; decoding.\n", argv[i]);
                }

                Response *r = 0;
                BaseTypeFactory factory;
                DataDDS dds(&factory);

                try {
                    if (strcmp(argv[i], "-") == 0) {
                        r = new StdinResponse(stdin);

                        if (!r->get_stream())
                            throw Error("Could not open standard input.");

                        if (mime_headers)
                            url->read_data(dds, r); // The default case
                        else
                            url->read_data_no_mime(dds, r);
                    }
                    else {
                        r = new Response(fopen(argv[i], "r"), 0);

                        if (!r->get_stream())
                            throw Error(string("The input source: ")
                                        + string(argv[i])
                                        + string(" could not be opened"));

                        url->read_data_no_mime(dds, r);
                    }
                }
                catch (Error & e) {
                    cerr << e.get_error_message() << endl;
                    delete r;
                    r = 0;
                    delete url;
                    url = 0;
                    break;
                }

                if (verbose)
                    fprintf(stderr, "DAP version: %s, Server version: %s\n",
                            url->get_protocol().c_str(),
                            url->get_version().c_str());

                print_data(dds, print_rows);

            }

            else if (get_version) {
                fprintf(stderr, "DAP version: %s, Server version: %s\n",
                        url->request_protocol().c_str(),
                        url->get_version().c_str());
            }

            else if (get_das) {
                for (int j = 0; j < times; ++j) {
                    DAS das;
                    try {
                        url->request_das(das);
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        delete url;
                        url = 0;
                        continue;
                    }

                    if (verbose) {
                        fprintf(stderr, "DAP version: %s, Server version: %s\n",
                                url->get_protocol().c_str(),
                                url->get_version().c_str());

                        fprintf(stderr, "DAS:\n");
                    }

                    das.print(stdout);
                }
            }

            else if (get_dds) {
                for (int j = 0; j < times; ++j) {
                    BaseTypeFactory factory;
                    DDS dds(&factory);
                    try {
                        url->request_dds(dds, expr);
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        delete url;
                        url = 0;
                        continue;       // Goto the next URL or exit the loop.
                    }

                    if (verbose) {
                        fprintf(stderr, "DAP version: %s, Server version: %s\n",
                                url->get_protocol().c_str(),
                                url->get_version().c_str());

                        fprintf(stderr, "DDS:\n");
                    }

                    dds.print(cout);
                }
            }

            else if (get_ddx) {
                for (int j = 0; j < times; ++j) {
                    BaseTypeFactory factory;
                    DDS dds(&factory);
                    try {
                        url->request_ddx(dds, expr);
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        continue;       // Goto the next URL or exit the loop.
                    }

                    if (verbose) {
                        fprintf(stderr, "DAP version: %s, Server version: %s\n",
                                url->get_protocol().c_str(),
                                url->get_version().c_str());

                        fprintf(stderr, "DDX:\n");
                    }

                    dds.print_xml(cout, false);
                }
            }

            else if (build_ddx) {
                for (int j = 0; j < times; ++j) {
                    BaseTypeFactory factory;
                    DDS dds(&factory);
                    try {
                        url->request_dds(dds, expr);
                        DAS das;
                        url->request_das(das);
                        dds.transfer_attributes(&das);
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        continue;       // Goto the next URL or exit the loop.
                    }

                    if (verbose) {
                        fprintf(stderr, "DAP version: %s, Server version: %s\n",
                                url->get_protocol().c_str(),
                                url->get_version().c_str());

                        fprintf(stderr, "Client-built DDX:\n");
                    }

                    dds.print_xml(cout, false);
                }
            }

            else if (get_data) {
                for (int j = 0; j < times; ++j) {
                    BaseTypeFactory factory;
                    DataDDS dds(&factory);
                    try {
                        DBG(cerr << "URL: " << url->URL(false) << endl);
                        DBG(cerr << "CE: " << expr << endl);
                        url->request_data(dds, expr);

                        if (verbose)
                            fprintf(stderr, "DAP version: %s, Server version: %s\n",
                                    url->get_protocol().c_str(),
                                    url->get_version().c_str());

                        print_data(dds, print_rows);
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        delete url;
                        url = 0;
                        continue;
                    }
                }
            }

            else if (get_data_ddx) {
                for (int j = 0; j < times; ++j) {
                    BaseTypeFactory factory;
                    DataDDS dds(&factory);
                    try {
                        DBG(cerr << "URL: " << url->URL(false) << endl);
                        DBG(cerr << "CE: " << expr << endl);
                        url->request_data_ddx(dds, expr);

                        if (verbose)
                            fprintf(stderr, "DAP version: %s, Server version: %s\n",
                                    url->get_protocol().c_str(),
                                    url->get_version().c_str());

                        print_data(dds, print_rows);
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        delete url;
                        url = 0;
                        continue;
                    }
                }
            }

            else {
                // if (!get_das && !get_dds && !get_data) This code uses
                // HTTPConnect::fetch_url which cannot be accessed using an
                // instance of Connect. So some of the options supported by
                // other URLs won't work here (e.g., the verbose option
                // doesn't show the server version number).
                HTTPConnect http(RCReader::instance());

                // This overrides the value set in the .dodsrc file.
                if (accept_deflate)
                    http.set_accept_deflate(accept_deflate);

                if (dap_client_major > 2)
                    url->set_xdap_protocol(dap_client_major, dap_client_minor);

                string url_string = argv[i];
                for (int j = 0; j < times; ++j) {
                    try {
                        Response *r = http.fetch_url(url_string);
                        if (!read_data(r->get_stream())) {
                            continue;
                        }
                        delete r;
                        r = 0;
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        continue;
                    }
                }
            }

            delete url;
            url = 0;
        }
    }
    catch (Error &e) {
        cerr << e.get_error_message() << endl;
        return 1;
    }
    catch (exception &e) {
        cerr << "C++ library exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
