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

//#define DODS_DEBUG
#define FILE_UN_MARSHALLER 1

#include <cstring>
#include <cerrno>

#include <fstream>
#include <algorithm>

#include "debug.h"
#include "DataDDS.h"
#include "Connect.h"
#include "escaping.h"
//#include "RCReader.h"
#include "DDXParserSAX2.h"
#if FILE_UN_MARSHALLER
#include "XDRFileUnMarshaller.h"
#else
#include "fdiostream.h"
#include "XDRStreamUnMarshaller.h"
#endif
#include "mime_util.h"

using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::min;

namespace libdap {

/** This private method process data from both local and remote sources. It
 exists to eliminate duplication of code. */
void Connect::process_data(DataDDS &data, Response *rs)
{
    DBG(cerr << "Entering Connect::process_data" << endl);

    data.set_version(rs->get_version());
    data.set_protocol(rs->get_protocol());

    DBG(cerr << "Entering process_data: d_stream = " << rs << endl);
    switch (rs->get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs->get_stream()))
                throw InternalErr(__FILE__, __LINE__, "Could not parse the Error object returned by the server!");
            throw e;
        }

        case web_error:
            // Web errors (those reported in the return document's MIME header)
            // are processed by the WWW library.
            throw InternalErr(__FILE__, __LINE__,
                    "An error was reported by the remote httpd; this should have been processed by HTTPConnect..");

#if 0
            // This code triggers a security warning from Coverity; since it is not used,
            // I have removed it. jhrg 5/5/16
        case dods_data_ddx: {
            // Parse the DDX; throw an exception on error.
            DDXParser ddx_parser(data.get_factory());

            // Read the MPM boundary and then read the subsequent headers
            string boundary = read_multipart_boundary(rs->get_stream());
            DBG(cerr << "MPM Boundary: " << boundary << endl);
            read_multipart_headers(rs->get_stream(), "text/xml", dods_ddx);

            // Parse the DDX, reading up to and including the next boundary.
            // Return the CID for the matching data part
            string data_cid;
            ddx_parser.intern_stream(rs->get_stream(), &data, data_cid, boundary);

            // Munge the CID into something we can work with
            data_cid = cid_to_header_value(data_cid);
            DBG(cerr << "Data CID: " << data_cid << endl);

            // Read the data part's MPM part headers (boundary was read by
            // DDXParse::intern)
            read_multipart_headers(rs->get_stream(), "application/octet-stream", dap4_data, data_cid);

            // Now read the data
#if FILE_UN_MARSHALLER
            XDRFileUnMarshaller um(rs->get_stream());
#else
        fpistream in ( rs->get_stream() );
            XDRStreamUnMarshaller um( in );
#endif
            for (DDS::Vars_iter i = data.var_begin(); i != data.var_end(); i++) {
                (*i)->deserialize(um, &data);
            }
            return;
        }
#endif

        case dods_data:
        default: {
            // Parse the DDS; throw an exception on error.
            data.parse(rs->get_stream());
#if FILE_UN_MARSHALLER
            XDRFileUnMarshaller um(rs->get_stream());
#else
            fpistream in ( rs->get_stream() );
            XDRStreamUnMarshaller um( in );
#endif
            // Load the DDS with data.
            for (DDS::Vars_iter i = data.var_begin(); i != data.var_end(); i++) {
                (*i)->deserialize(um, &data);
            }
            return;
        }
    }
}

/** This private method process data from both local and remote sources. It
    exists to eliminate duplication of code. */
