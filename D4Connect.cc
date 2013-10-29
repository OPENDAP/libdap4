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

#include <cassert>
#include <cstring>

#include "D4Connect.h"
#include "HTTPConnect.h"
#include "Response.h"
#include "DMR.h"

#include "D4ParserSax2.h"
#include "chunked_istream.h"
#include "D4StreamUnMarshaller.h"

#include "mime_util.h"
#include "debug.h"

using namespace std;

namespace libdap {


/** This private method process data from both local and remote sources. It
    exists to eliminate duplication of code. */
void D4Connect::process_dmr(DMR &data, Response &rs)
{
	DBG(cerr << "Entering D4Connect::process_dmr" << endl);

	data.set_dap_version(rs.get_protocol());

	DBG(cerr << "Entering process_data: d_stream = " << rs << endl);
	switch (rs.get_type()) {
	case dods_error: {
		Error e;
		if (!e.parse(rs.get_stream()))
			throw InternalErr(__FILE__, __LINE__, "Could not parse the Error object returned by the server!");
		throw e;
	}

	case web_error:
		// Web errors (those reported in the return document's MIME header)
		// are processed by the WWW library.
		throw InternalErr(__FILE__, __LINE__,
				"An error was reported by the remote httpd; this should have been processed by HTTPConnect..");

	case dap4_ddx: {
		// Read the byte-order byte; used later on
		char byte_order;
		*rs.get_cpp_stream() >> byte_order;

		// get a chunked input stream
		chunked_istream cis(*rs.get_cpp_stream(), 1024, byte_order);

		// parse the DMR, stopping when the boundary is found.
		try {
			// force chunk read
			// get chunk size
			int chunk_size = cis.read_next_chunk();
			// get chunk
			char chunk[chunk_size];
			cis.read(chunk, chunk_size);
			// parse char * with given size
			D4ParserSax2 parser;
			// '-2' to discard the CRLF pair
			parser.intern(chunk, chunk_size - 2, &data, /*debug*/false);
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

	data.set_dap_version(rs.get_protocol());

	DBG(cerr << "Entering process_data: d_stream = " << rs << endl);
	switch (rs.get_type()) {
	case dods_error: {
		Error e;
		if (!e.parse(rs.get_stream()))
			throw InternalErr(__FILE__, __LINE__, "Could not parse the Error object returned by the server!");
		throw e;
	}

	case web_error:
		// Web errors (those reported in the return document's MIME header)
		// are processed by the WWW library.
		throw InternalErr(__FILE__, __LINE__,
				"An error was reported by the remote httpd; this should have been processed by HTTPConnect..");

	case dap4_data_ddx: {
		// Read the byte-order byte; used later on
		char byte_order;
		*rs.get_cpp_stream() >> byte_order;
		//if (debug) cerr << "Byte order: " << ((byte_order) ? "big endian" : "little endian") << endl;

		// get a chunked input stream
		chunked_istream cis(*rs.get_cpp_stream(), 1024, byte_order);

		// parse the DMR, stopping when the boundary is found.
		try {
			// force chunk read
			// get chunk size
			int chunk_size = cis.read_next_chunk();
			// get chunk
			char chunk[chunk_size];
			cis.read(chunk, chunk_size);
			// parse char * with given size
			D4ParserSax2 parser;
			// '-2' to discard the CRLF pair
			parser.intern(chunk, chunk_size - 2, &data, /*debug*/false);
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

		D4StreamUnMarshaller um(cis, byte_order);

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

    fstream &data_source = *rs.get_cpp_stream();
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
        d_http(0), d_local(false), d_URL(""), d_ce(""), d_server("unknown"), d_protocol("4.0")
{
    string name = prune_spaces(url);

    // Figure out if the URL starts with 'http', if so, make sure that we
    // talk to an instance of HTTPConnect.
    if (name.find("http") == 0) {
        DBG(cerr << "Connect: The identifier is an http URL" << endl);
        d_http = new HTTPConnect(RCReader::instance());

        // Find and store any CE given with the URL.
        string::size_type dotpos = name.find('?');
        if (dotpos != name.npos) {
            d_URL = name.substr(0, dotpos);
            d_ce = name.substr(dotpos + 1);
        }
        else {
            d_URL = name;
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

#if 0
/** Get version information from the server. This is a new method which will
 ease the transition to DAP 4.

 @note Use request_protocol() to get the DAP protocol version.

 @return The DAP version string.
 @see request_protocol() */
string D4D4Connect::request_version()
{
    string version_url = _URL + ".ver";
    if (_proj.length() + _sel.length())
        version_url = version_url + "?" + id2www_ce(_proj + _sel);

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(version_url);
    }
    catch (Error &e) {
        delete rs;
        throw;
    }

    d_server = rs.get_server();
    d_protocol = rs.get_protocol();

    delete rs;

    return d_version;
}

/** Get protocol version  information from the server. This is a new method
 which will ease the transition to DAP 4. Note that this method returns
 the version of the DAP protocol implemented by the server. The
 request_version() method returns the \e server's version number, not
 the DAP protocol version.

 @note This method actually asks the server for the protocol version - use
 get_protocol() to get the protocol information from the most recent
 response (e.g., from the last DDX response returned by the server).

 @return The DAP protocol version string. */
string D4D4Connect::request_protocol()
{
    string version_url = _URL + ".ver";
    if (_proj.length() + _sel.length())
        version_url = version_url + "?" + id2www_ce(_proj + _sel);

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(version_url);
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }

    d_version = rs.get_version();
    d_protocol = rs.get_protocol();

    delete rs;
    rs = 0;

    return d_protocol;
}

/** Reads the DAS corresponding to the dataset in the Connect
 object's URL. Although DAP does not support using CEs with DAS
 requests, if present in the Connect object's instance, they will be
 escaped and passed as the query string of the request.

 @brief Get the DAS from a server.
 @param das Result. */
void D4Connect::request_das(DAS &das)
{
    string das_url = _URL + ".das";
    if (_proj.length() + _sel.length())
        das_url = das_url + "?" + id2www_ce(_proj + _sel);

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(das_url);
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }

    d_version = rs.get_version();
    d_protocol = rs.get_protocol();

    switch (rs.get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs.get_stream())) {
                delete rs;
                rs = 0;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            rs = 0;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            break;

        case dods_das:
        default:
            // DAS::parse throws an exception on error.
            try {
                das.parse(rs.get_stream()); // read and parse the das from a file
            }
            catch (Error &e) {
                delete rs;
                rs = 0;
                throw;
            }

            break;
    }

    delete rs;
    rs = 0;
}

/** Reads the DAS corresponding to the dataset in the Connect
 object's URL. Although DAP does not support using CEs with DAS
 requests, if present in the Connect object's instance, they will be
 escaped and passed as the query string of the request.

 Different from request_das method in that this method uses the URL as
 given without attaching .das or projections or selections.

 @brief Get the DAS from a server.
 @param das Result. */
void D4Connect::request_das_url(DAS &das)
{
    string use_url = _URL + "?" + _proj + _sel;
    Response *rs = 0;
    try {
        rs = d_http->fetch_url(use_url);
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }

    d_version = rs.get_version();
    d_protocol = rs.get_protocol();

    switch (rs.get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs.get_stream())) {
                delete rs;
                rs = 0;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            rs = 0;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            break;

        case dods_das:
        default:
            // DAS::parse throws an exception on error.
            try {
                das.parse(rs.get_stream()); // read and parse the das from a file
            }
            catch (Error &e) {
                delete rs;
                rs = 0;
                throw;
            }

            break;
    }

    delete rs;
    rs = 0;
}

/** Reads the DDS corresponding to the dataset in the Connect object's URL.
 If present in the Connect object's instance, a CE will be escaped,
 combined with \c expr and passed as the query string of the request.

 @note If you need the DDS to hold specializations of the type classes,
 be sure to include the factory class which will instantiate those
 specializations in the DDS. Either pass a pointer to the factory to
 DDS constructor or use the DDS::set_factory() method after the
 object is built.

 @brief Get the DDS from a server.
 @param dds Result.
 @param expr Send this constraint expression to the server. */
void D4Connect::request_dds(DDS &dds, string expr)
{
    string proj, sel;
    string::size_type dotpos = expr.find('&');
    if (dotpos != expr.npos) {
        proj = expr.substr(0, dotpos);
        sel = expr.substr(dotpos);
    }
    else {
        proj = expr;
        sel = "";
    }

    string dds_url = _URL + ".dds" + "?" + id2www_ce(_proj + proj + _sel + sel);

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(dds_url);
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }

    d_version = rs.get_version();
    d_protocol = rs.get_protocol();

    switch (rs.get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs.get_stream())) {
                delete rs;
                rs = 0;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            rs = 0;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            break;

        case dods_dds:
        default:
            // DDS::prase throws an exception on error.
            try {
                dds.parse(rs.get_stream()); // read and parse the dds from a file
            }
            catch (Error &e) {
                delete rs;
                rs = 0;
                throw;
            }
            break;
    }

    delete rs;
    rs = 0;
}

/** Reads the DDS corresponding to the dataset in the Connect object's URL.
 If present in the Connect object's instance, a CE will be escaped,
 combined with \c expr and passed as the query string of the request.

 Different from request_dds method above in that this method assumes
 URL is complete and does not add anything to the command, such as .dds
 or projections or selections.

 @note If you need the DDS to hold specializations of the type classes,
 be sure to include the factory class which will instantiate those
 specializations in the DDS. Either pass a pointer to the factory to
 DDS constructor or use the DDS::set_factory() method after the
 object is built.

 @brief Get the DDS from a server.
 @param dds Result. */
void D4Connect::request_dds_url(DDS &dds)
{
    string use_url = _URL + "?" + _proj + _sel;
    Response *rs = 0;
    try {
        rs = d_http->fetch_url(use_url);
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }

    d_version = rs.get_version();
    d_protocol = rs.get_protocol();

    switch (rs.get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs.get_stream())) {
                delete rs;
                rs = 0;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            rs = 0;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            break;

        case dods_dds:
        default:
            // DDS::prase throws an exception on error.
            try {
                dds.parse(rs.get_stream()); // read and parse the dds from a file
            }
            catch (Error &e) {
                delete rs;
                rs = 0;
                throw;
            }
            break;
    }

    delete rs;
    rs = 0;
}

/** Reads the DDX corresponding to the dataset in the Connect object's URL.
 If present in the Connect object's instance, a CE will be escaped,
 combined with \c expr and passed as the query string of the request.

 @note A DDX is represented as XML on the wire but in memory libdap uses a
 DDS object with variables that hold their own attributes (the DDS itself holds
 the global attributes).

 @brief Get the DDX from a server.
 @param dds Result.
 @param expr Send this constraint expression to the server. */
void D4Connect::request_ddx(DDS &dds, string expr)
{
    string proj, sel;
    string::size_type dotpos = expr.find('&');
    if (dotpos != expr.npos) {
        proj = expr.substr(0, dotpos);
        sel = expr.substr(dotpos);
    }
    else {
        proj = expr;
        sel = "";
    }

    string ddx_url = _URL + ".ddx" + "?" + id2www_ce(_proj + proj + _sel + sel);

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(ddx_url);
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }

    d_version = rs.get_version();
    d_protocol = rs.get_protocol();

    switch (rs.get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs.get_stream())) {
                delete rs;
                rs = 0;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            rs = 0;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            break;

        case dap4_ddx:
        case dods_ddx:
            try {
                string blob;

                DDXParser ddxp(dds.get_factory());
                ddxp.intern_stream(rs.get_stream(), &dds, blob);
            }
            catch (Error &e) {
                delete rs;
                rs = 0;
                throw;
            }
            break;

        default:
            delete rs;
            rs = 0;
            throw Error(
                    "The site did not return a valid response (it lacked the\n\
expected content description header value of 'dap4-ddx' and\n\
instead returned '"
                            + long_to_string(rs.get_type())
                            + "').\n\
This may indicate that the server at the site is not correctly\n\
configured, or that the URL has changed.");
    }

    delete rs;
    rs = 0;
}

/** @brief The 'url' version of request_ddx
 @see D4Connect::request_ddx. */
void D4Connect::request_ddx_url(DDS &dds)
{
    string use_url = _URL + "?" + _proj + _sel;

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(use_url);
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }

