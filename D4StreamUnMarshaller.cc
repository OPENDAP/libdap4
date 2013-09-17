// D4StreamUnMarshaller.cc

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2012 OPeNDAP, Inc.
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

#include "config.h"
#include "D4StreamUnMarshaller.h"

#include <byteswap.h>

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>

//#define DODS_DEBUG2 1
//#define DODS_DEBUG 1

#include "util.h"
#include "XDRUtils.h"
#include "InternalErr.h"
#include "debug.h"

namespace libdap {

/**
 * @brief Build a DAP4 Stream unMarshaller.
 *
 * Build a DAP4 Stream UnMarshaller initialed to read from am istream object.
 * Figure out if the words read for values need to be 'twiddled' based on the
 * byte-order of the stream an this host (see set_twiddle_bytes()).
 *
 * @param in Read from this input stream
 * @param is_stream_bigendian The byte order of the data in the stream
 */
D4StreamUnMarshaller::D4StreamUnMarshaller(istream &in, bool twiddle_bytes) : d_in( in ), d_twiddle_bytes(twiddle_bytes)
{
    // XDR is used to handle transforming non-ieee754 reals, nothing else.
    xdrmem_create(&d_source, d_buf, sizeof(dods_float64), XDR_DECODE);

    // This will cause exceptions to be thrown on i/o errors. The exception
    // will be ostream::failure
    d_in.exceptions(istream::failbit | istream::badbit);

    DBG(cerr << "Host is big endian: " << is_host_big_endian() << endl);
#if 0
    set_twiddle_bytes(twiddle_bytes);
#endif
}

/**
 * When using this constructor, set_twiddle_bytes() should be called
 * before data are processed.
 *
 * @param in
 */
D4StreamUnMarshaller::D4StreamUnMarshaller(istream &in) : d_in( in ), d_twiddle_bytes(false)
{
    // XDR is used to handle transforming non-ieee754 reals, nothing else.
    xdrmem_create(&d_source, d_buf, sizeof(dods_float64), XDR_DECODE);

    // This will cause exceptions to be thrown on i/o errors. The exception
    // will be ostream::failure
    d_in.exceptions(istream::failbit | istream::badbit);
}

D4StreamUnMarshaller::~D4StreamUnMarshaller( )
{
    xdr_destroy(&d_source);
}

#if 0
void
D4StreamUnMarshaller::set_twiddle_bytes(bool twiddle)
{
#if 0
    if ((is_host_big_endian() && is_stream_bigendian)
        || (!is_host_big_endian() && !is_stream_bigendian))
        d_twiddle_bytes = false;
    else
        d_twiddle_bytes = true;
#endif
    d_twiddle_bytes = twiddle
}
#endif

Crc32::checksum D4StreamUnMarshaller::get_checksum()
{
    Crc32::checksum c;
    d_in.read(reinterpret_cast<char*>(&c), sizeof(Crc32::checksum));

    return c;
}

string D4StreamUnMarshaller::get_checksum_str()
{
    ostringstream oss;
    oss.setf(ios::hex, ios::basefield);
    oss << setfill('0') << setw(8) << get_checksum();

    return oss.str();
}

void
D4StreamUnMarshaller::get_byte( dods_byte &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_byte));
}

void
D4StreamUnMarshaller::get_int8( dods_int8 &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_int8));
}

void
D4StreamUnMarshaller::get_int16( dods_int16 &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_int16));
    if (d_twiddle_bytes)
        val = bswap_16(val);
}

void
D4StreamUnMarshaller::get_int32( dods_int32 &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_int32));
    if (d_twiddle_bytes)
        val = bswap_32(val);
}

void
D4StreamUnMarshaller::get_int64( dods_int64 &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_int64));
    if (d_twiddle_bytes)
        val = bswap_64(val);
}

void
D4StreamUnMarshaller::get_float32( dods_float32 &val )
{
    if (std::numeric_limits<float>::is_iec559) {
        d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_float32));
        if (d_twiddle_bytes) {
            dods_int32 *i = reinterpret_cast<dods_int32*>(&val);
            *i = bswap_32(*i);
        }

    }
    else {
        xdr_setpos( &d_source, 0);
        d_in.read(d_buf, sizeof(dods_float32));

        if (!xdr_float(&d_source, &val))
            throw Error("Network I/O Error. Could not read float 64 data.");
    }
}

void
D4StreamUnMarshaller::get_float64( dods_float64 &val )
{
    if (std::numeric_limits<float>::is_iec559) {
        d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_float64));
        if (d_twiddle_bytes) {
            dods_int64 *i = reinterpret_cast<dods_int64*>(&val);
            *i = bswap_32(*i);
        }
    }
    else {
        xdr_setpos( &d_source, 0);
        d_in.read(d_buf, sizeof(dods_float64));

        if (!xdr_double(&d_source, &val))
            throw Error("Network I/O Error. Could not read float 64 data.");
    }
}

void
D4StreamUnMarshaller::get_uint16( dods_uint16 &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_uint16));
    if (d_twiddle_bytes)
        val = bswap_16(val);
}

void
D4StreamUnMarshaller::get_uint32( dods_uint32 &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_uint32));
    if (d_twiddle_bytes)
        val = bswap_32(val);
}

void
D4StreamUnMarshaller::get_uint64( dods_uint64 &val )
{
    d_in.read(reinterpret_cast<char*>(&val), sizeof(dods_uint64));
    if (d_twiddle_bytes)
        val = bswap_64(val);
}

