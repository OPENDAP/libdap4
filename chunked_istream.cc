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

#include <stdint.h>
#include <byteswap.h>

#include <cstring>
#include <vector>

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
  This code does not use a 'put back' buffer, but here's a picture of the
  d_buffer pointer, eback(), gptr() and egptr() that can be used to see how
  the I/O Stream library's streambuf class works. For the case with no
  putback, just imagine it as zero and eliminate the leftmost extension. This
  might also come in useful if the code was extended to support put back. I
  removed that feature because I don't see it being used with our chunked
  transmission protocol and it requires an extra call to memcopy() when data
  are added to the internal buffer.

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

	// gptr() == egptr() so read more data from the underlying input source.

	// To read data from the chunked stream, first read the header
	uint32_t header;
	d_is.read((char *) &header, 4);

	// There are two 'EOF' cases: One where the END chunk is zero bytes and one where
	// it holds data. In the latter case, bytes those will be read and moved into the
	// buffer. Once those data are consumed, we'll be back here again and this read()
	// will return EOF. See below for the other case...
	if (d_is.eof()) return traits_type::eof();
	if (d_twiddle_bytes) header = bswap_32(header);

	uint32_t chunk_size = header & CHUNK_SIZE_MASK;

	DBG2(cerr << "read_next_chunk: chunk size from header: " << chunk_size << endl); DBG2(cerr << "read_next_chunk: chunk type from header: " << (void*)(header & CHUNK_TYPE_MASK) << endl);

	// Handle the case where the buffer is not big enough to hold the incoming chunk
	if (chunk_size > d_buf_size) {
		d_buf_size = chunk_size;
		m_buffer_alloc();
	}

	// If the END chunk has zero bytes, return EOF. See above for more information
	if (chunk_size == 0 && (header & CHUNK_TYPE_MASK) == CHUNK_END) return traits_type::eof();

	// Read the chunk's data
	d_is.read(d_buffer, chunk_size);
	DBG2(cerr << "read_next_chunk: size read: " << d_is.gcount() << ", eof: " << d_is.eof() << ", bad: " << d_is.bad() << endl);
	if (d_is.bad()) return traits_type::eof();

	DBG2(cerr << "eback(): " << (void*)eback() << ", gptr(): " << (void*)(gptr()-eback()) << ", egptr(): " << (void*)(egptr()-eback()) << endl);
	setg(d_buffer, 						// beginning of put back area
			d_buffer,                	// read position (gptr() == eback())
			d_buffer + chunk_size);  	// end of buffer (egptr()) chunk_size == d_is.gcount() unless there's an error

	DBG2(cerr << "eback(): " << (void*)eback() << ", gptr(): " << (void*)(gptr()-eback()) << ", egptr(): " << (void*)(egptr()-eback()) << endl);

	switch (header & CHUNK_TYPE_MASK) {
	case CHUNK_END:
		DBG(cerr << "Found end chunk" << endl);
		return traits_type::to_int_type(*gptr());
	case CHUNK_DATA:
		return traits_type::to_int_type(*gptr());

	case CHUNK_ERR:
		// this is pretty much the end of the show... Assume the buffer/chunk holds
		// the error message text.
		d_error = true;
		d_error_message = string(d_buffer, chunk_size);
		return traits_type::eof();
	}

	return traits_type::eof();	// Can never get here; this quiets g++
#if 0
	// read_next_chunk() returns EOF or the size of the chunk while underflow
	// returns EOF or the next char in the input.
	std::streambuf::int_type result = read_next_chunk();
	DBG2(cerr << "underflow: read_next_chunk: " << result << endl);

	return (result == traits_type::eof()) ? traits_type::eof(): traits_type::to_int_type(*gptr());
#endif
}

/**
 * @brief Read a block of data
 * This specialization of xsgetn() reads \c num bytes and puts them in \c s
 * first reading from the internal beffer and then from the stream. Any
 * characters read from the last chunk that won't fit in to \c s are put
 * in the buffer, otherwise all data are read directly into \c s, bypassing
 * the internal buffer (and the extra copy operation that would imply). If
 * the END chunk is found EOF is not returned and the final read of the
 * underlying stream is not made; the next call to read(), get(), ..., will
 * return EOF.
 * @param s Address of a buffer to hold the data
 * @param num Number of bytes to read
 * @return NUmber of bytes actually transferred into \c s. Note that this
 * number does not include the bytes read from the last chunk that won't
 * fit into \c s so this will never return a number greater than num.
 */
