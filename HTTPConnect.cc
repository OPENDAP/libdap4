
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
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used =
    { "$Id: HTTPConnect.cc,v 1.10 2003/03/13 23:51:31 jimg Exp $" };

#include <stdio.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <strstream>
#include <functional>
#include <algorithm>
#ifdef WIN32
#include <sstream>
#endif

#include "debug.h"
#include "Regex.h"
#include "HTTPConnect.h"
#include "HTTPCache.h"
#include "RCReader.h"
#include "HTTPResponse.h"
#include "HTTPCacheResponse.h"

using std::cerr;
using std::endl;
using std::string;
using std::istrstream;
using std::vector;

// This global variable is not MT-Safe, but I'm leaving it as is because it
// is used only for debugging (set them in a debugger like gdb or ddd). They
// are not static because I *believe* that many debuggers cannot access
// static variables. 08/07/02 jhrg

// Set this to 1 to turn on libcurl's verbose mode (for debugging).
int www_trace = 0;

/** Functor to parse the headers in the d_headers field. After the headers
    have been read off the wire and written into the d_headers field, scan
    them ans set special fields for certain headers special to the DAP. */

struct ParseHeader : public unary_function<const string &, void> {
    ObjectType type;		// What type of object is in the stream?
    string server;		// Server's version string.

    ParseHeader() :type(unknown_type), server("dods/0.0") { }

