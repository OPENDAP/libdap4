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

using std::istream;
//using std::cout;

#include "Type.h"
#include "UnMarshaller.h"
#include "InternalErr.h"

namespace libdap
{

class Vector;

/** @brief Read data from the stream made by DAP4StreamMarshaller.
 */
class D4StreamUnMarshaller: public UnMarshaller {
public:
    const static unsigned int c_checksum_length = 4;

private:
    istream &d_in;
    bool d_twiddle_bytes;

    // These are used for reals that need to be converted from IEEE 754
    XDR d_source;
    char *d_buf;

    D4StreamUnMarshaller();
    D4StreamUnMarshaller(const D4StreamUnMarshaller &);
    D4StreamUnMarshaller & operator=(const D4StreamUnMarshaller &);

    void m_deserialize_reals(char *val, unsigned int num, int width, Type type);
    void m_twidle_vector_elements(char *vals, unsigned int num, int width);

public:
#if 0
    struct checksum {
        unsigned char md[c_checksum_length];
    };
#endif
    D4StreamUnMarshaller(istream &in, bool is_stream_bigendian);
    virtual ~D4StreamUnMarshaller();

#if 0
    checksum get_checksum();
    string get_checksum(checksum c);
#endif

    Crc32::checksum get_checksum();
    string get_checksum(Crc32::checksum c);

    virtual void get_byte(dods_byte &val);
    virtual void get_int8(dods_int8 &val);

    virtual void get_int16(dods_int16 &val);
    virtual void get_int32(dods_int32 &val);

    virtual void get_int64(dods_int64 &val);

    virtual void get_float32(dods_float32 &val);
    virtual void get_float64(dods_float64 &val);

    virtual void get_uint16(dods_uint16 &val);
    virtual void get_uint32(dods_uint32 &val);

    virtual void get_uint64(dods_uint64 &val);

    virtual void get_str(string &val);
    virtual void get_url(string &val);

    virtual void get_opaque(char *val, unsigned int len);
    virtual void get_opaque(char **val, unsigned int &len);

    virtual void get_int(int &) {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }

    virtual dods_uint64 get_length_prefix();

    // Note that DAP4 assumes clients know the size of arrays when they
    // read the data; it's the 'varying' get methods that read & return the
    // number of elements. These methods are here to appease the UnMarshaller
    // 'interface' code
    virtual void get_vector(char **, unsigned int &, Vector &) {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }

    virtual void get_vector(char **, unsigned int &, int, Vector & ) {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }

    virtual void get_vector(char *val, unsigned int num);
    virtual void get_vector(char *val, unsigned int num, int width, Type type);

    virtual void get_varying_vector(char **val, unsigned int &num);
    virtual void get_varying_vector(char **val, unsigned int &num, int width, Type type);

    virtual void dump(ostream &strm) const;
};

} // namespace libdap

#endif // I_DAP4StreamUnMarshaller_h