    d_version = rs.get_version();
    d_protocol = rs.get_protocol();

    switch (rs.get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs.get_stream())) {
                delete rs;
                rs = 0;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            rs = 0;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            break;

        case dap4_ddx:
        case dods_ddx:
            try {
                string blob;

                DDXParser ddxp(dds.get_factory());
                ddxp.intern_stream(rs.get_stream(), &dds, blob);
            }
            catch (Error &e) {
                delete rs;
                rs = 0;
                throw;
            }
            break;

        default:
            delete rs;
            rs = 0;
            throw Error(
                    "The site did not return a valid response (it lacked the\n\
expected content description header value of 'dap4-ddx' and\n\
instead returned '"
                            + long_to_string(rs.get_type())
                            + "').\n\
This may indicate that the server at the site is not correctly\n\
configured, or that the URL has changed.");
    }

    delete rs;
    rs = 0;
}

/** Reads the DataDDS object corresponding to the dataset in the Connect
 object's URL. If present in the Connect object's instance, a CE will be
 escaped, combined with \c expr and passed as the query string of the
 request. The result is a DataDDS which contains the data values bound to
 variables.

 @note If you need the DataDDS to hold specializations of the type classes,
 be sure to include the factory class which will instantiate those
 specializations in the DataDDS. Either pass a pointer to the factory to
 DataDDS constructor or use the DDS::set_factory() method after the
 object is built.

 @brief Get the DAS from a server.
 @param data Result.
 @param expr Send this constraint expression to the server. */
