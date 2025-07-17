
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
#include <fcntl.h>
#include <io.h>
#endif

#include <cstring>
#include <sstream>
#include <string>
#include <unistd.h> // getopt

#include "D4BaseTypeFactory.h"
#include "D4Connect.h"
#include "D4Group.h"
#include "D4Sequence.h"
#include "DMR.h"
#include "HTTPConnect.h"
#include "HTTPResponse.h"
#include "RCReader.h"
#include "StdinResponse.h"
#include "XMLWriter.h"

using namespace std;
using namespace libdap;

const char *version = CVER " (" DVR " DAP/" DAP_PROTOCOL_VERSION ")";

/**
 * @brief Converts a bool to a string of either "true" or "false" as appropriate.
 * @param b The boolean value to stringify.
 * @return Either "true" or "false" according to b.
 */
string torf(bool b) { return {b ? "true" : "false"}; }

/**
 * @brief Simple log formater
 * @param ofstrm The stream to write to
 * @param msg The message to write.
 */
void logd(const string &msg, const bool verbose) {
    if (verbose) {
        std::stringstream ss(msg); // Create a stringstream from the string
        std::string msg_line;

        // Read lines from the stringstream until the end
        while (std::getline(ss, msg_line)) {
            cerr << "# " << msg_line << "\n";
        }
    }
}

void err_msg(const string &msg) { cerr << "ERROR: " << msg << "\n"; }

static void usage(const string &) {
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
               of the data referenced in the DMR.
            C: Used in conjunction with -D will cause the DAP4 service to
               compute and return DAP4 data checksums.
)";

    cerr << message << endl;
}

// Used for raw http access/transfer
bool read_data(FILE *fp) {
    if (!fp) {
        fprintf(stderr, "getdap4: Whoa!!! Null stream pointer.\n");
        return false;
    }
    // Changed from a loop that used getc() to one that uses fread(). getc()
    // worked fine for transfers of text information, but *not* for binary
    // transfers. fread() will handle both.
    char c = 0;
    while (fp && !feof(fp) && fread(&c, 1, 1, fp))
        printf("%c", c); // stick with stdio

    return true;
}

static void read_response_from_file(D4Connect &url, DMR &dmr, Response &r, bool mime_headers, bool get_dap4_data,
                                    bool get_dmr) {
    if (mime_headers) {
        if (get_dap4_data)
            url.read_data(dmr, r);
        else if (get_dmr)
            url.read_dmr(dmr, r);
        else
            throw Error("Only supports Data or DMR responses");
    } else {
        if (get_dap4_data)
            url.read_data_no_mime(dmr, r);
        else if (get_dmr)
            url.read_dmr_no_mime(dmr, r);
        else
            throw Error("Only supports Data or DMR responses");
    }
}

static void print_group_data(const D4Group *g, bool print_rows = false) {
    for (const auto var : g->variables()) {
        if (print_rows && var->type() == dods_sequence_c)
            dynamic_cast<D4Sequence &>(*var).print_val_by_rows(cout);
        else
            var->print_val(cout);
    }

    for (const auto group : g->groups()) {
        print_group_data(group, print_rows);
    }
}

