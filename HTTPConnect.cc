
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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
    { "$Id: HTTPConnect.cc,v 1.4 2003/02/20 23:13:52 jimg Exp $" };

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

// #define DODS_DEBUG 1
#include "debug.h"
#include "Regex.h"
#include "HTTPConnect.h"
#include "HTTPCache.h"
#include "RCReader.h"

using std::cerr;
using std::endl;
using std::string;
using std::istrstream;
using std::vector;

#ifdef WIN32
using std::istringstream;
#endif

// These two global variables are not MT-Safe, but I'm leaving them as is
// because they are used only for debugging (set them in a debugger like gdb
// or ddd). They are not static because I *believe* that debuggers cannot
// access static variables. 08/07/02 jhrg

// This is a convenience used when running this software under a debugger.
// Set the global 'keep_temps' to 1 to keep all the temporary files used to
// store libwww retrieved documents. 6/17/2002 jhrg
int keep_temps = 0;

// Set this to 1 to turn on libcurl's verbose mode (for debugging).
int www_trace = 0;

/** Functor to parse the headers in the d_headers field. After the headers
    have been read off the wire and written into the d_headers field, scan
    them ans set special fields for certain headers special to the DAP. */

struct ParseHeader : public unary_function<const string &, void> {
    HTTPConnect *d_http;

    ParseHeader(HTTPConnect *http) : d_http(http) {}

