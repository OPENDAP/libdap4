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

#include "chunked_stream.h"

#include <streambuf>
#include <ostream>
#include <stdexcept>      // std::out_of_range

#include "util.h"

namespace libdap {

class chunked_ostream;

/**
 * @brief output buffer for a chunked stream
 * This performs buffered output encoding the data in the stream using
 * the simple chunking protocol defined for DAP4's binary data transmission.
 * Each block of data is prefixed by four bytes: A CHUNK TYPE byte followed
 * by three bytes that are the CHUNK SIZE. There are three CHUNK TYPES:
 * data, end and error, indicated by the code values 0x00, 0x01 and 0x02.
 * The size of a chunk is limited to 2^24 data bytes + 4 bytes for the
 * chunk header.
 */
class chunked_outbuf: public std::streambuf {
	friend class chunked_ostream;
protected:
	std::ostream &d_os;			// Write stuff here
	unsigned int d_buf_size; 	// Size of the data buffer
	char *d_buffer;				// Data buffer
	bool d_big_endian;

public:
	chunked_outbuf(std::ostream &os, unsigned int buf_size) : d_os(os), d_buf_size(buf_size), d_buffer(0) {
		if (d_buf_size & CHUNK_TYPE_MASK)
			throw std::out_of_range("A chunked_outbuf (or chunked_ostream) was built using a buffer larger than 0x00ffffff");

		d_big_endian = is_host_big_endian();
		d_buffer = new char[buf_size];
		// Trick: making the pointers think the buffer is one char smaller than it
		// really is ensures that overflow() will be called when there's space for
		// one more character.
		setp(d_buffer, d_buffer + (buf_size - 1));
	}

	virtual ~chunked_outbuf() {
		// call end_chunk() and not sync()
		end_chunk();

		delete[] d_buffer;
	}

protected:
	// data_chunk and end_chunk might not be needed because they
	// are called via flush() and ~chunked_outbuf(), resp. jhrg 9/13/13
	int_type data_chunk();	// sync() and overflow() call this
	int_type end_chunk();

	int_type err_chunk(const std::string &msg);

	virtual std::streamsize xsputn(const char *s, std::streamsize num);
	// Manipulate the buffer pointers using pbump() after filling the buffer
	// and then call data_chunk(). Leave remainder in buffer. Or copy logic
	// for data_chunk() into loop in this code.

	virtual int_type overflow(int c);
	virtual int_type sync();
};

/**
 * @brief A C++ stream class for chunked data.
 * This class uses the chunked_outbuf class to provide for chunked
 * binary serialization of data as specified by DAP4. Information
 * to be serialized is broken into 'chunks' that are no more than
 * 2^24 bytes in length. Each chunk is prefixed by a 4 byte header
 * that indicates the type of chunk and size (number of bytes in the
 * chunk body). There are three types of chunk: Data; End; and Error.
 * In normal operation, a DAP4 data document/response is serialized as
 * a sequence of DATA chunks followed by one END chunk (which may be
 * zero bytes in length). If, during serialization, an error is detected,
 * the currently buffered (but not sent) data are discarded and an
 * ERROR chunk is sent with an error message.
 *
 * This class sends the END chunk when its destructor is called.
 *
 * Calling flush() on the ostream object will force a DATA chunk to be
 * sent with the currently buffered data. Normal operation is to wait
 * for the buffer to fill before sending a DATA chunk.
 *
 * @see chunked_outbuf
 */
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
	 * @note An end chunk is sent when the stream is closed.
	 * @return EOF on error or the number of bytes sent in the chunk body.
	 */
	int_type write_end_chunk() { return d_cbuf.end_chunk(); }

	/**
	 * @brief Send the current contents of the buffer as a data chunk.
	 * Normally, the chunked_ostream object waits until the buffer is full before sending
	 * the next data chunk. This will force a send with whatever is in the buffer (e.g.,
	 * the DMR text). Data added after this call will be sent in subsequent chunks.
	 * @note Calling flush() on the stream forces a data chunk to be sent.
	 * @return EOF on error, otherwise the number of bytes sent in the chunk body.
	 */
	int_type write_data_chunk() { return d_cbuf.data_chunk(); }

	/**
	 * @brief Send an error message down the stream.
	 * When called, this method dumps all the data currently in the buffer and
	 * sends the error message text instead, using a chunk type of CHUNK_ERR. The
	 * write buffer is maintained, however, so the stream ibject can still be used.
	 * @param msg The error message text
	 * @return The number of bytes 'dumped' from the write buffer.
	 */
	int_type write_err_chunk(const std::string &msg) { return d_cbuf.err_chunk(msg); }
};

}

#endif		// _chunkedostream_h
