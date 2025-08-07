
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

#include "config.h"

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/stat.h>

#include <curl/curl.h>
// No longer used in CURL - pwest April 09, 2012
// #include <curl/types.h>
#include <curl/easy.h>

#include "GNURegex.h"
#include "HTTPCache.h"
#include "HTTPCacheResponse.h"
#include "HTTPConnect.h"
#include "HTTPResponse.h"
#include "RCReader.h"
#include "debug.h"
#include "mime_util.h"

using namespace std;

namespace libdap {

#ifndef NDEBUG
// If this is a developer build (so NDEBUG is not defined) then if the HTTPConnect
// field d_verbose_runtime is true, then the VERBOSE_RUNTIME macro will print stuff.
// This will only work inside HTTPConnect methods with code that has access to the
// private field d_verbose_runtime. 02/22/23 jhrg
#define VERBOSE_RUNTIME(x)                                                                                             \
    do {                                                                                                               \
        if (d_verbose_runtime)                                                                                         \
            (x);                                                                                                       \
    } while (false)
#else
#define VERBOSE_RUNTIME(x) /* x */
#endif

// These global variables are not MT-Safe, but I'm leaving them as is because
// they are used only for debugging (set them in a debugger like gdb or ddd).
// They are not static because I think that many debuggers cannot access
// static variables. 08/07/02 jhrg

// Set this to 1 to turn on libcurl's verbose mode (for debugging).
int www_trace = 0;

// Set this to 1 to turn on libcurl's VERY verbose mode.
int www_trace_extensive = 0;

// Keep the temporary files; useful for debugging.
int dods_keep_temps = 0;

#define CLIENT_ERR_MIN 400
#define CLIENT_ERR_MAX 417
static const char *http_client_errors[CLIENT_ERR_MAX - CLIENT_ERR_MIN + 1] = {
    "Bad Request:",
    "Unauthorized: Contact the server administrator.",
    "Payment Required.",
    "Forbidden: Contact the server administrator.",
    "Not Found: The data source or server could not be found.\n\
        Often this means that the OPeNDAP server is missing or needs attention.\n\
        Please contact the server administrator.",
    "Method Not Allowed.",
    "Not Acceptable.",
    "Proxy Authentication Required.",
    "Request Time-out.",
    "Conflict.",
    "Gone:.",
    "Length Required.",
    "Precondition Failed.",
    "Request Entity Too Large.",
    "Request URI Too Large.",
    "Unsupported Media Type.",
    "Requested Range Not Satisfiable.",
    "Expectation Failed."};

#define SERVER_ERR_MIN 500
#define SERVER_ERR_MAX 505
static const char *http_server_errors[SERVER_ERR_MAX - SERVER_ERR_MIN + 1] = {
    "Internal Server Error.", "Not Implemented.",  "Bad Gateway.",
    "Service Unavailable.",   "Gateway Time-out.", "HTTP Version Not Supported."};

/** This function translates the HTTP status codes into error messages. It
    works for those code greater than or equal to 400. */
static string http_status_to_string(int status) {
    if (status >= CLIENT_ERR_MIN && status <= CLIENT_ERR_MAX)
        return {http_client_errors[status - CLIENT_ERR_MIN]};
    else if (status >= SERVER_ERR_MIN && status <= SERVER_ERR_MAX)
        return {http_server_errors[status - SERVER_ERR_MIN]};
    else
        return {"Unknown Error: This indicates a problem with libdap++.\nPlease report this to support@opendap.org."};
}

static ObjectType determine_object_type(const string &header_value) {
    string::size_type plus = header_value.find('+');
    string base_type;
    string type_extension;
    if (plus != string::npos) {
        base_type = header_value.substr(0, plus);
        type_extension = header_value.substr(plus + 1);
    } else
        base_type = header_value;

    if (base_type == DMR_Content_Type ||
        (base_type.find("application/") != string::npos && base_type.find("dap4.dataset-metadata") != string::npos)) {
        if (type_extension == "xml")
            return dap4_dmr;
        else
            return unknown_type;
    } else if (base_type == DAP4_DATA_Content_Type ||
               (base_type.find("application/") != string::npos && base_type.find("dap4.data") != string::npos)) {
        return dap4_data;
    } else if (header_value.find("text/html") != string::npos) {
        return web_error;
    } else
        return unknown_type;
}

/** Functor to parse the headers in the d_headers field. After the headers
    have been read off the wire and written into the d_headers field, scan
    them and set special fields for certain headers special to the DAP. */

class ParseHeader : public function<void(const string &)> {
    ObjectType type = unknown_type; // What type of object is in the stream?
    string server = "dods/0.0";     // Server's version string.
    string protocol = "2.0";        // Server's protocol version.
    string location;                // Url returned by server

public:
    ParseHeader() = default;

