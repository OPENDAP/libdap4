
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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/stat.h>

#ifdef WIN32
#include <io.h>
#endif

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <cstdlib>
#include <cstring>

//#define DODS_DEBUG2
//#define HTTP_TRACE
//#define DODS_DEBUG

#undef USE_GETENV


#include "debug.h"
#include "mime_util.h"
#include "GNURegex.h"
#include "HTTPCache.h"
#include "HTTPConnect.h"
#include "RCReader.h"
#include "HTTPResponse.h"
#include "HTTPCacheResponse.h"

using namespace std;

namespace libdap {

// These global variables are not MT-Safe, but I'm leaving them as is because
// they are used only for debugging (set them in a debugger like gdb or ddd).
// They are not static because I think that many debuggers cannot access
// static variables. 08/07/02 jhrg

// Set this to 1 to turn on libcurl's verbose mode (for debugging).
int www_trace = 0;

// Keep the temporary files; useful for debugging.
int dods_keep_temps = 0;

#define CLIENT_ERR_MIN 400
#define CLIENT_ERR_MAX 417
static const char *http_client_errors[CLIENT_ERR_MAX - CLIENT_ERR_MIN +1] =
    {
        "Bad Request:",
        "Unauthorized: Contact the server administrator.",
        "Payment Required.",
        "Forbidden: Contact the server administrator.",
        "Not Found: The data source or server could not be found.\n\
        Often this means that the OPeNDAP server is missing or needs attention;\n\
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
        "Expectation Failed."
    };

#define SERVER_ERR_MIN 500
#define SERVER_ERR_MAX 505
static const char *http_server_errors[SERVER_ERR_MAX - SERVER_ERR_MIN + 1] =
    {
        "Internal Server Error.",
        "Not Implemented.",
        "Bad Gateway.",
        "Service Unavailable.",
        "Gateway Time-out.",
        "HTTP Version Not Supported."
    };

/** This function translates the HTTP status codes into error messages. It
    works for those code greater than or equal to 400. */
static string
http_status_to_string(int status)
{
    if (status >= CLIENT_ERR_MIN && status <= CLIENT_ERR_MAX)
        return string(http_client_errors[status - CLIENT_ERR_MIN]);
    else if (status >= SERVER_ERR_MIN && status <= SERVER_ERR_MAX)
        return string(http_server_errors[status - SERVER_ERR_MIN]);
    else
        return string("Unknown Error: This indicates a problem with libdap++.\nPlease report this to support@opendap.org.");
}

/** Functor to parse the headers in the d_headers field. After the headers
    have been read off the wire and written into the d_headers field, scan
    them and set special fields for certain headers special to the DAP. */

class ParseHeader : public unary_function<const string &, void>
{
    ObjectType type;  // What type of object is in the stream?
    string server;  // Server's version string.
    string protocol;            // Server's protocol version.
    string location;            // Url returned by server

public:
    ParseHeader() : type(unknown_type), server("dods/0.0"), protocol("2.0")
    { }

    void operator()(const string &line)
    {
        string name, value;
        parse_mime_header(line, name, value);
        if (name == "content-description") {
            DBG2(cerr << name << ": " << value << endl);
            type = get_description_type(value);
        }
        // The second test (== "dods/0.0") tests if xopendap-server has already
        // been seen. If so, use that header in preference to the old
        // XDODS-Server header. jhrg 2/7/06
        else if (name == "xdods-server" && server == "dods/0.0") {
            DBG2(cerr << name << ": " << value << endl);
            server = value;
        }
        else if (name == "xopendap-server") {
            DBG2(cerr << name << ": " << value << endl);
            server = value;
        }
        else if (name == "xdap") {
            DBG2(cerr << name << ": " << value << endl);
            protocol = value;
        }
        else if (server == "dods/0.0" && name == "server") {
            DBG2(cerr << name << ": " << value << endl);
            server = value;
        }
       	else if (name == "location") {
       	    DBG2(cerr << name << ": " << value << endl);
       	    location = value;
        }
        else if (type == unknown_type && name == "content-type"
                 && line.find("text/html") != string::npos) {
            DBG2(cerr << name << ": text/html..." << endl);
            type = web_error;
        }
    }

    ObjectType get_object_type()
    {
        return type;
    }

    string get_server()
    {
        return server;
    }

    string get_protocol()
    {
        return protocol;
    }

