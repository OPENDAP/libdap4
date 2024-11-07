// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2011 OPeNDAP, Inc.
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

#include "config.h"

#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_UUID_UUID_H
#include <uuid/uuid.h> // used to build CID header value for data ddx
#elif defined(HAVE_UUID_H)
#include <uuid.h>
#else
#error "Could not find UUID library header"
#endif

#ifndef WIN32
#include <sys/wait.h>
#else
#include <fcntl.h>
#include <io.h>
#include <process.h>
#endif

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <cstring>
#include <ctime>

// #define DODS_DEBUG

#include "Ancillary.h"
#include "ConstraintEvaluator.h"
#include "DAS.h"
#include "DDS.h"
#include "DDXParserSAX2.h"
#include "ResponseBuilder.h"
#include "XDRFileUnMarshaller.h"
#include "XDRStreamMarshaller.h"

// #include "DAPCache3.h"
// #include "ResponseCache.h"

#include "debug.h"
#include "escaping.h"
#include "mime_util.h" // for last_modified_time() and rfc_822_date()
#include "util.h"

#ifndef WIN32
#include "AlarmHandler.h"
#include "EventHandler.h"
#include "SignalHandler.h"
#endif

#define CRLF "\r\n" // Change here, expr-test.cc

using namespace std;
using namespace libdap;

/** Called when initializing a ResponseBuilder that's not going to be passed
 command line arguments. */
void ResponseBuilder::initialize() {
    // Set default values. Don't use the C++ constructor initialization so
    // that a subclass can have more control over this process.
    d_dataset = "";
    d_dap2ce = "";
    d_dap2_btp_func_ce = "";
    d_timeout = 0;

    d_default_protocol = DAP_PROTOCOL_VERSION;
}

ResponseBuilder::~ResponseBuilder() {
    // If an alarm was registered, delete it. The register code in SignalHandler
    // always deletes the old alarm handler object, so only the one returned by
    // remove_handler needs to be deleted at this point.
    delete dynamic_cast<AlarmHandler *>(SignalHandler::instance()->remove_handler(SIGALRM));
}

/** Return the entire constraint expression in a string.  This
 includes both the projection and selection clauses, but not the
 question mark.

 @brief Get the constraint expression.
 @return A string object that contains the constraint expression. */
string ResponseBuilder::get_ce() const { return d_dap2ce; }

/** Set the constraint expression. This will filter the CE text removing
 * any 'WWW' escape characters except space. Spaces are left in the CE
 * because the CE parser uses whitespace to delimit tokens while some
 * datasets have identifiers that contain spaces. It's possible to use
 * double quotes around identifiers too, but most client software doesn't
 * know about that.
 *
 * @@brief Set the CE
 * @param _ce The constraint expression
 */
void ResponseBuilder::set_ce(string _ce) { d_dap2ce = www2id(_ce, "%", "%20"); }

/** The ``dataset name'' is the filename or other string that the
 filter program will use to access the data. In some cases this
 will indicate a disk file containing the data.  In others, it
 may represent a database query or some other exotic data
 access method.

 @brief Get the dataset name.
 @return A string object that contains the name of the dataset. */
string ResponseBuilder::get_dataset_name() const { return d_dataset; }

/** Set the dataset name, which is a string used to access the dataset
 * on the machine running the server. That is, this is typically a pathname
 * to a data file, although it doesn't have to be. This is not
 * echoed in error messages (because that would reveal server
 * storage patterns that data providers might want to hide). All WWW-style
 * escapes are replaced except for spaces.
 *
 * @brief Set the dataset pathname.
 * @param ds The pathname (or equivalent) to the dataset.
 */
void ResponseBuilder::set_dataset_name(const string ds) { d_dataset = www2id(ds, "%", "%20"); }
#if 0
/** Set the server's timeout value. A value of zero (the default) means no
 timeout.

 @see To establish a timeout, call establish_timeout(ostream &)
 @param t Server timeout in seconds. Default is zero (no timeout). */
