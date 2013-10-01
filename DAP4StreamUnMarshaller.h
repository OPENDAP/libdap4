// DAP4StreamUnMarshaller.h

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef I_DAP4StreamUnMarshaller_h
#define I_DAP4StreamUnMarshaller_h 1

#include <iostream>

using std::istream ;
using std::cin ;

#include "UnMarshaller.h"
#include "BaseType.h"
#include "XDRUtils.h"

namespace libdap
{

class Vector;

/** @brief Read data from the stream made by DAP4StreamMarshaller.
 */
class DAP4StreamUnMarshaller: public UnMarshaller {
public:
    const static unsigned int c_md5_length = 16;

private:
    istream &d_in;
    bool d_twiddle_bytes;

    // These are used for reals that need to be converted from IEEE 754
    XDR d_source;
    dods_float64 d_buf;

    DAP4StreamUnMarshaller();
#if 0
    : d_in(cin) {
        throw InternalErr( __FILE__, __LINE__, "not implemented." ) ;
    }
#endif
    DAP4StreamUnMarshaller(const DAP4StreamUnMarshaller &);
#if 0
    : UnMarshaller(), d_in(cin) {
        throw InternalErr( __FILE__, __LINE__, "not implemented." ) ;
    }
#endif

    DAP4StreamUnMarshaller & operator=(const DAP4StreamUnMarshaller &);
#if 0
    {
        throw InternalErr( __FILE__, __LINE__, "not implemented." ) ;
    }
#endif

    void m_deserialize_reals(char *val, unsigned int num, int width, Type type);
    void m_twidle_vector_elements(char *vals, unsigned int num, int width);

public:
    struct checksum {
        unsigned char md[c_md5_length];
    };

    DAP4StreamUnMarshaller(istream &in, bool is_stream_bigendian);
    virtual ~DAP4StreamUnMarshaller();

    checksum get_checksum();
    string get_checksum(checksum c);

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