void D4Connect::request_data(DataDDS &data, string expr)
{
    string proj, sel;
    string::size_type dotpos = expr.find('&');
    if (dotpos != expr.npos) {
        proj = expr.substr(0, dotpos);
        sel = expr.substr(dotpos);
    }
    else {
        proj = expr;
        sel = "";
    }

    string data_url = _URL + ".dods?" + id2www_ce(_proj + proj + _sel + sel);

    Response *rs = 0;
    // We need to catch Error exceptions to ensure calling close_output.
    try {
        rs = d_http->fetch_url(data_url);

        d_version = rs.get_version();
        d_protocol = rs.get_protocol();

        process_data(data, rs);
        delete rs;
        rs = 0;
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }
}

/** Reads the DataDDS object corresponding to the dataset in the Connect
 object's URL. If present in the Connect object's instance, a CE will be
 escaped, combined with \c expr and passed as the query string of the
 request. The result is a DataDDS which contains the data values bound to
 variables.

 Different from request_data in that this method uses the syntax of the
 new OPeNDAP server commands using dispatch

 @note If you need the DataDDS to hold specializations of the type classes,
 be sure to include the factory class which will instantiate those
 specializations in the DataDDS. Either pass a pointer to the factory to
 DataDDS constructor or use the DDS::set_factory() method after the
 object is built.

 @brief Get the DAS from a server.
 @param data Result. */
