// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//         Dan Holloway <dholloway@gso.uri.edu>
//         Reza Nekovei <reza@intcomm.net>
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

// (c) COPYRIGHT URI/MIT 1994-2002
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>
//      dan             Dan Holloway <dholloway@gso.uri.edu>
//      reza            Reza Nekovei <reza@intcomm.net>

#include "config.h"
// #define DODS_DEBUG 1



#include <cassert>
#include <cstring>
#include <sstream>

#include "D4Connect.h"
#include "HTTPConnect.h"
#include "Response.h"
#include "DMR.h"
#include "D4Group.h"

#include "D4ParserSax2.h"
#include "chunked_stream.h"
#include "chunked_istream.h"
#include "D4StreamUnMarshaller.h"

#include "escaping.h"
#include "mime_util.h"
#include "debug.h"



using namespace std;

namespace libdap {

/** This private method process data from both local and remote sources. It
 exists to eliminate duplication of code. */
void D4Connect::process_dmr(DMR &dmr, Response &rs)
{
    DBG(cerr << "Entering D4Connect::process_dmr" << endl);

    dmr.set_dap_version(rs.get_protocol());

    DBG(cerr << "Entering process_data. Response.getVersion() = " << rs.get_version() << endl);
    switch (rs.get_type()) {
    case dap4_error: {
#if 0
        Error e;
        if (!e.parse(rs.get_stream()))
        throw InternalErr(__FILE__, __LINE__, "Could not parse the Error object returned by the server!");
        throw e;
#endif
        throw InternalErr(__FILE__, __LINE__, "DAP4 errors not processed yet: FIXME!");
    }

    case web_error:
        // Web errors (those reported in the return document's MIME header)
        // are processed by the WWW library.
        throw InternalErr(__FILE__, __LINE__,
            "An error was reported by the remote httpd; this should have been processed by HTTPConnect..");

    case dap4_dmr: {
        // parse the DMR
        try {
            D4ParserSax2 parser;
            // When parsing a data response, we use the permissive mode of the DMR parser
            // (which allows Map elements to reference Arrays that are not in the DMR).
            // Do not use that mode when parsing the DMR response - assume the DMR is
            // valid. jhrg 4/13/16
            parser.intern(*rs.get_cpp_stream(), &dmr);
        }
        catch (Error &e) {
            cerr << "Exception: " << e.get_error_message() << endl;
            return;
        }
        catch (std::exception &e) {
            cerr << "Exception: " << e.what() << endl;
            return;
        }
        catch (...) {
            cerr << "Exception: unknown error" << endl;
            return;
        }

        return;
    }

    default:
        throw Error("Unknown response type");
    }
}

/** This private method process data from both local and remote sources. It
 exists to eliminate duplication of code. */
void D4Connect::process_data(DMR &data, Response &rs)
{
    DBG(cerr << "Entering D4Connect::process_data" << endl);

    assert(rs.get_cpp_stream());	// DAP4 code uses cpp streams

    data.set_dap_version(rs.get_protocol());

    DBG(cerr << "Entering process_data. Response.getVersion() = " << rs.get_version() << endl);
    switch (rs.get_type()) {
    case dap4_error: {
#if 0
        Error e;
        if (!e.parse(rs.get_cpp_stream()))
        throw InternalErr(__FILE__, __LINE__, "Could not parse the Error object returned by the server!");
        throw e;
#endif
        throw InternalErr(__FILE__, __LINE__, "DAP4 errors not processed yet: FIXME!");
    }

    case web_error:
        // Web errors (those reported in the return document's MIME header)
        // are processed by the WWW library.
        throw InternalErr(__FILE__, __LINE__,
            "An error was reported by the remote httpd; this should have been processed by HTTPConnect..");

    case dap4_data: {
#if BYTE_ORDER_PREFIX
        // Read the byte-order byte; used later on
        char byte_order;
        *rs.get_cpp_stream() >> byte_order;
        //if (debug) cerr << "Byte order: " << ((byte_order) ? "big endian" : "little endian") << endl;
#endif
        // get a chunked input stream
#if BYTE_ORDER_PREFIX
        chunked_istream cis(*rs.get_cpp_stream(), 1024, byte_order);
#else
        chunked_istream cis(*(rs.get_cpp_stream()), CHUNK_SIZE);
#endif
        // parse the DMR, stopping when the boundary is found.
        try {
            // force chunk read
            // get chunk size
            int chunk_size = cis.read_next_chunk();
            if (chunk_size < 0)
                throw Error("Found an unexpected end of input (EOF) while reading a DAP4 data response. (1)");

            // get chunk
            char chunk[chunk_size];
            cis.read(chunk, chunk_size);
            // parse char * with given size
            D4ParserSax2 parser;
            // permissive mode allows references to Maps that are not in the response.
            // Use this mode when parsing a data response (but not the DMR). jhrg 4/13/16
            parser.set_strict(false);

            // '-2' to discard the CRLF pair
            parser.intern(chunk, chunk_size - 2, &data);
        }
        catch (Error &e) {
            cerr << "Exception: " << e.get_error_message() << endl;
            return;
        }
        catch (std::exception &e) {
            cerr << "Exception: " << e.what() << endl;
            return;
        }
        catch (...) {
            cerr << "Exception: unknown error" << endl;
            return;
        }

#if BYTE_ORDER_PREFIX
        D4StreamUnMarshaller um(cis, byte_order);
#else
        D4StreamUnMarshaller um(cis, cis.twiddle_bytes());
#endif
        data.root()->deserialize(um, data);

        return;
    }

    default:
        throw Error("Unknown response type");
    }
}

/** Use when you cannot use libcurl.

 @note This method tests for MIME headers with lines terminated by CRLF
 (\r\n) and Newlines (\n). In either case, the line terminators are removed
 before each header is processed.

 @param rs Value/Result parameter. Dump version and type information here.
 */
void D4Connect::parse_mime(Response &rs)
{
    rs.set_version("dods/0.0"); // initial value; for backward compatibility.
    rs.set_protocol("2.0");

    istream &data_source = *rs.get_cpp_stream();
    string mime = get_next_mime_header(data_source);
    while (!mime.empty()) {
        string header, value;
        parse_mime_header(mime, header, value);

        // Note that this is an ordered list
        if (header == "content-description") {
            DBG(cout << header << ": " << value << endl);
            rs.set_type(get_description_type(value));
        }
        // Use the value of xdods-server only if no other value has been read
        else if (header == "xdods-server" && rs.get_version() == "dods/0.0") {
            DBG(cout << header << ": " << value << endl);
            rs.set_version(value);
        }
        // This trumps 'xdods-server' and 'server'
        else if (header == "xopendap-server") {
            DBG(cout << header << ": " << value << endl);
            rs.set_version(value);
        }
        else if (header == "xdap") {
            DBG(cout << header << ": " << value << endl);
            rs.set_protocol(value);
        }
        // Only look for 'server' if no other header supplies this info.
        else if (rs.get_version() == "dods/0.0" && header == "server") {
            DBG(cout << header << ": " << value << endl);
            rs.set_version(value);
        }

        mime = get_next_mime_header(data_source);
    }
}

// public mfuncs

/** The D4Connect constructor requires a URL or local file.

 @param n The URL for the virtual connection.
 @param uname Use this username for authentication. Null by default.
 @param password Password to use for authentication. Null by default.
 @brief Create an instance of Connect. */
D4Connect::D4Connect(const string &url, string uname, string password) :
    d_http(0), d_local(false), d_URL(""), d_UrlQueryString(""), d_server("unknown"), d_protocol("4.0")
{
    string name = prune_spaces(url);

    // Figure out if the URL starts with 'http', if so, make sure that we
    // talk to an instance of HTTPConnect.
    if (name.find("http") == 0) {
        DBG(cerr << "Connect: The identifier is an http URL" << endl);
        d_http = new HTTPConnect(RCReader::instance());
        d_http->set_use_cpp_streams(true);

        d_URL = name;

        // Find and store any CE given with the URL.
        string::size_type dotpos = name.find('?');
        if (dotpos != std::string::npos) { // Found a match.
            d_URL = name.substr(0, dotpos);

            d_UrlQueryString = name.substr(dotpos + 1);

            if (d_UrlQueryString.find(DAP4_CE_QUERY_KEY) != std::string::npos) {
                std::stringstream msg;
                msg << endl;
                msg << "WARNING: A DAP4 constraint expression key was found in the query string!" << endl;
                msg << "The submitted dataset URL: " << name << endl;
                msg << "Contains the query string: " << d_UrlQueryString << endl;
                msg << "This will cause issues when making DAP4 requests that specify additional constraints. " << endl;
                cerr << msg.str() << endl;
                // throw Error(malformed_expr, msg.str());
            }

        }
    }
    else {
        DBG(cerr << "Connect: The identifier is a local data source." << endl);
        d_local = true; // local in this case means non-DAP
    }

    set_credentials(uname, password);
}

D4Connect::~D4Connect()
{
    if (d_http) delete d_http;
}

std::string D4Connect::build_dap4_ce(const string requestSuffix, const string dap4ce)
{
    std::stringstream url;
    bool needsAmpersand = false;

    url << d_URL << requestSuffix << "?";

    if (d_UrlQueryString.length() > 0) {
        url << d_UrlQueryString;
        needsAmpersand = true;
    }

    if (dap4ce.length() > 0) {
        if (needsAmpersand) url << "&";

        url << DAP4_CE_QUERY_KEY << "=" << id2www_ce(dap4ce);
    }

    DBG(cerr << "D4Connect::build_dap4_ce() - Source URL: " << d_URL << endl);
    DBG(cerr << "D4Connect::build_dap4_ce() - Source URL Query String: " << d_UrlQueryString << endl);
    DBG(cerr << "D4Connect::build_dap4_ce() - dap4ce: " << dap4ce << endl);
    DBG(cerr << "D4Connect::build_dap4_ce() - request URL: " << url.str() << endl);

    return url.str();
}

void D4Connect::request_dmr(DMR &dmr, const string expr)
{
    string url = build_dap4_ce(".dmr", expr);

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(url);

        d_server = rs->get_version();
        d_protocol = rs->get_protocol();

        switch (rs->get_type()) {
        case unknown_type:
            DBG(cerr << "Response type unknown, assuming it's a DMR response." << endl);
            /* no break */
        case dap4_dmr: {
            D4ParserSax2 parser;
            parser.intern(*rs->get_cpp_stream(), &dmr);
            break;
        }

        case dap4_error:
            throw InternalErr(__FILE__, __LINE__, "DAP4 errors are not processed yet.");

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            throw InternalErr(__FILE__, __LINE__, "Web error found where it should never be.");
            break;

        default:
            throw InternalErr(__FILE__, __LINE__,
                "Response type not handled (got " + long_to_string(rs->get_type()) + ").");
        }
    }
    catch (...) {
        delete rs;
        throw;
    }

