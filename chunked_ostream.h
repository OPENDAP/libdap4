// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

#ifndef _chunkedostream_h
#define _chunkedostream_h

// These define the kind of chunk. Each chunk has a 32-bit header where the
// 3 LS Bytes are the size (so chunks are limited to 2^24 bytes in size) and
// the MSB is the kind of chunk. Use bit-wise or ...
#define CHUNK_DATA 0x00000000
#define CHUNK_END 0x01000000
#define CHUNK_ERR 0x02000000

#include <streambuf>
#include <ostream>
#include <stdexcept>      // std::out_of_range

namespace libdap {

class chunked_ostream;

class chunked_outbuf: public std::streambuf {
	friend class chunked_ostream;
protected:
	std::ostream &d_os;			// Write stuff here
	unsigned int d_buf_size; 	// Size of the data buffer
	char *d_buffer;				// Data buffer

public:
	chunked_outbuf(std::ostream &os, unsigned int buf_size) : d_os(os), d_buf_size(buf_size), d_buffer(0) {
		if (d_buf_size & 0xff000000)
			throw std::out_of_range("A chunked_outbuf (or chunked_ostream) was built using a buffer larger than 0x00ffffff");
		d_buffer = new char[buf_size];
		// Trick: making the pointers think the buffer is one char smaller than it
		// really is ensures that overflow() will be called when there's space for
		// one more character.
		setp(d_buffer, d_buffer + (buf_size - 1));
	}

	virtual ~chunked_outbuf() {
		// sync();
		// replace sync() with end_chunk()
		end_chunk();

		delete[] d_buffer;
	}

protected:
	int data_chunk();	// sync() and overflow() call this
	int end_chunk();
	int err_chunk();

	virtual int overflow(int c);
	virtual int sync();
};

class chunked_ostream: public std::ostream {
protected:
	chunked_outbuf d_cbuf;
public:
	/**
	 * Get a chunked_ostream with a buffer.
	 * @note The buffer size must not be more than 2^24 bytes (0x00ffffff)
	 * @param buf_size The size of the buffer in bytes.
	 */
	chunked_ostream(std::ostream &os, unsigned int buf_size) : std::ostream(&d_cbuf), d_cbuf(os, buf_size) { }

	/**
	 * @brief Send an end chunk.
	 * Normally, an end chunk is sent by closing the chunked_ostream, but this
	 * method can be used to force sending it without closing the stream. Subsequent
	 * calls to send data will send data chunks.
	 * @return EOF on error or the number of bytes sent in the chunk body.
	 */
	int write_end_chunk() { return d_cbuf.end_chunk(); }

	/**
	 * @brief Send the current contents of the buffer as a data chunk.
	 * Normally, the chunked_ostream object waits until the buffer is full before sending
	 * the next data chunk. This will force a send with whatever is in the buffer (e.g.,
	 * the DMR text). Data added after this call will be sent in subsequent chunks.
	 * @return EOF on error, otherwise the number of bytes sent in the chunk body.
	 */
	int write_data_chunk() { return d_cbuf.data_chunk(); }

	// TODO Added err_chunk(const string &message);
};

}

#endif		// _chunkedostream_h