    string get_location() {
	   return location;
    }
};

/** A libcurl callback function used to read response headers. Read headers,
    line by line, from ptr. The fourth param is really supposed to be a FILE
    *, but libcurl just holds the pointer and passes it to this function
    without using it itself. I use that to pass in a pointer to the
    HTTPConnect that initiated the HTTP request so that there's some place to
    dump the headers. Note that this function just saves the headers in a
    vector of strings. Later on the code (see fetch_url()) parses the headers
    special to the DAP.

    @param ptr A pointer to one line of character data; one header.
    @param size Size of each character (nominally one byte).
    @param nmemb Number of bytes.
    @param resp_hdrs A pointer to a vector<string>. Set in read_url.
    @return The number of bytes processed. Must be equal to size * nmemb or
    libcurl will report an error. */

static size_t
save_raw_http_headers(void *ptr, size_t size, size_t nmemb, void *resp_hdrs)
{
    DBG2(cerr << "Inside the header parser." << endl);
    vector<string> *hdrs = static_cast<vector<string> * >(resp_hdrs);

    // Grab the header, minus the trailing newline. Or \r\n pair.
    string complete_line;
    if (nmemb > 1 && *(static_cast<char*>(ptr) + size * (nmemb - 2)) == '\r')
        complete_line.assign(static_cast<char *>(ptr), size * (nmemb - 2));
    else
        complete_line.assign(static_cast<char *>(ptr), size * (nmemb - 1));

    // Store all non-empty headers that are not HTTP status codes
    if (complete_line != "" && complete_line.find("HTTP") == string::npos) {
        DBG(cerr << "Header line: " << complete_line << endl);
        hdrs->push_back(complete_line);
    }

    return size * nmemb;
}

/** A libcurl callback for debugging protocol issues. */
static int
curl_debug(CURL *, curl_infotype info, char *msg, size_t size, void  *)
{
    string message(msg, size);

    switch (info) {
    case CURLINFO_TEXT:
        cerr << "Text: " << message; break;
    case CURLINFO_HEADER_IN:
        cerr << "Header in: " << message; break;
    case CURLINFO_HEADER_OUT:
        cerr << "Header out: " << message; break;
    case CURLINFO_DATA_IN:
        cerr << "Data in: " << message; break;
    case CURLINFO_DATA_OUT:
        cerr << "Data out: " << message; break;
    case CURLINFO_END:
        cerr << "End: " << message; break;
#ifdef CURLINFO_SSL_DATA_IN
    case CURLINFO_SSL_DATA_IN:
        cerr << "SSL Data in: " << message; break;
#endif
#ifdef CURLINFO_SSL_DATA_OUT
    case CURLINFO_SSL_DATA_OUT:
        cerr << "SSL Data out: " << message; break;
#endif
    default:
        cerr << "Curl info: " << message; break;
    }
    return 0;
}

/** Initialize libcurl. Create a libcurl handle that can be used for all of
    the HTTP requests made through this instance. */

void
HTTPConnect::www_lib_init()
{
    d_curl = curl_easy_init();
    if (!d_curl)
        throw InternalErr(__FILE__, __LINE__, "Could not initialize libcurl.");

    // Now set options that will remain constant for the duration of this
    // CURL object.

    // Set the proxy host.
    if (!d_rcr->get_proxy_server_host().empty()) {
        DBG(cerr << "Setting up a proxy server." << endl);
        DBG(cerr << "Proxy host: " << d_rcr->get_proxy_server_host()
            << endl);
        DBG(cerr << "Proxy port: " << d_rcr->get_proxy_server_port()
            << endl);
        DBG(cerr << "Proxy pwd : " << d_rcr->get_proxy_server_userpw()
            << endl);
        curl_easy_setopt(d_curl, CURLOPT_PROXY,
                         d_rcr->get_proxy_server_host().c_str());
        curl_easy_setopt(d_curl, CURLOPT_PROXYPORT,
                         d_rcr->get_proxy_server_port());

	// As of 4/21/08 only NTLM, Digest and Basic work.
#ifdef CURLOPT_PROXYAUTH
        curl_easy_setopt(d_curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY);
#endif

        // Password might not be required. 06/21/04 jhrg
        if (!d_rcr->get_proxy_server_userpw().empty())
            curl_easy_setopt(d_curl, CURLOPT_PROXYUSERPWD,
                             d_rcr->get_proxy_server_userpw().c_str());
    }

    curl_easy_setopt(d_curl, CURLOPT_ERRORBUFFER, d_error_buffer);
    // We have to set FailOnError to false for any of the non-Basic
    // authentication schemes to work. 07/28/03 jhrg
    curl_easy_setopt(d_curl, CURLOPT_FAILONERROR, 0);

    // This means libcurl will use Basic, Digest, GSS Negotiate, or NTLM,
    // choosing the the 'safest' one supported by the server.
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
    if (!d_rcr->get_validate_ssl() == 0) {
        curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYHOST, 0);
    }

