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

#include <byteswap.h>

#include <algorithm>

#include "chunked_stream.h"
#include "chunked_istream.h"

#include "Error.h"

//#define DODS_DEBUG
//#define DODS_DEBUG2
#ifdef DODS_DEBUG
#include <iostream>
#endif
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

/**
 * @brief Insert new characters into the buffer
 * This specialization of underflow is called when the gptr() is advanced to
 * the end of the input buffer. At that point it calls the underlying I/O stream
 * to read the next chunk of data and transfers the data read to the internal
 * buffer. If an error is found, EOF is returned. If an END chunk with zero
 * bytes is found, an EOF is returned.
 * @return The character at the gptr() or EOF
 */
std::streambuf::int_type
chunked_inbuf::underflow()
{
    DBG(cerr << "underflow..." << endl);
    DBG2(cerr << "eback(): " << (void*)eback() << ", gptr(): " << (void*)(gptr()-eback()) << ", egptr(): " << (void*)(egptr()-eback()) << endl);

	// return the next character; uflow() increments the puffer pointer.
	if (gptr() < egptr())
		return traits_type::to_int_type(*gptr());

	// gptr() == egptr() so shuffle the last putBack chars to the 'put back'
	// area of the buffer and read more data from the underlying input source.

	// read_next_chunk() returns EOF or the size of the chunk while underflow
	// returns EOF or the next char in the input.
	std::streambuf::int_type result = read_next_chunk();
	DBG2(cerr << "underflow: read_next_chunk: " << result << endl);

	return (result == traits_type::eof()) ? traits_type::eof(): traits_type::to_int_type(*gptr());

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

#if 1
std::streamsize
chunked_inbuf::xsgetn(char* s, std::streamsize num)
{
	DBG(cerr << "xsgetn... num: " << num << endl);

	// if num is < the chars currently in the buffer
	if (num <= (egptr() - gptr())) {
		DBG2(cerr << "xsgetn... getting chars from buffer: " << num << endl);
		memcpy(s, gptr(), num);
		gbump(num);

		return traits_type::not_eof(num);
	}

	DBG2(cerr << "xsgetn... going to read from stream" << endl);
	// else they asked for more
	int bytes_left_to_read = num;
		// are there any bytes in the buffer?
	if (gptr() < egptr()) {
		int bytes_to_transfer = egptr() - gptr();
		DBG2(cerr << "xsgetn... getting chars from buffer first: " << bytes_to_transfer << endl);
		memcpy(s, gptr(), bytes_to_transfer);
		gbump(bytes_to_transfer);
		s += bytes_to_transfer;
		bytes_left_to_read -= bytes_to_transfer;
	}

	// We need to get more bytes from the underlying stream; at this
	// point the internal buffer is empty.

	// read the remaining bytes to transfer, a chunk at a time
	// and put any leftover stuff in the buffer.
	bool done = false;
	while (!done) {
		DBG2(cerr << "xsgetn... getting chunks from underlying stream: " << bytes_left_to_read << endl);
		// Get a chunk
	    // To read data from the chunked stream, first read the header
	    uint32_t  header;
	    d_is.read((char *)&header, 4);

	    // There are two EOF cases: One where the END chunk is zero bytes and one where
	    // it holds data. In the latter case, those will be read and moved into the
	    // buffer. Once those data are consumed, we'll be back here again and this read()
	    // will return EOF. See below for the other case...
	    if (d_is.eof())
	    	return traits_type::eof();
	    if (d_twiddle_bytes)
	        header = bswap_32(header);

	    d_chunk_size = header & CHUNK_SIZE_MASK;
	    DBG2(cerr << "xsgetn: chunk size from header: " << d_chunk_size << endl);
	    DBG2(cerr << "xsgetn: chunk type from header: " << (void*)(header & CHUNK_TYPE_MASK) << endl);

	    // TODO move this down inside if
	    // Handle the case where the buffer is not big enough to hold the incoming chunk
	    if (d_chunk_size > d_buf_size) {
	        DBG(cerr << "Chunk size too big, reallocating buffer" << endl);
	        d_buf_size = d_chunk_size;
	        m_buffer_alloc();
	    }

		if (d_chunk_size > bytes_left_to_read) {
			d_is.read(s, bytes_left_to_read);
			DBG2(cerr << "xsgetn: size read: " << d_is.gcount() << ", eof: " << d_is.eof() << ", bad: " << d_is.bad() << endl);
			if (d_is.bad()) return traits_type::eof();

			// Now slurp out the remain part of the chunk and store it in the buffer
			int bytes_leftover = d_chunk_size - bytes_left_to_read;
			// read the remain stuff in to d_buffer
			d_is.read(d_buffer, bytes_leftover);
			DBG2(cerr << "xsgetn: size read: " << d_is.gcount() << ", eof: " << d_is.eof() << ", bad: " << d_is.bad() << endl);
			if (d_is.bad()) return traits_type::eof();

			setg(d_buffer, 										// beginning of put back area
				 d_buffer,                						// read position (gptr() == eback())
				 d_buffer + bytes_leftover /*d_is.gcount()*/); 	// end of buffer (egptr())

			bytes_left_to_read = 0 /* -= d_is.gcount()*/;
		}
		else {
			d_is.read(s, d_chunk_size);
			DBG2(cerr << "xsgetn: size read: " << d_is.gcount() << ", eof: " << d_is.eof() << ", bad: " << d_is.bad() << endl);
			if (d_is.bad()) return traits_type::eof();
			bytes_left_to_read -= d_chunk_size /*d_is.gcount()*/;
			s += d_chunk_size;
		}

	    switch (header & CHUNK_TYPE_MASK) {
	    case CHUNK_END:
	    	// in this case bytes_left_to_read can be > 0 because we ran out of data
	    	// before reading all the requested bytes. The next read() call will return
	    	// eof; this call returns the number of bytes read and transferred to 's'.
	    	done = true;
	    	break;
	    case CHUNK_DATA:
	    	done = bytes_left_to_read == 0;
	        break;
	    case CHUNK_ERR:
	    	// TODO
	        break;
	    }
	}

	return traits_type::not_eof(num-bytes_left_to_read);
}
#endif
std::streambuf::int_type
chunked_inbuf::read_next_chunk()
{
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
#endif
    // To read data from the chunked stream, first read the header
    uint32_t  header;
    d_is.read((char *)&header, 4);

    // There are two EOF cases: One where the END chunk is zero bytes and one where
    // it holds data. In the latter case, those will be read and moved into the
    // buffer. Once those data are consumed, we'll be back here again and this read()
    // will return EOF. See below for the other case...
    if (d_is.eof())
    	return traits_type::eof();
    if (d_twiddle_bytes) {
        header = bswap_32(header);
    }

    d_chunk_size = header & CHUNK_SIZE_MASK;
    DBG2(cerr << "read_next_chunk: chunk size from header: " << d_chunk_size << endl);
    DBG2(cerr << "read_next_chunk: chunk type from header: " << (void*)(header & CHUNK_TYPE_MASK) << endl);

    // Handle the case where the buffer is not big enough to hold the incoming chunk
    if (d_chunk_size > d_buf_size) {
        DBG(cerr << "Chunk size too big, reallocating buffer" << endl);
        d_buf_size = d_chunk_size;
        m_buffer_alloc();
    }

    // If the END chunk has zero bytes, return EOF. See about for more information
    if (d_chunk_size == 0 && (header & CHUNK_TYPE_MASK) == CHUNK_END)
    	return traits_type::eof();

    // NB: d_buffer is d_buf_size + putBack characters in length
    d_is.read(d_buffer/* + putBack*/, d_chunk_size);
    DBG2(cerr << "read_next_chunk: size read: " << d_is.gcount() << ", eof: " << d_is.eof() << ", bad: " << d_is.bad() << endl);
    if (d_is.bad())
        return traits_type::eof();

    DBG2(cerr << "eback(): " << (void*)eback() << ", gptr(): " << (void*)(gptr()-eback()) << ", egptr(): " << (void*)(egptr()-eback()) << endl);
#if 0
    setg(d_buffer + (putBack - numPutBack), // beginning of put back area
         d_buffer + putBack,                // read position (gptr() == eback())
         d_buffer + putBack + d_is.gcount()); // end of buffer (egptr())
#endif
    setg(d_buffer, // beginning of put back area
         d_buffer,                // read position (gptr() == eback())
         d_buffer + d_is.gcount()); // end of buffer (egptr())

    DBG2(cerr << "eback(): " << (void*)eback() << ", gptr(): " << (void*)(gptr()-eback()) << ", egptr(): " << (void*)(egptr()-eback()) << endl);

    switch (header & CHUNK_TYPE_MASK) {
    case CHUNK_END:
    	DBG(cerr << "Found end chunk" << endl);
    case CHUNK_DATA:
        return d_chunk_size;
        break;
    case CHUNK_ERR:
        // this is pretty much the end of the show... Assume the buffer/chunk holds
        // the error message text.

        DBG(cerr << "read_next_chunk: About to throw an exception" << endl);

        d_error = true;
        d_error_message = string(d_buffer/* + putBack*/, d_chunk_size);
        return traits_type::eof();
        break;
    }

    return traits_type::eof();
}

}
