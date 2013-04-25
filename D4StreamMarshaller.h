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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef I_D4StreamMarshaller_h
#define I_D4StreamMarshaller_h 1

#include <iostream>

#ifdef WIN32
#include <rpc.h>
#include <winsock2.h>
#include <xdr.h>
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include <crc.h>

using std::ostream;
//using std::cout;

#include "Type.h"
#include "Marshaller.h"
#include "InternalErr.h"

namespace libdap {

class Vector;

/** @brief Marshaller that knows how to marshal/serialize dap data objects
 * to a C++ iostream using DAP4's receiver-makes-right scheme. This code
 * adds checksums to the stream and uses the xdr library to encode real
 * values if the underlying representation is not IEEE 754. It also supports
 * computing the checksum only.
 *
 * @note This class uses the Marshaller interface; it could be rewritten
 * to use far fewer methods since all of the put_*() methods take different
 * types.
 */
class D4StreamMarshaller: public Marshaller {

private:
    XDR d_scalar_sink;
    char *d_ieee754_buf; // used to serialize a float or double

    ostream &d_out;
    bool d_write_data; // jhrg 1/27/12

    Crc32 d_checksum;

    // These are private so they won't ever get used.

    D4StreamMarshaller();
    D4StreamMarshaller(const D4StreamMarshaller &);
    D4StreamMarshaller & operator=(const D4StreamMarshaller &);

    void m_serialize_reals(char *val, unsigned int num, int width, Type type);

public:
    D4StreamMarshaller(ostream &out, bool write_data = true);
    virtual ~D4StreamMarshaller();

    virtual string get_endian() const;

    virtual void reset_checksum();
    virtual string get_checksum();
    virtual void checksum_update(const void *data, unsigned long len);
    void put_checksum();

    virtual void put_byte(dods_byte val);
    virtual void put_int8(dods_int8 val);

    virtual void put_int16(dods_int16 val);
    virtual void put_int32(dods_int32 val);
    // Added
    virtual void put_int64(dods_int64 val);

    virtual void put_float32(dods_float32 val);
    virtual void put_float64(dods_float64 val);

    virtual void put_uint16(dods_uint16 val);
    virtual void put_uint32(dods_uint32 val);
    // Added
    virtual void put_uint64(dods_uint64 val);

    virtual void put_str(const string &val);
    virtual void put_url(const string &val);

    virtual void put_opaque(char *val, unsigned int len);

    // Never use put_int() to send length information in DAP4, use
    // put_length_prefix() instead.
    virtual void put_int(int) {
        throw InternalErr(__FILE__, __LINE__, "Not Implemented; use put_length_prefix.");
    }

    // Added; This method does not add its argument to the checksum;
    // put_uint64() does.
    virtual void put_length_prefix(dods_uint64 val);

    virtual void put_vector(char *val, unsigned int num);
    virtual void put_vector(char *val, unsigned int num, int width, Type type);

    virtual void put_varying_vector(char *val, unsigned int num);
    virtual void put_varying_vector(char *val, unsigned int num, int width, Type type);

    virtual void put_vector(char *, int , Vector &) {
        throw InternalErr(__FILE__, __LINE__, "Not Implemented; use put_length_prefix.");
    }
    virtual void put_vector(char *, int , int , Vector &) {
        throw InternalErr(__FILE__, __LINE__, "Not Implemented; use put_length_prefix.");
    }


    virtual void dump(ostream &strm) const;
};

} // namespace libdap

#endif // I_D4StreamMarshaller_h