    // Look to see if cookies are turned on in the .dodsrc file. If so,
    // activate here. We honor 'session cookies' (cookies without an
    // expiration date) here so that session-base SSO systems will work as
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

class BuildHeaders : public unary_function<const string &, void>
{
    struct curl_slist *d_cl;

public:
    BuildHeaders() : d_cl(0)
    {}

    void operator()(const string &header)
    {
        DBG(cerr << "Adding '" << header.c_str() << "' to the header list."
            << endl);
        d_cl = curl_slist_append(d_cl, header.c_str());
    }

    struct curl_slist *get_headers()
    {
        return d_cl;
    }
};

/** Use libcurl to dereference a URL. Read the information referenced by \c
    url into the file pointed to by \c stream.

    @param url The URL to dereference.
    @param stream The destination for the data; the caller can assume that
    the body of the response can be found by reading from this pointer. A
    value/result parameter
    @param resp_hdrs Value/result parameter for the HTTP Response Headers.
    @param headers A pointer to a vector of HTTP request headers. Default is
    null. These headers will be appended to the list of default headers.
    @return The HTTP status code.
    @exception Error Thrown if libcurl encounters a problem; the libcurl
    error message is stuffed into the Error object. */

long
HTTPConnect::read_url(const string &url, FILE *stream,
                      vector<string> *resp_hdrs,
                      const vector<string> *headers)
{
    curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());

#ifdef WIN32
    //  See the curl documentation for CURLOPT_FILE (aka CURLOPT_WRITEDATA)
    //  and the CURLOPT_WRITEFUNCTION option.  Quote: "If you are using libcurl as
    //  a win32 DLL, you MUST use the CURLOPT_WRITEFUNCTION option if you set the
    //  CURLOPT_WRITEDATA option or you will experience crashes".  At the root of
    //  this issue is that one should not pass a FILE * to a windows DLL.  Close
    //  inspection of libcurl yields that their default write function when using
    //  the CURLOPT_WRITEDATA is just "fwrite".
    curl_easy_setopt(d_curl, CURLOPT_FILE, stream);
    curl_easy_setopt(d_curl, CURLOPT_WRITEFUNCTION, &fwrite);
#else
    curl_easy_setopt(d_curl, CURLOPT_FILE, stream);
#endif

    DBG(copy(d_request_headers.begin(), d_request_headers.end(),
             ostream_iterator<string>(cerr, "\n")));

    BuildHeaders req_hdrs;
    req_hdrs = for_each(d_request_headers.begin(), d_request_headers.end(),
                        req_hdrs);
    if (headers)
        req_hdrs = for_each(headers->begin(), headers->end(), req_hdrs);
    curl_easy_setopt(d_curl, CURLOPT_HTTPHEADER, req_hdrs.get_headers());

    // Turn off the proxy for this URL?
    bool temporary_proxy = false;
    if ((temporary_proxy = url_uses_no_proxy_for(url))) {
        DBG(cerr << "Suppress proxy for url: " << url << endl);
        curl_easy_setopt(d_curl, CURLOPT_PROXY, 0);
    }

    string::size_type at_sign = url.find('@');
    // Assume username:password present *and* assume it's an HTTP URL; it *is*
    // HTTPConnect, after all. 7 is position after "http://"; the second arg
    // to substr() is the sub string length.
    if (at_sign != url.npos)
        d_upstring = url.substr(7, at_sign - 7);

    if (!d_upstring.empty())
        curl_easy_setopt(d_curl, CURLOPT_USERPWD, d_upstring.c_str());

    // Pass save_raw_http_headers() a pointer to the vector<string> where the
    // response headers may be stored. Callers can use the resp_hdrs
    // value/result parameter to get the raw response header information .
    curl_easy_setopt(d_curl, CURLOPT_WRITEHEADER, resp_hdrs);

    // This is the call that causes curl to go and get the remote resource and "write it down"
    // utilizing the configuration state that has been previously conditioned by various perturbations
    // of calls to curl_easy_setopt().
    CURLcode res = curl_easy_perform(d_curl);

