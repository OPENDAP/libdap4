
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

#include <cstdio> //SBL 12.3.19

#include "GetOpt.h"

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
using namespace libdap ;

const char *version = CVER " (" DVR " DAP/" DAP_PROTOCOL_VERSION ")";
#if 0
extern int libdap::dods_keep_temps;     // defined in HTTPResponse.h
extern int libdap::www_trace;
#endif
static void usage(const string &name)
{
	cerr << "Usage: " << name << endl;
	cerr << " [dD vVikmzstM][-c <expr>][-m <num>] <url> [<url> ...] | <file> [<file> ...]" << endl;
	cerr << endl;
	cerr << "In the first form of the command, dereference the URL and" << endl;
	cerr << "perform the requested operations. This includes routing" << endl;
	cerr << "the returned information through the DAP processing" << endl;
	cerr << "library (parsing the returned objects, et c.). If none" << endl;
	cerr << "of a, d, or D are used with a URL, then the DAP library" << endl;
	cerr << "routines are NOT used and the URLs contents are dumped" << endl;
	cerr << "to standard output." << endl;
	cerr << "Note: If the URL contains a query string the query string" << endl;
	cerr << "will be preserved in the request. However, if the query " << endl;
	cerr << "string contains DAP4 keys they may interfere with the" << endl;
	cerr << "operation of " << name << ". A warning will be" << endl;
	cerr << "written to stderr when "<< name << " identifies" << endl;
	cerr << "the presence of a DAP4 query key in the submitted" << endl;
	cerr << "URL's query string." << endl;
	cerr << endl;
	cerr << "In the second form of the command, assume the files are" << endl;
	cerr << "DataDDS objects (stored in files or read from pipes)" << endl;
	cerr << "and process them as if -D were given. In this case the" << endl;
	cerr << "information *must* contain valid MIME header in order" << endl;
	cerr << "to be processed." << endl;
	cerr << endl;
	cerr << "Options:" << endl;
    cerr << "        d: For each URL, get the (DAP4) DMR object. Does not get data." << endl;
	cerr << "        D: For each URL, get the DAP4 Data response." << endl;
	cerr << endl;
	cerr << "        v: Verbose output." << endl;
	cerr << "        V: Version of this client; see 'i' for server version." << endl;
	cerr << "        i: For each URL, get the server version." << endl;
	// cerr << "        k: Keep temporary files created by libdap." << endl;
	cerr << "        m: Request the same URL <num> times." << endl;
	cerr << "        z: Ask the server to compress data." << endl;
	cerr << "        s: Print Sequences using numbered rows." << endl;
	// cerr << "        t: Trace www accesses." << endl;
	cerr << "        M: Assume data read from a file has no MIME headers; use only with files" << endl;
	cerr << endl;
	cerr << "        c: <expr> is a constraint expression. Used with -d/D" << endl;
	cerr << "           NB: You can use a `?' for the CE also." << endl;
    cerr << "        S: Used in conjunction with -d and will report the total size of the data "
            "referenced in the DMR." << endl;
}

// Used for raw http access/transfer
bool read_data(FILE * fp)
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

static void read_response_from_file(D4Connect *url, DMR &dmr, Response &r, bool mime_headers, bool get_dap4_data, bool get_dmr)
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
unsigned long long get_size(D4Group *grp, bool constrained=false)
{
    unsigned long long  w = 0;

    for (auto var_itr = grp->var_begin(); var_itr != grp->var_end(); var_itr++) {
        if (constrained) {
            if ((*var_itr)->send_p())
                w += (*var_itr)->width(constrained);
        }
        else {
            w += (*var_itr)->width(constrained);
        }
    }
    for(auto grp_itr = grp->grp_begin(); grp_itr != grp->grp_end(); grp_itr++){
        w += get_size(*grp_itr,constrained);
    }

    return w;
}

