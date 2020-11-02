
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

#ifndef _httpconnect_h
#define _httpconnect_h


#include <string>

#include <curl/curl.h>
//No longer used in CURL - pwest April 09, 2012
//#include <curl/types.h>
#include <curl/easy.h>

#ifndef _rc_reader_h_
#include "RCReader.h"
#endif

#ifndef _object_type_h
#include "ObjectType.h"
#endif

#ifndef _http_cache_h
#include "HTTPCache.h"
#endif

#ifndef http_response_h
#include "HTTPResponse.h"
#endif

#ifndef _util_h
#include "util.h"
#endif

using std::string;
using std::vector;

namespace libdap
{

extern int www_trace;
extern int www_trace_extensive;
extern int dods_keep_temps;

/** Use the CURL library to dereference a HTTP URL. Scan the response for
    headers used by DAP 2.0 and extract their values. The body of the
    response is made available using a FILE pointer.

    @author jhrg */

class HTTPConnect
{
private:
    CURL *d_curl;
    RCReader *d_rcr;
    HTTPCache *d_http_cache;

    char d_error_buffer[CURL_ERROR_SIZE]; // A human-readable message.
    std::string d_content_type; // apparently read by libcurl; this is valid only after curl_easy_perform()

    bool d_accept_deflate;

    string d_username;  // extracted from URL
    string d_password;  // extracted from URL
    string d_upstring;  // used to pass info into curl

    string d_cookie_jar;

    vector<string> d_request_headers; // Request headers

    int d_dap_client_protocol_major;
    int d_dap_client_protocol_minor;

    bool d_use_cpp_streams;	// Build HTTPResponse objects using fstream and not FILE*

    void www_lib_init();
    long read_url(const string &url, FILE *stream, vector<string> *resp_hdrs,
                  const vector<string> *headers = 0);

    HTTPResponse *plain_fetch_url(const string &url);
    HTTPResponse *caching_fetch_url(const string &url);

    bool url_uses_proxy_for(const string &url);
    bool url_uses_no_proxy_for(const string &url) throw();

    void extract_auth_info(string &url);

    friend size_t save_raw_http_header(void *ptr, size_t size, size_t nmemb,
                                       void *http_connect);
    friend class HTTPConnectTest;
    friend class ParseHeader;

protected:
    /** @name Suppress default methods
    These methods are not supported and are implemented here as protected
    methods to suppress the C++-supplied default versions (which will
    break this object). */
    //@{
    HTTPConnect();
	HTTPConnect(const HTTPConnect &);
	HTTPConnect &operator=(const HTTPConnect &);
    //@}

public:
    HTTPConnect(RCReader *rcr, bool use_cpp = false);

    virtual ~HTTPConnect();

    void set_credentials(const string &u, const string &p);
    void set_accept_deflate(bool defalte);
    void set_xdap_protocol(int major, int minor);

    bool use_cpp_streams() const { return d_use_cpp_streams; }
    void set_use_cpp_streams(bool use_cpp_streams) { d_use_cpp_streams = use_cpp_streams; }

    /** Set the cookie jar. This function sets the name of a file used to store
    cookies returned by servers. This will help with things like single
    sign on systems.

    @param cookie_jar The pathname to the file that stores cookies. If this
    is the empty string saving cookies is disabled. */
    void set_cookie_jar(const string &cookie_jar) { d_cookie_jar = cookie_jar; }

    /** Set the state of the HTTP cache. By default, the HTTP cache is
    enabled or disabled using the value of the \c USE_CACHE property in
    the \c .dodsrc file. Use this method to set the state from within a
    program.
    @param enabled True to use the cache, False to disable. */
    void set_cache_enabled(bool enabled) {
        if (d_http_cache)
            d_http_cache->set_cache_enabled(enabled);
    }

    /** Return the current state of the HTTP cache. */
    bool is_cache_enabled() { return (d_http_cache) ? d_http_cache->is_cache_enabled() : false; }

    HTTPResponse *fetch_url(const string &url);
};

} // namespace libdap

#endif // _httpconnect_h
