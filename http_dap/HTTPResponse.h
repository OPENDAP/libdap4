
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

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "Response.h"
#include "util.h"

namespace libdap {
extern int dods_keep_temps;

/** Encapsulate an http response. Instead of directly returning the FILE
    pointer from which a response is read and vector of headers, return an
    instance of this object.
*/

class HTTPResponse : public Response {
private:
    std::vector<std::string> d_headers; // Response headers
    std::string d_file;                 // Temp file that holds response body

public:
    HTTPResponse() = default;
    HTTPResponse(const HTTPResponse &rs) = delete;
    HTTPResponse &operator=(const HTTPResponse &) = delete;

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
    HTTPResponse(FILE *s, int status, const std::vector<std::string> &h, const std::string &temp_file)
        : Response(s, status), d_headers(h), d_file(temp_file) {}

    /**
     * @brief Build a HTTPResponse using a cpp fstream
     * When working with DAP4 responses, use C++ streams for I/0.
     * @param s
     * @param status
     * @param h
     * @param temp_file
     */
    HTTPResponse(std::fstream *s, int status, const std::vector<std::string> &h, const std::string &temp_file)
        : Response(s, status), d_headers(h), d_file(temp_file) {}

    /** When an instance is destroyed, free the temporary resources: the
    temp_file and headers are deleted. If the tmp file name is "", it is
    not deleted. */
    ~HTTPResponse() override {
        if (!dods_keep_temps && !d_file.empty()) {
            (void)unlink(d_file.c_str());
        }
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
        set_cpp_stream(new std::fstream(d_file.c_str(), std::ios::in | std::ios::binary));
        // This hack was added to work around a memory leak introduced here but which surfaces
        // in the parent class. Deleting the fstream* in all cases can lead to a double free;
        // not deleting it in some cases leads to a leak. This set of classes should be redesigned.
        // jhrg 2/9/25
        d_delete_cpp_stream_ptr = true;
    }

    /** @name Accessors */
    //@{
    virtual std::vector<std::string> &get_headers() { return d_headers; }
    virtual std::string get_file() const { return d_file; }
    //@}

    /** @name Mutators */
    //@{
    virtual void set_headers(std::vector<std::string> &h) { d_headers = h; }
    virtual void set_file(const std::string &n) { d_file = n; }
    //@}
};

} // namespace libdap

#endif // http_response_h