    void operator()(const string &line) {
        string name, value;
        parse_mime_header(line, name, value);

        DBG2(cerr << name << ": " << value << endl);

        // Content-Type is used to determine the content of DAP4 responses, but allow the
        // Content-Description header to override CT o preserve operation with DAP2 servers.
        // jhrg 11/12/13
        if (type == unknown_type && name == "content-type") {
            type = determine_object_type(value); // see above
        }
        if (name == "content-description" && !(type == dap4_dmr || type == dap4_data || type == dap4_error)) {
            type = get_description_type(value); // defined in mime_util.cc
        }
        // The second test (== "dods/0.0") tests if xopendap-server has already
        // been seen. If so, use that header in preference to the old
        // XDODS-Server header. jhrg 2/7/06
        else if (name == "xdods-server" && server == "dods/0.0") {
            server = value;
        } else if (name == "xopendap-server") {
            server = value;
        } else if (name == "xdap") {
            protocol = value;
        } else if (server == "dods/0.0" && name == "server") {
            server = value;
        } else if (name == "location") {
            location = value;
        }
    }

    ObjectType get_object_type() const { return type; }

    string get_server() const { return server; }

    string get_protocol() const { return protocol; }

    string get_location() const { return location; }
};

/** A libcurl callback function used to read response headers. Read headers,
    line by line, from ptr. The fourth param is really supposed to be a FILE
    *, but libcurl just holds the pointer and passes it to this function
    without using it itself. I use that to pass in a pointer to a vector
    of strings so that there's some place to
    dump the headers. Later on the code (see fetch_url()) parses the headers
    special to the DAP.

    @param ptr A pointer to one line of character data; one header.
    @param size Size of each character (nominally one byte).
    @param nmemb Number of bytes.
    @param resp_hdrs A pointer to a vector<string>. Set in read_url.
    @return The number of bytes processed. Must be equal to size * nmemb or
    libcurl will report an error. */

static size_t save_raw_http_headers(void *ptr, size_t size, size_t nmemb, void *resp_hdrs) {
    DBG2(cerr << "Inside the header parser." << endl);
    auto hdrs = static_cast<vector<string> *>(resp_hdrs);

    // Grab the header, minus the trailing newline. Or \r\n pair.
    string complete_line;
    if (nmemb > 1 && *(static_cast<char *>(ptr) + size * (nmemb - 2)) == '\r')
        complete_line.assign(static_cast<char *>(ptr), size * (nmemb - 2));
    else
        complete_line.assign(static_cast<char *>(ptr), size * (nmemb - 1));

    // Store all non-empty headers that are not HTTP status codes
    if (!complete_line.empty() && complete_line.find("HTTP") == string::npos) {
        DBG(cerr << "Header line: " << complete_line << endl);
        hdrs->push_back(complete_line);
    }

    return size * nmemb;
}

/** A libcurl callback for debugging protocol issues. */
static int curl_debug(CURL *, curl_infotype info, char *msg, size_t size, void *) {
    string message(msg, size);

    switch (info) {
    case CURLINFO_TEXT:
        cerr << "Text: " << message;
        break;
    case CURLINFO_HEADER_IN:
        cerr << "Header in: " << message;
        break;
    case CURLINFO_HEADER_OUT:
        cerr << "Header out: " << message;
        break;
    case CURLINFO_DATA_IN:
        if (www_trace_extensive)
            cerr << "Data in: " << message;
        break;
    case CURLINFO_DATA_OUT:
        if (www_trace_extensive)
            cerr << "Data out: " << message;
        break;
    case CURLINFO_END:
        cerr << "End: " << message;
        break;
#ifdef CURLINFO_SSL_DATA_IN
    case CURLINFO_SSL_DATA_IN:
        cerr << "SSL Data in: " << message;
        break;
#endif
#ifdef CURLINFO_SSL_DATA_OUT
    case CURLINFO_SSL_DATA_OUT:
        cerr << "SSL Data out: " << message;
        break;
#endif
    default:
        if (www_trace_extensive)
            cerr << "Curl info: " << message;
        break;
    }
    return 0;
}

/** Initialize libcurl. Create a libcurl handle that can be used for all of
    the HTTP requests made through this instance. */

void HTTPConnect::www_lib_init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    d_curl = curl_easy_init();
    if (!d_curl)
        throw InternalErr(__FILE__, __LINE__, "Could not initialize libcurl.");

