
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

#ifndef stdin_response_h
#define stdin_response_h

#include <cstdio>

#include "Response.h"

namespace libdap {

/** @brief Encapsulate a response read from stdin.

    This class holds stdin and provides an interface from which
    Connect can read DAP2 information from standard input. Unlike the
    other Response classes, StdinResponse does \e not close the input
    stream when it's done reading.

    @note Modified 10/25/13 to accommodate C++ istreams. */
class StdinResponse : public Response {
private:
    FILE *d_stdin;
    std::istream *d_cin;

protected:
public:
    /** @brief Initialize with standard input.

        Create an instance initialized to standard input. When done, does
        not close stdin.

        @see Response

        @param s Pointer to standard input.
        */
    StdinResponse(FILE *s) : Response(), d_stdin(s), d_cin(0) {}

    /**
     * @brief Build a instance using a C++ istream
     * @param in A pointer to the input stream
     */
    StdinResponse(std::istream &in) : Response(), d_stdin(0), d_cin(&in) {}

    /** Destructor. Does not close standard input. */
    ~StdinResponse() override {}

    /** @brief Return the `FILE*` input stream.
     * @return Stored C stdio input stream.
     */
    FILE *get_stream() const override { return d_stdin; }

    /** @brief Set the `FILE*` input stream.
     * @param s C stdio input stream.
     */
    void set_stream(FILE *s) override { d_stdin = s; }

    /** @brief Return the C++ input stream.
     * @return Stored C++ input stream pointer.
     */
    std::istream *get_cpp_stream() const override { return d_cin; }

    /** @brief Set the C++ input stream.
     * @param in C++ input stream pointer.
     */
    void set_cpp_stream(std::istream *in) override { d_cin = in; }
};

} // namespace libdap

#endif // pipe_response_h