    // Free the header list and null the value in d_curl.
    curl_slist_free_all(req_hdrs.get_headers());
    curl_easy_setopt(d_curl, CURLOPT_HTTPHEADER, 0);

    // Reset the proxy?
    if (temporary_proxy && !d_rcr->get_proxy_server_host().empty())
        curl_easy_setopt(d_curl, CURLOPT_PROXY,
                         d_rcr->get_proxy_server_host().c_str());

    if (res != 0)
        throw Error(d_error_buffer);

    long status;
    res = curl_easy_getinfo(d_curl, CURLINFO_HTTP_CODE, &status);
    if (res != 0)
        throw Error(d_error_buffer);

    return status;
}

/** If the .dodsrc file gives a value for PROXY_FOR, return true if the
    current URL matches the regular expression. */

bool
HTTPConnect::url_uses_proxy_for(const string &url) throw()
{
    if (d_rcr->is_proxy_for_used()) {
        Regex host_regex(d_rcr->get_proxy_for_regexp().c_str());
        int index = 0, matchlen;
        return host_regex.search(url.c_str(), url.size(), matchlen, index) != -1;
    }

    return false;
}

/** If the NO_PROXY option is used in the dodsrc file, does this URL match
    the no proxy URL regex? */

bool
HTTPConnect::url_uses_no_proxy_for(const string &url) throw()
{
    return d_rcr->is_no_proxy_for_used()
           && url.find(d_rcr->get_no_proxy_for_host()) != string::npos;
}

// Public methods. Mostly...

/** Build a virtual connection to a remote data source that will be
    accessed using HTTP.

    @param rcr A pointer to the RCReader object which holds configuration
    file information to be used by this virtual connection. */

HTTPConnect::HTTPConnect(RCReader *rcr) : d_username(""), d_password(""),
  					  d_cookie_jar(""),
					  d_dap_client_protocol_major(2),
					  d_dap_client_protocol_minor(0)

{
    d_accept_deflate = rcr->get_deflate();
    d_rcr = rcr;

    // Load in the default headers to send with a request. The empty Pragma
    // headers overrides libcurl's default Pragma: no-cache header (which
    // will disable caching by Squid, et c.). The User-Agent header helps
    // make server logs more readable. 05/05/03 jhrg
    d_request_headers.push_back(string("Pragma:"));
    string user_agent = string("User-Agent: ") + string(CNAME)
                        + string("/") + string(CVER);
    d_request_headers.push_back(user_agent);
    if (d_accept_deflate)
        d_request_headers.push_back(string("Accept-Encoding: deflate, gzip, compress"));

    // HTTPCache::instance returns a valid ptr or 0.
    if (d_rcr->get_use_cache())
        d_http_cache = HTTPCache::instance(d_rcr->get_dods_cache_root(),true);
    else
        d_http_cache = 0;

    DBG2(cerr << "Cache object created (" << hex << d_http_cache << dec
         << ")" << endl);

    if (d_http_cache) {
        d_http_cache->set_cache_enabled(d_rcr->get_use_cache());
        d_http_cache->set_expire_ignored(d_rcr->get_ignore_expires() != 0);
        d_http_cache->set_max_size(d_rcr->get_max_cache_size());
        d_http_cache->set_max_entry_size(d_rcr->get_max_cached_obj());
        d_http_cache->set_default_expiration(d_rcr->get_default_expires());
        d_http_cache->set_always_validate(d_rcr->get_always_validate() != 0);
    }

    d_cookie_jar = rcr->get_cookie_jar();

    www_lib_init();  // This may throw either Error or InternalErr
}