void Connect::process_data(DDS &data, Response *rs)
{
    DBG(cerr << "Entering Connect::process_data" << endl);

    data.set_dap_version(rs->get_protocol());

    DBG(cerr << "Entering process_data: d_stream = " << rs << endl);
    switch (rs->get_type()) {
    case dods_error: {
        Error e;
        if (!e.parse(rs->get_stream()))
            throw InternalErr(__FILE__, __LINE__, "Could not parse the Error object returned by the server!");
        throw e;
    }

    case web_error:
        // Web errors (those reported in the return document's MIME header)
        // are processed by the WWW library.
        throw InternalErr(__FILE__, __LINE__,
            "An error was reported by the remote web server; this should have been processed by HTTPConnect.");

#if 0
        // FIXME: The following case is never used. There is no such response. jhrg 10/20/15
        // This code triggers a security warning from Coverity; since it is not used,
        // I have removed it. jhrg 5/5/16
    case dods_data_ddx: {
        // Parse the DDX; throw an exception on error.
        DDXParser ddx_parser(data.get_factory());

        // Read the MPM boundary and then read the subsequent headers
        string boundary = read_multipart_boundary(rs->get_stream());
        DBG(cerr << "MPM Boundary: " << boundary << endl);
        read_multipart_headers(rs->get_stream(), "text/xml", dods_ddx);

        // Parse the DDX, reading up to and including the next boundary.
        // Return the CID for the matching data part
        string data_cid;
        ddx_parser.intern_stream(rs->get_stream(), &data, data_cid, boundary);

        // Munge the CID into something we can work with
        data_cid = cid_to_header_value(data_cid);
        DBG(cerr << "Data CID: " << data_cid << endl);

        // Read the data part's MPM part headers (boundary was read by
        // DDXParse::intern)
        read_multipart_headers(rs->get_stream(), "application/octet-stream", dap4_data, data_cid);

        // Now read the data
        XDRFileUnMarshaller um(rs->get_stream());
        for (DDS::Vars_iter i = data.var_begin(); i != data.var_end(); i++) {
            (*i)->deserialize(um, &data);
        }
        return;
    }
#endif

    case dods_data:
    default: {
        // Parse the DDS; throw an exception on error.
        data.parse(rs->get_stream());

        XDRFileUnMarshaller um(rs->get_stream());

        // Load the DDS with data.
        for (DDS::Vars_iter i = data.var_begin(); i != data.var_end(); i++) {
            (*i)->deserialize(um, &data);
        }

        return;
    }
    }
}

// Barely a parser... This is used when reading from local sources of DODS
// Data objects. It simulates the important actions of the libwww MIME header
// parser. Those actions fill in certain fields in the Connect object. jhrg
// 5/20/97
//
// Make sure that this parser reads from data_source without disturbing the
// information in data_source that follows the MIME header. Since the DDS
// (which follows the MIME header) is parsed by a flex/bison scanner/parser,
// make sure to use I/O calls that will mesh with ANSI C I/O calls. In the
// old GNU libg++, the C++ calls were synchronized with the C calls, but that
// may no longer be the case. 5/31/99 jhrg

/** Use when you cannot use libcurl.

 @note This method tests for MIME headers with lines terminated by CRLF
 (\r\n) and Newlines (\n). In either case, the line terminators are removed
 before each header is processed.

 @note FIXME The code uses tainted data via get_next_mime_header() whcih
 should be fixed. See the note in mime_util.cc

 @param data_source Read from this stream.
 @param rs Value/Result parameter. Dump version and type information here.
 */
void Connect::parse_mime(Response *rs)
{
    rs->set_version("dods/0.0"); // initial value; for backward compatibility.
    rs->set_protocol("2.0");

    FILE *data_source = rs->get_stream();
    string mime = get_next_mime_header(data_source);
    while (!mime.empty()) {
        string header, value;
        parse_mime_header(mime, header, value);

        // Note that this is an ordered list
        if (header == "content-description:") {
            DBG(cout << header << ": " << value << endl);
            rs->set_type(get_description_type(value));
        }
        // Use the value of xdods-server only if no other value has been read
        else if (header == "xdods-server:" && rs->get_version() == "dods/0.0") {
            DBG(cout << header << ": " << value << endl);
            rs->set_version(value);
        }
        // This trumps 'xdods-server' and 'server'
        else if (header == "xopendap-server:") {
            DBG(cout << header << ": " << value << endl);
            rs->set_version(value);
        }
        else if (header == "xdap:") {
            DBG(cout << header << ": " << value << endl);
            rs->set_protocol(value);
        }
        // Only look for 'server' if no other header supplies this info.
        else if (rs->get_version() == "dods/0.0" && header == "server:") {
            DBG(cout << header << ": " << value << endl);
            rs->set_version(value);
        }

        mime = get_next_mime_header(data_source);
    }
}

// public mfuncs

/** The Connect constructor requires a <tt>name</tt>, which is the URL to
 which the connection is to be made.

 @param n The URL for the virtual connection.
 @param uname Use this username for authentication. Null by default.
 @param password Password to use for authentication. Null by default.
 @brief Create an instance of Connect. */
