
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

// This is the source to `geturl'; a simple tool to exercise the Connect
// class. It can be used to get naked URLs as well as the DAP2 DAS and DDS
// objects.  jhrg.

#define not_used

static char rcsid[] not_used =
    { "$Id$" };

#include <stdio.h>
#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <GetOpt.h>
#include <string>

#include "AISConnect.h"
#include "Response.h"
#include "StdinResponse.h"

using std::cerr;
using std::endl;

const char *version = "$Revision$";

extern int dods_keep_temps;     // defined in HTTPResponse.h

void usage(string name)
{
    cerr << "Usage: " << name << endl;
    cerr <<
        " [idDaxAVvks] [-B <db>][-c <expr>][-m <num>] <url> [<url> ...]" <<
        endl;
    cerr << " [Vvks] <file> [<file> ...]" << endl;
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
    cerr << "        A: Use the AIS for DAS objects." << endl;
    cerr << "        D: For each URL, get the the DataDDS." << endl;
    cerr <<
        "        x: For each URL, get the DDX object. Does not get data."
        << endl;
    cerr << "        B: <AIS xml dataBase>. Overrides .dodsrc." << endl;
    cerr << "        v: Verbose." << endl;
    cerr << "        V: Version." << endl;
    cerr << "        c: <expr> is a contraint expression. Used with -D." <<
        endl;
    cerr << "           NB: You can use a `?' for the CE also." << endl;
    cerr << "        k: Keep temporary files created by libdap core\n" <<
        endl;
    cerr << "        m: Request the same URL <num> times." << endl;
    cerr << "        z: Ask the server to compress data." << endl;
    cerr << "        s: Print Sequences using numbered rows." << endl;
}

bool read_data(FILE * fp)
{
    if (!fp) {
        fprintf(stderr, "geturl: Whoa!!! Null stream pointer.\n");
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
    fprintf(stdout, "The data:\n");

    for (DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++) {
        BaseType *v = *i;
        if (print_rows && (*i)->type() == dods_sequence_c)
            dynamic_cast < Sequence * >(*i)->print_val_by_rows(stdout);
        else
            v->print_val(stdout);
    }

    fprintf(stdout, "\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "idaDxAVvkB:c:m:zsh?");
    int option_char;

    bool get_das = false;
    bool get_dds = false;
    bool get_data = false;
    bool get_ddx = false;
    bool get_version = false;
    bool cexpr = false;
    bool verbose = false;
    bool multi = false;
    bool accept_deflate = false;
    bool print_rows = false;
    bool use_ais = false;
    int times = 1;
    string expr = "";
    string ais_db = "";

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
        case 'A':
            use_ais = true;
            break;
        case 'V':
            fprintf(stderr, "geturl version: %s\n", version);
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
            use_ais = true;
            ais_db = getopt.optarg;
            break;
        case 'z':
            accept_deflate = true;
            break;
        case 's':
            print_rows = true;
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
            Connect *url = 0;;
            if (use_ais) {
                if (!ais_db.empty())
                    url = new AISConnect(name, ais_db);
                else
                    url = new AISConnect(name);
            } else {
                url = new Connect(name);
            }

            // This overrides the value set in the .dodsrc file.
            if (accept_deflate)
                url->set_accept_deflate(accept_deflate);

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

                        url->read_data(dds, r);
                    } else {
                        r = new Response(fopen(argv[i], "r"));

                        if (!r->get_stream())
                            throw Error(string("The input source: ")
                                        + string(argv[i])
                                        + string(" could not be opened"));

                        url->read_data_no_mime(dds, r);
                    }
                }
                catch(Error & e) {
                    cerr << e.get_error_message() << endl;
                    delete r;
                    r = 0;
                    delete url;
                    url = 0;
                    break;
                }

                if (verbose)
                    fprintf(stderr, "Server version: %s\n",
                            url->get_version().c_str());

                print_data(dds, print_rows);

            }

            else if (get_version) {
                fprintf(stderr, "Server version: %s\n",
                        url->request_version().c_str());
            }

            else if (get_das) {
                for (int j = 0; j < times; ++j) {
                    DAS das;
                    try {
                        url->request_das(das);
                    }
                    catch(Error & e) {
                        cerr << e.get_error_message() << endl;
                        delete url;
                        url = 0;
                        continue;
                    }

                    if (verbose) {
                        fprintf(stderr, "Server version: %s\n",
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
                        url->request_dds(dds);
                    }
                    catch(Error & e) {
                        cerr << e.get_error_message() << endl;
                        delete url;
                        url = 0;
                        continue;       // Goto the next URL or exit the loop.
                    }

                    if (verbose) {
                        fprintf(stderr, "Server version: %s\n",
                                url->get_version().c_str());
                        fprintf(stderr, "DDS:\n");
                    }

                    dds.print(stdout);
                }
            }

            else if (get_ddx) {
                for (int j = 0; j < times; ++j) {
                    BaseTypeFactory factory;
                    DDS dds(&factory);
                    try {
                        url->request_dds(dds);
                    }
                    catch(Error & e) {
                        cerr << e.get_error_message() << endl;
                        continue;       // Goto the next URL or exit the loop.
                    }

                    if (verbose) {
                        fprintf(stderr, "Server version: %s\n",
                                url->get_version().c_str());
                        fprintf(stderr, "DDS:\n");
                    }

                    dds.print_xml(stdout, false, "geturl; no blob yet");
                }
            }

            else if (get_data) {
                if (expr.empty() && name.find('?') == string::npos)
                    expr = "";

                for (int j = 0; j < times; ++j) {
                    BaseTypeFactory factory;
                    DataDDS dds(&factory);
                    try {
                        DBG(cerr << "URL: " << url->URL(false) << endl);
                        DBG(cerr << "CE: " << expr << endl);
                        url->request_data(dds, expr);

                        if (verbose)
                            fprintf(stderr, "Server version: %s\n",
                                    url->get_version().c_str());

                        print_data(dds, print_rows);
                    }
                    catch(Error & e) {
                        cerr << e.get_error_message() << endl;
                        delete url;
                        url = 0;
                        continue;
                    }
                }
            }

            else {              // if (!get_das && !get_dds && !get_data)
                // This code uses HTTPConnect::fetch_url which cannot be
                // accessed using an instance of Connect. So some of the
                // options supported by other URLs won't work here (e.g., the
                // verbose option doesn't show the server version number).
                HTTPConnect http(RCReader::instance());

                // This overrides the value set in the .dodsrc file.
                if (accept_deflate)
                    http.set_accept_deflate(accept_deflate);

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
                    catch(Error & e) {
                        cerr << e.get_error_message() << endl;
                        continue;
                    }
                }
            }

            delete url;
            url = 0;
        }
    }
    catch(Error & e) {
        cerr << e.get_error_message() << endl;
    }

    return 0;
}