    delete rs;
}

void D4Connect::request_dap4_data(DMR &dmr, const string expr)
{
    string url = build_dap4_ce(".dap", expr);

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(url);

        d_server = rs->get_version();
        d_protocol = rs->get_protocol();

        switch (rs->get_type()) {
        case unknown_type:
            DBG(cerr << "Response type unknown, assuming it's a DAP4 Data response." << endl);
            /* no break */
        case dap4_data: {
#if BYTE_ORDER_PREFIX
            istream &in = *rs->get_cpp_stream();
            // Read the byte-order byte; used later on
            char byte_order;
            in >> byte_order;
#endif

            // get a chunked input stream
#if BYTE_ORDER_PREFIX
            chunked_istream cis(*(rs->get_cpp_stream()), 1024, byte_order);
#else
            chunked_istream cis(*(rs->get_cpp_stream()), CHUNK_SIZE);
#endif

            // parse the DMR, stopping when the boundary is found.

            // force chunk read
            // get chunk size
            int chunk_size = cis.read_next_chunk();
            if (chunk_size < 0)
                throw Error("Found an unexpected end of input (EOF) while reading a DAP4 data response. (2)");

            // get chunk
            char chunk[chunk_size];
            cis.read(chunk, chunk_size);
            // parse char * with given size
            D4ParserSax2 parser;
            // permissive mode allows references to Maps that are not in the response.
            parser.set_strict(false);
            // '-2' to discard the CRLF pair
            parser.intern(chunk, chunk_size - 2, &dmr, false /*debug*/);

            // Read data and store in the DMR
#if BYTE_ORDER_PREFIX
            D4StreamUnMarshaller um(cis, byte_order);
#else
            D4StreamUnMarshaller um(cis, cis.twiddle_bytes());
#endif
            dmr.root()->deserialize(um, dmr);

            break;
        }

        case dap4_error:
            throw InternalErr(__FILE__, __LINE__, "DAP4 errors are not processed yet.");

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            throw InternalErr(__FILE__, __LINE__, "Web error found where it should never be.");
            break;

        default:
            throw InternalErr(__FILE__, __LINE__,
                "Response type not handled (got " + long_to_string(rs->get_type()) + ").");
        }
    }
    catch (...) {
        delete rs;
        throw;
    }

    delete rs;
}