Connect::Connect(const string &n, string uname, string password) :
        d_http(0), d_version("unknown"), d_protocol("2.0")
{
    string name = prune_spaces(n);

    // Figure out if the URL starts with 'http', if so, make sure that we
    // talk to an instance of HTTPConnect.
    if (name.find("http") == 0) {
        DBG(cerr << "Connect: The identifier is an http URL" << endl);
        d_http = new HTTPConnect(RCReader::instance());

        // Find and store any CE given with the URL.
        string::size_type dotpos = name.find('?');
        if (dotpos != name.npos) {
            _URL = name.substr(0, dotpos);
            string expr = name.substr(dotpos + 1);

            dotpos = expr.find('&');
            if (dotpos != expr.npos) {
                _proj = expr.substr(0, dotpos);
                _sel = expr.substr(dotpos); // XXX includes '&'
            }
            else {
                _proj = expr;
                _sel = "";
            }
        }
        else {
            _URL = name;
            _proj = "";
            _sel = "";
        }

        _local = false;
    }
    else {
        DBG(cerr << "Connect: The identifier is a local data source." << endl);

        d_http = 0;
        _URL = "";
        _local = true; // local in this case means non-DAP
    }

    set_credentials(uname, password);
}

Connect::~Connect()
{
    DBG2(cerr << "Entering the Connect dtor" << endl);

    if (d_http)
        delete d_http;
    d_http = 0;

    DBG2(cerr << "Leaving the Connect dtor" << endl);
}

/** Get version information from the server. This is a new method which will
 ease the transition to DAP 4.

 @note Use request_protocol() to get the DAP protocol version.

 @return The DAP version string.
 @see request_protocol() */
string Connect::request_version()
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

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

    delete rs;
    rs = 0;

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
string Connect::request_protocol()
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

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

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
void Connect::request_das(DAS &das)
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

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

    switch (rs->get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs->get_stream())) {
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
                das.parse(rs->get_stream()); // read and parse the das from a file
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
void Connect::request_das_url(DAS &das)
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

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

    switch (rs->get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs->get_stream())) {
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
                das.parse(rs->get_stream()); // read and parse the das from a file
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
void Connect::request_dds(DDS &dds, string expr)
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

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

    switch (rs->get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs->get_stream())) {
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
                dds.parse(rs->get_stream()); // read and parse the dds from a file
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
void Connect::request_dds_url(DDS &dds)
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

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

    switch (rs->get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs->get_stream())) {
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
                dds.parse(rs->get_stream()); // read and parse the dds from a file
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
void Connect::request_ddx(DDS &dds, string expr)
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
        throw;
    }

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

    switch (rs->get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs->get_stream())) {
                delete rs;
                rs = 0;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            break;

        case dods_ddx:
            try {
                string blob;

                DDXParser ddxp(dds.get_factory());
                ddxp.intern_stream(rs->get_stream(), &dds, blob);
            }
            catch (Error &e) {
                delete rs;
                throw;
            }
            break;

        default:
            ObjectType ot = rs->get_type();
            delete rs;
            throw Error("Invalid response type when requesting a DDX response. Response type: " + long_to_string(ot));
    }

    delete rs;
}

/** @brief The 'url' version of request_ddx
 @see Connect::request_ddx. */