/**
 * Read a varint (128-bit varying integer). Not the most optimized version
 * possible. It would be better if the values were in memory and we could
 * operate on them without a separate read for each byte.
 */
dods_uint64 D4StreamUnMarshaller::get_length_prefix()
{
    uint8_t b;
    int count = 0;
    dods_uint64 result = 0;
    do {
        d_in.read(reinterpret_cast<char*>(&b), 1);

        uint64_t v = (b & 0x7f) << 7 * count;
        result += v; // is v needed? Does it matter?
        count++;

    } while (b & 0x80);

    return result;
}


void
D4StreamUnMarshaller::get_str( string &val )
{
    dods_int64 len = get_length_prefix();
    vector<char> raw(len+1);
    d_in.read(&raw[0], len);

    val.reserve(len);
    val.assign(&raw[0], len);
}

void
D4StreamUnMarshaller::get_url( string &val )
{
    get_str( val ) ;
}

/**
 * Get opaque data when the size of the data to read is known.
 *
 * @param val Pointer to 'len' bytes; store the data here.
 * @param len Number of bytes referenced adn expected
 * @exception Error if the number of bytes indicated in the stream does not
 * match 'len'
 */
void
D4StreamUnMarshaller::get_opaque( char *val, unsigned int len )
{
    dods_int64 rlen = get_length_prefix();
    if (len != rlen)
        throw Error("Expected opaque data of " + long_to_string(len)
                + " bytes, but got " + long_to_string(rlen) + " instead.");

    d_in.read(val, len);
}

/**
 * Get opaque data when the size of the data to be read is not known in
 * advance.
 *
 * @param val Value-result parameter for the data; caller must delete.
 * @param len value-result parameter for the length of the data
 */
void
D4StreamUnMarshaller::get_opaque( char **val, unsigned int &len )
{
    len = get_length_prefix();

    *val = new char[len];
    d_in.read(*val, len);
}

void
D4StreamUnMarshaller::get_vector( char *val, unsigned int num )
{
    d_in.read(val, num);
}

/**
 * @todo recode this so that it does not copy data to a new buffer but
 * serializes directly to the stream (element by element) and compare the
 * run times.
 */
void D4StreamUnMarshaller::m_deserialize_reals(char *val, unsigned int num, int width, Type type)
{
    dods_uint64 size = num * width;
    // char *buf = (char*)malloc(size); jhrg 7/23/13
    vector<char> buf(size);
    XDR xdr;
    xdrmem_create(&xdr, &buf[0], size, XDR_DECODE);
    try {
        xdr_setpos(&d_source, 0);
        d_in.read(&buf[0], size);

        if(!xdr_array(&xdr, &val, (unsigned int *)&num, size, width, XDRUtils::xdr_coder(type)))
            throw InternalErr(__FILE__, __LINE__, "Error deserializing a Float64 array");

        if (xdr_getpos(&xdr) != size)
            throw InternalErr(__FILE__, __LINE__, "Error deserializing a Float64 array");
    }
    catch (...) {
        xdr_destroy(&xdr);
        throw;
    }
    xdr_destroy(&xdr);
}

void D4StreamUnMarshaller::m_twidle_vector_elements(char *vals, unsigned int num, int width)
{
    switch (width) {
        case 2: {
            dods_int16 *local = reinterpret_cast<dods_int16*>(vals);
            while (num--) {
                *local = bswap_16(*local);
                local++;
            }
            break;
        }
        case 4: {
            dods_int32 *local = reinterpret_cast<dods_int32*>(vals);;
            while (num--) {
                *local = bswap_32(*local);
                local++;
            }
            break;
        }
        case 8: {
            dods_int64 *local = reinterpret_cast<dods_int64*>(vals);;
            while (num--) {
                *local = bswap_64(*local);
                local++;
            }
            break;
        }
        default:
            throw InternalErr(__FILE__, __LINE__, "Unrecognized word size.");
    }
}

void
D4StreamUnMarshaller::get_vector( char *val, unsigned int num, int width, Type type )
{
    if (type == dods_float32_c && !std::numeric_limits<float>::is_iec559) {
        // If not using IEEE 754, use XDR to get it that way.
        m_deserialize_reals(val, num, 4, type);
    }
    else if (type == dods_float64_c && !std::numeric_limits<double>::is_iec559) {
        m_deserialize_reals(val, num, 8, type);
    }
    else {
        d_in.read(val, num * width);
        if (d_twiddle_bytes)
            m_twidle_vector_elements(val, num, width);
    }
}

void
D4StreamUnMarshaller::get_varying_vector( char **val, unsigned int &num )
{
    get_opaque(val, num);
}

void
D4StreamUnMarshaller::get_varying_vector( char **val, unsigned int &num, int width, Type type )
{
    num = get_length_prefix();

    int size = num * width;
    *val = new char[size];

    if (type == dods_float32_c && !std::numeric_limits<float>::is_iec559) {
        m_deserialize_reals(*val, num, 4, type);
    }
    else if (type == dods_float64_c && !std::numeric_limits<double>::is_iec559) {
        m_deserialize_reals(*val, num, 8, type);
    }
    else {
        d_in.read(*val, size);
        if (d_twiddle_bytes)
            m_twidle_vector_elements(*val, num, width);
    }
}

void
D4StreamUnMarshaller::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "D4StreamUnMarshaller::dump - ("
         << (void *)this << ")" << endl ;
}

} // namespace libdap

