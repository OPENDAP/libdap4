// D4StreamUnMarshaller.h

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

#ifndef I_D4StreamUnMarshaller_h
#define I_D4StreamUnMarshaller_h 1

#include <iostream>

// See comment in D4StreamMarshaller
#define USE_XDR_FOR_IEEE754_ENCODING 0

#if USE_XDR_FOR_IEEE754_ENCODING
#ifdef WIN32
#include <rpc.h>
#include <winsock2.h>
#include <xdr.h>
#else
#include <netinet/in.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#endif
#endif

#include <crc.h>

// #include "Type.h"
#include "InternalErr.h"
#include "UnMarshaller.h"
#include "dods-datatypes.h"

#include "debug.h"
#include "util.h"

using std::istream;

namespace libdap {

class Vector;

/** @brief Read data from the stream made by D4StreamMarshaller.
 */
class D4StreamUnMarshaller : public UnMarshaller {
public:
    /** @brief Number of bytes used by a serialized checksum trailer. */
    const static unsigned int c_checksum_length = 4;

private:
    istream &d_in;
    bool d_twiddle_bytes;

#if USE_XDR_FOR_IEEE754_ENCODING
    // These are used for reals that need to be converted from IEEE 754
    XDR d_source;
    char d_buf[sizeof(dods_float64)];
#endif

    D4StreamUnMarshaller();
    D4StreamUnMarshaller(const D4StreamUnMarshaller &);
    D4StreamUnMarshaller &operator=(const D4StreamUnMarshaller &);
#if USE_XDR_FOR_IEEE754_ENCODING
    void m_deserialize_reals(char *val, int64_t num, int width, Type type);
#endif
    void m_twidle_vector_elements(char *vals, int64_t num, int width);

public:
    /**
     * @brief Builds a DAP4 stream unmarshaller.
     * @param in Source stream.
     * @param twiddle_bytes True when multi-byte values must be byte-swapped.
     */
    D4StreamUnMarshaller(istream &in, bool twiddle_bytes);
    D4StreamUnMarshaller(istream &in);
    ~D4StreamUnMarshaller() override;

    /**
     * @brief Enables or disables byte swapping for multi-byte values.
     * @param twiddle True to swap bytes.
     */
    void set_twiddle_bytes(bool twiddle) { d_twiddle_bytes = twiddle; }

    /**
     * @brief Is the data source we are reading from a big-endian machine?
     * We need this because the value of the CRC32 checksum is dependent on
     * byte order.
     *
     * @note This is somewhat tortured logic, but if this host is big-endian and
     * twiddle_bytes is not true, then the remote host must be big-endian. Similarly,
     * if this host is not big-endian and twiddle_bytes is true, then the remote
     * host must be big-endian
     */
    bool is_source_big_endian() const {
        return (is_host_big_endian() && !d_twiddle_bytes) || (!is_host_big_endian() && d_twiddle_bytes);
    }

    /**
     * @brief Reads and returns the checksum value from the stream.
     * @return Decoded checksum.
     */
    Crc32::checksum get_checksum();
    /**
     * @brief Reads and returns the checksum as hex text.
     * @return Checksum string.
     */
    string get_checksum_str();
    int64_t get_count();

    /** @copydoc UnMarshaller::get_byte */
    void get_byte(dods_byte &val) override;
    /** @brief Deserialize one DAP4 `Int8` value.
     * @param val Destination for the decoded value.
     */
    virtual void get_int8(dods_int8 &val);

    /** @copydoc UnMarshaller::get_int16 */
    void get_int16(dods_int16 &val) override;
    /** @copydoc UnMarshaller::get_int32 */
    void get_int32(dods_int32 &val) override;

    /** @brief Deserialize one DAP4 `Int64` value.
     * @param val Destination for the decoded value.
     */
    virtual void get_int64(dods_int64 &val);

    /** @copydoc UnMarshaller::get_float32 */
    void get_float32(dods_float32 &val) override;
    /** @copydoc UnMarshaller::get_float64 */
    void get_float64(dods_float64 &val) override;

    /** @copydoc UnMarshaller::get_uint16 */
    void get_uint16(dods_uint16 &val) override;
    /** @copydoc UnMarshaller::get_uint32 */
    void get_uint32(dods_uint32 &val) override;

    /** @brief Deserialize one DAP4 `UInt64` value.
     * @param val Destination for the decoded value.
     */
    virtual void get_uint64(dods_uint64 &val);

    /** @copydoc UnMarshaller::get_str */
    void get_str(string &val) override;
    /** @copydoc UnMarshaller::get_url */
    void get_url(string &val) override;

    void get_opaque(char *, unsigned int) override {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4, use get_opaque_dap4() instead.");
    }

    /**
     * @brief Deserialize DAP4 opaque bytes into a caller-owned buffer.
     * @param val Output pointer to allocated byte buffer.
     * @param len Number of decoded bytes.
     */
    virtual void get_opaque_dap4(char **val, int64_t &len);
    /**
     * @brief Deserialize DAP4 opaque bytes into a vector.
     * @param val Destination byte vector.
     */
    virtual void get_opaque_dap4(vector<uint8_t> &val);

    void get_int(int &) override { throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4"); }

    // Note that DAP4 assumes clients know the size of arrays when they
    // read the data; it's the 'varying' get methods that read & return the
    // number of elements. These methods are here to appease the UnMarshaller
    // 'interface' code
    void get_vector(char **, unsigned int &, Vector &) override {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }

    void get_vector(char **, unsigned int &, int, Vector &) override {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }

    /**
     * @brief Deserialize a fixed-size byte block.
     * @param val Destination buffer.
     * @param num_bytes Number of bytes to read.
     */
    virtual void get_vector(char *val, int64_t num_bytes);
    /**
     * @brief Deserialize fixed-size elements into a caller buffer.
     * @param val Destination buffer.
     * @param num_elem Number of elements to read.
     * @param elem_size Bytes per element.
     */
    virtual void get_vector(char *val, int64_t num_elem, int elem_size);
    /**
     * @brief Deserialize `Float32` elements.
     * @param val Destination buffer.
     * @param num_elem Number of elements to read.
     */
    virtual void get_vector_float32(char *val, int64_t num_elem);
    /**
     * @brief Deserialize `Float64` elements.
     * @param val Destination buffer.
     * @param num_elem Number of elements to read.
     */
    virtual void get_vector_float64(char *val, int64_t num_elem);

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // I_D4StreamUnMarshaller_h
