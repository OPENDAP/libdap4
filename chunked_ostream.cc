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

#include <string>
#include <streambuf>

#include <cstring>

//#define DODS_DEBUG

#include "chunked_stream.h"
#include "chunked_ostream.h"
#include "debug.h"

namespace libdap {

// flush the characters in the buffer
/**
 * @brief Write out the contents of the buffer as a chunk.
 *
 * @return EOF on error, otherwise the number of bytes in the chunk body.
 */
std::streambuf::int_type
chunked_outbuf::data_chunk()
{
	DBG(cerr << "In chunked_outbuf::data_chunk" << endl);

	int32_t num = pptr() - pbase();	// num needs to be signed for the call to pbump

	// Since this is called by sync() (e.g., flush()), return 0 and do nothing
	// when there's no data to send.
	if (num == 0)
		return 0;

	// here, write out the chunk headers: CHUNKTYPE and CHUNKSIZE
	// as a 32-bit unsigned int. Here I assume that num is never
	// more than 2^24 because that was tested in the constructor

	// Trick: This method always writes CHUNK_DATA type chunks so
	// the chunk type is always 0x00, and given that num never has
	// anything bigger than 24-bits, the high order byte is always
	// 0x00. Of course bit-wise OR with 0x00 isn't going to do
	// much anyway... Here's the general idea all the same:
	//
	// unsigned int chunk_header = (unsigned int)num | CHUNK_type;
	uint32_t header = num;
#if !BYTE_ORDER_PREFIX
	// Add encoding of host's byte order. jhrg 11/24/13
	if (!d_big_endian) header |= CHUNK_LITTLE_ENDIAN;
    // network byte order for the header
    htonl(header);
#endif

	d_os.write((const char *)&header, sizeof(int32_t));

	// Should bad() throw an error?
	// Are these functions fast or would the bits be faster?
	d_os.write(d_buffer, num);
	if (d_os.eof() || d_os.bad())
		return traits_type::eof();

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
std::streambuf::int_type
chunked_outbuf::end_chunk()
{
	DBG(cerr << "In chunked_outbuf::end_chunk" << endl);

	int32_t num = pptr() - pbase();	// num needs to be signed for the call to pbump

	// write out the chunk headers: CHUNKTYPE and CHUNKSIZE
	// as a 32-bit unsigned int. Here I assume that num is never
	// more than 2^24 because that was tested in the constructor

	uint32_t header = (uint32_t)num | CHUNK_END;

#if !BYTE_ORDER_PREFIX
    // Add encoding of host's byte order. jhrg 11/24/13
    if (!d_big_endian) header |= CHUNK_LITTLE_ENDIAN;
    // network byte order for the header
    htonl(header);
#endif

    // Write out the CHUNK_END header with the byte count.
	// This should be called infrequently, so it's probably not worth
	// optimizing away chunk_header
	d_os.write((const char *)&header, sizeof(uint32_t));

	// Should bad() throw an error?
	// Are these functions fast or would the bits be faster?
	d_os.write(d_buffer, num);
	if (d_os.eof() || d_os.bad())
		return traits_type::eof();

	pbump(-num);
	return num;
}

/**
 * @brief Send an error chunk
 * While building up the next chunk, send an error chunk, ignoring the data currently
 * write buffer. The buffer is left in a consistent state.
 * @param msg The error message to include in the error chunk
 * @return The number of characters ignored.
 */
std::streambuf::int_type
chunked_outbuf::err_chunk(const std::string &m)
{
	DBG(cerr << "In chunked_outbuf::err_chunk" << endl);
	std::string msg = m;

	// Figure out how many chars are in the buffer - these will be
	// ignored.
	int32_t num = pptr() - pbase();	// num needs to be signed for the call to pbump

	// write out the chunk headers: CHUNKTYPE and CHUNKSIZE
	// as a 32-bit unsigned int. Here I assume that num is never
	// more than 2^24 because that was tested in the constructor
	if (msg.length() > 0x00FFFFFF)
		msg = "Error message too long";

	uint32_t header = (uint32_t)msg.length() | CHUNK_ERR;

#if !BYTE_ORDER_PREFIX
    // Add encoding of host's byte order. jhrg 11/24/13
    if (!d_big_endian) header |= CHUNK_LITTLE_ENDIAN;
    // network byte order for the header
    htonl(header);
#endif

    // Write out the CHUNK_END header with the byte count.
	// This should be called infrequently, so it's probably not worth
	// optimizing away chunk_header
	d_os.write((const char *)&header, sizeof(uint32_t));

	// Should bad() throw an error?
	// Are these functions fast or would the bits be faster?
	d_os.write(msg.data(), msg.length());
	if (d_os.eof() || d_os.bad())
		return traits_type::eof();

	// Reset the buffer pointer, effectively ignoring what's in there now
	pbump(-num);

	// return the number of characters ignored
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
std::streambuf::int_type
chunked_outbuf::overflow(int c)
{
	DBG(cerr << "In chunked_outbuf::overflow" << endl);

	// Note that the buffer and eptr() were set so that when pptr() is
	// at the end of the buffer, there is actually one more character
	// available in the buffer.
	if (!traits_type::eq_int_type(c, traits_type::eof())) {
		*pptr() = traits_type::not_eof(c);
		pbump(1);
	}
	// flush the buffer
	if (data_chunk() == traits_type::eof()) {
		//Error
		return traits_type::eof();
	}

	return traits_type::not_eof(c);
}

/*

  d_buffer
  |
  v
  |--------------------------------------------|....
  |                                            |   .
  |--------------------------------------------|....
  ^                         ^                   ^
  |                         |                   |
  pbase()                   pptr()              epptr()

 */

/**
 * @brief Write bytes to the chunked stream
 * Write the bytes in \c s to the chunked stream
 * @param s
 * @param num
 * @return The number of bytes written
 */
std::streamsize
chunked_outbuf::xsputn(const char *s, std::streamsize num)
{
	DBG(cerr << "In chunked_outbuf::xsputn: num: " << num << endl);

	// if the current block of data will fit in the buffer, put it there.
	// else, there is at least a complete chunk between what's in the buffer
	// and what's in 's'; send a chunk header, the stuff in the buffer and
	// bytes from 's' to make a complete chunk. Then iterate over 's' sending
	// more chunks until there's less than a complete chunk left in 's'. Put
	// the bytes remaining 's' in the buffer. Return the number of bytes sent
	// or 0 if an error is encountered.

	int32_t bytes_in_buffer = pptr() - pbase();	// num needs to be signed for the call to pbump

	// Will num bytes fit in the buffer? The location of epptr() is one back from
	// the actual end of the buffer, so the next char written will trigger a write
	// of the buffer as a new data chunk.
	if (bytes_in_buffer + num < d_buf_size) {
		DBG2(cerr << ":xsputn: buffering num: " << num << endl);
		memcpy(pptr(), s, num);
		pbump(num);
		return traits_type::not_eof(num);
	}

	// If here, write a chunk header and a chunk's worth of data by combining the
	// data in the buffer and some data from 's'.
	uint32_t header = d_buf_size;
#if !BYTE_ORDER_PREFIX
    // Add encoding of host's byte order. jhrg 11/24/13
    if (!d_big_endian) header |= CHUNK_LITTLE_ENDIAN;
    // network byte order for the header
    htonl(header);
#endif
	d_os.write((const char *)&header, sizeof(int32_t));	// Data chunk's CHUNK_TYPE is 0x00000000

	// Reset the pptr() and epptr() now in case of an error exit. See the 'if'
	// at teh end of this for the only code from here down that will modify the
	// pptr() value.
	setp(d_buffer, d_buffer + (d_buf_size - 1));

	d_os.write(d_buffer, bytes_in_buffer);
	if (d_os.eof() || d_os.bad())
		return traits_type::not_eof(0);

	int bytes_to_fill_out_buffer =  d_buf_size - bytes_in_buffer;
	d_os.write(s, bytes_to_fill_out_buffer);
	if (d_os.eof() || d_os.bad())
		return traits_type::not_eof(0);
	s += bytes_to_fill_out_buffer;
	uint32_t bytes_still_to_send = num - bytes_to_fill_out_buffer;

	// Now send all the remaining data in s until the amount remaining doesn't
	// fill a complete chunk and buffer those data.
	while (bytes_still_to_send >= d_buf_size) {
		// This is header for  a chunk of d_buf_size bytes; the size was set above
		d_os.write((const char *) &header, sizeof(int32_t));
		d_os.write(s, d_buf_size);
		if (d_os.eof() || d_os.bad()) return traits_type::not_eof(0);
		s += d_buf_size;
		bytes_still_to_send -= d_buf_size;
	}

	if (bytes_still_to_send > 0) {
		// if the code is here, one or more chunks have been sent, the
		// buffer is empty and there are < d_buf_size bytes to send. Buffer
		// them.
		memcpy(d_buffer, s, bytes_still_to_send);
		pbump(bytes_still_to_send);
	}

	// Unless an error was detected while writing to the stream, the code must
	// have sent num bytes.
	return traits_type::not_eof(num);
}

/**
 * @brief Synchronize the stream with its data sink.
 * @note This method is called by flush() among others
 * @return -1 on error, 0 otherwise.
 */
std::streambuf::int_type
chunked_outbuf::sync()
{
	DBG(cerr << "In chunked_outbuf::sync" << endl);

	if (data_chunk() == traits_type::eof()) {
		// Error
		return traits_type::not_eof(-1);
	}
	return traits_type::not_eof(0);
}

} // namespace libdap
