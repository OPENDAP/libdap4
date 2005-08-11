
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

#ifndef http_response_h
#define http_response_h

#include <stdio.h>

#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>

#ifndef response_h
#include "Response.h"
#endif

#ifndef _debug_h
#include "debug.h"
#endif

extern int dods_keep_temps;	// defined in HTTPConnect.cc
extern void close_temp(FILE *s, const string &name);

/** Encapsulate an http response. Instead of directly returning the FILE
    pointer from which a response is read and vector of headers, return an
    instance of this object. */
class HTTPResponse : public Response {
private:
    vector<string> *d_headers;	// Response headers
    string d_file;		// Temp file that holds response body

protected:
    /** @name Suppressed default methods */
    //@{
    HTTPResponse() {}
    HTTPResponse(const HTTPResponse &rs) : Response(rs) {}
    HTTPResponse &operator=(const HTTPResponse &) {
	throw InternalErr(__FILE__, __LINE__, "Unimplemented assignment");
    }
    //@}

public:
    /** Build an HTTPResponse object. An instance of this class is used to
	return an HTTP response (body and headers). If the response is really
	from a remote server, the current HTTP code stores the body in a
	temporary file and the headers in a vector<string> object. This class
	will delete those resources when its destructor is called. If the
	response does not have a temporary file that needs to be deleted (say
	it actually comes from a local cache or was read directly into
	memory), the temp file should be set to "".

	@param s FILE * to the response. Read the response body from this
	stream. 
	@param h Response headers. This class will delete the pointer when
	the instance that contains it is destroyed.
	@param temp_file Name a the temporary file that holds the response
	body; this file is deleted when this instance is deleted. */
    HTTPResponse(FILE *s, vector<string> *h, const string &temp_file) 
	: Response(s), d_headers(h), d_file(temp_file) {
	DBG(cerr << "Headers: " << endl);
	DBGN(copy(d_headers->begin(), d_headers->end(),
		  ostream_iterator<string>(cerr, "\n")));
	DBGN(cerr << "end of headers." << endl);
    }

    /** When an instance is destroyed, free the temporary resources: the
	temp_file and headers are deleted. If the tmp file name is "", it is
	not deleted. */
    virtual ~HTTPResponse() {
	DBG(cerr << "Freeing HTTPConnect resources (" + d_file + ")... ");

	if (!dods_keep_temps && !d_file.empty()) {
            close_temp(get_stream(), d_file);
            set_stream(0);
        }

	delete d_headers; d_headers = 0;

	DBGN(cerr << endl);
    }
    /** @name Accessors */
    //@{
    virtual vector<string> *get_headers() const { return d_headers; }
    //@}

    /** @name Mutators */
    //@{
    virtual void set_headers(vector<string> *h) { d_headers = h; }
    //@}
};

// $Log: HTTPResponse.h,v $
// Revision 1.4  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.2.2.3  2004/08/24 20:03:15  jimg
// Changed the way HTTPResponse deletes the temporary files created to hold
// HTTP responses. Before this was done without using HTTPConnect's
// close_temp() function. Instead, ~HTTPResponse() called unlink() on the
// filename and then ~Response() called fclose on the FILE *. I think this
// breaks on win32. THe simplest solution was to make ~HTTPResponse() use
// the close_temp() function. I also had to edit the ~Response() method to
// check that d_stream was not null before calling fclose() there.
//
// Revision 1.3  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.2.2.2  2003/10/10 23:06:35  jimg
// Added some instrumentation which helped track down bug 672.
//
// Revision 1.2.2.1  2003/05/06 06:44:15  jimg
// Modified HTTPConnect so that the response headers are no longer a class
// member. This cleans up the class interface and paves the way for using
// the multi interface of libcurl. That'll have to wait for another day...
//
// Revision 1.2  2003/03/04 21:39:52  jimg
// Added dods_keep_temps global. This is handy for debugging.
//
// Revision 1.1  2003/03/04 05:57:40  jimg
// Added.
//

#endif // http_response_h
