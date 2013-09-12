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

#include "chunked_ostream.h"
#include "debug.h"

namespace libdap {

// flush the characters in the buffer
int chunked_outbuf::data_chunk()
{
	cerr << "In chunked_outbuf::data_chunk" << endl;

	int32_t num = pptr() - pbase();	// int needs to be signed for the call to pbump

	// here, write out the chunk headers: CHUNKTYPE and CHUNKSIZE
	// as a 32-bit unsigned int. Here I assume that num is never
	// more than 2^24 because that was tested in the constructor

	// Trick: This method always writes CHUNK_DATA type chunks so the chunk type is
	// always 0x00, and given that num never has anything bigger than 24-bits, the
	// high order byte is always 0x00. Of course bit-wise OR with 0x00 isn't going to
	// do much anyway... Here's the general idea al the same:
	//
	// unsigned int chunk_header = (unsigned int)num | CHUNK_type;

	// Write out the CHUNK_DATA header with the byte count
	d_os.write((const char *)&num, sizeof(int32_t));

	// Should bad() throw an error?
	// Are these functions fast or would the bits be faster?
	d_os.write(d_buffer, num);
	if (d_os.eof() || d_os.bad())
		return EOF;

	pbump(-num);
	return num;
}

/**
 * @brief Send an end chunk.
 *
 * This is like calling flush_chunk(), but it sends a chunk header with a type of
 * CHUNK_END (instead of CHUNK_DATA). Whatever is in the buffer is written out, but
 * the stream is can be used to send more chunks.
 * @note This is called by the chunked_outbuf destructor, so closing a stream using
 * chunked_outbuf always sends a CHUNK_END type chunk, even if it will have zero
 * bytes
 * @return EOF on error, otherwise the number of bytes sent in the chunk.
 */
int chunked_outbuf::end_chunk()
{
	cerr << "In chunked_outbuf::end_chunk" << endl;

	int32_t num = pptr() - pbase();	// int needs to be signed for the call to pbump

	// write out the chunk headers: CHUNKTYPE and CHUNKSIZE
	// as a 32-bit unsigned int. Here I assume that num is never
	// more than 2^24 because that was tested in the constructor

	uint32_t chunk_header = (uint32_t)num | CHUNK_END;

	// Write out the CHUNK_END header with the byte count.
	// This should be called infrequently, so it's probably not worth
	// optimizing away chunk_header
	d_os.write((const char *)&chunk_header, sizeof(uint32_t));

	// Should bad() throw an error?
	// Are these functions fast or would the bits be faster?
	d_os.write(d_buffer, num);
	if (d_os.eof() || d_os.bad())
		return EOF;

	pbump(-num);
	return num;
}

/**
 * @brief Virtual method called when the internal buffer would overflow.
 * When the internal buffer fills, this method is called by the byte that
 * would cause that overflow. The buffer pointers have been set so that
 * there is actually space for one more character, so \c c can really be
 * sent. Put \c c into the buffer and send it, prefixing the buffer
 * contents with a chunk header.
 * @note This method is called by the std::ostream code.
 * @param c The last character to add to the buffer before sending the
 * next chunk.
 * @return EOF on error, otherwise the value of \c c.
 */
int chunked_outbuf::overflow(int c)
{
	cerr << "In chunked_outbuf::overflow" << endl;

	if (c != EOF) {
		*pptr() = c;
		pbump(1);
	}
	// flush the buffer
	if (data_chunk() == EOF) {
		//Error
		return EOF;
	}

	return c;
}

/**
 * @brief Synchronize the stream with its data sink.
 * @note This method is called by ?
 * @return -1 on error, 0 otherwise.
 */
int chunked_outbuf::sync()
{
	cerr << "In chunked_outbuf::sync" << endl;

	if (data_chunk() == EOF) {
		// Error
		return -1;
	}
	return 0;
}

} // namespace libdap