std::streamsize
chunked_inbuf::xsgetn(char* s, std::streamsize num)
{
	DBG(cerr << "xsgetn... num: " << num << endl);

	// if num is <= the chars currently in the buffer
	if (num <= (egptr() - gptr())) {
		memcpy(s, gptr(), num);
		gbump(num);

		return traits_type::not_eof(num);
	}

	// else they asked for more
	uint32_t bytes_left_to_read = num;

	// are there any bytes in the buffer? if so grab them first
	if (gptr() < egptr()) {
		int bytes_to_transfer = egptr() - gptr();
		memcpy(s, gptr(), bytes_to_transfer);
		gbump(bytes_to_transfer);
		s += bytes_to_transfer;
		bytes_left_to_read -= bytes_to_transfer;
	}

	// We need to get more bytes from the underlying stream; at this
	// point the internal buffer is empty.

	// read the remaining bytes to transfer, a chunk at a time,
	// and put any leftover stuff in the buffer.

	// note that when the code is here, gptr() == egptr(), so the
	// next call to read() will fall through the previous tests and
	// read at least one chunk here.
	bool done = false;
	while (!done) {
		// Get a chunk header
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

	    uint32_t chunk_size = header & CHUNK_SIZE_MASK;
	    // handle error chunks here
	    if ((header & CHUNK_TYPE_MASK) == CHUNK_ERR) {
			d_error = true;
			// Note that d_buffer is not used to avoid calling resize if it is too
			// small to hold the error message. At this point, there's not much reason
			// to optimize transport efficiency, however.
			std::vector<char> message(chunk_size);
			d_is.read(&message[0], chunk_size);
			d_error_message = string(&message[0], chunk_size);
			// leave the buffer and gptr(), ..., in a consistent state (empty)
			setg(d_buffer, d_buffer, d_buffer);
	    }
	    // The first case is complicated because we read some data from the current
	    // chunk into 's' an some into the internal buffer.
	    else if (chunk_size > bytes_left_to_read) {
			d_is.read(s, bytes_left_to_read);
			if (d_is.bad()) return traits_type::eof();

			// Now slurp up the remain part of the chunk and store it in the buffer
			uint32_t bytes_leftover = chunk_size - bytes_left_to_read;
			// expand the internal buffer if needed
		    if (bytes_leftover > d_buf_size) {
		        d_buf_size = chunk_size;
		        m_buffer_alloc();
		    }
		    // read the remain stuff in to d_buffer
			d_is.read(d_buffer, bytes_leftover);
			if (d_is.bad()) return traits_type::eof();

			setg(d_buffer, 										// beginning of put back area
				 d_buffer,                						// read position (gptr() == eback())
				 d_buffer + bytes_leftover /*d_is.gcount()*/); 	// end of buffer (egptr())

			bytes_left_to_read = 0 /* -= d_is.gcount()*/;
		}
		else {
			// expand the internal buffer if needed
		    if (chunk_size > d_buf_size) {
		        d_buf_size = chunk_size;
		        m_buffer_alloc();
		    }
			d_is.read(s, chunk_size);
			if (d_is.bad()) return traits_type::eof();
			bytes_left_to_read -= chunk_size /*d_is.gcount()*/;
			s += chunk_size;
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
			// this is pretty much the end of the show... The error message has
	    	// already been read above

			return traits_type::eof();
	        break;
	    }
	}

	return traits_type::not_eof(num-bytes_left_to_read);
}

/**
 * @brief Read a chunk
 * Normally the chunked nature of a chunked_istream/chunked_inbuf is
 * hidden from the caller. This method provides a way to get one chunk
 * from the stream by forcing its read and returning the size. A subsequent
 * call to read() for that number of bytes will return all of the data in
 * the chunk. If there is any data in the chunk_inbuf object's buffer, it is
 * lost.
 *
 * @todo If this is going to be used after regular calls to read() or get()
 * are used, add a method to return the number of bytes remaining in the buffer.
 * That will enable a caller to read those data before this method erases them.
 *
 * @return The number of bytes read, which is exactly the size of the
 * next chunk in the stream. Returns EOF on error.
 */
std::streambuf::int_type
chunked_inbuf::read_next_chunk()
{
	// To read data from the chunked stream, first read the header
	uint32_t header;
	d_is.read((char *) &header, 4);

	// There are two 'EOF' cases: One where the END chunk is zero bytes and one where
	// it holds data. In the latter case, bytes those will be read and moved into the
	// buffer. Once those data are consumed, we'll be back here again and this read()
	// will return EOF. See below for the other case...
	if (d_is.eof()) return traits_type::eof();
	if (d_twiddle_bytes) header = bswap_32(header);

	uint32_t chunk_size = header & CHUNK_SIZE_MASK;

	DBG2(cerr << "read_next_chunk: chunk size from header: " << chunk_size << endl);
	DBG2(cerr << "read_next_chunk: chunk type from header: " << (void*)(header & CHUNK_TYPE_MASK) << endl);

	// Handle the case where the buffer is not big enough to hold the incoming chunk
	if (chunk_size > d_buf_size) {
		d_buf_size = chunk_size;
		m_buffer_alloc();
	}

	// If the END chunk has zero bytes, return EOF. See above for more information
	if (chunk_size == 0 && (header & CHUNK_TYPE_MASK) == CHUNK_END) return traits_type::eof();

	// Read the chunk's data
	d_is.read(d_buffer, chunk_size);
	DBG2(cerr << "read_next_chunk: size read: " << d_is.gcount() << ", eof: " << d_is.eof() << ", bad: " << d_is.bad() << endl);
	if (d_is.bad()) return traits_type::eof();

	DBG2(cerr << "eback(): " << (void*)eback() << ", gptr(): " << (void*)(gptr()-eback()) << ", egptr(): " << (void*)(egptr()-eback()) << endl);
	setg(d_buffer, 						// beginning of put back area
			d_buffer,                	// read position (gptr() == eback())
			d_buffer + chunk_size);  	// end of buffer (egptr()) chunk_size == d_is.gcount() unless there's an error

	DBG2(cerr << "eback(): " << (void*)eback() << ", gptr(): " << (void*)(gptr()-eback()) << ", egptr(): " << (void*)(egptr()-eback()) << endl);

	switch (header & CHUNK_TYPE_MASK) {
	case CHUNK_END:
		DBG(cerr << "Found end chunk" << endl);
		return traits_type::not_eof(chunk_size);
	case CHUNK_DATA:
		return traits_type::not_eof(chunk_size);

	case CHUNK_ERR:
		// this is pretty much the end of the show... Assume the buffer/chunk holds
		// the error message text.
		d_error = true;
		d_error_message = string(d_buffer, chunk_size);
		return traits_type::eof();
	}

	return traits_type::eof();	// Can never get here; this quiets g++
}

}