void ResponseBuilder::set_timeout(int t)
{
    d_timeout = t;
}

/** Get the server's timeout value. */
int ResponseBuilder::get_timeout() const
{
    return d_timeout;
}

/** Use values of this instance to establish a timeout alarm for the server.
 If the timeout value is zero, do nothing.
*/
void ResponseBuilder::establish_timeout(ostream &stream) const
{
#ifndef WIN32
    if (d_timeout > 0) {
        SignalHandler *sh = SignalHandler::instance();
        EventHandler *old_eh = sh->register_handler(SIGALRM, new AlarmHandler(stream));
        delete old_eh;
        alarm(d_timeout);
    }
#endif
}
#endif

/**
 *  Split the CE so that the server functions that compute new values are
 *  separated into their own string and can be evaluated separately from
 *  the rest of the CE (which can contain simple and slicing projection
 *  as well as other types of function calls).
 */
void ResponseBuilder::split_ce(ConstraintEvaluator &eval, const string &expr) {
    string ce;
    if (!expr.empty())
        ce = expr;
    else
        ce = d_dap2ce;

    string btp_function_ce = "";
    string::size_type pos = 0;
    DBG(cerr << "ce: " << ce << endl);

    string::size_type first_paren = ce.find("(", pos);
    string::size_type closing_paren = ce.find(")", pos);
    while (first_paren != string::npos && closing_paren != string::npos) {
        // Maybe a BTP function; get the name of the potential function
        string name = ce.substr(pos, first_paren - pos);
        DBG(cerr << "name: " << name << endl);
        // is this a BTP function
        btp_func f;
        if (eval.find_function(name, &f)) {
            // Found a BTP function
            if (!btp_function_ce.empty())
                btp_function_ce += ",";
            btp_function_ce += ce.substr(pos, closing_paren + 1 - pos);
            ce.erase(pos, closing_paren + 1 - pos);
            if (ce[pos] == ',')
                ce.erase(pos, 1);
        } else {
            pos = closing_paren + 1;
            // exception?
            if (pos < ce.length() && ce.at(pos) == ',')
                ++pos;
        }

        first_paren = ce.find("(", pos);
        closing_paren = ce.find(")", pos);
    }

    DBG(cerr << "Modified constraint: " << ce << endl);
    DBG(cerr << "BTP Function part: " << btp_function_ce << endl);

    d_dap2ce = ce;
    d_dap2_btp_func_ce = btp_function_ce;
}

#if 0
/** This function formats and prints an ASCII representation of a
 DAS on stdout.  This has the effect of sending the DAS object
 back to the client program.

 @note This is the DAP2 attribute response.

 @brief Send a DAS.

 @param out The output stream to which the DAS is to be sent.
 @param das The DAS object to be sent.
 @param with_mime_headers If true (the default) send MIME headers.
 @return void
 @see DAS
 @deprecated */
void ResponseBuilder::send_das(ostream &out, DAS &das, bool with_mime_headers) const
{
    if (with_mime_headers)
        set_mime_text(out, dods_das, x_plain, last_modified_time(d_dataset), "2.0");

    das.print(out);

    out << flush;
}

/** Send the DAP2 DAS response to the given stream. This version of
 * send_das() uses the DDS object, assuming that it contains attribute
 * information. If there is a constraint expression associated with this
 * instance of ResponseBuilder, then it will be applied. This means
 * that CEs that contain server functions will populate the response cache
 * even if the server's initial request is for a DAS. This is different
 * from the older behavior of libdap where CEs were never evaluated for
 * the DAS response. This does not actually change the resulting DAS,
 * just the behavior 'under the covers'.
 *
 * @param out Send the response to this ostream
 * @param dds Use this DDS object
 * @param eval A Constraint Evaluator to use for any CE bound to this
 * ResponseBuilder instance
 * @param constrained Should the result be constrained
 * @param with_mime_headers Should MIME headers be sent to out?
 */
