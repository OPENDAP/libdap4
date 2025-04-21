
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

#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include <curl/curl.h>

#include "HTTPCache.h"

namespace libdap {

class RCReader;
class HTTPCache;
class HTTPResponse;

extern int www_trace;
extern int www_trace_extensive;

/** Use the CURL library to dereference a HTTP URL. Scan the response for
    headers used by DAP 2.0 and extract their values. The body of the
    response is made available using a FILE pointer.

    @author jhrg */
class HTTPConnect {
private:
    CURL *d_curl = nullptr;
    RCReader *d_rcr = nullptr;
    HTTPCache *d_http_cache = nullptr;

    char d_error_buffer[CURL_ERROR_SIZE]{}; // A human-readable message.
    std::string d_content_type;             // apparently read by libcurl; this is valid only after curl_easy_perform()

    bool d_accept_deflate = false; // Use deflate encoding for HTTP requests

    std::string d_username; // extracted from URL
    std::string d_password; // extracted from URL
    std::string d_upstring; // used to pass info into curl

    std::string d_cookie_jar;

    std::vector<std::string> d_request_headers; // Request headers

    int d_dap_client_protocol_major = 2;
    int d_dap_client_protocol_minor = 0;

    bool d_use_cpp_streams; // Build HTTPResponse objects using fstream and not FILE*

    bool d_verbose_runtime = false;
    bool d_cached_response = false;

    std::mutex d_connect_mutex; // Used to lock the public interface.

    void www_lib_init();

    long read_url(const std::string &url, FILE *stream, std::vector<std::string> &resp_hdrs,
                  const std::vector<std::string> &headers);

    long read_url(const std::string &url, FILE *stream, std::vector<std::string> &resp_hdrs);

    HTTPResponse *plain_fetch_url(const std::string &url);

    HTTPResponse *caching_fetch_url(const std::string &url);

    bool url_uses_proxy_for(const std::string &url);

    bool url_uses_no_proxy_for(const std::string &url) const noexcept;

    void set_verbose_runtime(bool verbose) { d_verbose_runtime = verbose; }

    bool is_cached_response() const { return d_cached_response; }

    static std::function<bool(const std::string &)> header_match(const std::string &d_header);

    friend class HTTPConnectTest;

    friend class HTTPThreadsConnectTest;

    friend class ParseHeader;

public:
    explicit HTTPConnect(RCReader *rcr, bool use_cpp = false);

    /** @name Suppress default methods
     These methods are not supported and are implemented here as protected
     methods to suppress the C++-supplied default versions (which will
     break this object). */
    ///@{
    HTTPConnect() = delete;

    HTTPConnect(const HTTPConnect &) = delete;

    HTTPConnect &operator=(const HTTPConnect &) = delete;
    ///@}

    virtual ~HTTPConnect();

    void set_credentials(const std::string &u, const std::string &p);

    void set_accept_deflate(bool deflate);

    void set_xdap_protocol(int major, int minor);

    bool use_cpp_streams() const { return d_use_cpp_streams; }

    void set_use_cpp_streams(bool use_cpp_streams) {
        std::lock_guard<std::mutex> lock(d_connect_mutex);
        d_use_cpp_streams = use_cpp_streams;
    }

    /** Set the cookie jar. This function sets the name of a file used to store
    cookies returned by servers. This will help with things like single
    sign on systems.

    @param cookie_jar The pathname to the file that stores cookies. If this
    is the empty string saving cookies is disabled. */
    void set_cookie_jar(const std::string &cookie_jar) {
        std::lock_guard<std::mutex> lock(d_connect_mutex);
        d_cookie_jar = cookie_jar;
    }

    /** Set the state of the HTTP cache. By default, the HTTP cache is
    enabled or disabled using the value of the \c USE_CACHE property in
    the \c .dodsrc file. Use this method to set the state from within a
    program.
    @param enabled True to use the cache, False to disable. */
    void set_cache_enabled(bool enabled) {
        std::lock_guard<std::mutex> lock(d_connect_mutex);
        if (d_http_cache)
            d_http_cache->set_cache_enabled(enabled);
    }

    /** Return the current state of the HTTP cache. */
    bool is_cache_enabled() const { return (d_http_cache) != nullptr && d_http_cache->is_cache_enabled(); }

    HTTPResponse *fetch_url(const std::string &url);
};

} // namespace libdap

#endif // _httpconnect_h