HTTPConnect::~HTTPConnect()
{
    DBG2(cerr << "Entering the HTTPConnect dtor" << endl);

    curl_easy_cleanup(d_curl);

    DBG2(cerr << "Leaving the HTTPConnect dtor" << endl);
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

HTTPResponse *
HTTPConnect::fetch_url(const string &url)
{
#ifdef HTTP_TRACE
    cout << "GET " << url << " HTTP/1.0" << endl;
#endif

    HTTPResponse *stream;

    if (d_http_cache && d_http_cache->is_cache_enabled()) {
        stream = caching_fetch_url(url);
    }
    else {
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

    ParseHeader parser;

    parser = for_each(stream->get_headers()->begin(),
                      stream->get_headers()->end(), ParseHeader());

#ifdef HTTP_TRACE
    cout << endl << endl;
#endif

    // handle redirection case (2007-04-27, gaffigan@sfos.uaf.edu)
    if (parser.get_location() != "" &&
	    url.substr(0,url.find("?",0)).compare(parser.get_location().substr(0,url.find("?",0))) != 0) {
	delete stream;
        return fetch_url(parser.get_location());
    }

    stream->set_type(parser.get_object_type());
    stream->set_version(parser.get_server());
    stream->set_protocol(parser.get_protocol());

    return stream;
}

// Look around for a reasonable place to put a temporary file. Check first
// the value of the TMPDIR env var. If that does not yeild a path that's
// writable (as defined by access(..., W_OK|R_OK)) then look at P_tmpdir (as
// defined in stdio.h. If both come up empty, then use `./'.

// Change this to a version that either returns a string or an open file
// descriptor. Use information from https://buildsecurityin.us-cert.gov/
// (see open()) to make it more secure. Ideal solution: get deserialize()
// methods to read from a stream returned by libcurl, not from a temporary
// file. 9/21/07 jhrg Updated to use strings, so other misc changes. 3/22/11
static string
get_tempfile_template(const string &file_template)
{
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

    c= getenv("TMP");
    if (c && directory.match(c.c_str(), c.length()) && (access(c.c_str(), 6) == 0))
	goto valid_temp_directory;
#endif // USE_GETENV

    // The windows default
    c = "c:\tmp";
    if (c && directory.match(c.c_str(), c.length()) && (access(c.c_str(), 6) == 0))
	goto valid_temp_directory;

#else	// Unix/Linux/OSX has another...
    // white list for a directory
    Regex directory("[-a-zA-Z0-9_/]*");
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

#endif  // WIN32

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

string
get_temp_file(FILE *&stream) throw(InternalErr)
{
    string dods_temp = get_tempfile_template((string)"dodsXXXXXX");

    vector<char> pathname(dods_temp.length() + 1);

    strncpy(&pathname[0], dods_temp.c_str(), dods_temp.length());

    DBG(cerr << "pathanme: " << &pathname[0] << " (" << dods_temp.length() + 1 << ")" << endl);

    // Open truncated for update. NB: mkstemp() returns a file descriptor.
#if defined(WIN32) || defined(TEST_WIN32_TEMPS)
    stream = fopen(_mktemp(&pathname[0]), "w+b");
#else
    // Make sure that temp files are accessible only by the owner.
    umask(077);
    stream = fdopen(mkstemp(&pathname[0]), "w+");
#endif

    if (!stream) {
	throw InternalErr(__FILE__, __LINE__,
		"Failed to open a temporary file for the data values ("
		+ dods_temp + ")");
    }

    dods_temp = &pathname[0];
    return dods_temp;
}

/** Close the temporary file opened for read_url(). */
void
close_temp(FILE *s, const string &name)
{
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

HTTPResponse *
HTTPConnect::caching_fetch_url(const string &url)
{
    DBG(cerr << "Is this URL (" << url << ") in the cache?... ");

    vector<string> *headers = new vector<string>;
    string file_name;
    FILE *s = d_http_cache->get_cached_response(url, *headers, file_name);
    if (!s) {
        // url not in cache; get it and cache it
        DBGN(cerr << "no; getting response and caching." << endl);
        delete headers; headers = 0;
        time_t now = time(0);
        HTTPResponse *rs = plain_fetch_url(url);
        d_http_cache->cache_response(url, now, *(rs->get_headers()), rs->get_stream());

        return rs;
    }
    else { // url in cache
        DBGN(cerr << "yes... ");

        if (d_http_cache->is_url_valid(url)) { // url in cache and valid
            DBGN(cerr << "and it's valid; using cached response." << endl);
            HTTPCacheResponse *crs = new HTTPCacheResponse(s, 200, headers, file_name, d_http_cache);
            return crs;
        }
        else { // url in cache but not valid; validate
            DBGN(cerr << "but it's not valid; validating... ");

            d_http_cache->release_cached_response(s); // This closes 's'
            headers->clear();
            vector<string> cond_hdrs = d_http_cache->get_conditional_request_headers(url);
            FILE *body = 0;
            string dods_temp = get_temp_file(body);
            time_t now = time(0); // When was the request made (now).
            long http_status;

            try {
                http_status = read_url(url, body, /*resp_hdrs*/headers, &cond_hdrs);
                rewind(body);
            }
            catch (Error &e) {
                close_temp(body, dods_temp);
                delete headers;
                throw ;
            }

            switch (http_status) {
                case 200: { // New headers and new body
                    DBGN(cerr << "read a new response; caching." << endl);

                    d_http_cache->cache_response(url, now, /* *resp_hdrs*/*headers, body);
                    HTTPResponse *rs = new HTTPResponse(body, http_status, /*resp_hdrs*/headers, dods_temp);

                    return rs;
                }

                case 304: { // Just new headers, use cached body
                    DBGN(cerr << "cached response valid; updating." << endl);

                    close_temp(body, dods_temp);
                    d_http_cache->update_response(url, now, /* *resp_hdrs*/ *headers);
                    string file_name;
                    FILE *hs = d_http_cache->get_cached_response(url, *headers, file_name);
                    HTTPCacheResponse *crs = new HTTPCacheResponse(hs, 304, headers, file_name, d_http_cache);
                    return crs;
                }

                default: { // Oops.
                    close_temp(body, dods_temp);
                    if (http_status >= 400) {
                	delete headers; headers = 0;
                        string msg = "Error while reading the URL: ";
                        msg += url;
                        msg
                        += ".\nThe OPeNDAP server returned the following message:\n";
                        msg += http_status_to_string(http_status);
                        throw Error(msg);
                    }
                    else {
                	delete headers; headers = 0;
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

HTTPResponse *
HTTPConnect::plain_fetch_url(const string &url)
{
    DBG(cerr << "Getting URL: " << url << endl);
    FILE *stream = 0;
    string dods_temp = get_temp_file(stream);
    vector<string> *resp_hdrs = new vector<string>;

    int status = -1;
    try {
        status = read_url(url, stream, resp_hdrs); // Throws Error.
        if (status >= 400) {
        	// delete resp_hdrs; resp_hdrs = 0;
            string msg = "Error while reading the URL: ";
            msg += url;
            msg += ".\nThe OPeNDAP server returned the following message:\n";
            msg += http_status_to_string(status);
            throw Error(msg);
        }
    }

    catch (Error &e) {
    	delete resp_hdrs;
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
void
HTTPConnect::set_accept_deflate(bool deflate)
{
    d_accept_deflate = deflate;

    if (d_accept_deflate) {
        if (find(d_request_headers.begin(), d_request_headers.end(),
                 "Accept-Encoding: deflate, gzip, compress") == d_request_headers.end())
            d_request_headers.push_back(string("Accept-Encoding: deflate, gzip, compress"));
        DBG(copy(d_request_headers.begin(), d_request_headers.end(),
                 ostream_iterator<string>(cerr, "\n")));
    }
    else {
        vector<string>::iterator i;
        i = remove_if(d_request_headers.begin(), d_request_headers.end(),
                      bind2nd(equal_to<string>(),
                              string("Accept-Encoding: deflate, gzip, compress")));
        d_request_headers.erase(i, d_request_headers.end());
    }
}

/** Look for a certain header */
class HeaderMatch : public unary_function<const string &, bool> {
    const string &d_header;
    public:
        HeaderMatch(const string &header) : d_header(header) {}
        bool operator()(const string &arg) { return arg.find(d_header) == 0; }
};

/** Set the <em>xdap_accept</em> property/HTTP-header. This sets the value
    of the DAP which the client advertises to servers that it understands.
    The information (client protocol major and minor versions) are recorded
    in the instance and the information is sent to servers using the
    XDAP-Accept HTTP request header.

    @param major The dap client major protocol version
    @param minor The dap client minor protocol version */
void
HTTPConnect::set_xdap_protocol(int major, int minor)
{
    // Look for, and remove if one exists, an XDAP-Accept header
    vector<string>::iterator i;
    i = find_if(d_request_headers.begin(), d_request_headers.end(),
                HeaderMatch("XDAP-Accept:"));
    if (i != d_request_headers.end())
        d_request_headers.erase(i);

    // Record and add the new header value
    d_dap_client_protocol_major = major;
    d_dap_client_protocol_minor = minor;
    ostringstream xdap_accept;
    xdap_accept << "XDAP-Accept: " << major << "." << minor;

    d_request_headers.push_back(xdap_accept.str());

    DBG(copy(d_request_headers.begin(), d_request_headers.end(),
             ostream_iterator<string>(cerr, "\n")));
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

void
HTTPConnect::set_credentials(const string &u, const string &p)
{
    if (u.empty())
        return;

    // Store the credentials locally.
    d_username = u;
    d_password = p;

    d_upstring = u + ":" + p;
}

} // namespace libdap
