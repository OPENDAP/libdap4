// D4StreamMarshaller.h

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2012 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>,
//         James Gallagher <jgallagher@opendap.org>
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef I_D4StreamMarshaller_h
#define I_D4StreamMarshaller_h 1

// By default, only support platforms that use IEEE754 for floating point values.
// Hacked up code leftover from an older version of the class; largely untested.
// jhrg 10/3/13
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

#include "crc.h"

#include "InternalErr.h"
#include "Marshaller.h"

namespace libdap {

class Vector;
class MarshallerThread;

/** @brief Marshaller that knows how to marshal/serialize dap data objects
 * to a C++ iostream using DAP4's receiver-makes-right scheme. This code
 * adds checksums to the stream and uses the xdr library to encode real
 * values if the underlying representation is not IEEE 754. It also supports
 * computing the checksum only.
 *
 * @note This class uses the Marshaller interface; it could be rewritten
 * to use far fewer methods since all the put_*() methods take different
 * types.
 */
class D4StreamMarshaller : public Marshaller {

#if USE_XDR_FOR_IEEE754_ENCODING
    XDR d_scalar_sink;
    char d_ieee754_buf[sizeof(dods_float64)]; // used to serialize a float or double
#endif

    ostream &d_out;
    bool d_write_data = true;        // jhrg 1/27/12
    bool d_compute_checksum = false; // ndp 08/03/25

    Crc32 d_checksum;

    MarshallerThread *tm = nullptr;

#if USE_XDR_FOR_IEEE754_ENCODING
    void m_serialize_reals(char *val, int64_t num, int width, Type type);
#endif

public:
    explicit D4StreamMarshaller(std::ostream &out, bool write_data = true, bool compute_checksums = false);

    D4StreamMarshaller() = delete;
    D4StreamMarshaller(const D4StreamMarshaller &) = delete;
    D4StreamMarshaller &operator=(const D4StreamMarshaller &) = delete;
    D4StreamMarshaller(const D4StreamMarshaller &&) = delete;
    D4StreamMarshaller &operator=(const D4StreamMarshaller &&) = delete;

    ~D4StreamMarshaller() override;

    virtual void reset_checksum();
    virtual string get_checksum();
    virtual void checksum_update(const void *data, unsigned long len);

    virtual void put_checksum();
    virtual void put_count(int64_t count);

    void put_byte(dods_byte val) override;
    virtual void put_int8(dods_int8 val);

    void put_int16(dods_int16 val) override;
    void put_int32(dods_int32 val) override;
    // Added
    virtual void put_int64(dods_int64 val);

    void put_float32(dods_float32 val) override;
    void put_float64(dods_float64 val) override;

    void put_uint16(dods_uint16 val) override;
    void put_uint32(dods_uint32 val) override;
    // Added
    virtual void put_uint64(dods_uint64 val);

    void put_str(const string &val) override;
    void put_url(const string &val) override;

    /// @brief Not supported by DAP4
    void put_opaque(char *, unsigned int) override {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4; use put_opaque_dap4() instead.");
    }

    virtual void put_opaque_dap4(const char *val, int64_t num_bytes);

    /// @brief Not supported by DAP4
    void put_int(int) override { throw InternalErr(__FILE__, __LINE__, "Not Implemented; use put_length_prefix."); }

    virtual void put_vector(char *val, int64_t num_bytes);
    virtual void put_vector(char *val, int64_t num_elem, int elem_size);
    virtual void put_vector_float32(char *val, int64_t num_elem);
    virtual void put_vector_float64(char *val, int64_t num_elem);

    /// @brief Not supported by DAP4
    void put_vector(char *, int, Vector &) override {
        throw InternalErr(__FILE__, __LINE__, "Not Implemented; use other put_vector() versions.");
    }
    /// @brief Not supported by DAP4
    void put_vector(char *, int, int, Vector &) override {
        throw InternalErr(__FILE__, __LINE__, "Not Implemented; use other put_vector() versions.");
    }

    /// @brief Does nothing in DAP4
    void put_vector_start(int /*num*/) override {}

    void put_vector_part(char * /*val*/, unsigned int /*num*/, int /*width*/, Type /*type*/) override;

    /// @brief Does nothing in DAP4
    void put_vector_end() override {}

    void dump(std::ostream &strm) const override;
};

} // namespace libdap

#endif // I_D4StreamMarshaller_h