    void operator()(const string &header) {
#ifdef WIN32
	std::istringstream line(header);
#else
	istrstream line(header.c_str());
#endif
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
    @param http_connect A pointer to the HTTPConnect object that initiated
    the request.
    @return The number of bytes processed. Must be equal to size * nmemb or
    libcurl will report an error. */

size_t 
save_raw_http_header(void *ptr, size_t size, size_t nmemb, void *http_connect)
{
    DBG2(cerr << "Inside the header parser." << endl);
    HTTPConnect *http = static_cast<HTTPConnect *>(http_connect);

    // Grab the header, minus the trailing newline.
    string complete_line(static_cast<char *>(ptr), size * nmemb - 1);
    // Store all non-empty headers that are not HTTP status codes
    if (complete_line != "" && complete_line.find("HTTP") == string::npos) {
	DBG2(cerr << "Header line: " << complete_line << endl);
	http->d_headers.push_back(complete_line);
    }

    return size * nmemb;
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

    // We must set the proxy host (it's really a host, if a URL is given, the
    // protocol part is ignored by libcurl).
    if (d_rcr->get_proxy_server_host_url() != "") {
	string proxy = d_rcr->get_proxy_server_host_url();
	curl_easy_setopt(d_curl, CURLOPT_PROXY, proxy.c_str());
	// If the port number is not part of the proxy server host, then use
	// the protocol to divine a default port number. Support http, https
	// and ftp for now. If protocol is not one of those, throw and error.
	Regex find_port("^.*:[0-9]+$");
	int index=0, matchlen;
	if (find_port.search(proxy.c_str(), proxy.length(), matchlen, index)
	    == -1) {
	    string protocol = d_rcr->get_proxy_server_protocol();
	    if (protocol == "http")
		curl_easy_setopt(d_curl, CURLOPT_PROXYPORT, (long)80);
	    else if (protocol == "https")
		curl_easy_setopt(d_curl, CURLOPT_PROXYPORT, (long)443);
	    else if (protocol == "ftp")
		curl_easy_setopt(d_curl, CURLOPT_PROXYPORT, (long)21);
	    else 
		throw Error("Proxies for the protocol '" + protocol 
			    + "' are not supported.");
	}
    }

    curl_easy_setopt(d_curl, CURLOPT_ERRORBUFFER, d_error_buffer);
    curl_easy_setopt(d_curl, CURLOPT_FAILONERROR, 1);

    curl_easy_setopt(d_curl, CURLOPT_NOPROGRESS, 1);
    curl_easy_setopt(d_curl, CURLOPT_MUTE, 1);

    curl_easy_setopt(d_curl, CURLOPT_HEADERFUNCTION, save_raw_http_header);
    // By passing `this' to the writeheader function we can load values read
    // from the headers into this object.
    curl_easy_setopt(d_curl, CURLOPT_WRITEHEADER, this);

    if (www_trace)
	curl_easy_setopt(d_curl, CURLOPT_VERBOSE, 1);
}

/** Functor to add a single string to a curl_slist. This is used to transfer
    a list of headers from a vector<string> object to a curl_slist. */

struct BuildHeaders : public unary_function<const string &, void> {
    struct curl_slist *d_cl;

    BuildHeaders() : d_cl(0) {}

    void operator()(const string &header) {
	d_cl = curl_slist_append(d_cl, header.c_str());
    }
};

/** Use libcurl to dereference a URL. Read the information referenced by \c
    url into the file pointed to by \c stream.

    @param url The URL to dereference.
    @param stream The distination for the data; the caller can assume that
    the body of the response can be found by reading from this pointer.
    @param headers A pointer to a vector of HTTP request headers. Default is
    null.
    @return The HTTP status code.
    @exception Error Thrown if libcurl encounters a problem; the libcurl
    error message is stuffed into the Error object. */

long
HTTPConnect::read_url(const string &url, FILE *stream, 
		      const vector<string> *headers) throw(Error)
{
    curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(d_curl, CURLOPT_FILE, stream);

    struct curl_slist *header_list = 0;
    if (headers) {
	BuildHeaders curl_hdrs;
	curl_hdrs = for_each(headers->begin(), headers->end(), curl_hdrs);
	header_list = curl_hdrs.d_cl; // save to later delete.
	curl_easy_setopt(d_curl, CURLOPT_HTTPHEADER, header_list);
    }

    bool temporary_proxy = false;
    if ((temporary_proxy = url_uses_proxy_for(url))) {
	// Set the new proxy.
	curl_easy_setopt(d_curl, CURLOPT_PROXY,
			 d_rcr->get_proxy_for_proxy_host_url().c_str());
    }
    else if ((temporary_proxy = url_uses_no_proxy_for(url))) {
	// Turn off the proxy for this URL.
	curl_easy_setopt(d_curl, CURLOPT_PROXY, 0);	
    }

    CURLcode res = curl_easy_perform(d_curl);

    if (header_list)
	curl_slist_free_all(header_list);

    // Reset the proxy?
    if (temporary_proxy && d_rcr->get_proxy_server_host_url() != "")
	curl_easy_setopt(d_curl, CURLOPT_PROXY,
			 d_rcr->get_proxy_server_host_url().c_str());
	
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

    @name The URL to the remote data source.
    @param rcr A pointer to the RCReader object which holds configuration
    file information to be used by this virtual connection. */

HTTPConnect::HTTPConnect(RCReader *rcr) throw(Error, InternalErr)
    : d_username(""), d_password("")
{
    d_accept_deflate = rcr->get_deflate();
    d_rcr = rcr;

    // HTTPCache::instance returns a valid ptr or 0.
    d_http_cache = HTTPCache::instance(d_rcr->get_dods_cache_root(), true);
    DBG2(cerr << "Cache object created (" << hex << d_http_cache << dec
	 << ")" << endl);

    if (d_http_cache) {
	d_http_cache->set_cache_enabled(d_rcr->get_use_cache());
	d_http_cache->set_expire_ignored(d_rcr->get_ignore_expires());
	d_http_cache->set_max_size(d_rcr->get_max_cache_size());
	d_http_cache->set_max_entry_size(d_rcr->get_max_cached_obj());
	d_http_cache->set_default_expiration(d_rcr->get_default_expires());
	d_http_cache->set_always_validate(d_rcr->get_always_validate());
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

Response *
HTTPConnect::fetch_url(const string &url) throw(Error, InternalErr)
{
    // Clear/Reset values from previous requests.
    d_headers.clear();
    Response *stream;

    if (d_http_cache->is_cache_enabled())
	stream = caching_fetch_url(url);
    else
	stream = plain_fetch_url(url);
    
    ParseHeader parser;
    parser = for_each(d_headers.begin(), d_headers.end(), ParseHeader());

    stream->set_type(parser.type);
    stream->set_version(parser.server);
    stream->set_headers(d_headers);
    
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

    Delete the returned char* using delete[].

    A private method.

    @param stream A value-result parameter; the open file descriptor is
    returned via this parameter.
    @return The name of the temporary file
    @exception InternalErr thrown if the FILE* could not be opened. */

char *
HTTPConnect::get_temp_file(FILE *&stream) throw(InternalErr)
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

    return dods_temp;
}

/** Close the temporary file opened for read_url(). */
inline static void
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

    @param url The URL to dereference.
    @return A pointer to the open stream.
    @exception Error Thrown if the URL could not be dereferenced.
    @exception InternalErr Thrown if a temporary file to hold the response
    could not be opened. */

Response *
HTTPConnect::caching_fetch_url(const string &url) throw(Error, InternalErr)
{
    DBG(cerr << "Is this URL (" << url << ") in the cache?... ");

    if (d_http_cache->is_url_in_cache(url)) { // url in cache
	DBGN(cerr << "yes... ");

	if (d_http_cache->is_url_valid(url)) { // url in cache and valid
	    DBGN(cerr << "and it's valid; using cached response." << endl);

	    FILE *s = d_http_cache->get_cached_response(url, d_headers);
	    HTTPCacheResponse *crs = new HTTPCacheResponse(s, d_http_cache);
	    
	    return crs;
	}
	else {			// url in cache but not valid; validate
	    DBGN(cerr << "but it's not valid; validating... ");

	    vector<string> cond_hdrs 
		= d_http_cache->get_conditional_request_headers(url);
	    FILE *body = 0;
	    string dods_temp = get_temp_file(body);
	    time_t now = time(0); // When was the request made (now).
	    long http_status;

	    try {
		http_status = read_url(url, body, &cond_hdrs);
		rewind(body);
	    }
	    catch(Error &e) {
		close_temp(body, dods_temp);
		throw;
	    }

	    switch (http_status) {
	      case 200: {		// New headers and new body
		    DBGN(cerr << "read a new response; caching." << endl);

		    d_http_cache->cache_response(url, now, d_headers, body);
		    HTTPResponse *rs = new HTTPResponse(body, dods_temp);

		    return rs;
		}
		break;

	      case 304: {		// Just new headers, use cached body
		    DBGN(cerr << "cached response valid; updating." << endl);

		    close_temp(body, dods_temp);
		    d_http_cache->update_response(url, now, d_headers);
		    FILE *s = d_http_cache->get_cached_response_body(url);
		    HTTPCacheResponse *crs = new HTTPCacheResponse(s, d_http_cache);
		    return crs;
		}
		break;

	      default: {		// Oops.
		  close_temp(body, dods_temp);

		  throw Error("Bad response from the HTTP server: This implemenation of the DAP does not understand how to handle an HTTP status response of " + long_to_string(http_status));
		}
		break;
	    }
	}
    }
    else {			// url not in cache; get it and cache it
	DBGN(cerr << "no; getting response and caching." << endl);
	time_t now = time(0);
	Response *rs = plain_fetch_url(url);
	d_http_cache->cache_response(url, now, d_headers, rs->get_stream());
	
	return rs;
    }

    throw InternalErr(__FILE__, __LINE__, "Unexpected cache response.");
}


/** Dereference a URL and load its body into a temporary file. This
    method ignores the HTTP cache.

    @todo make this return a Response or HTTPResponse.
    
    A private method.

    @param url The URL to dereference.
    @return A pointer to the open stream.
    @exception Error Thrown if the URL could not be dereferenced.
    @exception InternalErr Thrown if a temporary file to hold the response
    could not be opened. */

Response *
HTTPConnect::plain_fetch_url(const string &url) throw(Error, InternalErr)
{
    DBG(cerr << "Getting URL: " << url << endl);
    FILE *stream = 0;
    string dods_temp = get_temp_file(stream);

    try {
	read_url(url, stream);	// Throws Error.
    }
    catch(...) {
	close_temp(stream, dods_temp);
	throw;
    }

    rewind(stream);

    return new HTTPResponse(stream, dods_temp);
}

/** Set the <c>accept deflate</c> property. If true, the DAP client announces
    to a server that it can accept responses compressed using the \c deflate
    algorithm. This property is automatically set using a value from the
    <c>.dodsrc</c> configuration file. This method provides a way to override
    that behavior. NB: If the configuration file is not present or does not
    include a value for this property, it is set to \c false.

    @param deflate True sets the <c>accept deflate</c> property, False clears
    it. */
void
HTTPConnect::set_accept_deflate(bool deflate)
{
    d_accept_deflate = deflate;
}

/** Set the credentials for responding to challenges while dereferencing
    URLs. Alternatively, these can be embedded in the URL. This method
    provides a way for clients of HTTPConnect to get credentials from users
    (say using a pop up dialog) and to not hack the URL to pass that
    information to libcurl. Note that the 'credentials in the URL' scheme \i
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

    if (curl_easy_setopt(d_curl, CURLOPT_USERPWD, d_upstring.c_str()) != 0)
	throw InternalErr(__FILE__, __LINE__, 
	      "Could not set the username and password string using libcurl.");
}

// $Log: HTTPConnect.cc,v $
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
