
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

#ifndef http_response_h
#define http_response_h

#include <unistd.h>

#include <cstdio>

#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

#include "Response.h"
#include "util.h"
#include "debug.h"

namespace libdap
{

// defined in HTTPConnect.cc
extern int dods_keep_temps;
extern void close_temp(FILE *s, const string &name);

/** Encapsulate an http response. Instead of directly returning the FILE
    pointer from which a response is read and vector of headers, return an
    instance of this object.

    @todo Maybe refactor so that the header parsing code is here and not in
    HTTPConnect? */
class HTTPResponse : public Response
{
private:
    std::vector<std::string> *d_headers; // Response headers
    std::string d_file;  // Temp file that holds response body

protected:
    /** @name Suppressed default methods */
    //@{
    HTTPResponse();
    HTTPResponse(const HTTPResponse &rs);
    HTTPResponse &operator=(const HTTPResponse &);
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
    @param status The HTTP response status code.
    @param h Response headers. This class will delete the pointer when
    the instance that contains it is destroyed.
    @param temp_file Name a the temporary file that holds the response
    body; this file is deleted when this instance is deleted. */
    HTTPResponse(FILE *s, int status, std::vector<std::string> *h, const std::string &temp_file)
            : Response(s, status), d_headers(h), d_file(temp_file)
    {
        DBG(cerr << "Headers: " << endl);
        DBGN(copy(d_headers->begin(), d_headers->end(),
                  ostream_iterator<string>(cerr, "\n")));
        DBGN(cerr << "end of headers." << endl);
    }

    /**
     * @brief Build a HTTPResponse using a cpp fstream
     * When working with DAP4 responses, use C++ streams for I/0.
     * @todo Decide on how the temp files fit into DAP4
     * @param s
     * @param status
     * @param h
     * @param temp_file
     */
    HTTPResponse(std::fstream *s, int status, std::vector<std::string> *h, const std::string &temp_file)
            : Response(s, status), d_headers(h), d_file(temp_file)
    {
        DBG(cerr << "Headers: " << endl);
        DBGN(copy(d_headers->begin(), d_headers->end(),
                  ostream_iterator<string>(cerr, "\n")));
        DBGN(cerr << "end of headers." << endl);
    }

    /** When an instance is destroyed, free the temporary resources: the
    temp_file and headers are deleted. If the tmp file name is "", it is
    not deleted. */
    virtual ~HTTPResponse()
    {
        DBG(cerr << "Freeing HTTPConnect resources (" + d_file + ")... ");

        // This can always be done - if the cpp_stream is null, delete has no effect;
        // if non-null in this class it was allocated in HTTPConnect::plain_fetch_url
        // (or caching_fetch_url when that's implemented)
		delete get_cpp_stream();
		set_cpp_stream(0);

        if (!dods_keep_temps && !d_file.empty()) {
			if (get_stream()) {
				close_temp(get_stream(), d_file);
				set_stream(0);
			}
			else {
			    (void) unlink(d_file.c_str());
#if 0
				long res = unlink(d_file.c_str());
				if (res != 0) throw InternalErr(__FILE__, __LINE__, "!FAIL! " + long_to_string(res));
#endif
			}
        }

        delete d_headers;

        DBGN(cerr << endl);
    }

    /**
     * Build a new HTTPResponse object that works with C++ streams. Assume that
     * the FILE* references a disk file.
     * @return
     */
    void transform_to_cpp() {
    	// ~Response() will take care of closing the FILE*. A better version of this
    	// code would not leave the FILE* open when it's not needed, but this implementation
    	// can use the existing HTTPConnect and HTTPCache software with very minimal
    	// (or no) modification. jhrg 11/8/13
    	set_cpp_stream(new std::fstream(d_file.c_str(), std::ios::in|std::ios::binary));
    }

    /** @name Accessors */
    //@{
    virtual std::vector<std::string> *get_headers() const { return d_headers; }
    virtual std::string get_file() const { return d_file; }
    //@}

    /** @name Mutators */
    //@{
    virtual void set_headers(std::vector<std::string> *h) { d_headers = h; }
    virtual void set_file(const std::string &n) { d_file = n; }
    //@}
};

} // namespace libdap

#endif // http_response_h
