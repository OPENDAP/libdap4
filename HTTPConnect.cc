
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#ifdef __GNUG__
// #pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used =
    { "$Id: HTTPConnect.cc,v 1.19 2005/01/28 17:25:12 jimg Exp $" };

#include <stdio.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iterator>

#include "debug.h"
#include "Regex.h"
#include "HTTPConnect.h"
#include "HTTPCache.h"
#include "RCReader.h"
#include "HTTPResponse.h"
#include "HTTPCacheResponse.h"

using namespace std;

// These global variables are not MT-Safe, but I'm leaving them as is because
// they are used only for debugging (set them in a debugger like gdb or ddd).
// They are not static because I *believe* that many debuggers cannot access
// static variables. 08/07/02 jhrg

// Set this to 1 to turn on libcurl's verbose mode (for debugging).
int www_trace = 0;

// Keep the temporary files; useful for debugging.
int dods_keep_temps = 0;

#define CLIENT_ERR_MIN 400
#define CLIENT_ERR_MAX 417
static char *http_client_errors[CLIENT_ERR_MAX - CLIENT_ERR_MIN +1] = {
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
static char *http_server_errors[SERVER_ERR_MAX - SERVER_ERR_MIN +1] = {
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
	return string(
"Unknown Error: This indicates a problem with libdap++.\n\
Please report this to support@unidata.ucar.edu.");
}

/** Functor to parse the headers in the d_headers field. After the headers
    have been read off the wire and written into the d_headers field, scan
    them and set special fields for certain headers special to the DAP. */

class ParseHeader : public unary_function<const string &, void> {
    ObjectType type;		// What type of object is in the stream?
    string server;		// Server's version string.

public:
    ParseHeader() :type(unknown_type), server("dods/0.0") { }

    void operator()(const string &header) {
	std::istringstream line(header);

	string name;
	line >> name;
	downcase(name);
	if (name == "content-description:") {
	    string value; 
	    line >> value;
	    downcase(value);
	    DBG2(cout << name << ": " << value << endl);
	    type = get_type(value);
	}
	else if (name == "xdods-server:") {
	    string value; 
	    line >> value;
	    downcase(value);
	    DBG2(cout << name << ": " << value << endl);
	    server = value;
	}
	else if (server == "dods/0.0" && name == "server:") {
	    string value; 
	    line >> value;
	    downcase(value);
	    DBG2(cout << name << ": " << value << endl);
	    server = value;
	}
	else if (type == unknown_type && name == "content-type:" 
		 && line.str().find("text/html") != string::npos) {
	    DBG2(cout << name << ": text/html..." << endl);
	    type = web_error;
	}
    }

    ObjectType get_object_type() {
	return type;
    }
    
    string get_server() {
	return server;
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
    vector<string> *hdrs = static_cast<vector<string> *>(resp_hdrs);

    // Grab the header, minus the trailing newline.
    string complete_line(static_cast<char *>(ptr), size * nmemb - 1);

    // Store all non-empty headers that are not HTTP status codes
    if (complete_line != "" && complete_line.find("HTTP") == string::npos) {
	DBG(cerr << "Header line: " << complete_line << endl);
	hdrs->push_back(complete_line);
    }

    return size * nmemb;
}

/** A libcurl callback for debugging protocol issues. */
static int
curl_debug(CURL *curl, curl_infotype info, char *msg, size_t size, void  *data)
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
    }
    return 0;
}

/** Initialize libcurl. Create a libcurl handle that can be used for all of
    the HTTP requests made through this instance. */ 

void 
HTTPConnect::www_lib_init() throw(Error, InternalErr)
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

    if (www_trace) {
	cerr << "Curl version: " << curl_version() << endl;
	curl_easy_setopt(d_curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(d_curl, CURLOPT_DEBUGFUNCTION, curl_debug);
    }
	
}

/** Functor to add a single string to a curl_slist. This is used to transfer
    a list of headers from a vector<string> object to a curl_slist. */

class BuildHeaders : public unary_function<const string &, void> {
    struct curl_slist *d_cl;

public:
    BuildHeaders() : d_cl(0) {}

    void operator()(const string &header) {
	DBG(cerr << "Adding '" << header.c_str() << "' to the header list." 
	    << endl);
	d_cl = curl_slist_append(d_cl, header.c_str());
    }

    struct curl_slist *get_headers() {
	return d_cl;
    }
};

