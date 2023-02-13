
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

#ifndef pipe_response_h
#define pipe_response_h

#include <cstdio>

#ifndef response_h
#include "Response.h"
#endif

#ifndef _debug_h
#include "debug.h"
#endif

using namespace std;

namespace libdap
{

/** @brief Encapsulate a response.
    This class provides special treatment for 'stream pipes.' It arranges
    to close them using pclose() instead of fclose(). */
class PipeResponse: public Response
{
private:
    FILE *d_pstream;

protected:

public:
    /** @brief Initialize with a stream returned by popen().
      
        Create an instance initialized to a stream returned by
	popen(). By default get_type() and get_version() return
	default values of unknown_type and "dods/0.0", respectively.
	Note that this class closes the stream.
        
        @note Since Unix provides no easy way to differentiate between a 
        FILE* returned by fopen() or popen(), you're on your own here. Make
        sure you use the correct type of FILE Pointer. 
        
        @see Response
        
        @param s Pointer to a pipe stream returned by popen().
        */
    PipeResponse(FILE *s) : Response(0), d_pstream(s)
    {}

    /** Close the stream. */
    virtual ~PipeResponse()
    {
        if (d_pstream)
            pclose(d_pstream);
    }

    virtual FILE *get_stream() const
    {
        return d_pstream;
    }
    virtual void set_stream(FILE *s)
    {
        d_pstream = s;
    }
};

} // namespace libdap

#endif // pipe_response_h
