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

namespace libdap {

class chunked_inbuf: public std::streambuf {
protected:
	std::istream &d_is;

	static const int putBack = 128;
	int d_buf_size; 	// Size of the data buffer
	char *d_buffer;		// data buffer

	int d_chunk_size;

	/**
	 * @brief allocate the internal buffer.
	 * Allocate d_buf_size + putBack characters for the read buffer.
	 * @param size How much can the buffer hold? Does not include the putBack
	 * chars.
	 */
	void m_buffer_alloc() {
		d_buffer = new char[d_buf_size + putBack];

		setg(d_buffer + putBack, 	// beginning of put back area
			 d_buffer + putBack, 	// read position
		     d_buffer + putBack); 	// end position
	}

public:
	chunked_inbuf(std::istream &is, int size) : d_is(is), d_buf_size(size), d_buffer(0), d_chunk_size(0) {
		if (d_buf_size & CHUNK_TYPE_MASK)
			throw std::out_of_range("A chunked_outbuf (or chunked_ostream) was built using a buffer larger than 0x00ffffff");

		m_buffer_alloc();
	}

	virtual ~chunked_inbuf() {
		delete d_buffer;
	}

    int read_next_chunk();

    int chunk_size() { return d_chunk_size; }

protected:
	virtual int underflow();

	// TODO
	// virtual std::streamsize xsgetn(char* s, std::streamsize num);
};

class chunked_istream: public std::istream {
protected:
	chunked_inbuf d_cbuf;
public:
	chunked_istream(std::istream &is, int size) : std::istream(&d_cbuf), d_cbuf(is, size) { }
	int read_next_chunk() { return d_cbuf.read_next_chunk(); }
};

}

#endif	// _chunked_istream_h
