
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 2002
// Please first read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

#ifndef _httpconnect_h
#define _httpconnect_h

#ifdef __GNUG__
#pragma interface
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

#ifndef _util_h
#include "util.h"
#endif

using std::string;
#ifdef WIN32
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

#ifdef WIN32
    // We need to keep the different filenames associated with d_output
    // (over time) around under win32 because an unlink() at time 'now'
    // doesn't delete a file at some time (now + n) as it does under
    // UNIX. Unix will delete the file when the last process using the
    // file closes it - win32 will not. Under win32, we count on the
    // Connect destructor using d_tfname to remove such intermediate
    // files..
    vector<string> d_tfname;			
#endif

    char d_error_buffer[CURL_ERROR_SIZE]; // A human-readable message.

    // Set these before calling fetch_url();
    bool d_accept_deflate;

    string d_username;		// extracted from URL
    string d_password;		// extracted from URL
    string d_upstring;		// used to pass info into curl

    bool d_is_response_present;	// Is there something to look at?

    // These four members are valid only after a fetch_url() call.
    vector<string> d_headers;	// Response headers
    ObjectType d_type;		// What type of object is in the stream?
    string d_server;		// Server's version string.
#if 0
    FILE *d_output;		// Data reside in this temp file.
#endif
    bool d_cached_response;	// True if response was from cache.

    // These methods are not supported and are implmented here as private
    // methods to suppress the C++-supplied default versions (which will
    // break this object). 6/14/2002 jhrg
    HTTPConnect() { }
    HTTPConnect(const HTTPConnect &copy_from) { }
    HTTPConnect &operator=(const HTTPConnect &rhs) { 
	throw InternalErr(__FILE__, __LINE__, "Call to unsupported op=");
    }

    void www_lib_init() throw(Error, InternalErr);
    long read_url(const string &url, FILE *stream,
		  const vector<string> *headers = 0) throw(Error);
    char *get_temp_file(FILE *&stream) throw(InternalErr);
    FILE *plain_fetch_url(const string &url) throw(Error, InternalErr);
    FILE *caching_fetch_url(const string &url) throw(Error, InternalErr);

    bool url_uses_proxy_for(const string &url) throw();
    bool url_uses_no_proxy_for(const string &url) throw();

    void extract_auth_info(string &url);

    bool cond_fetch_url(const string &url, const vector<string> &headers);

    friend size_t save_raw_http_header(void *ptr, size_t size, size_t nmemb, 
				       void *http_connect);
    friend class HTTPConnectTest;
    friend struct ParseHeader;

public:
    HTTPConnect(RCReader *rcr) throw(Error, InternalErr);

    virtual ~HTTPConnect();

    void set_credentials(string u, string p) throw(InternalErr);

    FILE *fetch_url(const string &url) throw(Error, InternalErr);
    bool is_response_present();

    vector<string> get_response_headers() throw(InternalErr);
    ObjectType type() throw(InternalErr);
    string server_version() throw(InternalErr);

#if 0
    FILE *output() throw(InternalErr);
    void close_output() throw(InternalErr);
#endif
};

// $Log: HTTPConnect.h,v $
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
