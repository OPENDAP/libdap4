// XDRStreamMarshaller.h

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

// Hacked massively to add all kinds of things, including pthred support.
// jhrg 8/21/15

#ifndef I_XDRStreamMarshaller_h
#define I_XDRStreamMarshaller_h 1

#include <iostream>

#include "Marshaller.h"
#include "XDRUtils.h"

namespace libdap {

class BaseType;
class MarshallerThread;

/**
 * @brief Marshaller that knows how serialize dap data objects to a C++ iostream using XDR
 *
 */
class XDRStreamMarshaller : public Marshaller {
private:
    static char *d_buf;
    XDR d_sink;
    ostream &d_out;

    int d_partial_put_byte_count;

    MarshallerThread *tm;

    XDRStreamMarshaller();
    XDRStreamMarshaller(const XDRStreamMarshaller &m);
    XDRStreamMarshaller &operator=(const XDRStreamMarshaller &);

    void put_vector(char *val, unsigned int num, int width, Type type);

    friend class MarshallerTest;

public:
    XDRStreamMarshaller(ostream &out); //, bool checksum = false, bool write_data = true) ;
    virtual ~XDRStreamMarshaller();

    void put_byte(dods_byte val) override;

    void put_int16(dods_int16 val) override;
    void put_int32(dods_int32 val) override;

    void put_float32(dods_float32 val) override;
    void put_float64(dods_float64 val) override;

    void put_uint16(dods_uint16 val) override;
    void put_uint32(dods_uint32 val) override;

    void put_str(const string &val) override;
    void put_url(const string &val) override;

    void put_opaque(char *val, unsigned int len) override;
    void put_int(int val) override;

    void put_vector(char *val, int num, Vector &vec) override;
    void put_vector(char *val, int num, int width, Vector &vec) override;

    void put_vector_start(int num) override;
    void put_vector_part(char *val, unsigned int num, int width, Type type) override;
    void put_vector_end() override;

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // I_XDRStreamMarshaller_h