void D4Connect::request_data_url(DataDDS &data)
{
    string use_url = _URL + "?" + _proj + _sel;
    Response *rs = 0;
    // We need to catch Error exceptions to ensure calling close_output.
    try {
        rs = d_http->fetch_url(use_url);

        d_version = rs.get_version();
        d_protocol = rs.get_protocol();

        process_data(data, rs);
        delete rs;
        rs = 0;
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }
}

void D4Connect::request_data_ddx(DataDDS &data, string expr)
{
    string proj, sel;
    string::size_type dotpos = expr.find('&');
    if (dotpos != expr.npos) {
        proj = expr.substr(0, dotpos);
        sel = expr.substr(dotpos);
    }
    else {
        proj = expr;
        sel = "";
    }

    string data_url = _URL + ".dap?" + id2www_ce(_proj + proj + _sel + sel);

    Response *rs = 0;
    // We need to catch Error exceptions to ensure calling close_output.
    try {
        rs = d_http->fetch_url(data_url);

        d_version = rs.get_version();
        d_protocol = rs.get_protocol();

        process_data(data, rs);
        delete rs;
        rs = 0;
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }
}

void D4Connect::request_data_ddx_url(DataDDS &data)
{
    string use_url = _URL + "?" + _proj + _sel;
    Response *rs = 0;
    // We need to catch Error exceptions to ensure calling close_output.
    try {
        rs = d_http->fetch_url(use_url);

        d_version = rs.get_version();
        d_protocol = rs.get_protocol();

        process_data(data, rs);
        delete rs;
        rs = 0;
    }
    catch (Error &e) {
        delete rs;
        rs = 0;
        throw;
    }
}
#endif