/** Use libcurl to dereference a URL. Read the information referenced by \c
    url into the file pointed to by \c stream.

    @param url The URL to dereference.
    @param stream The distination for the data; the caller can assume that
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
		      const vector<string> *headers) throw(Error)
{
    curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(d_curl, CURLOPT_FILE, stream);

    DBG(copy(d_request_headers.begin(), d_request_headers.end(),
	     ostream_iterator<string>(cerr, "\n")));

    BuildHeaders req_hdrs;
    req_hdrs = for_each(d_request_headers.begin(), d_request_headers.end(),
			 req_hdrs);
    if (headers)
	req_hdrs = for_each(headers->begin(), headers->end(), req_hdrs);
    curl_easy_setopt(d_curl, CURLOPT_HTTPHEADER, req_hdrs.get_headers());

    if (d_accept_deflate)
	curl_easy_setopt(d_curl, CURLOPT_ENCODING, "deflate");

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
	d_upstring = url.substr(7, at_sign-7);

    if (!d_upstring.empty())
	curl_easy_setopt(d_curl, CURLOPT_USERPWD, d_upstring.c_str());

    // Pass save_raw_http_headers() a pointer to the vector<string> where the
    // response headers may be stored. Callers can use the resp_hdrs
    // value/result parameter to get the raw response header information .
    curl_easy_setopt(d_curl, CURLOPT_WRITEHEADER, resp_hdrs);

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
	int index=0, matchlen;
	return host_regex.search(url.c_str(), url.size(), matchlen, index)
	    != -1;
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

HTTPConnect::HTTPConnect(RCReader *rcr) throw(Error, InternalErr)
    : d_username(""), d_password("")
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
	d_request_headers.push_back(string("Accept-Encoding: deflate"));

    // HTTPCache::instance returns a valid ptr or 0.
    if (d_rcr->get_use_cache())
	d_http_cache = HTTPCache::instance(d_rcr->get_dods_cache_root(),
					   false);
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
    
    www_lib_init();		// This may throw either Error or InternalErr
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
HTTPConnect::fetch_url(const string &url) throw(Error, InternalErr)
{
    HTTPResponse *stream;

    if (d_http_cache && d_http_cache->is_cache_enabled())
	stream = caching_fetch_url(url);
    else
	stream = plain_fetch_url(url);
    
    ParseHeader parser;

    parser = for_each(stream->get_headers()->begin(), 
		      stream->get_headers()->end(), ParseHeader());

    stream->set_type(parser.get_object_type());
    stream->set_version(parser.get_server());
    
    return stream;
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
    // get_tempfile_template() uses new, must call delete
    char *dods_temp = get_tempfile_template("dodsXXXXXX");

    // Open truncated for update. NB: mkstemp() returns a file descriptor.
#if defined(WIN32) || defined(TEST_WIN32_TEMPS)
    stream = fopen(_mktemp(dods_temp), "w+b");
#else
    stream = fdopen(mkstemp(dods_temp), "w+");
#endif

    if (!stream)
	throw InternalErr("I/O Error: Failed to open a temporary file for the data values.");

    string dods_temp_s = dods_temp;
    delete[] dods_temp; dods_temp = 0;

    return dods_temp_s;
}

/** Close the temporary file opened for read_url(). */
void
close_temp(FILE *s, const string &name)
{
    int res = fclose(s);
    if (res)
	DBG(cerr << "Failed to close " << (void *)s << endl);
	
    unlink(name.c_str());
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
HTTPConnect::caching_fetch_url(const string &url) throw(Error, InternalErr)
{
    DBG(cerr << "Is this URL (" << url << ") in the cache?... ");

    if (d_http_cache->is_url_in_cache(url)) { // url in cache
	DBGN(cerr << "yes... ");

	if (d_http_cache->is_url_valid(url)) { // url in cache and valid
	    DBGN(cerr << "and it's valid; using cached response." << endl);

	    vector<string> *headers = new vector<string>;;
	    FILE *s = d_http_cache->get_cached_response(url, *headers);
	    HTTPCacheResponse *crs = new HTTPCacheResponse(s, headers, d_http_cache);
	    
	    return crs;
	}
	else {			// url in cache but not valid; validate
	    DBGN(cerr << "but it's not valid; validating... ");

	    // *** auto_ptr??? resp_hdrs not deleted! 10/10/03 jhrg 
	    vector<string> *resp_hdrs = new vector<string>;
	    vector<string> cond_hdrs 
		= d_http_cache->get_conditional_request_headers(url);
	    FILE *body = 0;
	    string dods_temp = get_temp_file(body);
	    time_t now = time(0); // When was the request made (now).
	    long http_status;

	    try {
		http_status = read_url(url, body, resp_hdrs, &cond_hdrs);
		rewind(body);
	    }
	    catch(Error &e) {
		close_temp(body, dods_temp);
		throw;
	    }

	    switch (http_status) {
	      case 200: {		// New headers and new body
		    DBGN(cerr << "read a new response; caching." << endl);

		    d_http_cache->cache_response(url, now, *resp_hdrs, body);
		    HTTPResponse *rs = new HTTPResponse(body, resp_hdrs,
							dods_temp);

		    return rs;
		}
		break;

	      case 304: {		// Just new headers, use cached body
		    DBGN(cerr << "cached response valid; updating." << endl);

		    close_temp(body, dods_temp);
		    d_http_cache->update_response(url, now, *resp_hdrs);

		    vector<string> *headers = new vector<string>;;
		    FILE *s = d_http_cache->get_cached_response(url, *headers);
		    HTTPCacheResponse *crs 
			= new HTTPCacheResponse(s, headers, d_http_cache);
#if 0
		    // Problem: When a 304 comes back it sometimes (?) has
		    // only a Date header. We need all the headers, so after
		    // updating the cache with the single Date header, get
		    // _all_ the headers and return them. See bug 672.
		    // 10/10/03 jhrg
		    FILE *s = d_http_cache->get_cached_response_body(url);
		    HTTPCacheResponse *crs 
			= new HTTPCacheResponse(s, resp_hdrs, d_http_cache);
#endif
		    return crs;
		}
		break;

	      default: {		// Oops.
		  close_temp(body, dods_temp);
		  if (http_status >= 400) {
		      string msg = "Error while reading the URL: ";
		      msg += url;
		      msg += ".\nThe OPeNDAP server returned the following message:\n";
		      msg += http_status_to_string(http_status);
		      throw Error(msg);
		  }
		  else 
		      throw InternalErr(__FILE__, __LINE__,
"Bad response from the HTTP server: " + long_to_string(http_status));
		}
		break;
	    }
	}
    }
    else {			// url not in cache; get it and cache it
	DBGN(cerr << "no; getting response and caching." << endl);
	time_t now = time(0);
	HTTPResponse *rs = plain_fetch_url(url);
	d_http_cache->cache_response(url, now, *(rs->get_headers()), 
				     rs->get_stream());
	
	return rs;
    }

    throw InternalErr(__FILE__, __LINE__, "Unexpected cache response.");
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
HTTPConnect::plain_fetch_url(const string &url) throw(Error, InternalErr)
{
    DBG(cerr << "Getting URL: " << url << endl);
    FILE *stream = 0;
    string dods_temp = get_temp_file(stream);
    vector<string> *resp_hdrs = new vector<string>;

    try {
	int status = read_url(url, stream, resp_hdrs);	// Throws Error.
	if (status >= 400) {
	    string msg = "Error while reading the URL: ";
	    msg += url;
	    msg += ".\nThe OPeNDAP server returned the following message:\n";
	    msg += http_status_to_string(status);
	    throw Error(msg);
	}
    }

    catch(Error &e) {
	close_temp(stream, dods_temp);
	throw e;
    }

    rewind(stream);

    return new HTTPResponse(stream, resp_hdrs, dods_temp);
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
		 "Accept-Encoding: deflate") == d_request_headers.end())
	    d_request_headers.push_back(string("Accept-Encoding: deflate"));
	DBG(copy(d_request_headers.begin(), d_request_headers.end(),
		 ostream_iterator<string>(cerr, "\n")));
    }
    else {
	vector<string>::iterator i;
	i = remove_if(d_request_headers.begin(), d_request_headers.end(),
		      bind2nd(equal_to<string>(), 
			      string("Accept-Encoding: deflate")));
	d_request_headers.erase(i, d_request_headers.end());
    }
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
    throw(InternalErr)
{
    if (u.empty())
	return;

    // Store the credentials locally.
    d_username = u;
    d_password = p;

    d_upstring = u + ":" + p;
}