void ResponseBuilder::send_das(ostream &out, DDS &dds, ConstraintEvaluator &eval, bool constrained, bool with_mime_headers)
{
    // Set up the alarm.
    establish_timeout(out);
    dds.set_timeout(d_timeout);

    if (!constrained) {
        if (with_mime_headers)
            set_mime_text(out, dods_das, x_plain, last_modified_time(d_dataset), "2.0");

        dds.print_das(out);
        out << flush;

        return;
    }

    split_ce(eval);

    // If there are functions, parse them and eval.
    // Use that DDS and parse the non-function ce
    // Serialize using the second ce and the second dds
    if (!d_btp_func_ce.empty()) {
        DDS *fdds = 0;
        string cache_token = "";

        if (responseCache()) {
            DBG(cerr << "Using the cache for the server function CE" << endl);
            fdds = responseCache()->read_cached_dataset(dds, d_btp_func_ce, this, &eval, cache_token);
        }
        else {
            DBG(cerr << "Cache not found; (re)calculating" << endl);
            eval.parse_constraint(d_btp_func_ce, dds);
            fdds = eval.eval_function_clauses(dds);
        }

        if (with_mime_headers)
            set_mime_text(out, dods_das, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        fdds->print_das(out);

        if (responseCache())
        	responseCache()->unlock_and_close(cache_token);

        delete fdds;
    }
    else {
        DBG(cerr << "Simple constraint" << endl);

        eval.parse_constraint(d_dap2ce, dds); // Throws Error if the ce doesn't parse.

        if (with_mime_headers)
            set_mime_text(out, dods_das, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        dds.print_das(out);
    }

    out << flush;
}

/** This function formats and prints an ASCII representation of a
 DDS on stdout. Either an entire DDS or a constrained DDS may be sent.
 This function looks in the local cache and uses a DDS object there
 if it's valid. Otherwise, if the request CE contains server functions
 that build data for the response, the resulting DDS will be cached.

 @brief Transmit a DDS.
 @param out The output stream to which the DAS is to be sent.
 @param dds The DDS to send back to a client.
 @param eval A reference to the ConstraintEvaluator to use.
 @param constrained If this argument is true, evaluate the
 current constraint expression and send the `constrained DDS'
 back to the client.
 @param constrained If true, apply the constraint bound to this instance
 of ResponseBuilder
 @param with_mime_headers If true (default) send MIME headers.
 @return void
 @see DDS */
void ResponseBuilder::send_dds(ostream &out, DDS &dds, ConstraintEvaluator &eval, bool constrained,
        bool with_mime_headers)
{
    if (!constrained) {
        if (with_mime_headers)
            set_mime_text(out, dods_dds, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        dds.print(out);
        out << flush;
        return;
    }

    // Set up the alarm.
    establish_timeout(out);
    dds.set_timeout(d_timeout);

    // Split constraint into two halves
    split_ce(eval);

    // If there are functions, parse them and eval.
    // Use that DDS and parse the non-function ce
    // Serialize using the second ce and the second dds
    if (!d_btp_func_ce.empty()) {
        string cache_token = "";
        DDS *fdds = 0;

        if (responseCache()) {
            DBG(cerr << "Using the cache for the server function CE" << endl);
            fdds = responseCache()->read_cached_dataset(dds, d_btp_func_ce, this, &eval, cache_token);
        }
        else {
            DBG(cerr << "Cache not found; (re)calculating" << endl);
            eval.parse_constraint(d_btp_func_ce, dds);
            fdds = eval.eval_function_clauses(dds);
        }

        // Server functions might mark variables to use their read()
        // methods. Clear that so the CE in d_dap2ce will control what is
        // sent. If that is empty (there was only a function call) all
        // of the variables in the intermediate DDS (i.e., the function
        // result) will be sent.
        fdds->mark_all(false);

        eval.parse_constraint(d_dap2ce, *fdds);

        if (with_mime_headers)
            set_mime_text(out, dods_dds, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        fdds->print_constrained(out);

        if (responseCache())
        	responseCache()->unlock_and_close(cache_token);

        delete fdds;
    }
    else {
        DBG(cerr << "Simple constraint" << endl);

        eval.parse_constraint(d_dap2ce, dds); // Throws Error if the ce doesn't parse.

        if (with_mime_headers)
            set_mime_text(out, dods_dds, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        dds.print_constrained(out);
    }

    out << flush;
}
#endif

/**
 * Build/return the BLOB part of the DAP2 data response.
 */
void ResponseBuilder::dataset_constraint(ostream &out, DDS &dds, ConstraintEvaluator &eval, bool ce_eval) {
    DBG(cerr << "Inside dataset_constraint" << endl);

    dds.print_constrained(out);
    out << "Data:\n";
    out << flush;

    XDRStreamMarshaller m(out);

    try {
        // Send all variables in the current projection (send_p())
        for (DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++)
            if ((*i)->send_p()) {
                (*i)->serialize(eval, dds, m, ce_eval);
            }
    } catch (Error &e) {
        throw;
    }
}

/** Send the data in the DDS object back to the client program. The data is
 encoded using a Marshaller, and enclosed in a MIME document which is all sent
 to \c data_stream.

 @note This is the DAP2 data response.

 @brief Transmit data.
 @param dds A DDS object containing the data to be sent.
 @param eval A reference to the ConstraintEvaluator to use.
 @param data_stream Write the response to this stream.
 @param anc_location A directory to search for ancillary files (in
 addition to the CWD).  This is used in a call to
 get_data_last_modified_time().
 @param with_mime_headers If true, include the MIME headers in the response.
 Defaults to true.
 @return void */
void ResponseBuilder::send_data(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval, bool with_mime_headers) {
    // Split constraint into two halves
    split_ce(eval);

    // If there are functions, parse them and eval.
    // Use that DDS and parse the non-function ce
    // Serialize using the second ce and the second dds
    if (!d_dap2_btp_func_ce.empty()) {
        DBG(cerr << "Found function(s) in CE: " << d_btp_func_ce << endl);
        string cache_token = "";
        DDS *fdds = 0;

        // The BES code caches the function result
        eval.parse_constraint(d_dap2_btp_func_ce, dds);
        fdds = eval.eval_function_clauses(dds);

        DBG(fdds->print_constrained(cerr));

        // Server functions might mark variables to use their read()
        // methods. Clear that so the CE in d_dap2ce will control what is
        // sent. If that is empty (there was only a function call) all
        // of the variables in the intermediate DDS (i.e., the function
        // result) will be sent.
        fdds->mark_all(false);

        eval.parse_constraint(d_dap2ce, *fdds);

        fdds->tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

        if (fdds->get_response_limit() != 0 && fdds->get_request_size(true) > fdds->get_response_limit()) {
            string msg = "The Request for " + long_to_string(dds.get_request_size(true) / 1024) +
                         "KB is too large; requests for this user are limited to " +
                         long_to_string(dds.get_response_limit() / 1024) + "KB.";
            throw Error(msg);
        }

        if (with_mime_headers)
            set_mime_binary(data_stream, dods_data, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        DBG(cerr << "About to call dataset_constraint" << endl);
        dataset_constraint(data_stream, *fdds, eval, false);

        delete fdds;
    } else {

        DBG(cerr << "Simple constraint" << endl);

        eval.parse_constraint(d_dap2ce, dds); // Throws Error if the ce doesn't parse.

        dds.tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

        if (dds.get_response_limit() != 0 && dds.get_request_size(true) > dds.get_response_limit()) {
            string msg = "The Request for " + long_to_string(dds.get_request_size(true) / 1024) +
                         "KB is too large; requests for this user are limited to " +
                         long_to_string(dds.get_response_limit() / 1024) + "KB.";
            throw Error(msg);
        }

        if (with_mime_headers)
            set_mime_binary(data_stream, dods_data, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        dataset_constraint(data_stream, dds, eval);
    }

    data_stream << flush;
}
