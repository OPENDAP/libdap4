// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2009 OPeNDAP, Inc.
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
//
// Portions of this code were taken verbatim from  Josuttis,
// "The C++ Standard Library," p.672

#include "config.h"

#include <algorithm>

#include "chunked_stream.h"
#include "chunked_istream.h"

#include "Error.h"

#define DODS_DEBUG
#include "debug.h"

namespace libdap {


/*

  d_buffer  d_buffer + putBack
  |         |
  v         v
  |---------|--------------------------------------------|....
  |         |                                            |   .
  |---------|--------------------------------------------|....
            ^                         ^                   ^
            |                         |                   |
            eback()                   gptr()              egptr()

 */

/** Insert new characters into the buffer */
int chunked_inbuf::underflow()
{
	// return the next character; uflow() increments the puffer pointer.
	if (gptr() < egptr())
		return *gptr();

	// gptr() == egptr() so shuffle the last putBack chars to the 'put back'
	// area of the buffer and read more data from the underlying input source.

	// read_next_chunk() returns EOF or the size of the chunk while underflow
	// returns EOF or the next char in the input.
	int result = read_next_chunk();
	return (result == EOF) ? EOF: *gptr();

#if 0
	// How many characters are in the 'put back' part of the buffer? Cap
	// this number at putBack, which is nominally 128.
	int numPutBack = gptr() - eback();
	if (putBack < numPutBack) numPutBack = putBack;

	// In most cases numPutBack is putBack, so the code copies putBack chars
	// from the end of the buffer to the front. Note that if the code gets here
	// gptr() is likely == to egptr() (or one position past egptr()). In that
	// case gptr() - numPutBack is the position numPutBack characters before
	// the end of the buffer.
	memcpy(d_buffer + (putBack - numPutBack), gptr() - numPutBack, numPutBack);

	// To read data from the chunked stream, first read the header
	int32_t  header;
	d_is.read((char *)&header, 4);
	// Note that d_chunk_size is accessible via a protected method.
	d_chunk_size = header & CHUNK_SIZE_MASK;

	// Handle the case where the buffer is not big enough to hold the incoming chunk
	if (d_chunk_size > d_buf_size) {
		DBG(cerr << "Chunk size too big, reallocating buffer" << endl);
		d_buf_size = d_chunk_size;
		m_buffer_alloc();
	}

	// NB: d_buffer is d_buf_size + putBack characters in length
	d_is.read(d_buffer + putBack, d_chunk_size);
	if (d_is.bad() || d_is.eof())
		return EOF;

	setg(d_buffer + (putBack - numPutBack), // beginning of put back area
		 d_buffer + putBack, 				// read position (gptr() == eback())
		 d_buffer + putBack + d_is.gcount()); // end of buffer (egptr())

	switch (header & CHUNK_TYPE_MASK) {
	case CHUNK_DATA:
	case CHUNK_END:
		return *gptr();
		break;
	case CHUNK_ERR:
		// this is pretty much the end of the show...
		string msg(d_buffer + putBack, chunk_size);
		throw Error(msg);
		break;
	}

	return EOF;
#endif
}

int chunked_inbuf::read_next_chunk()
{
    // How many characters are in the 'put back' part of the buffer? Cap
    // this number at putBack, which is nominally 128.
    int numPutBack = gptr() - eback();
    if (putBack < numPutBack) numPutBack = putBack;

    // In most cases numPutBack is putBack, so the code copies putBack chars
    // from the end of the buffer to the front. Note that if the code gets here
    // gptr() is likely == to egptr() (or one position past egptr()). In that
    // case gptr() - numPutBack is the position numPutBack characters before
    // the end of the buffer.
    memcpy(d_buffer + (putBack - numPutBack), gptr() - numPutBack, numPutBack);

    // To read data from the chunked stream, first read the header
    int32_t  header;
    d_is.read((char *)&header, 4);
    // Note that d_chunk_size is accessible via a protected method.
    d_chunk_size = header & CHUNK_SIZE_MASK;

    // Handle the case where the buffer is not big enough to hold the incoming chunk
    if (d_chunk_size > d_buf_size) {
        DBG(cerr << "Chunk size too big, reallocating buffer" << endl);
        d_buf_size = d_chunk_size;
        m_buffer_alloc();
    }

    // NB: d_buffer is d_buf_size + putBack characters in length
    d_is.read(d_buffer + putBack, d_chunk_size);
    if (d_is.bad() || d_is.eof())
        return EOF;

    setg(d_buffer + (putBack - numPutBack), // beginning of put back area
         d_buffer + putBack,                // read position (gptr() == eback())
         d_buffer + putBack + d_is.gcount()); // end of buffer (egptr())

    switch (header & CHUNK_TYPE_MASK) {
    case CHUNK_DATA:
    case CHUNK_END:
        return d_chunk_size;
        break;
    case CHUNK_ERR:
        // this is pretty much the end of the show... Assume the buffer/chunk holds
        // the error message text.
        string msg(d_buffer + putBack, d_chunk_size);
        throw Error(msg);
        break;
    }

    return EOF;
}

}
