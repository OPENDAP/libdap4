
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

#ifndef _d4connect_h
#define _d4connect_h

#include <string>

#define DAP4_CE_QUERY_KEY "dap4.ce"

namespace libdap
{

class HTTPConnect;
class DMR;
class Response;

class D4Connect
{
private:
    HTTPConnect *d_http;

    bool d_local;  // Is this a local connection?
    std::string d_URL;  // URL to remote dataset (minus CE)
    std::string d_UrlQueryString; 	// CE

    std::string d_server; // Server implementation information (the XDAP-Server header)
    std::string d_protocol; // DAP protocol from the server (XDAP)

    void process_data(DMR &data, Response &rs);
    void process_dmr(DMR &data, Response &rs);

    // Use when you cannot use but have a complete response with MIME headers
    void parse_mime(Response &rs);

    std::string build_dap4_ce(const std::string requestSuffix, const std::string expr);

protected:
    /** @name Suppress the C++ defaults for these. */
    D4Connect();
    D4Connect(const D4Connect &);
    D4Connect &operator=(const D4Connect &);

public:
    D4Connect(const std::string &url, std::string uname = "", std::string password = "");

    virtual ~D4Connect();

    bool is_local() const { return d_local; }

    virtual std::string URL() const { return d_URL; }
    virtual std::string CE() const { return d_UrlQueryString; }

    void set_credentials(std::string u, std::string p);
    void set_accept_deflate(bool deflate);
    void set_xdap_protocol(int major, int minor);

    void set_cache_enabled(bool enabled);
    bool is_cache_enabled();

    void set_xdap_accept(int major, int minor);

    /** Return the protocol/implementation version of the most recent
    response. This is a poorly designed method, but it returns
    information that is useful when used correctly. Before a response is
    made, this contains the std::string "unknown." This should ultimately hold
    the \e protocol version; it currently holds the \e implementation
    version.

        @see get_protocol()
        @deprecated */
    std::string get_version() { return d_server; }

    /** Return the DAP protocol version of the most recent
        response. Before a response is made, this contains the std::string "2.0."
        */
    std::string get_protocol() { return d_protocol; }

    virtual void request_dmr(DMR &dmr, const std::string expr = "");
    virtual void request_dap4_data(DMR &dmr, const std::string expr = "");
#if 0
    virtual void request_version();
#endif

    virtual void read_dmr(DMR &dmr, Response &rs);
    virtual void read_dmr_no_mime(DMR &dmr, Response &rs);

    virtual void read_data(DMR &data, Response &rs);
    virtual void read_data_no_mime(DMR &data, Response &rs);
};

} // namespace libdap

#endif // _d4connect_h
