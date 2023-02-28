
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

#ifndef http_test_response_h
#define http_test_response_h

#include <unistd.h>

#include <cstdio>

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include "HTTPResponse.h"
#include "util.h"

namespace libdap
{

/** Encapsulate an http response. Instead of directly returning the FILE
    pointer from which a response is read and vector of headers, return an
    instance of this object.
 */
class HTTPTestResponse : public HTTPResponse
{
public:
    /** @name Suppressed default methods */
    ///@{
    HTTPTestResponse() = delete;
    HTTPTestResponse(const HTTPTestResponse &rs) = delete;
    HTTPTestResponse &operator=(const HTTPTestResponse &) = delete;
    ///@}

    /** @name Constructors and destructor */
    ///@{
    /**
     * Build a test response using body and headers files.
     * @param body_file
     * @param header_file
     * @param status
     */
    HTTPTestResponse(const std::string &body_file, const std::string &header_file, int status)
    {
        set_status(status);
        set_file(body_file);

        FILE *s = fopen(body_file.c_str(), "r");
        set_stream(s);

        auto ifs = std::ifstream(header_file.c_str());
        auto headers = std::vector<std::string>();
        std::string line;
        std::getline(ifs, line);
        while (!line.empty() && ifs.good()) {
            headers.push_back(line);
            std::getline(ifs, line);
        }
        set_headers(headers);
    }

    /**
     * Build a test response using a base name for the body and headers files.
     * @param base_name
     * @param status
     */
    HTTPTestResponse(const std::string &base_name, int status)
        : HTTPTestResponse(base_name + ".body", base_name + ".headers", status) { }

        /**
         * This destructor sets the file name to the empty string, which prevents the
         * parent class destructor from unlinking the file.
         */
    ~HTTPTestResponse() override {  set_file(""); }
    ///@}
};

} // namespace libdap

#endif // http_test_response_h