unsigned long long get_size(DMR &dmr, bool constrained=false)
{
    return get_size(dmr.root(),constrained);
}


int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "[dDvVikrm:Mzstc:S]");
    int option_char;

    bool get_dmr = false;
    bool get_dap4_data = false;
    bool get_version = false;
    bool cexpr = false;
    bool verbose = false;
    bool multi = false;
    bool accept_deflate = false;
    bool print_rows = false;
    bool mime_headers = true;
    bool report_errors = false;
    int times = 1;
    int dap_client_major = 4;
    int dap_client_minor = 0;
    string expr = "";
    bool compute_size = false;

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    while ((option_char = getopt()) != -1)
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
        case 'i':
            get_version = true;
            break;
        case 'S':
            compute_size = true;
            break;
#if 0
        case 'k':
            dods_keep_temps = 1;
            break;              // keep_temp is in Connect.cc
#endif
        case 'r':
        	report_errors = true;
        	break;
        case 'm':
            multi = true;
            times = atoi(getopt.optarg);
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
#if 0
        case 't':
            www_trace = 1;
            break;
#endif
        case 'c':
            cexpr = true;
            expr = getopt.optarg;
            break;
        case 'h':
        case '?':
        default:
            usage(argv[0]);
            exit(1);
        }

    try {
        // If after processing all the command line options there is nothing
        // left (no URL or file) assume that we should read from stdin.
        for (int i = getopt.optind; i < argc; ++i) {
            if (verbose)
                cerr << "Fetching: " << argv[i] << endl;

            string name = argv[i];
            D4Connect *url = 0;
            // auto_ptr? jhrg 10/19/15
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
                    		throw Error((string)"Could not open: " + argv[i]);

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
                catch (Error & e) {
                    cerr << "Error: " << e.get_error_message() << endl;
                    delete url; url = 0;
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
                            cout << "DAP version: " << url->get_protocol() << ", Server version: " << url->get_version() << endl;
                            cout << "DMR:" << endl;
                        }

                        XMLWriter xml;
                        dmr.print_dap4(xml);
                        cout << xml.get_doc() << endl;
                        if(compute_size){
                            cout << "DMR References " << get_size(dmr) << " bytes of data," << endl;
                        }
                    }
                    catch (Error & e) {
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
                             cout << "DAP version: " << url->get_protocol() << ", Server version: " << url->get_version() << endl;
                             cout << "DMR:" << endl;
                         }

                         XMLWriter xml;
                         dmr.print_dap4(xml);
                         cout << xml.get_doc() << endl;

                         print_data(dmr, print_rows);
                    }
                     catch (Error & e) {
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
                        	vector<string> *headers = r->get_headers();
                        	copy(headers->begin(), headers->end(), ostream_iterator<string>(cout, "\n"));
                        }
                        if (!read_data(r->get_stream())) {
                            continue;
                        }
                        delete r;
                        r = 0;
                    }
                    catch (Error & e) {
                        cerr << e.get_error_message() << endl;
                        if (report_errors)
                            return EXIT_FAILURE;
                        continue;
                    }
                }
            }

#if 0
            else if (get_version) {
                fprintf(stderr, "DAP version: %s, Server version: %s\n",
                        url->request_protocol().c_str(),
                        url->get_version().c_str());
            }
#endif

            delete url;  url = 0;
        }
    }
    catch (Error &e) {

    	if(e.get_error_code() == malformed_expr){
        	cerr << e.get_error_message() << endl;
        	usage(argv[0]);
    	}
    	else {
        	cerr << e.get_error_message() << endl;

    	}

        cerr << "Exiting." << endl;
        //return 1;
        return EXIT_FAILURE;
    }
    catch (exception &e) {
        cerr << "C++ library exception: " << e.what() << endl;
        cerr << "Exiting." << endl;
        //return 1;
        return EXIT_FAILURE;
    }

    //return 0;
    return EXIT_SUCCESS;
}