static void print_data(DMR &dmr, bool print_rows = false) {
    cout << "The data:" << endl;

    const auto g = dmr.root();

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
unsigned long long get_size(D4Group *grp, bool constrained = false) {
    unsigned long long w = 0;

    for (auto var_itr = grp->var_begin(); var_itr != grp->var_end(); var_itr++) {
        if (constrained) {
            if ((*var_itr)->send_p())
                w += (*var_itr)->width(constrained);
        } else {
            w += (*var_itr)->width(constrained);
        }
    }
    for (auto grp_itr = grp->grp_begin(); grp_itr != grp->grp_end(); grp_itr++) {
        w += get_size(*grp_itr, constrained);
    }

    return w;
}

unsigned long long get_size(DMR &dmr, bool constrained = false) { return get_size(dmr.root(), constrained); }

/**
 *
 * @param url The D4Connect instance with the service URL inside.
 * @param constraint_expression The dap4 CE to apply to the dataset pointed to by `url`
 * @param compute_size  If true compute the size of the variables.
 * @param report_errors If true, (re)throw exceptions
 * @param verbose If true make the chatty output.
 */
void get_dmr(D4Connect &url, const string &constraint_expression, const bool compute_size, const bool report_errors,
             const bool verbose) {

    D4BaseTypeFactory factory;
    DMR dmr(&factory);
    try {
        url.request_dmr(dmr, constraint_expression);

        logd("   DAP version: " + url.get_protocol(), verbose);
        logd("Server version: " + url.get_version(), verbose);
        logd("DMR: ", true);

        XMLWriter xml;
        dmr.print_dap4(xml);
        cout << xml.get_doc() << endl;
        if (compute_size) {
            cout << "DMR References " << get_size(dmr) << " bytes of data," << endl;
        }
    } catch (Error &e) {
        err_msg(e.get_error_message());
        if (report_errors)
            throw e;
    }
}

/**
 *
 * @param url The D4Connect instance with the service URL inside.
 * @param constraint_expression The dap4 CE to apply to the dataset pointed to by `url`
 * @param use_checksums If true, as the service to return variable check sums
 * @param print_rows If true, print the rows.
 * @param report_errors If true, (re)throw exceptions
 * @param verbose If true make the chatty output.
 */
void get_dap4_data(D4Connect &url, const string &constraint_expression, const bool use_checksums, const bool print_rows,
                   const bool report_errors, const bool verbose) {

    D4BaseTypeFactory factory;
    DMR dmr(&factory);
    dmr.use_checksums(use_checksums);

    try {

        url.request_dap4_data(dmr, constraint_expression);

        logd("   DAP version: " + url.get_protocol(), verbose);
        logd("Server version: " + url.get_version(), verbose);
        logd("DMR:", true);

        XMLWriter xml;
        dmr.print_dap4(xml);
        cout << xml.get_doc() << endl;

        print_data(dmr, print_rows);

    } catch (Error &e) {
        err_msg(e.get_error_message());
        if (report_errors) {
            throw e;
        }
    }
}
/**
 *
 * @param url_string The URL to which to add the dap4.checksum parameter if it's not in there already.
 * @param use_checksums If true add the dap4.checksum query parameter to the URL
 */
void add_dap4_checksum_parameter_to_url(string &url_string, const bool use_checksums) {
    // Use checksums but it's not in the URL?
    if (use_checksums && url_string.find("dap4.checksum") == string::npos) {
        if (url_string.find("?") == string::npos)
            // If there is no constraint add one with the checksum param
            url_string += "?dap4.checksum=true";
        else {
            // Add the checksum param  as an additional parameter
            url_string += "&dap4.checksum=true";
        }
    }
}

void read_local_dap4(D4Connect &url, const string &name, const bool get_dmr_flag, const bool get_data_flag,
                     const bool mime_headers, const bool print_rows, const bool use_checksums, const bool report_errors,
                     const bool verbose) {

    logd("Assuming " + name + " is a file that contains a response object; decoding.", verbose);

    try {
        D4BaseTypeFactory factory;
        DMR dmr(&factory);
        dmr.use_checksums(use_checksums);

        if (name == "-") {
            StdinResponse r(cin);

            if (!r.get_cpp_stream())
                throw Error("Could not open standard input.");

            read_response_from_file(url, dmr, r, mime_headers, get_data_flag, get_dmr_flag);
        } else {
            fstream f(name, std::ios_base::in);
            if (!f.is_open() || f.bad() || f.eof())
                throw Error((string) "Could not open: " + name);

            Response r(&f, 0);

            read_response_from_file(url, dmr, r, mime_headers, get_data_flag, get_dmr_flag);
        }

        logd("   DAP version: " + url.get_protocol(), verbose);
        logd("Server version: " + url.get_version(), verbose);

        // Always write the DMR
        XMLWriter xml;
        dmr.print_dap4(xml);
        cout << xml.get_doc() << endl;

        if (get_data_flag)
            print_data(dmr, print_rows);

    } catch (Error &e) {
        err_msg(e.get_error_message());
        if (report_errors) {
            throw e;
        }
    }
}

void get_remote_dap4(HTTPConnect &http, const string &url_string, const bool report_errors, const bool verbose) {

    HTTPResponse *r = nullptr;
    try {
        r = http.fetch_url(url_string);
        if (verbose) {
            const auto &headers = r->get_headers();
            for (const auto &header : headers) {
                logd(header, verbose);
            }
        }
        if (!read_data(r->get_stream())) {
            return;
        }
        delete r;

    } catch (Error &e) {
        delete r;
        err_msg(e.get_error_message());
        if (report_errors) {
            throw e;
        }
    }
}

int main(int argc, char *argv[]) {
    int option_char;
    bool verbose = false;
    bool get_dmr_flag = false;
    bool get_dap4_data_flag = false;
    bool accept_deflate = false;
    bool print_rows = false;
    bool mime_headers = true;
    bool report_errors = false;
    int times = 1;
    int dap_client_major = 4;
    int dap_client_minor = 0;
    string constraint_expression;
    bool use_checksums = false;
    bool compute_size = false;

#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    while ((option_char = getopt(argc, argv, "CdDvVrm:Mzsc:S")) != -1) {
        switch (option_char) {
        case 'C':
            use_checksums = true;
            break;
        case 'd':
            get_dmr_flag = true;
            break;
        case 'D':
            get_dap4_data_flag = true;
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
            constraint_expression = optarg;
            break;
        case 'h':
        case '?':
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    try {
        // If after processing all the command line options there is nothing
        // left (no URL or file) assume that we should read from stdin.
        for (int i = optind; i < argc; ++i) {
            string name = argv[i];
            logd("      Fetching: " + name, verbose);
            logd("       dap4.ce: " + constraint_expression, verbose);
            logd(" dap4.checksum: " + torf(use_checksums), verbose);

            D4Connect url(name);

            // This overrides the value set in the .dodsrc file.
            if (accept_deflate)
                url.set_accept_deflate(accept_deflate);

            if (dap_client_major > 2)
                url.set_xdap_protocol(dap_client_major, dap_client_minor);

            if (url.is_local()) {
                // Read the DAP4 response from a local file.
                read_local_dap4(url, name, get_dmr_flag, get_dap4_data_flag, mime_headers, print_rows, use_checksums,
                                report_errors, verbose);
            } else if (get_dmr_flag) {
                // Retrieve the DAP4 DMR for url.
                for (int j = 0; j < times; ++j) {
                    get_dmr(url, constraint_expression, compute_size, report_errors, verbose);
                }
            } else if (get_dap4_data_flag) {
                // Retrieve the DAP4 Data Response for url.
                for (int j = 0; j < times; ++j) {
                    get_dap4_data(url, constraint_expression, use_checksums, print_rows, report_errors, verbose);
                }
            } else {

                HTTPConnect http(RCReader::instance());

                if (dap_client_major > 2)
                    http.set_xdap_protocol(dap_client_major, dap_client_minor);

                // This overrides the value set in the .dodsrc file.
                if (accept_deflate)
                    http.set_accept_deflate(accept_deflate);

                string url_string = argv[i];
                add_dap4_checksum_parameter_to_url(url_string, use_checksums);

                for (int j = 0; j < times; ++j) {
                    get_remote_dap4(http, url_string, report_errors, verbose);
                }
            }
        }
    } catch (Error &e) {
        if (e.get_error_code() == malformed_expr) {
            err_msg(e.get_error_message());
            usage(argv[0]);
        } else {
            err_msg(e.get_error_message());
        }

        cerr << "Exiting." << endl;
        return EXIT_FAILURE;
    } catch (exception &e) {
        err_msg(string("C++ library exception! message: ") + e.what());
        cerr << "Exiting." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