// $Log: HTTPConnect.cc,v $
// Revision 1.19  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.14.2.17  2004/08/24 20:03:15  jimg
// Changed the way HTTPResponse deletes the temporary files created to hold
// HTTP responses. Before this was done without using HTTPConnect's
// close_temp() function. Instead, ~HTTPResponse() called unlink() on the
// filename and then ~Response() called fclose on the FILE *. I think this
// breaks on win32. THe simplest solution was to make ~HTTPResponse() use
// the close_temp() function. I also had to edit the ~Response() method to
// check that d_stream was not null before calling fclose() there.
//
// Revision 1.18  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.14.2.16  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.14.2.15  2004/06/21 20:45:16  jimg
// Changes for proxy use: Proxy passwords are now supported. The proxy_for
// feature has been disabled.
//
// Revision 1.17  2004/04/12 16:21:56  jimg
// Removed obsolete CURLOPT_MUTE.
//
// Revision 1.14.2.14  2004/04/08 23:46:12  jimg
// Fixes for the build on the Alpha/Tru64 platform. I removed the call to
// curl's MUTE option since it's deprecated by the latest rev of that
// library. I added regex-0.12 to the code and modified configure.in,
// Makefile.in and GNU/Regex.cc to use it in place of librx (which won't
// build on this plaform). Doing this gets rid of a library dependency, too.
//
// Revision 1.16  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.14.2.13  2004/02/11 21:52:23  jimg
// Added two char* arrays which hold HTTP 1.1 error messages. These are used to
// provide better feedback when the OPeNDAP server's httpd returns an error
// (i.e., when the request does not even make it into the Dispatch code). I also
// tidied up some calls to delete by setting 'x' in delete x to null after the
// call to delete.
//
// Revision 1.14.2.12  2004/02/04 00:05:11  jimg
// Memory errors: I've fixed a number of memory errors (leaks, references)
// found using valgrind. Many remain. I need to come up with a systematic
// way of running the tests under valgrind.
//
// Revision 1.15  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.14.2.11  2003/10/10 23:13:12  jimg
// Fix for bug 672: When a cached response needs to be validated and the origin
// server returns a 304 response (i.e., use what you've got) only the Date
// header is sent with the 304 response. This code was using the
// HTTPCache::get_cahced_body() method and the header from the 304 response.
// This meant that Content-Description and server/DAP-protocol version headers
// were missing in the Response object returned by caching_fetch_url(). This
// caused an Error to be thrown in Connect (because we just modified it to
// require the Content-Description information). I fixed this by using the
// get_cached_response() method which reads the body _and_ headers from the
// cache.
//
// Revision 1.14.2.10  2003/10/03 16:21:17  jimg
// Minor changes: save_raw_http_headers is now static and some comments were
// fixed.
//
// Revision 1.14.2.9  2003/09/08 18:54:41  jimg
// Now we check to see that USE_CACHE is set (1) in .dodsrc before trying to
// create an instance of HTTPCache. This makes sense since HTTPCache will create
// teh cache directory structure regardless of the value of USE_CACHE. If
// USE_CACHE is clear (0), the the pointer to the HTTPCache instance is zero.
//
// Revision 1.14.2.8  2003/09/06 22:37:50  jimg
// Updated the documentation.
//
// Revision 1.14.2.7  2003/08/18 00:33:14  rmorris
// Win32-related cleanup.
//
// Revision 1.14.2.6  2003/07/29 01:46:33  jimg
// Now supports digest authentication.
//
// Revision 1.14.2.5  2003/07/26 02:08:18  jimg
// Added curl options to suppress signals and supply passwords to proxy servers.
//
// Revision 1.14.2.4  2003/07/16 04:20:55  jimg
// Fixed some documentation.
//
// Revision 1.14.2.3  2003/05/06 06:44:15  jimg
// Modified HTTPConnect so that the response headers are no longer a class
// member. This cleans up the class interface and paves the way for using
// the multi interface of libcurl. That'll have to wait for another day...
//
// Revision 1.14.2.2  2003/05/05 21:45:30  jimg
// Added support for the http://user:passwd@machine... convention.
//
// Revision 1.14.2.1  2003/05/05 19:45:25  jimg
// Fixed a problem where libcurl sent a Pragma: no-cache request header. This
// caused third-party caches to not cache requests. I fixed this by forcing
// libcurl to send an empty Pragma: header (suggested by Benno Blumenthal and
// the libcurl list). At the same time I repaired the set_accept_deflate method
// so that it worked (it didn't but nobody noticed...) and changed the class so
// that *any* headers present in the new filed d_request_headers are sent
// whenever read_url() is run. I also now set the User-Agent request header to
// a value that announces who we are.
//
// Revision 1.14  2003/05/02 16:22:52  jimg
// Minor fixes: Fixed a spelling mistake and removed #if 1 ... #endif. Mostly I
// wanted this code in for the 3.4 branch.
//
// Revision 1.13  2003/05/01 23:37:01  jimg
// Changed some of struct Functors to classes.
//
// Revision 1.12  2003/05/01 23:25:14  jimg
// Fixed a bug in the code that handles conditional requests. Once a conditional
// request was made, the curl_slist was freed but the HTTPHEADER option (set
// with curl_easy_setopt()) was not cleared. Subsequent calls to
// curl_easy_perform() would try to use the freed slist with predictably bad
// results. Also removed strstream code in place of Rob's stringstream
// replacement for Win32.
//
// Revision 1.11  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.3.2.4  2003/04/18 03:22:58  jimg
// Undid a typo that made it into CVS...
//
// Revision 1.3.2.3  2003/04/18 03:21:17  jimg
// I removed some old code (some of which has been seriously hacked already on
// the trunk) and changed the catch part of a try-catch block so that it only
// catches and re-throws the objects listed in the throw() declaration. Using
// catch(...) and re-throwing when exceptions have been explicitly declared
// aborts.
//
// Revision 1.3.2.2  2003/04/09 20:36:21  jimg
// Changed the catch(...) clauses to ones that explicitly name the objects. By
// just using catch(...) with a method that declares the types of exceptions it
// throws causes an abort if the catch(...) re-throws the exception.
// I also removed some old code that called output(), et cetera.
//
// Revision 1.10  2003/03/13 23:51:31  jimg
// Added a DBG2 statement to track down a bug. Useful when you cannot use a
// debugger.
//
// Revision 1.9  2003/03/04 21:41:44  jimg
// Removed code in #if 0 ... #endif. This was mostly WIN32 code that I don't
// think we need anymore.
//
// Revision 1.8  2003/03/04 17:28:37  jimg
// Switched to Response objects. Removed unneeded methods. The Response objects
// now control the release of resources such as deleting temporary files, et c.
//
// Revision 1.7  2003/02/27 23:36:10  jimg
// Added set_accept_deflate() method.
//
// Revision 1.6  2003/02/27 08:32:51  jimg
// Removed old code.
//
// Revision 1.5  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.4  2003/02/20 23:13:52  jimg
// Fixed the comment for getch_url.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.14  2002/12/27 00:56:24  jimg
// is_response_present() is now deprecated.
//
// Revision 1.1.2.13  2002/12/24 00:22:00  jimg
// Removed the output() and close_output() methods from the interface.
// The fetch_url() method now returns a FILE * instead of forcing clients to
// call the output() method.
//
// Revision 1.1.2.12  2002/12/05 20:36:19  pwest
// Corrected problems with IteratorAdapter code, making methods non-inline,
// creating source files and template instantiation file. Cleaned up file
// descriptors and memory management problems. Corrected problem in Connect
// where the xdr source was not being cleaned up or a new one created when a
// new file was opened for reading.
//
// Revision 1.1.2.11  2002/11/21 21:24:17  pwest
// memory leak cleanup and file descriptor cleanup
//
// Revision 1.1.2.10  2002/10/18 22:48:46  jimg
// Resolved conflicts with Rob's changes
//
// Revision 1.1.2.9  2002/09/22 14:31:56  rmorris
// Minor mod's to get the code to go through VC++.
//
// Revision 1.1.2.8  2002/09/14 03:40:32  jimg
// Added get_response_headers() method. This method provides a way to access the
// headers included in a response, specifically so that they can be cached.
// Added is_response_present() which returns true when fetch_url() has been
// called and a response is present. Also added tests which cause the four
// methods that access information from the response (output, et c.) to throw an
// InternalErr if they are called and is_response_present() would return false.
//
// Revision 1.1.2.7  2002/08/22 21:23:23  jimg
// Fixes for the Win32 Build made at ESRI by Vlad Plenchoy and myslef.
//
// Revision 1.1.2.6  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.1.2.5  2002/08/06 22:09:49  jimg
// Removed old code about cache control.
// Fixed set_credentials() so that it is MT-safe.
//
// Revision 1.1.2.4  2002/07/06 20:57:08  jimg
// *** empty log message ***
//
// Revision 1.1.2.3  2002/07/06 20:08:08  jimg
// Added support for proxies. Fixed the deflate stuff so that it matches the new
// `sense' of the _dods_deflate field. Compression still does not work since
// libcurl doesn't handle it yet.
//
// Revision 1.1.2.2  2002/06/20 03:18:48  jimg
// Fixes and modifications to the Connect and HTTPConnect classes. Neither
// of these two classes is complete, but they should compile and their
// basic functions should work.
//
// Revision 1.1.2.1  2002/06/18 21:57:24  jimg
// Created. This is a DODS interface to a HTTP library. This version uses
// libwww. A lot of the methods here should be removed...
//