    curl_easy_setopt(d_curl, CURLOPT_ERRORBUFFER, d_error_buffer);

    curl_easy_setopt(d_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2); // enables TLSv1.2 / TLSv1.3 version only

    // Now set options that will remain constant for the duration of this
    // CURL object.

    // Set the proxy host.
    if (!d_rcr->get_proxy_server_host().empty()) {
        DBG(cerr << "Setting up a proxy server." << endl);
        DBG(cerr << "Proxy host: " << d_rcr->get_proxy_server_host() << endl);
        DBG(cerr << "Proxy port: " << d_rcr->get_proxy_server_port() << endl);
        DBG(cerr << "Proxy pwd : " << d_rcr->get_proxy_server_userpw() << endl);
        curl_easy_setopt(d_curl, CURLOPT_PROXY, d_rcr->get_proxy_server_host().c_str());
        curl_easy_setopt(d_curl, CURLOPT_PROXYPORT, d_rcr->get_proxy_server_port());

        // As of 4/21/08 only NTLM, Digest and Basic work.
#ifdef CURLOPT_PROXYAUTH
        curl_easy_setopt(d_curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY);
#endif

        // Password might not be required. 06/21/04 jhrg
        if (!d_rcr->get_proxy_server_userpw().empty())
            curl_easy_setopt(d_curl, CURLOPT_PROXYUSERPWD, d_rcr->get_proxy_server_userpw().c_str());
    }

    // We have to set FailOnError to false for any of the non-Basic
    // authentication schemes to work. 07/28/03 jhrg
    curl_easy_setopt(d_curl, CURLOPT_FAILONERROR, 0);

    // This means libcurl will use Basic, Digest, GSS Negotiate, or NTLM,
    // choosing the 'safest' one supported by the server.
    // This requires curl 7.10.6 which is still in pre-release. 07/25/03 jhrg
    curl_easy_setopt(d_curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);

