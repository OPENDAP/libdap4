
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
 
#ifndef _httpconnect_h
#define _httpconnect_h

#ifndef __POWERPC__
#ifdef __GNUG__
// #pragma interface
#endif
#endif

#include <stdio.h>

#include <string>

#include <curl/curl.h>
#include <curl/types.h>
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
#if defined(_MSC_VER) && (_MSC_VER == 1200)  //  VC++ 6.0 only
using std::vector<string>;
#else
using std::vector;
#endif

// This is defined in Connect.cc. ObjectType should be its own class.
// 6/17/2002 jhrg
extern ObjectType get_type(const string &value);

/** Use the CURL library to dereference a HTTP URL. Scan the response for
    headers used by the DAP 2.0 and extract their values. The body of the
    response is made available by a FILE pointer.

    @todo Change the way this class returns information so that the headers
    and the stream (aka FILE pointer) are returned using an object. Design
    this object so that its destructor closes the stream (this will prevent
    resource leaks). It will also obviate the need for the (now broken)
    is_response_present() predicate.

    @author jhrg */

class HTTPConnect {
private:
    CURL *d_curl;
    RCReader *d_rcr;
    HTTPCache *d_http_cache;

    char d_error_buffer[CURL_ERROR_SIZE]; // A human-readable message.

    bool d_accept_deflate;

    string d_username;		// extracted from URL
    string d_password;		// extracted from URL
    string d_upstring;		// used to pass info into curl

    vector<string> d_request_headers; // Request headers

    void www_lib_init() throw(Error, InternalErr);
    long read_url(const string &url, FILE *stream, vector<string> *resp_hdrs,
		  const vector<string> *headers = 0) throw(Error);
    // string get_temp_file(FILE *&stream) throw(InternalErr);
    HTTPResponse *plain_fetch_url(const string &url) 
	throw(Error, InternalErr);
    HTTPResponse *caching_fetch_url(const string &url) 
	throw(Error, InternalErr);

    bool url_uses_proxy_for(const string &url) throw();
    bool url_uses_no_proxy_for(const string &url) throw();

    void extract_auth_info(string &url);

    // bool cond_fetch_url(const string &url, const vector<string> &headers);

    friend size_t save_raw_http_header(void *ptr, size_t size, size_t nmemb, 
				       void *http_connect);
    friend class HTTPConnectTest;
    friend class ParseHeader;

protected:
    /** @name Suppress default methods
	These methods are not supported and are implmented here as protected
	methods to suppress the C++-supplied default versions (which will
	break this object). */
    //@{
    HTTPConnect() { }
    HTTPConnect(const HTTPConnect &copy_from) { }
    HTTPConnect &operator=(const HTTPConnect &rhs) { 
	throw InternalErr(__FILE__, __LINE__, "Unimplemented assignment");
    }
    //@}

public:
    HTTPConnect(RCReader *rcr) throw(Error, InternalErr);

    virtual ~HTTPConnect();

    void set_credentials(const string &u, const string &p) throw(InternalErr);
    void set_accept_deflate(bool defalte);

    /** Set the state of the HTTP cache. By default, the HTTP cache is
	enabled of disabled using the value of the \c USE_CACHE property in
	the \c .dodsrc file. Use this method to set the state from within a
	program. 
	@param enabled True to use the cache, False to disable. */
    void set_cache_enabled(bool enabled) { 
	if (d_http_cache)
	    d_http_cache->set_cache_enabled(enabled);
    }

    /** Return the current state of the HTTP cache. */
    bool is_cache_enabled() { 
	return (d_http_cache) ? d_http_cache->is_cache_enabled() : false; 
    }

    HTTPResponse *fetch_url(const string &url) throw(Error, InternalErr);
};