void D4Connect::read_dmr(DMR &dmr, Response &rs)
{
    parse_mime(rs);
    if (rs.get_type() == unknown_type) throw Error("Unknown response type.");

    read_dmr_no_mime(dmr, rs);
}

void D4Connect::read_dmr_no_mime(DMR &dmr, Response &rs)
{
    // Assume callers know what they are doing
    if (rs.get_type() == unknown_type) rs.set_type(dap4_dmr);

    switch (rs.get_type()) {
    case dap4_dmr:
        process_dmr(dmr, rs);
        d_server = rs.get_version();
        d_protocol = dmr.dap_version();
        break;
    default:
        throw Error("Expected a DAP4 DMR response.");
    }
}

void D4Connect::read_data(DMR &data, Response &rs)
{
    parse_mime(rs);
    if (rs.get_type() == unknown_type) throw Error("Unknown response type.");

    read_data_no_mime(data, rs);
}

void D4Connect::read_data_no_mime(DMR &data, Response &rs)
{
    // Assume callers know what they are doing
    if (rs.get_type() == unknown_type) rs.set_type(dap4_data);

    switch (rs.get_type()) {
    case dap4_data:
        process_data(data, rs);
        d_server = rs.get_version();
        d_protocol = data.dap_version();
        break;
    default:
        throw Error("Expected a DAP4 Data response.");
    }
}

/** @brief Set the credentials for responding to challenges while dereferencing
 URLs.
 @param u The username.
 @param p The password.
 @see extract_auth_info() */
void D4Connect::set_credentials(string u, string p)
{
    if (d_http) d_http->set_credentials(u, p);
}

/** Set the \e accept deflate property.
 @param deflate True if the client can accept compressed responses, False
 otherwise. */
void D4Connect::set_accept_deflate(bool deflate)
{
    if (d_http) d_http->set_accept_deflate(deflate);
}

/** Set the \e XDAP-Accept property/header. This is used to send to a server
 the (highest) DAP protocol version number that this client understands.

 @param major The client dap protocol major version
 @param minor The client dap protocol minor version */
void D4Connect::set_xdap_protocol(int major, int minor)
{
    if (d_http) d_http->set_xdap_protocol(major, minor);
}

/** Disable any further use of the client-side cache. In a future version
 of this software, this should be handled so that the www library is
 not initialized with the cache running by default. */
void D4Connect::set_cache_enabled(bool cache)
{
    if (d_http) d_http->set_cache_enabled(cache);
}

bool D4Connect::is_cache_enabled()
{
    if (d_http)
        return d_http->is_cache_enabled();
    else
        return false;
}

} // namespace libdap
