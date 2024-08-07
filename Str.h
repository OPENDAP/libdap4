
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for Str type.
//
// jhrg 9/7/94

#ifndef _str_h
#define _str_h 1

#include <string>

#include "BaseType.h"
#include "dods-limits.h"

namespace libdap {

// max_str_len should be large since we always send strings with length bytes
// as a prefix (so xdr_string will always know how much memory to malloc) but
// if deserialize gets confused and thinks a ctor (in particular) is a string
// xdr_string in turn will max_str_len if it cannot get a length byte. A long
// term solution is to fix libdap, but strings should not routinely be > 32k
// for the time being... jhrg 4/30/97

const unsigned int max_str_len = DODS_USHRT_MAX - 1;

/** @brief Holds character string data.

    @see BaseType
    @see Url
    */

class Str : public BaseType {
protected:
    string d_buf;

public:
    Str(const string &n);
    Str(const string &n, const string &d);

    virtual ~Str() {}

    Str(const Str &copy_from);

    Str &operator=(const Str &rhs);

    BaseType *ptr_duplicate() override;

    unsigned int width(bool = false) const override { return sizeof(string); }

    int64_t width_ll(bool = false) const override { return sizeof(string); }

    // Return the length of the stored string or zero if no string has been
    // stored in the instance's internal buffer.
    int length() const override;
    int64_t length_ll() const override;

    // DAP2
    bool serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval = true) override;
    bool deserialize(UnMarshaller &um, DDS *dds, bool reuse = false) override;

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    unsigned int val2buf(void *val, bool reuse = false) override;
    unsigned int buf2val(void **val) override;

    virtual bool set_value(const string &value);
    virtual string value() const;

    virtual string esc_string_variable_value(const string &s);

    void print_val(FILE *out, string space = "", bool print_decl_p = true) override;
    void print_val(ostream &out, string space = "", bool print_decl_p = true) override;

    bool ops(BaseType *b, int op) override;
    bool d4_ops(BaseType *b, int op) override;

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _str_h