    void operator()(const string &header) {
#ifdef WIN32
	istringstream line(header);
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
	    d_http->d_type = get_type(value);
	}
	else if (name == "xdods-server:") {
	    string value; 
	    line >> value;
	    downcase(value);
	    DBG2(cout << name << ": " << value << endl);
	    d_http->d_server = value;
	}
	else if (d_http->d_server == "dods/0.0" && name == "server:") {
	    string value; 
	    line >> value;
	    downcase(value);
	    DBG2(cout << name << ": " << value << endl);
	    d_http->d_server = value;
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
    : d_username(""), d_password(""), d_is_response_present(false),
      d_type(unknown_type), d_server("dods/0.0"), // d_output(0), 
      d_cached_response(false)
{
    d_accept_deflate = rcr->get_deflate();
    d_rcr = rcr;

    // HTTPCache::instance returns a valid ptr or 0.
    d_http_cache = HTTPCache::instance(d_rcr->get_dods_cache_root(), true);
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

#if defined(WIN32) || defined(TEST_WIN32_TEMPS)
    //  Get rid of any intermediate files
    std::vector < string >::const_iterator i;
    for (i = d_tfname.begin(); i != d_tfname.end(); i++)
	remove((*i).c_str());
#endif

    curl_easy_cleanup(d_curl);

#if 0
    close_output();
#endif

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

FILE *
HTTPConnect::fetch_url(const string &url) throw(Error, InternalErr)
{
    // Clear/Reset values from previous requests.
    d_type = unknown_type;
    d_headers.clear();
    d_is_response_present = false;
    FILE *stream;

    if (d_http_cache->is_cache_enabled())
	stream = caching_fetch_url(url);
    else
	stream = plain_fetch_url(url);

    for_each(d_headers.begin(), d_headers.end(), ParseHeader(this));

    d_is_response_present = true;

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
    // On WIN32 unlink removes the file regardless of other open descriptors;
    // on Unix the file exists until no process holds an open descriptor to
    // it. 
#if defined(WIN32) || defined(TEST_WIN32_TEMPS)
    stream = fopen(_mktemp(dods_temp), "w+b");
    if (stream)
	d_tfname.push_back(string(dods_temp));
#else
    stream = fdopen(mkstemp(dods_temp), "w+");
#endif

    if (!stream)
	throw InternalErr("I/O Error: Failed to open a temporary file for the data values.");

    return dods_temp;
}

/** Simple function that handles freeing the temporary file created using the
    get_temp_File() method. This function, under UNIX, also removes the name
    from the file system. If the caller has the file open, it can still use
    that descriptor/FILE pointer. This trick keeps us from having to remember
    to delete the temp file. The file is deleted even if the process calls
    abort(). We can't do this under Win32.

    @param dods_temp The name of the temporary file to delete. This pointer
    \e should have been returned by get_temp_file(). */

static inline void
delete_temp_file(char *dods_temp)
{
#ifndef WIN32
    if (!keep_temps)
	unlink(dods_temp);
    else
	cerr << "Temporary file for Data: " << dods_temp << endl;
#endif
    delete[] dods_temp;
}

/** Dereference a URL. This method looks first in the HTTP cache to see if a
    cached response may be used. It may get the response from the cache, it
    may validate a response in the cache and/or update the response from the
    cache or it may get a new response from the network. In any of those
    cases, the information returned by dereferencing the URL will be stored
    in the cache. 
    
    A private method.

    @param url The URL to dereference.
    @return A pointer to the open stream.
    @exception Error Thrown if the URL could not be dereferenced.
    @exception InternalErr Thrown if a temporary file to hold the response
    could not be opened. */

FILE *
HTTPConnect::caching_fetch_url(const string &url) throw(Error, InternalErr)
{
    FILE *stream;
    DBG(cerr << "Is this URL (" << url << ") in the cache?... ");
    if (d_http_cache->is_url_in_cache(url)) { // url in cache
	DBG(cerr << "yes... ");
	if (d_http_cache->is_url_valid(url)) { // url in cache and valid
	    DBG(cerr << "and it's valid; using cached response." << endl);
#if 0
	    close_output() ;
#endif
	    stream = d_http_cache->get_cached_response(url, d_headers);
	    d_cached_response = true;
	}
	else {			// url in cache but not valid; validate
	    DBG(cerr << "but it's not valid; validating... ");
	    vector<string> cond_hdrs 
		= d_http_cache->get_conditional_request_headers(url);
	    FILE *body = 0;
	    char *dods_temp = get_temp_file(body);
	    time_t now = time(0); // When was the request made (now).
	    long http_status;

	    try {
		http_status = read_url(url, body, &cond_hdrs);
	    }
	    catch(Error &e) {
		int res = fclose(body);
		if( res ) {
		    DBG(cerr << "HTTPConnect::caching_fetch_url - Failed to close " << (void *)body << endl ;) ;
		}
		body = 0 ;
		delete_temp_file(dods_temp);
		throw;
	    }

	    switch (http_status) {
	      case 200: {		// New headers and new body
		    DBG(cerr << "read a brand new response; caching." << endl);
		    d_http_cache->cache_response(url, now, d_headers, body);
#if 0
		    close_output() ;
#endif
		    stream = body;
		    d_cached_response = false;
		}
		break;

	      case 304: {		// Just new headers, use cached body
		    DBG(cerr << "cached response valid; updating." << endl);
		    int res = fclose(body);
		    if( res ) {
			DBG(cerr << "HTTPConnect::caching_fetch_url - Failed to close " << (void *)body << endl ;) ;
		    }
		    body = 0 ;
		    delete_temp_file(dods_temp);
		    d_http_cache->update_response(url, now, d_headers);
#if 0
		    close_output() ;
#endif
		    stream = d_http_cache->get_cached_response_body(url);
		    d_cached_response = true;
		}
		break;

	      default: {		// Oops.
		    int res = fclose(body);
		    if( res ) {
			DBG(cerr << "HTTPConnect::caching_fetch_url - Failed to close " << (void *)body << endl ;) ;
		    }
		    body = 0 ;
		    delete_temp_file(dods_temp);
		    throw Error("Bad response from the HTTP server: This implemenation of the DAP does not understand how to handle an HTTP status response of " + long_to_string(http_status));
		}
		break;
	    }
	}
    }
    else {			// url not in cache; get it and cache it
	DBG(cerr << "no; getting response and caching." << endl);
	time_t now = time(0);
	stream = plain_fetch_url(url);
	d_http_cache->cache_response(url, now, d_headers, stream);
	d_cached_response = false;
    }

    return stream;
}

/** Dereference a URL and load its body into a temporary file. Use
    HTTPConnect::output() to get a pointer to that temporary file. This
    method ignores the HTTP cache.
    
    A private method.

    @param url The URL to dereference.
    @return A pointer to the open stream.
    @exception Error Thrown if the URL could not be dereferenced.
    @exception InternalErr Thrown if a temporary file to hold the response
    could not be opened. */

FILE *
HTTPConnect::plain_fetch_url(const string &url) throw(Error, InternalErr)
{
    DBG(cerr << "Getting URL: " << url << endl);
    FILE *stream = 0;
    char *dods_temp = get_temp_file(stream);

    try {
	read_url(url, stream);	// Throws Error.
    }
    catch(...) {
	int res = fclose(stream);
	if( res ) {
	    DBG(cerr << "HTTPConnect::plain_fetch_url - Failed to close " << (void *)stream << endl ;) ;
	}
	stream = 0 ;
	delete_temp_file(dods_temp);
	throw;
    }

    rewind(stream);		// This may break on linux 2.2; in that case,
				// close and reopen the file. However, this
				// bug might have been a problem in libwww
    delete_temp_file(dods_temp);

#if 0
    close_output() ;
    d_output = stream;
#endif
    return stream;
    d_cached_response = false;
}

/** True if fetch_url() has been called and a response has been received. 
    @deprecated */

bool
HTTPConnect::is_response_present()
{
    return d_is_response_present;
}

/** Get the HTTP response headers. 
    @return A vector of strings, one string for each header. 
    @exeception IntenalErr The is_response_present() property is false. */

std::vector<string>
HTTPConnect::get_response_headers() throw(InternalErr)
{
    if (!d_is_response_present)
	throw InternalErr(__FILE__, __LINE__, "Caller tried to access invalid response information");

    return d_headers;
}

#if 0
/** Returns a file pointer which can be used to read the data
    fetched from a URL.

    Note that occasionally this may be directed to #stdout#.  If this
    is the case, users should avoid closing it.

    @memo Access the information contained in this Connect instance.
    @see Connect::fetch_url
    @return A <code>(FILE *)</code> indicating a file containing the data
    received from a dereferenced URL.  
    @exeception IntenalErr The is_response_present() property is false. */

FILE *
HTTPConnect::output() throw(InternalErr)
{
    if (!d_is_response_present)
	throw InternalErr(__FILE__, __LINE__, "Caller tried to access response invalid response information");

    return d_output;
}
#endif

#if 0
/** Close the output stream of the Connect object. This closes the FILE
    pointer returned by #output()#. In addition, it also deletes the internal
    XDR stream object, although users should not have to know about
    that\ldots Note that calling this method resets (clears) the <code>response
    valid</code> property.

    @memo Close the object's output stream if it is not NULL or STDOUT. */

void 
HTTPConnect::close_output() throw(InternalErr)
{
    d_is_response_present = false;

    if (d_output) {
	if (ferror(d_output))
	    throw InternalErr(__FILE__, __LINE__,
			      "Error detected in the data stream.");
	int res = fclose(d_output);
	if( res ) {
	    DBG(cerr << "HTTPConnect::close_output - Failed to close " << (void *)d_output << endl ;) ;
	}
	if (d_cached_response)
	    d_http_cache->release_cached_response(d_output);

	d_output = 0;
	d_cached_response = false;
    }
}
#endif

/** During the parse of the message headers returned from the dereferenced
    URL, the object type is set. Use this function to read that type
    information. This will be valid {\it before} the return object is
    completely parsed so it can be used to decide which parser to call to
    read the data remaining in the input stream.

    The object types are Data, DAS, DDS, Error, and undefined.

    @memo What type is the most recent object sent from the server?

    @return The type of the object.
    @see ObjectType 
    @exeception IntenalErr The is_response_present() property is false. */

ObjectType 
HTTPConnect::type() throw(InternalErr)
{
    if (!d_is_response_present)
	throw InternalErr(__FILE__, __LINE__, "Caller tried to access response invalid response information");

    return d_type;
}

/** Returns a string containing the version of DODS used by the server. 
    @exeception IntenalErr The is_response_present() property is false. */

string 
HTTPConnect::server_version() throw(InternalErr)
{
    if (!d_is_response_present)
	throw InternalErr(__FILE__, __LINE__, "Caller tried to access response invalid response information");

    return d_server;
}

/** Set the credentials for responding to challenges while dereferencing
    URLs. Alternatively, these can be embedded in the URL. This method
    provide a way for clients of HTTPConnect to get credentials from users
    (say using a popup dialog) and not have to hack the URL to pass that
    information to libcurl. 
	
    @param u The username.
    @param p The password. 
    @see extract_auth_info() */

void 
HTTPConnect::set_credentials(string u, string p) throw(InternalErr)
{
    // Store the credentials locally.
    d_username = u;
    d_password = p;

    d_upstring = u + ":" + p;

    if (curl_easy_setopt(d_curl, CURLOPT_USERPWD, d_upstring.c_str()) != 0)
	throw InternalErr(__FILE__, __LINE__, 
	      "Could not set the username and password string using libcurl.");
}

// $Log: HTTPConnect.cc,v $
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
