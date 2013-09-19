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
// Portions of this code were taken verbatim from Josuttis,
// "The C++ Standard Library," p.672

#ifndef _chunked_istream_h
#define _chunked_istream_h

#include "chunked_stream.h"

#include <streambuf>
#include <istream>
#include <stdexcept>
#include <string>

namespace libdap {

class chunked_inbuf: public std::streambuf {
private:
	std::istream &d_is;

	int d_buf_size; 	// Size of the data buffer
	char *d_buffer;		// data buffer

	// TODO Not needed?
	int d_chunk_size;	// size of the current chunk

	bool d_twiddle_bytes; // receiver-makes-right encoding (endianness)...

	std::string d_error_message;
	bool d_error;

	/**
	 * @brief allocate the internal buffer.
	 * Allocate d_buf_size + putBack characters for the read buffer.
	 * @param size How much can the buffer hold? Does not include the putBack
	 * chars.
	 */
	void m_buffer_alloc() {
		delete d_buffer;
		d_buffer = new char[d_buf_size];
		setg(d_buffer, 	// beginning of put back area
			 d_buffer, 	// read position
		     d_buffer); // end position
	}

	// TODO remove if not needed
	int_type read_next_chunk();

public:
	/**
	 * @brief Build a chunked input buffer.
	 *
	 * This reads from a chunked stream, extracting an entire chunk and storing it in a
	 * buffer in one operation. If the chunked_inbuf reads a chunk header that indicates
	 * the next chunk is goin gto be bigger than its current buffer size, the object will
	 * make the buffer larger. This object supprt 128 characters of 'put back' space. Since
	 * DAP4 uses receiver makes right, the buffer must be told if it should 'twiddle' the
	 * header size information. In DAP4 the byte order is sent using a one-byte code _before_
	 * the chunked transmission starts.
	 *
	 * @param is Use this as a data source
	 * @param size The size of the input buffer. This should match the likely chunk size.
	 * If it is smaller than a chunk, it will be resized.
	 * @param twiddle_bytes Should the header bytes be twiddled? True if this host and the
	 * send use a different byte-order. The sender's byte order must be sent out-of-band.
	 */
	chunked_inbuf(std::istream &is, int size, bool twiddle_bytes = false)
        : d_is(is), d_buf_size(size), d_buffer(0), d_chunk_size(0), d_twiddle_bytes(twiddle_bytes), d_error(false) {
		if (d_buf_size & CHUNK_TYPE_MASK)
			throw std::out_of_range("A chunked_outbuf (or chunked_ostream) was built using a buffer larger than 0x00ffffff");

		m_buffer_alloc();
	}

	virtual ~chunked_inbuf() {
		delete d_buffer;
	}

	bool error() const { return d_error; }
	std::string error_message() const { return d_error_message; }

protected:
	virtual int_type underflow();

	virtual std::streamsize xsgetn(char* s, std::streamsize num);
};

class chunked_istream: public std::istream {
protected:
	chunked_inbuf d_cbuf;
public:
	chunked_istream(std::istream &is, int size, bool twiddle_bytes = false) : std::istream(&d_cbuf), d_cbuf(is, size, twiddle_bytes) { }
	bool error() const { return d_cbuf.error(); }
	std::string error_message() const { return d_cbuf.error_message(); }
};

}

#endif	// _chunked_istream_h
