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
#include <unistd.h>
#include <sys/stat.h>
#include <uuid/uuid.h>  // used to build CID header value for data ddx

#ifndef WIN32
#include <sys/wait.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <cstring>
#include <ctime>

//#define DODS_DEBUG

#include "DAS.h"
#include "DDS.h"
#include "ConstraintEvaluator.h"
#include "DDXParserSAX2.h"
#include "Ancillary.h"
#include "ResponseBuilder.h"
#include "XDRStreamMarshaller.h"
#include "XDRFileUnMarshaller.h"

//#include "DAPCache3.h"
#include "ResponseCache.h"

#include "debug.h"
#include "mime_util.h"	// for last_modified_time() and rfc_822_date()
#include "escaping.h"
#include "util.h"

#ifndef WIN32
#include "SignalHandler.h"
#include "EventHandler.h"
#include "AlarmHandler.h"
#endif

#define CRLF "\r\n"             // Change here, expr-test.cc

using namespace std;

namespace libdap {

/** Called when initializing a ResponseBuilder that's not going to be passed
 command line arguments. */
void ResponseBuilder::initialize()
{
    // Set default values. Don't use the C++ constructor initialization so
    // that a subclass can have more control over this process.
    d_dataset = "";
    d_ce = "";
    d_btp_func_ce = "";
    d_timeout = 0;

    d_default_protocol = DAP_PROTOCOL_VERSION;

    d_response_cache = 0;
}

/** Lazy getter for the ResponseCache. */
ResponseCache *
ResponseBuilder::responseCache()
{
	if (!d_response_cache) d_response_cache = new ResponseCache();
	return d_response_cache->is_available() ? d_response_cache: 0;
}

ResponseBuilder::~ResponseBuilder()
{
	if (d_response_cache) delete d_response_cache;

	// If an alarm was registered, delete it. The register code in SignalHandler
	// always deletes the old alarm handler object, so only the one returned by
	// remove_handler needs to be deleted at this point.
	delete dynamic_cast<AlarmHandler*>(SignalHandler::instance()->remove_handler(SIGALRM));
}

/** Return the entire constraint expression in a string.  This
 includes both the projection and selection clauses, but not the
 question mark.

 @brief Get the constraint expression.
 @return A string object that contains the constraint expression. */
string ResponseBuilder::get_ce() const
{
    return d_ce;
}

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
void ResponseBuilder::set_ce(string _ce)
{
    d_ce = www2id(_ce, "%", "%20");
}

/** The ``dataset name'' is the filename or other string that the
 filter program will use to access the data. In some cases this
 will indicate a disk file containing the data.  In others, it
 may represent a database query or some other exotic data
 access method.

 @brief Get the dataset name.
 @return A string object that contains the name of the dataset. */
string ResponseBuilder::get_dataset_name() const
{
    return d_dataset;
}

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
void ResponseBuilder::set_dataset_name(const string ds)
{
    d_dataset = www2id(ds, "%", "%20");
}

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

/**
 *  Split the CE so that the server functions that compute new values are
 *  separated into their own string and can be evaluated separately from
 *  the rest of the CE (which can contain simple and slicing projection
 *  as well as other types of function calls).
 */
void
ResponseBuilder::split_ce(ConstraintEvaluator &eval, const string &expr)
{
    string ce;
    if (!expr.empty())
        ce = expr;
    else
        ce = d_ce;

    string btp_function_ce = "";
    string::size_type pos = 0;
    DBG(cerr << "ce: " << ce << endl);

    string::size_type first_paren = ce.find("(", pos);
    string::size_type closing_paren = ce.find(")", pos);
    while (first_paren != string::npos && closing_paren != string::npos) {
        // Maybe a BTP function; get the name of the potential function
        string name = ce.substr(pos, first_paren-pos);
        DBG(cerr << "name: " << name << endl);
        // is this a BTP function
        btp_func f;
        if (eval.find_function(name, &f)) {
            // Found a BTP function
            if (!btp_function_ce.empty())
                btp_function_ce += ",";
            btp_function_ce += ce.substr(pos, closing_paren+1-pos);
            ce.erase(pos, closing_paren+1-pos);
            if (ce[pos] == ',')
                ce.erase(pos, 1);
        }
        else {
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

    d_ce = ce;
    d_btp_func_ce = btp_function_ce;
}

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

        eval.parse_constraint(d_ce, dds); // Throws Error if the ce doesn't parse.

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
        // methods. Clear that so the CE in d_ce will control what is
        // sent. If that is empty (there was only a function call) all
        // of the variables in the intermediate DDS (i.e., the function
        // result) will be sent.
        fdds->mark_all(false);

        eval.parse_constraint(d_ce, *fdds);

        if (with_mime_headers)
            set_mime_text(out, dods_dds, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        fdds->print_constrained(out);

        if (responseCache())
        	responseCache()->unlock_and_close(cache_token);

        delete fdds;
    }
    else {
        DBG(cerr << "Simple constraint" << endl);

        eval.parse_constraint(d_ce, dds); // Throws Error if the ce doesn't parse.

        if (with_mime_headers)
            set_mime_text(out, dods_dds, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        dds.print_constrained(out);
    }

    out << flush;
}

/**
 * Build/return the BLOB part of the DAP2 data response.
 */
void ResponseBuilder::dataset_constraint(ostream &out, DDS & dds, ConstraintEvaluator & eval, bool ce_eval)
{
    // send constrained DDS
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
    }
    catch (Error & e) {
        throw;
    }
}

/**
 * Build/return the DDX and the BLOB part of the DAP3.x data response.
 * This was never actually used by any server or client, but it has
 * been used to cache responses for some of the OPULS unstructured
 * grid work. It was originally intended to be used for DAP4.
 */
void ResponseBuilder::dataset_constraint_ddx(ostream &out, DDS &dds, ConstraintEvaluator &eval,
        const string &boundary, const string &start, bool ce_eval)
{
    // Write the MPM headers for the DDX (text/xml) part of the response
    libdap::set_mime_ddx_boundary(out, boundary, start, dap4_ddx, x_plain);

    // Make cid
    uuid_t uu;
    uuid_generate(uu);
    char uuid[37];
    uuid_unparse(uu, &uuid[0]);
    char domain[256];
    if (getdomainname(domain, 255) != 0 || strlen(domain) == 0)
        strncpy(domain, "opendap.org", 255);

    string cid = string(&uuid[0]) + "@" + string(&domain[0]);
    // Send constrained DDX with a data blob reference
    dds.print_xml_writer(out, true, cid);

    // write the data part mime headers here
    set_mime_data_boundary(out, boundary, cid, dap4_data, x_plain);

    XDRStreamMarshaller m(out);

    // Send all variables in the current projection (send_p()). In DAP4,
    // all of the top-level variables are serialized with their checksums.
    // Internal variables are not.
    for (DDS::Vars_iter i = dds.var_begin(); i != dds.var_end(); i++) {
        if ((*i)->send_p()) {
            (*i)->serialize(eval, dds, m, ce_eval);
        }
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
void ResponseBuilder::send_data(ostream &data_stream, DDS &dds, ConstraintEvaluator &eval, bool with_mime_headers)
{
    // Set up the alarm.
    establish_timeout(data_stream);
    dds.set_timeout(d_timeout);

    // Split constraint into two halves
    split_ce(eval);

    // If there are functions, parse them and eval.
    // Use that DDS and parse the non-function ce
    // Serialize using the second ce and the second dds
    if (!d_btp_func_ce.empty()) {
        DBG(cerr << "Found function(s) in CE: " << d_btp_func_ce << endl);
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

        DBG(fdds->print_constrained(cerr));

        // Server functions might mark variables to use their read()
        // methods. Clear that so the CE in d_ce will control what is
        // sent. If that is empty (there was only a function call) all
        // of the variables in the intermediate DDS (i.e., the function
        // result) will be sent.
        fdds->mark_all(false);

        eval.parse_constraint(d_ce, *fdds);

        fdds->tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

        if (fdds->get_response_limit() != 0 && fdds->get_request_size(true) > fdds->get_response_limit()) {
            string msg = "The Request for " + long_to_string(dds.get_request_size(true) / 1024)
                    + "KB is too large; requests for this user are limited to "
                    + long_to_string(dds.get_response_limit() / 1024) + "KB.";
            throw Error(msg);
        }

        if (with_mime_headers)
            set_mime_binary(data_stream, dods_data, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        DBG(cerr << "About to call dataset_constraint" << endl);
        dataset_constraint(data_stream, *fdds, eval, false);

        if (responseCache())
        	responseCache()->unlock_and_close(cache_token);

        delete fdds;
    }
    else {
        DBG(cerr << "Simple constraint" << endl);

        eval.parse_constraint(d_ce, dds); // Throws Error if the ce doesn't parse.

        dds.tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

        if (dds.get_response_limit() != 0 && dds.get_request_size(true) > dds.get_response_limit()) {
            string msg = "The Request for " + long_to_string(dds.get_request_size(true) / 1024)
                    + "KB is too large; requests for this user are limited to "
                    + long_to_string(dds.get_response_limit() / 1024) + "KB.";
            throw Error(msg);
        }

        if (with_mime_headers)
            set_mime_binary(data_stream, dods_data, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        dataset_constraint(data_stream, dds, eval);
    }

    data_stream << flush;
}

/** Send the DDX response. The DDX never contains data, instead it holds a
 reference to a Blob response which is used to get the data values. The
 DDS and DAS objects are built using code that already exists in the
 servers.

 @note This is the DAP3.x metadata response; it is supported by most DAP2
 servers as well. The DAP4 DDX will contain types not present in DAP2 or 3.x

 @param dds The dataset's DDS \e with attributes in the variables.
 @param eval A reference to the ConstraintEvaluator to use.
 @param out Destination
 @param with_mime_headers If true, include the MIME headers in the response.
 Defaults to true. */
void ResponseBuilder::send_ddx(ostream &out, DDS &dds, ConstraintEvaluator &eval, bool with_mime_headers)
{
    if (d_ce.empty()) {
        if (with_mime_headers)
            set_mime_text(out, dap4_ddx, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        dds.print_xml_writer(out, false /*constrained */, "");
        //dds.print(out);
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
        // methods. Clear that so the CE in d_ce will control what is
        // sent. If that is empty (there was only a function call) all
        // of the variables in the intermediate DDS (i.e., the function
        // result) will be sent.
        fdds->mark_all(false);

        eval.parse_constraint(d_ce, *fdds);

        if (with_mime_headers)
            set_mime_text(out, dap4_ddx, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        fdds->print_constrained(out);

        if (responseCache())
        	responseCache()->unlock_and_close(cache_token);

        delete fdds;
    }
    else {
        DBG(cerr << "Simple constraint" << endl);

        eval.parse_constraint(d_ce, dds); // Throws Error if the ce doesn't parse.

        if (with_mime_headers)
            set_mime_text(out, dap4_ddx, x_plain, last_modified_time(d_dataset), dds.get_dap_version());

        //dds.print_constrained(out);
        dds.print_xml_writer(out, true, "");
    }

    out << flush;
}

/** Send the data in the DDS object back to the client program. The data is
 encoded using a Marshaller, and enclosed in a MIME document which is all sent
 to \c data_stream.

 @note This is not the DAP4 data response, although that was the original
 intent.

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
void ResponseBuilder::send_data_ddx(ostream & data_stream, DDS & dds, ConstraintEvaluator & eval, const string &start,
        const string &boundary, bool with_mime_headers)
{
	// Set up the alarm.
    establish_timeout(data_stream);
    dds.set_timeout(d_timeout);

    eval.parse_constraint(d_ce, dds); // Throws Error if the ce doesn't parse.

    if (dds.get_response_limit() != 0 && dds.get_request_size(true) > dds.get_response_limit()) {
        string msg = "The Request for " + long_to_string(dds.get_request_size(true) / 1024)
                + "KB is too large; requests for this user are limited to "
                + long_to_string(dds.get_response_limit() / 1024) + "KB.";
        throw Error(msg);
    }

    dds.tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

    // Start sending the response...

    // Handle *functional* constraint expressions specially
    if (eval.function_clauses()) {
        // We could unique_ptr<DDS> here to avoid memory leaks if
        // dataset_constraint_ddx() throws an exception.
        DDS *fdds = eval.eval_function_clauses(dds);
        try {
            if (with_mime_headers)
                set_mime_multipart(data_stream, boundary, start, dap4_data_ddx, x_plain, last_modified_time(d_dataset));
            data_stream << flush;
            dataset_constraint_ddx(data_stream, *fdds, eval, boundary, start);
        }
        catch (...) {
            delete fdds;
            throw;
        }
        delete fdds;
    }
    else {
        if (with_mime_headers)
            set_mime_multipart(data_stream, boundary, start, dap4_data_ddx, x_plain, last_modified_time(d_dataset));
        data_stream << flush;
        dataset_constraint_ddx(data_stream, dds, eval, boundary, start);
    }

    data_stream << flush;

    if (with_mime_headers)
        data_stream << CRLF << "--" << boundary << "--" << CRLF;
}

#if 0
/** Write a DDS to an output stream. This method is intended to be used
    to write to a cache so that interim results can be reused w/o needing
    to be recomputed. I chose the 'data ddx' response because it combines
    the syntax and semantic metadata along with the data and all
    three DAP2 requests can be satisfied using it.

    @brief Cache data.

    @param cache_file_name Put the data here
    @param dds A DDS object containing the data to be sent.
    @return void */

void ResponseBuilder::cache_data_ddx(const string &cache_file_name, DDS &dds)
{
    ofstream data_stream(cache_file_name.c_str());
    if (!data_stream)
    	throw InternalErr(__FILE__, __LINE__, "Could not open '" + cache_file_name + "' to write cached response.");

    string start="dataddx_cache_start", boundary="dataddx_cache_boundary";

    ConstraintEvaluator eval;

    dds.set_dap_version("3.2");

    send_data_ddx(data_stream, dds, eval, start, boundary, true /*with_mime_headers*/);

    data_stream.close();

#if 0
	DBG(cerr << "Caching " << d_dataset + "?" + d_btp_func_ce << endl);

    ofstream data_stream(cache_file_name.c_str());
    // Test for a valid file open

    string start="dataddx_cache_start", boundary="dataddx_cache_boundary";

    // Does this really need the full set of MIME headers? Not including these
    // might make it comparable with the dapreader module in the BES.
    // set_mime_multipart(data_stream, boundary, start, dap4_data_ddx, x_plain, last_modified_time(d_dataset));
    set_mime_multipart(data_stream, boundary, start, dods_ddx, x_plain, last_modified_time(d_dataset));
    data_stream << flush;

    // dataset_constraint_ddx() needs a ConstraintEvaluator because
    // it calls serialize().
    ConstraintEvaluator eval;

    // Setting the DDS version to 3.2 causes the print_xml() code
    // to write out a 'blob' element with a valid cid. The reader
    // code in Connect needs this (or thinks it does...)
    dds.set_dap_version("3.2");

    dataset_constraint_ddx(data_stream, dds, eval, boundary, start);
    data_stream << flush;

    data_stream << CRLF << "--" << boundary << "--" << CRLF;
    data_stream.close();
#endif
}
#endif
#if 0
static const char *descrip[] = { "unknown", "dods_das", "dods_dds", "dods_data", "dods_error", "web_error", "dap4-ddx",
        "dap4-data", "dap4-error", "dap4-data-ddx", "dods_ddx" };
static const char *encoding[] = { "unknown", "deflate", "x-plain", "gzip", "binary" };

/** Generate an HTTP 1.0 response header for a text document. This is used
 when returning a serialized DAS or DDS object.

 @note In Hyrax these headers are not used. Instead the front end of the
 server will build the response headers

 @param strm Write the MIME header to this stream.
 @param type The type of this this response. Defaults to
 application/octet-stream.
 @param ver The version string; denotes the libdap implementation
 version.
 @param enc How is this response encoded? Can be plain or deflate or the
 x_... versions of those. Default is x_plain.
 @param last_modified The time to use for the Last-Modified header value.
 Default is zero which means use the current time. */
void ResponseBuilder::set_mime_text(ostream &strm, ObjectType type, EncodingType enc, const time_t last_modified,
        const string &protocol) const
{
    strm << "HTTP/1.0 200 OK" << CRLF;

    strm << "XDODS-Server: " << DVR<< CRLF;
    strm << "XOPeNDAP-Server: " << DVR<< CRLF;

    if (protocol == "")
        strm << "XDAP: " << d_default_protocol << CRLF;
    else
        strm << "XDAP: " << protocol << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    if (type == dap4_ddx)
        strm << "Content-Type: text/xml" << CRLF;
    else
        strm << "Content-Type: text/plain" << CRLF;

    // Note that Content-Description is from RFC 2045 (MIME, pt 1), not 2616.
    // jhrg 12/23/05
    strm << "Content-Description: " << descrip[type] << CRLF;
    if (type == dods_error) // don't cache our error responses.
        strm << "Cache-Control: no-cache" << CRLF;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;
    strm << CRLF;
}
#endif
#if 0
/** Generate an HTTP 1.0 response header for a html document.

 @param strm Write the MIME header to this stream.
 @param type The type of this this response.
 @param ver The version string; denotes the libdap implementation
 version.
 @param enc How is this response encoded? Can be plain or deflate or the
 x_... versions of those. Default is x_plain.
 @param last_modified The time to use for the Last-Modified header value.
 Default is zero which means use the current time. */
void ResponseBuilder::set_mime_html(ostream &strm, ObjectType type, EncodingType enc, const time_t last_modified,
        const string &protocol) const
{
    strm << "HTTP/1.0 200 OK" << CRLF;

    strm << "XDODS-Server: " << DVR<< CRLF;
    strm << "XOPeNDAP-Server: " << DVR<< CRLF;

    if (protocol == "")
        strm << "XDAP: " << d_default_protocol << CRLF;
    else
        strm << "XDAP: " << protocol << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    strm << "Content-type: text/html" << CRLF;
    // See note above about Content-Description header. jhrg 12/23/05
    strm << "Content-Description: " << descrip[type] << CRLF;
    if (type == dods_error) // don't cache our error responses.
        strm << "Cache-Control: no-cache" << CRLF;
    // Don't write a Content-Encoding header for x-plain since that breaks
    // Netscape on NT. jhrg 3/23/97
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;
    strm << CRLF;
}
#endif
#if 0
/** Write an HTTP 1.0 response header for our binary response document (i.e.,
 the DataDDS object).

 @param strm Write the MIME header to this stream.
 @param type The type of this this response. Defaults to
 application/octet-stream.
 @param ver The version string; denotes the libdap implementation
 version.
 @param enc How is this response encoded? Can be plain or deflate or the
 x_... versions of those. Default is x_plain.
 @param last_modified The time to use for the Last-Modified header value.
 Default is zero which means use the current time.
 */
void ResponseBuilder::set_mime_binary(ostream &strm, ObjectType type, EncodingType enc, const time_t last_modified,
        const string &protocol) const
{
    strm << "HTTP/1.0 200 OK" << CRLF;

    strm << "XDODS-Server: " << DVR<< CRLF;
    strm << "XOPeNDAP-Server: " << DVR<< CRLF;

    if (protocol == "")
        strm << "XDAP: " << d_default_protocol << CRLF;
    else
        strm << "XDAP: " << protocol << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    strm << "Content-Type: application/octet-stream" << CRLF;
    strm << "Content-Description: " << descrip[type] << CRLF;
    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;

    strm << CRLF;
}
#endif
#if 0
/** Build the initial headers for the DAP4 data response */

void ResponseBuilder::set_mime_multipart(ostream &strm, const string &boundary, const string &start, ObjectType type, EncodingType enc,
        const time_t last_modified, const string &protocol, const string &url) const
{
    strm << "HTTP/1.1 200 OK" << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;

    strm << "Last-Modified: ";
    if (last_modified > 0)
        strm << rfc822_date(last_modified).c_str() << CRLF;
    else
        strm << rfc822_date(t).c_str() << CRLF;

    strm << "Content-Type: multipart/related; boundary=" << boundary << "; start=\"<" << start
            << ">\"; type=\"text/xml\"" << CRLF;

    // data-ddx;"; removed as a result of the merge of the hyrax 1.8 release
    // branch.
    strm << "Content-Description: " << descrip[type] << ";";
    if (!url.empty())
        strm << " url=\"" << url << "\"" << CRLF;
    else
        strm << CRLF;

    if (enc != x_plain)
        strm << "Content-Encoding: " << encoding[enc] << CRLF;

    if (protocol == "")
        strm << "X-DAP: " << d_default_protocol << CRLF;
    else
        strm << "X-DAP: " << protocol << CRLF;

    strm << "X-OPeNDAP-Server: " << DVR<< CRLF;

    strm << CRLF;
}
#endif
#if 0
void ResponseBuilder::set_mime_ddx_boundary(ostream &strm, const string &boundary, const string &cid) const
{
    strm << "--" << boundary << CRLF;
    strm << "Content-Type: text/xml; charset=UTF-8" << CRLF;
    strm << "Content-Transfer-Encoding: binary" << CRLF;
    strm << "Content-Description: ddx" << CRLF;
    strm << "Content-Id: <" << cid << ">" << CRLF;

    strm << CRLF;
}
#endif
#if 0
/** Generate an HTTP 1.0 response header for an Error object.
 @param strm Write the MIME header to this stream.
 @param code HTTP 1.0 response code. Should be 400, ... 500, ...
 @param reason Reason string of the HTTP 1.0 response header.
 @param version The version string; denotes the DAP spec and implementation
 version. */
void ResponseBuilder::set_mime_error(ostream &strm, int code, const string &reason, const string &protocol) const
{
    strm << "HTTP/1.0 " << code << " " << reason.c_str() << CRLF;

    strm << "XDODS-Server: " << DVR<< CRLF;
    strm << "X-OPeNDAP-Server: " << DVR<< CRLF;

    if (protocol == "")
        strm << "X-DAP: " << d_default_protocol << CRLF;
    else
        strm << "X-DAP: " << protocol << CRLF;

    const time_t t = time(0);
    strm << "Date: " << rfc822_date(t).c_str() << CRLF;
    strm << "Cache-Control: no-cache" << CRLF;
    strm << CRLF;
}
#endif
} // namespace libdap