    curl_easy_setopt(d_curl, CURLOPT_NOPROGRESS, 1);
    curl_easy_setopt(d_curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(d_curl, CURLOPT_HEADERFUNCTION, save_raw_http_headers);
    // In read_url a call to CURLOPT_WRITEHEADER is used to set the fourth
    // param of save_raw_http_headers to a vector<string> object.

    // Follow 302 (redirect) responses
    curl_easy_setopt(d_curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(d_curl, CURLOPT_MAXREDIRS, 5);

    // If the user turns off SSL validation...
    if (d_rcr->get_validate_ssl() == 0) {
        curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYHOST, 0);
    }

    // Set libcurl to use netrc to access data behind URS auth.
    //  libcurl will use the provided pathname for the ~/.netrc info. 08/23/19 kln
    curl_easy_setopt(d_curl, CURLOPT_NETRC, 1);

    // Look to see if cookies are turned on in the .dodsrc file. If so,
    // activate here. We honor 'session cookies' (cookies without an
    // expiration date) here so that session-based SSO systems will work as
    // expected.
    if (!d_cookie_jar.empty()) {
        DBG(cerr << "Setting the cookie jar to: " << d_cookie_jar << endl);
        curl_easy_setopt(d_curl, CURLOPT_COOKIEJAR, d_cookie_jar.c_str());
        curl_easy_setopt(d_curl, CURLOPT_COOKIESESSION, 1);
    }

    if (www_trace) {
        cerr << "Curl version: " << curl_version() << endl;
        curl_easy_setopt(d_curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(d_curl, CURLOPT_DEBUGFUNCTION, curl_debug);
    }
}

/** Functor to add a single string to a curl_slist. This is used to transfer
    a list of headers from a vector<string> object to a curl_slist. */

/** Use libcurl to dereference a URL. Read the information referenced by \c
    url into the file pointed to by \c stream.

    @note This version of the method does not include a vector of headers.

    @param url The URL to dereference.
    @param stream The destination for the data; the caller can assume that
    the body of the response can be found by reading from this pointer. A
    value/result parameter
    @param resp_hdrs Value/result parameter for the HTTP Response Headers.
    @return The HTTP status code.
    @exception Error Thrown if libcurl encounters a problem; the libcurl
    error message is stuffed into the Error object. */

long HTTPConnect::read_url(const string &url, FILE *stream, vector<string> &resp_hdrs) {
    return read_url(url, stream, resp_hdrs, vector<string>());
}

/** Use libcurl to dereference a URL. Read the information referenced by \c
    url into the file pointed to by \c stream.

    @param url The URL to dereference.
    @param stream The destination for the data; the caller can assume that
    the body of the response can be found by reading from this pointer. A
    value/result parameter
    @param resp_hdrs Value/result parameter for the HTTP Response Headers.
    @param headers A pointer to a vector of HTTP request headers. Default is
    empty. These headers will be appended to the list of default headers.
    @return The HTTP status code.
    @exception Error Thrown if libcurl encounters a problem; the libcurl
    error message is stuffed into the Error object. */

long HTTPConnect::read_url(const string &url, FILE *stream, vector<string> &resp_hdrs, const vector<string> &headers) {
    curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(d_curl, CURLOPT_WRITEDATA, stream);

    DBG(copy(d_request_headers.begin(), d_request_headers.end(), ostream_iterator<string>(cerr, "\n")));

    struct curl_slist *req_hdrs = nullptr;
    auto header_append = [&req_hdrs](const string &header) {
        DBG(cerr << "Adding '" << header.c_str() << "' to the header list." << endl);
        req_hdrs = curl_slist_append(req_hdrs, header.c_str());
    };

    for_each(d_request_headers.begin(), d_request_headers.end(), header_append);
    for_each(headers.begin(), headers.end(), header_append);

    curl_easy_setopt(d_curl, CURLOPT_HTTPHEADER, req_hdrs);

    // Turn off the proxy for this URL?
    if (url_uses_no_proxy_for(url)) {
        DBG(cerr << "Suppress proxy for url: " << url << endl);
        curl_easy_setopt(d_curl, CURLOPT_PROXY, 0);
    }

    string::size_type at_sign = url.find('@');
    // Assume username:password present *and* assume it's an HTTP URL; it *is*
    // HTTPConnect, after all. 7 is position after "http://"; the second arg
    // to substr() is the sub string length.
    if (at_sign != string::npos)
        d_upstring = url.substr(7, at_sign - 7);

    if (!d_upstring.empty())
        curl_easy_setopt(d_curl, CURLOPT_USERPWD, d_upstring.c_str());

    // Pass save_raw_http_headers() a pointer to the vector<string> where the
    // response headers may be stored. Callers can use the resp_hdrs
    // value/result parameter to get the raw response header information .
    curl_easy_setopt(d_curl, CURLOPT_WRITEHEADER, &resp_hdrs);

    // This is the call that causes curl to go and get the remote resource and "write it down"
    // utilizing the configuration state that has been previously conditioned by various perturbations
    // of calls to curl_easy_setopt().
    CURLcode res = curl_easy_perform(d_curl);

    // Free the header list and null the value in d_curl.
    curl_slist_free_all(req_hdrs);
    curl_easy_setopt(d_curl, CURLOPT_HTTPHEADER, 0);

    // Reset the proxy?
    if (url_uses_no_proxy_for(url) && !d_rcr->get_proxy_server_host().empty())
        curl_easy_setopt(d_curl, CURLOPT_PROXY, d_rcr->get_proxy_server_host().c_str());

    if (res != 0)
        throw Error(d_error_buffer);

    long status;
    res = curl_easy_getinfo(d_curl, CURLINFO_HTTP_CODE, &status);
    if (res != 0)
        throw Error(d_error_buffer);

    char *ct_ptr = 0;
    res = curl_easy_getinfo(d_curl, CURLINFO_CONTENT_TYPE, &ct_ptr);
    if (res == CURLE_OK && ct_ptr)
        d_content_type = ct_ptr;
    else
        d_content_type = "";

    return status;
}

/** If the .dodsrc file gives a value for PROXY_FOR, return true if the
    current URL matches the regular expression. */

bool HTTPConnect::url_uses_proxy_for(const string &url) {
    if (d_rcr->is_proxy_for_used()) {
        // NB: This could be improved by moving the Regex instance into
        // the RCReader class, but the proxy stuff is all deprecated.
        // jhrg 12/1/21
        Regex host_regex(d_rcr->get_proxy_for_regexp().c_str());
        int index = 0, matchlen;
        return host_regex.search(url.c_str(), url.size(), matchlen, index) != -1;
    }

    return false;
}

/** If the NO_PROXY option is used in the dodsrc file, does this URL match
    the no proxy URL regex? */

bool HTTPConnect::url_uses_no_proxy_for(const string &url) const noexcept {
    return d_rcr->is_no_proxy_for_used() && url.find(d_rcr->get_no_proxy_for_host()) != string::npos;
}

// Public methods. Mostly...

/** Build a virtual connection to a remote data source that will be
    accessed using HTTP.

    @param rcr A pointer to the RCReader object which holds configuration
    file information to be used by this virtual connection.
    @param use_cpp If true, use C++ streams and not FILE* for the data read
    off the wire. This can be changed later using the set_use_cpp_streams()
    method. */

HTTPConnect::HTTPConnect(RCReader *rcr, bool use_cpp)
    : d_rcr(rcr), d_accept_deflate(rcr->get_deflate()), d_use_cpp_streams(use_cpp) {
    // Load in the default headers to send with a request. The empty Pragma
    // headers override libcurl's default Pragma: no-cache header (which
    // will disable caching by Squid, et c.). The User-Agent header helps
    // make server logs more readable. 05/05/03 jhrg
    d_request_headers.emplace_back("Pragma:");
    d_request_headers.emplace_back(string("User-Agent: ") + CNAME + "/" + CVER);
    if (d_accept_deflate)
        d_request_headers.emplace_back("Accept-Encoding: deflate, gzip, compress");

    // HTTPCache::instance returns a valid ptr or nullptr.
    if (d_rcr->get_use_cache())
        d_http_cache = HTTPCache::instance(d_rcr->get_dods_cache_root());
    else
        d_http_cache = nullptr;

    if (d_http_cache) {
        d_http_cache->set_cache_enabled(d_rcr->get_use_cache());
        d_http_cache->set_expire_ignored(d_rcr->get_ignore_expires() != 0);
        d_http_cache->set_max_size(d_rcr->get_max_cache_size());
        d_http_cache->set_max_entry_size(d_rcr->get_max_cached_obj());
        d_http_cache->set_default_expiration(d_rcr->get_default_expires());
        d_http_cache->set_always_validate(d_rcr->get_always_validate() != 0);
    }

    d_cookie_jar = rcr->get_cookie_jar();

    www_lib_init(); // This may throw either Error or InternalErr
}

HTTPConnect::~HTTPConnect() { curl_easy_cleanup(d_curl); }

/** Look for a certain header */
function<bool(const string &)> HTTPConnect::header_match(const string &d_header) {
    return [d_header](const string &arg) { return arg.find(d_header) == 0; };
}

/** Dereference a URL. This method dereferences a URL and stores the result
    (i.e., it formulates an HTTP request and processes the HTTP server's
    response). After this method is successfully called, the value of
    is_response_present() will be true and the methods
    get_response_headers(), type() and server_version() may be called.

    @param url The URL to dereference.
    @return A pointer to the stream.
    @exception Error Thrown if the URL could not be dereferenced.
    @exception InternalErr Thrown if a temporary file to hold the response
    could not be opened. */

HTTPResponse *HTTPConnect::fetch_url(const string &url) {
    HTTPResponse *stream = nullptr;

    if (is_cache_enabled()) {
        stream = caching_fetch_url(url);
    } else {
        stream = plain_fetch_url(url);
    }

#ifdef HTTP_TRACE
    stringstream ss;
    ss << "HTTP/1.0 " << stream->get_status() << " -" << endl;
    for (size_t i = 0; i < stream->get_headers()->size(); i++) {
        ss << stream->get_headers()->at(i) << endl;
    }
    cout << ss.str();
#endif

    // An apparent quirk of libcurl is that it does not pass the Content-type
    // header to the callback used to save them, but check and add it from the
    // saved state variable only if it's not there (without this a test failed
    // in HTTPCacheTest). jhrg 11/12/13
    if (!d_content_type.empty() && find_if(stream->get_headers().begin(), stream->get_headers().end(),
                                           header_match("Content-Type:")) == stream->get_headers().end())
        stream->get_headers().emplace_back("Content-Type: " + d_content_type);

    ParseHeader parser = for_each(stream->get_headers().begin(), stream->get_headers().end(), ParseHeader());

    // handle redirection case (2007-04-27, gaffigan@sfos.uaf.edu)
    if (!parser.get_location().empty() &&
        (url.substr(0, url.find('?')) == parser.get_location().substr(0, url.find('?')))) {
        delete stream;
        return fetch_url(parser.get_location());
    }

    stream->set_type(parser.get_object_type()); // uses the value of content-description

    stream->set_version(parser.get_server());
    stream->set_protocol(parser.get_protocol());

    if (d_use_cpp_streams) {
        stream->transform_to_cpp();
    }

    return stream;
}

// Look around for a reasonable place to put a temporary file. Check first
// the value of the TMPDIR env var. If that does not yield a path that's
// writable (as defined by access(..., W_OK|R_OK)) then look at P_tmpdir
// (defined in stdio.h). If both come up empty, then use `./'.

// Change this to a version that either returns a string or an open file
// descriptor. Use information from https://buildsecurityin.us-cert.gov/
// (see open()) to make it more secure. Ideal solution: get deserialize()
// methods to read from a stream returned by libcurl, not from a temporary
// file. 9/21/07 jhrg Updated to use strings, other misc changes. 3/22/11
static string get_tempfile_template(const string &file_template) {
    string c;

    // Windows has one idea of the standard name(s) for a temporary files dir
#ifdef WIN32
    // white list for a WIN32 directory
    Regex directory("[-a-zA-Z0-9_:\\]*");

    // If we're OK to use getenv(), try it.
#ifdef USE_GETENV
    c = getenv("TEMP");
    if (c && directory.match(c.c_str(), c.length()) && (access(c.c_str(), 6) == 0))
        goto valid_temp_directory;

    c = getenv("TMP");
    if (c && directory.match(c.c_str(), c.length()) && (access(c.c_str(), 6) == 0))
        goto valid_temp_directory;
#endif // USE_GETENV

    // The windows default
    c = "c:\tmp";
    if (c && directory.match(c.c_str(), c.length()) && (access(c.c_str(), 6) == 0))
        goto valid_temp_directory;

#else // Unix/Linux/OSX has another...
    // white list for a directory
    const Regex directory("[-a-zA-Z0-9_/]*");
#ifdef USE_GETENV
    c = getenv("TMPDIR");
    if (directory.match(c.c_str(), c.length()) && (access(c.c_str(), W_OK | R_OK) == 0))
        goto valid_temp_directory;
#endif // USE_GETENV

        // Unix defines this sometimes - if present, use it.
#ifdef P_tmpdir
    if (access(P_tmpdir, W_OK | R_OK) == 0) {
        c = P_tmpdir;
        goto valid_temp_directory;
    }
#endif

    // The Unix default
    c = "/tmp";
    if (directory.match(c.c_str(), c.length()) && (access(c.c_str(), W_OK | R_OK) == 0))
        goto valid_temp_directory;

#endif // WIN32

    // If we found nothing useful, use the current directory
    c = ".";

valid_temp_directory:

#ifdef WIN32
    c += "\\" + file_template;
#else
    c += "/" + file_template;
#endif

    return c;
}

/** Open a temporary file and return its name. This method opens a temporary
    file using get_tempfile_template(). The FILE* \c stream is opened for
    both reads and writes; if it already exists (highly unlikely), it is
    truncated. If used on Unix, it's the callers responsibility to unlink the
    named file so that when all descriptors to it are closed, it will be
    deleted. On Win32 platforms, this method pushes the name of the temporary
    file onto a vector which is used during object destruction to delete all
    the temporary files.

    @note Delete the returned char* using delete[].

    A private method.

    @param stream A value-result parameter; the open file descriptor is
    returned via this parameter.
    @return The name of the temporary file.
    @exception InternalErr thrown if the FILE* could not be opened. */

static string get_temp_file(FILE *&stream) {
    string dods_temp = get_tempfile_template((string) "dodsXXXXXX");

    vector<char> pathname(dods_temp.length() + 1);

    strncpy(pathname.data(), dods_temp.c_str(), dods_temp.length());

    // Open truncated for update. NB: mkstemp() returns a file descriptor.
    // Make sure that temp files are accessible only by the owner.
    mode_t mask = umask(077);
    int fd = mkstemp(pathname.data());
    if (fd < 0)
        throw Error("Could not create a temporary file to store the response: " + string(strerror(errno)));

    stream = fdopen(fd, "w+");
    umask(mask);

    if (!stream)
        throw Error("Failed to open a temporary file for the data values (" + dods_temp + ")");

    return {pathname.data()};
}

/**
 * close temporary files - used here and in ~HTTPResponse
 * @param s
 * @param name
 */
static void close_temp(FILE *s, const string &name) {
    int res = fclose(s);
    if (res)
        throw InternalErr(__FILE__, __LINE__, "!FAIL! " + long_to_string(res));

    res = unlink(name.c_str());
    if (res != 0)
        throw InternalErr(__FILE__, __LINE__, "!FAIL! " + long_to_string(res));
}

/** Dereference a URL. This method looks first in the HTTP cache to see if a
    cached response may be used. It may get the response from the cache, it
    may validate a response in the cache and/or update the response from the
    cache or it may get a new response from the network. In any of those
    cases, the information returned by dereferencing the URL will be stored
    in the cache.

    Return a Response pointer to fetch_url() which, in turn, uses
    ParseHeaders to read stuff from d_headers and fills in the Response
    version and type fields. Thus this method and plain_fetch_url() only have
    to get the stream pointer set, the resources to release and d_headers.

    A private method.

    @note This method assumes that d_http_cache is not null!
    @param url The URL to dereference.
    @return A pointer to the open stream.
    @exception Error Thrown if the URL could not be dereferenced.
    @exception InternalErr Thrown if a temporary file to hold the response
    could not be opened. */

HTTPResponse *HTTPConnect::caching_fetch_url(const string &url) {
    // This lock enables caching for threads that run simultaneously. A recursive
    // mutex is used because this private method can be called recursively by fetch_url().
    static recursive_mutex m;
    lock_guard<recursive_mutex> lock(m);

    VERBOSE_RUNTIME(cerr << "Is this URL (" << url << ") in the cache?... ");

    vector<string> headers;
    string file_name;
    FILE *s = d_http_cache->get_cached_response(url, headers, file_name);
    if (!s) {
        // url not in cache; get it and cache it
        VERBOSE_RUNTIME(cerr << "no; getting response and caching." << endl);
        time_t now = time(nullptr);
        HTTPResponse *rs = plain_fetch_url(url);
        d_http_cache->cache_response(url, now, rs->get_headers(), rs->get_stream());

        return rs;
    } else { // url in cache
        VERBOSE_RUNTIME(cerr << "yes... ");

        if (d_http_cache->is_url_valid(url)) { // url in cache and valid
            VERBOSE_RUNTIME(cerr << "and it's valid; using cached response." << endl);
            d_cached_response = true; // False by default
            auto crs = new HTTPCacheResponse(s, 200, headers, file_name, d_http_cache);
            return crs;
        } else { // url in cache but not valid; validate
            VERBOSE_RUNTIME(cerr << "but it's not valid; validating... ");

            d_http_cache->release_cached_response(s); // This closes 's'
            headers.clear();
            vector<string> cond_hdrs = d_http_cache->get_conditional_request_headers(url);
            FILE *body = 0;
            string dods_temp = get_temp_file(body);
            time_t now = time(0); // When was the request made (now).
            long http_status;

            try {
                http_status = read_url(url, body, /*resp_hdrs*/ headers, cond_hdrs);
                rewind(body);
            } catch (const Error &) {
                close_temp(body, dods_temp);
                throw;
            }

            switch (http_status) {
            case 200: { // New headers and new body
                VERBOSE_RUNTIME(cerr << "read a new response; caching." << endl);

                d_http_cache->cache_response(url, now, /*resp_hdrs*/ headers, body);
                auto rs = new HTTPResponse(body, http_status, /*resp_hdrs*/ headers, dods_temp);

                return rs;
            }

            case 304: { // Just new headers, use cached body
                VERBOSE_RUNTIME(cerr << "cached response valid; updating." << endl);

                close_temp(body, dods_temp);
                d_cached_response = true;
                d_http_cache->update_response(url, now, /*resp_hdrs*/ headers);
                string file_name;
                FILE *hs = d_http_cache->get_cached_response(url, headers, file_name);
                auto crs = new HTTPCacheResponse(hs, 304, headers, file_name, d_http_cache);
                return crs;
            }

            default: { // Oops.
                close_temp(body, dods_temp);
                if (http_status >= 400) {
                    string msg = "Error while reading the URL: ";
                    msg += url;
                    msg += ".\nThe OPeNDAP server returned the following message:\n";
                    msg += http_status_to_string(http_status);
                    throw Error(msg);
                } else {
                    throw InternalErr(__FILE__, __LINE__,
                                      "Bad response from the HTTP server: " + long_to_string(http_status));
                }
            }
            }
        }
    }

    throw InternalErr(__FILE__, __LINE__, "Should never get here");
}

/** Dereference a URL and load its body into a temporary file. This
    method ignores the HTTP cache.

    A private method.

    @param url The URL to dereference.
    @return A pointer to the open stream.
    @exception Error Thrown if the URL could not be dereferenced.
    @exception InternalErr Thrown if a temporary file to hold the response
    could not be opened. */

HTTPResponse *HTTPConnect::plain_fetch_url(const string &url) {
    DBG(cerr << "Getting URL: " << url << endl);
    FILE *stream = nullptr;
    string dods_temp = get_temp_file(stream);
    vector<string> resp_hdrs;

    int status = -1;
    try {
        status = read_url(url, stream, resp_hdrs); // Throws Error.
        if (status >= 400) {
            string msg = "Error while reading the URL: ";
            msg += url;
            msg += ".\nThe OPeNDAP server returned the following message:\n";
            msg += http_status_to_string(status);
            throw Error(msg);
        }
    } catch (const Error &) {
        close_temp(stream, dods_temp);
        throw;
    }

    rewind(stream);
    return new HTTPResponse(stream, status, resp_hdrs, dods_temp);
}

/** Set the <em>accept deflate</em> property. If true, the DAP client
    announces to a server that it can accept responses compressed using the
    \c deflate algorithm. This property is automatically set using a value
    from the <code>.dodsrc</code> configuration file. This method provides a
    way to override that behavior.

    @note If the configuration file is not present or does not include a
    value for this property, it is set to \c false.

    @param deflate True sets the <em>accept deflate</em> property, False clears
    it. */
void HTTPConnect::set_accept_deflate(bool deflate) {
    lock_guard<mutex> lock(d_connect_mutex);

    d_accept_deflate = deflate;

    if (d_accept_deflate) {
        if (find(d_request_headers.begin(), d_request_headers.end(), "Accept-Encoding: deflate, gzip, compress") ==
            d_request_headers.end())
            d_request_headers.emplace_back("Accept-Encoding: deflate, gzip, compress");
    } else {
        vector<string>::iterator i;
        i = remove_if(d_request_headers.begin(), d_request_headers.end(),
                      [](const string &header) { return header == "Accept-Encoding: deflate, gzip, compress"; });
        d_request_headers.erase(i, d_request_headers.end());
    }
}

/** Set the <em>xdap_accept</em> property/HTTP-header. This sets the value
    of the DAP which the client advertises to servers that it understands.
    The information (client protocol major and minor versions) are recorded
    in the instance and the information is sent to servers using the
    XDAP-Accept HTTP request header.

    @param major The dap client major protocol version
    @param minor The dap client minor protocol version */
void HTTPConnect::set_xdap_protocol(int major, int minor) {
    lock_guard<mutex> lock(d_connect_mutex);

    // Look for, and remove if one exists, an XDAP-Accept header
    vector<string>::iterator i;
    i = find_if(d_request_headers.begin(), d_request_headers.end(), header_match("XDAP-Accept:"));
    if (i != d_request_headers.end())
        d_request_headers.erase(i);

    // Record and add the new header value
    d_dap_client_protocol_major = major;
    d_dap_client_protocol_minor = minor;
    ostringstream xdap_accept;
    xdap_accept << "XDAP-Accept: " << major << "." << minor;

    d_request_headers.push_back(xdap_accept.str());

    DBG(copy(d_request_headers.begin(), d_request_headers.end(), ostream_iterator<string>(cerr, "\n")));
}

/** Set the credentials for responding to challenges while dereferencing
    URLs. Alternatively, these can be embedded in the URL. This method
    provides a way for clients of HTTPConnect to get credentials from users
    (say using a pop up dialog) and to not hack the URL to pass that
    information to libcurl. Note that the 'credentials in the URL' scheme \e
    is part of the URL standard.

    This method does nothing if \c u, the username, is empty.

    @param u The username.
    @param p The password.
    @exception InternalErr The credentials could not be registered with
    libcurl.
    @see extract_auth_info() */

void HTTPConnect::set_credentials(const string &u, const string &p) {
    lock_guard<mutex> lock(d_connect_mutex);

    if (u.empty())
        return;

    // Store the credentials locally.
    d_username = u;
    d_password = p;

    d_upstring = u + ":" + p;
}

} // namespace libdap