void
D4Connect::read_dmr(DMR &dmr, Response &rs)
{
    parse_mime(rs);
    if (rs.get_type() == unknown_type)
        throw Error("Unknown response type.");

    read_dmr_no_mime(dmr, rs);
}

void D4Connect::read_dmr_no_mime(DMR &dmr, Response &rs)
{
	// Assume callers know what they are doing
    if (rs.get_type() == unknown_type)
        rs.set_type(dap4_data_ddx);

    switch (rs.get_type()) {
    case dap4_ddx:
        process_dmr(dmr, rs);
        d_server = rs.get_version();
        d_protocol = dmr.dap_version();
        break;
    default:
        throw Error("Expected a DAP4 DMR response.");
    }
}

void
D4Connect::read_data(DMR &data, Response &rs)
{
    parse_mime(rs);
    if (rs.get_type() == unknown_type)
        throw Error("Unknown response type.");

    read_data_no_mime(data, rs);
}

void D4Connect::read_data_no_mime(DMR &data, Response &rs)
{
	// Assume callers know what they are doing
    if (rs.get_type() == unknown_type)
        rs.set_type(dap4_data_ddx);

    switch (rs.get_type()) {
    case dap4_data_ddx:
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
    if (d_http)
        d_http->set_credentials(u, p);
}

/** Set the \e accept deflate property.
 @param deflate True if the client can accept compressed responses, False
 otherwise. */
void D4Connect::set_accept_deflate(bool deflate)
{
    if (d_http)
        d_http->set_accept_deflate(deflate);
}

/** Set the \e XDAP-Accept property/header. This is used to send to a server
 the (highest) DAP protocol version number that this client understands.

 @param major The client dap protocol major version
 @param minor The client dap protocol minor version */
void D4Connect::set_xdap_protocol(int major, int minor)
{
    if (d_http)
        d_http->set_xdap_protocol(major, minor);
}

/** Disable any further use of the client-side cache. In a future version
 of this software, this should be handled so that the www library is
 not initialized with the cache running by default. */
void D4Connect::set_cache_enabled(bool cache)
{
    if (d_http)
        d_http->set_cache_enabled(cache);
}

bool D4Connect::is_cache_enabled()
{
    if (d_http)
        return d_http->is_cache_enabled();
    else
        return false;
}

} // namespace libdap