void Connect::request_ddx_url(DDS &dds)
{
    string use_url = _URL + "?" + _proj + _sel;

    Response *rs = 0;
    try {
        rs = d_http->fetch_url(use_url);
    }
    catch (Error &e) {
        delete rs;
        throw;
    }

    d_version = rs->get_version();
    d_protocol = rs->get_protocol();

    switch (rs->get_type()) {
        case dods_error: {
            Error e;
            if (!e.parse(rs->get_stream())) {
                delete rs;
                throw InternalErr(__FILE__, __LINE__, "Could not parse error returned from server.");
            }
            delete rs;
            throw e;
        }

        case web_error:
            // We should never get here; a web error should be picked up read_url
            // (called by fetch_url) and result in a thrown Error object.
            delete rs;
            throw InternalErr(__FILE__, __LINE__, "Web error.");

        case dods_ddx:
            try {
                string blob;

                DDXParser ddxp(dds.get_factory());
                ddxp.intern_stream(rs->get_stream(), &dds, blob);
            }
            catch (Error &e) {
                delete rs;
                throw;
            }
            break;

        default: {
            ObjectType ot = rs->get_type();
            delete rs;

            throw Error("Invalid response type when requesting a DDX response. Response type: " + long_to_string(ot));
        }
    }

    delete rs;
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
void Connect::request_data(DataDDS &data, string expr)
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

        d_version = rs->get_version();
        d_protocol = rs->get_protocol();

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
void Connect::request_data_url(DataDDS &data)
{
    string use_url = _URL + "?" + _proj + _sel;
    Response *rs = 0;
    // We need to catch Error exceptions to ensure calling close_output.
    try {
        rs = d_http->fetch_url(use_url);

        d_version = rs->get_version();
        d_protocol = rs->get_protocol();

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

// FIXME Unused?
void Connect::request_data_ddx(DataDDS &data, string expr)
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

        d_version = rs->get_version();
        d_protocol = rs->get_protocol();

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

// FIXME Unused?
void Connect::request_data_ddx_url(DataDDS &data)
{
    string use_url = _URL + "?" + _proj + _sel;
    Response *rs = 0;
    // We need to catch Error exceptions to ensure calling close_output.
    try {
        rs = d_http->fetch_url(use_url);

        d_version = rs->get_version();
        d_protocol = rs->get_protocol();

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

/** @brief Read data which is preceded by MIME headers.
 This method works for both data dds and data ddx responses.

 @note If you need the DataDDS to hold specializations of the type classes,
 be sure to include the factory class which will instantiate those
 specializations in the DataDDS. Either pass a pointer to the factory to
 DataDDS constructor or use the DDS::set_factory() method after the
 object is built.

 @see read_data_no_mime()
 @param data Result.
 @param rs Read from this Response object. */

void Connect::read_data(DataDDS &data, Response *rs)
{
    if (!rs)
        throw InternalErr(__FILE__, __LINE__, "Response object is null.");

    // Read from data_source and parse the MIME headers specific to DAP2/4.
    parse_mime(rs);

    read_data_no_mime(data, rs);
}
void
Connect::read_data(DDS &data, Response *rs)
{
    if (!rs)
        throw InternalErr(__FILE__, __LINE__, "Response object is null.");

    // Read from data_source and parse the MIME headers specific to DAP2/4.
    parse_mime(rs);

    read_data_no_mime(data, rs);
}

// This function looks at the input stream and makes its best guess at what
// lies in store for downstream processing code. Definitely heuristic.
// Assumptions:
// #1 The current file position is past any MIME headers (if they were present).
// #2 We must reset the FILE* position to the start of the DDS or DDX headers
static void divine_type_information(Response *rs)
{
    // Consume whitespace
    int c = getc(rs->get_stream());
    while (!feof(rs->get_stream()) && !ferror(rs->get_stream()) && isspace(c)) {
        c = getc(rs->get_stream());
    }


    if (ferror(rs->get_stream()))
        throw Error("Error reading response type information: " + string(strerror(errno)));
    if (feof(rs->get_stream()))
        throw Error("Error reading response type information: Found EOF");

    // The heuristic here is that a DataDDX is a multipart MIME document and
    // The first non space character found after the headers is the start of
    // the first part which looks like '--<boundary>' while a DataDDS starts
    // with a DDS (;Dataset {' ...). I take into account that our parsers have
    // accepted both 'Dataset' and 'dataset' for a long time.
    switch (c) {
        case '-':
            rs->set_type(dods_data_ddx);
            break;
        case 'D':
        case 'd':
            rs->set_type(dods_data);
            break;
        default:
            throw InternalErr(__FILE__, __LINE__, "Could not determine type of response object in stream.");
    }

    ungetc(c, rs->get_stream());
}

/** @brief Read data from a file which does not have response MIME headers.
 This method is a companion to read_data(). While read_data() assumes that
 the response has MIME headers, this method does not. If you call this
 with a Response that does contain headers, it will throw an Error (and
 the message is likely to be inscrutable).

 @note This method will use the 'type' information in the Response object
 to choose between processing the response as a data dds or data ddx. If
 there is no type information, it will attempt to figure it out.

 @param data Result.
 @param rs Read from this Response object. */
void Connect::read_data_no_mime(DataDDS &data, Response *rs)
{
    if (rs->get_type() == unknown_type)
        divine_type_information(rs);

    switch (rs->get_type()) {
        case dods_data:
            d_version = rs->get_version();
            d_protocol = rs->get_protocol();
            process_data(data, rs);
            break;
        case dods_data_ddx:
            process_data(data, rs);
            d_version = rs->get_version();
            d_protocol = data.get_protocol();
            break;
        default:
            throw InternalErr(__FILE__, __LINE__, "Should have been a DataDDS or DataDDX.");
    }
}
void Connect::read_data_no_mime(DDS &data, Response *rs)
{
    if (rs->get_type() == unknown_type)
        divine_type_information(rs);

    switch (rs->get_type()) {
    case dods_data:
        d_version = rs->get_version();
        d_protocol = rs->get_protocol();
        process_data(data, rs);
        break;
    case dods_data_ddx:
        process_data(data, rs);
        d_version = rs->get_version();
        // TODO should check to see if this hack is a correct replacement
        // for get_protocol from DataDDS
        d_protocol = data.get_dap_version();
        break;
    default:
        throw InternalErr(__FILE__, __LINE__, "Should have been a DataDDS or DataDDX.");
    }
}

bool
Connect::is_local()
{
    return _local;
}

/** Return the Connect object's URL in a string.  The URL was set by
 the class constructor, and may not be reset.  If you want to
 open another URL, you must create another Connect object.  There
 is a Connections class created to handle the management of
 multiple Connect objects.

 @brief Get the object's URL.
 @see Connections
 @return A string containing the URL of the data to which the
 Connect object refers.  If the object refers to local data,
 the function returns the null string.
 @param ce If TRUE, the returned URL will include any constraint
 expression enclosed with the Connect object's URL (including the
 <tt>?</tt>).  If FALSE, any constraint expression will be removed from
 the URL.  The default is TRUE.
 */
string Connect::URL(bool ce)
{
    if (_local)
        throw InternalErr(__FILE__, __LINE__, "URL(): This call is only valid for a DAP data source.");

    if (ce)
        return _URL + "?" + _proj + _sel;
    else
        return _URL;
}

/** Return the constraint expression (CE) part of the Connect URL. Note
 that this CE is supplied as part of the URL passed to the
 Connect's constructor.  It is not the CE passed to the
 <tt>request_data()</tt> function.

 @brief Get the Connect's constraint expression.
 @return A string containing the constraint expression (if any)
 submitted to the Connect object's constructor.  */
string Connect::CE()
{
    if (_local)
        throw InternalErr(__FILE__, __LINE__, "CE(): This call is only valid for a DAP data source.");

    return _proj + _sel;
}

/** @brief Set the credentials for responding to challenges while dereferencing
 URLs.
 @param u The username.
 @param p The password.
 @see extract_auth_info() */
void Connect::set_credentials(string u, string p)
{
    if (d_http)
        d_http->set_credentials(u, p);
}

/** Set the \e accept deflate property.
 @param deflate True if the client can accept compressed responses, False
 otherwise. */
void Connect::set_accept_deflate(bool deflate)
{
    if (d_http)
        d_http->set_accept_deflate(deflate);
}

/** Set the \e XDAP-Accept property/header. This is used to send to a server
 the (highest) DAP protocol version number that this client understands.

 @param major The client dap protocol major version
 @param minor The client dap protocol minor version */
void Connect::set_xdap_protocol(int major, int minor)
{
    if (d_http)
        d_http->set_xdap_protocol(major, minor);
}

/** Disable any further use of the client-side cache. In a future version
 of this software, this should be handled so that the www library is
 not initialized with the cache running by default. */
void Connect::set_cache_enabled(bool cache)
{
    if (d_http)
        d_http->set_cache_enabled(cache);
}

bool Connect::is_cache_enabled()
{
    bool status;
    DBG(cerr << "Entering is_cache_enabled (" << hex << d_http << dec
            << ")... ");
    if (d_http)
        status = d_http->is_cache_enabled();
    else
        status = false;
    DBGN(cerr << "exiting" << endl);
    return status;
}

} // namespace libdap