// $Log: HTTPConnect.h,v $
// Revision 1.13  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.9.2.10  2004/08/24 20:03:15  jimg
// Changed the way HTTPResponse deletes the temporary files created to hold
// HTTP responses. Before this was done without using HTTPConnect's
// close_temp() function. Instead, ~HTTPResponse() called unlink() on the
// filename and then ~Response() called fclose on the FILE *. I think this
// breaks on win32. THe simplest solution was to make ~HTTPResponse() use
// the close_temp() function. I also had to edit the ~Response() method to
// check that d_stream was not null before calling fclose() there.
//
// Revision 1.12  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.9.2.9  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.11  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.9.2.8  2004/02/04 00:05:11  jimg
// Memory errors: I've fixed a number of memory errors (leaks, references)
// found using valgrind. Many remain. I need to come up with a systematic
// way of running the tests under valgrind.
//
// Revision 1.9.2.7  2004/01/17 13:37:50  rmorris
// Mod's to account for differences in usage statements containing template
// reference between MS VC++ 6.0 and MS VC++ 7.0.
//
// Revision 1.10  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.9.2.6  2003/09/30 16:36:38  jimg
// I removed some old code and fixed a typo where ParseHeaders was declared
// as a struct here and a class in the .cc file.
//
// Revision 1.9.2.5 2003/09/30 16:29:45 jimg Fixed bug #670. As a result of
// fixing the bug where DODS_CONF was ignored, I changed the way the HTTP
// cache was used. HTTPConnect was always initializing the cache, so it was
// OK to assume that the d_http_cache member was a valid pointer. One result
// of fixing the DODS_CONF bug was that the HTTPCache instance was not always
// initialized. The server helper programs asciival and www_int assume that
// they must disable the cache, so they call Connect::set_cacne_enable(false)
// which then calls the matching method here. I added a test so that
// d_http_cache is only used if it's non-null.
//
// Revision 1.9.2.4  2003/06/23 11:49:18  rmorris
// The // #pragma interface directive to GCC makes the dynamic typing
// functionality go completely haywire under OS X on the PowerPC. We can't
// use that directive on that platform and it was ifdef'd out for that case.
//
// Revision 1.9.2.3  2003/06/14 21:59:04  rmorris
// Change to some interim code (in transition) in regards to parseHeaders
// defined as both class and struct - to get it to compile under win32.
// UNIX compiler allows, VC++ does not.
//
// Revision 1.9.2.2  2003/05/06 06:44:15  jimg
// Modified HTTPConnect so that the response headers are no longer a class
// member. This cleans up the class interface and paves the way for using
// the multi interface of libcurl. That'll have to wait for another day...
//
// Revision 1.9.2.1  2003/05/05 19:46:17  jimg
// Added d_request_headers. See HTTPConnect.cc for an explanation.
//
// Revision 1.9  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.8  2003/03/04 21:40:40  jimg
// Removed code in #if 0 ... #endif.
//
// Revision 1.7  2003/03/04 17:28:18  jimg
// Switched to Response objects. Removed unneeded methods. The Response objects
// now control the release of resources such as deleting temporary files, et c.
//
// Revision 1.6  2003/02/27 23:36:10  jimg
// Added set_accept_deflate() method.
//
// Revision 1.5  2003/02/27 18:21:48  jimg
// I removed old code (code inside #if 0 ... #endif) and changed the private
// definitions of HTTPConnect(), ... to protected definitions. These methods
// are present to prevent C++ defaults from being called. I made them
// protected so that specializations of this class can hide their own
// empty constructor, et cetera. When these are private a subclass cannot do
// that.
//
// Revision 1.4  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
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
// Revision 1.1.2.9  2002/12/27 00:55:25  jimg
// Added a note about future modifications that the interface of this class
// won't depend on its methods being called in a particular order.
//
// Revision 1.1.2.8  2002/12/24 00:22:59  jimg
// Removed output() and close_output(); fetch_url() now returns a FILE *.
//
// Revision 1.1.2.7  2002/10/18 01:51:42  jimg
// Added methods that support using the HTTP 1.1 cache in HTTPCache.cc,h. Added
// a pointer to an instance of HTTPCache.
//
// Revision 1.1.2.6  2002/09/14 03:40:54  jimg
// Added get_response_headers() method. This method provides a way to access
// the headers included in a response, specifically so that they can be
// cached. Added is_response_present() which returns true when fetch_url()
// has been called and a response is present. Also added tests which cause
// the four methods that access information from the response (output, et c.)
// to throw an InternalErr if they are called and is_response_present() would
// return false.
//
// Revision 1.1.2.5  2002/08/06 22:08:56  jimg
// Added a string (d_upstring) so that password information can be passed to
// libcurl using something that's persistent for the duration of the
// `connection' without using local static storage in the set_credentials
// method (which causes problems with MT-safety).
//
// Revision 1.1.2.4  2002/07/06 20:19:01  jimg
// Added two methods that make twiddling the proxy stuff on the fly easier.
// These look at a specific URL to help decide if the current proxy
// configuration should be changed for *this particular* url. I've also removed
// a number of methods that didn't belong here.
//
// Revision 1.1.2.3  2002/06/20 03:18:48  jimg
// Fixes and modifications to the Connect and HTTPConnect classes. Neither
// of these two classes is complete, but they should compile and their
// basic functions should work.
//
// Revision 1.1.2.2  2002/06/18 23:00:25  jimg
// Moved ObjectType to its own header file. This enables other code to include
// the definition without dragging all the HTTPConnect stuff along.
//
// Revision 1.1.2.1  2002/06/18 21:58:08  jimg
// The interface Connect uses to interact with libwww. Needs work...
//

#endif // _httpconnect_h
