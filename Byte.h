
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for Byte type.
//
// jhrg 9/7/94

#ifndef _byte_h
#define _byte_h 1

#ifndef _dods_datatypes_h
#include "dods-datatypes.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif

namespace libdap {

/** This class is used to hold eight bits of information.  No sign
    information is implied in its value.

    @brief Holds a single byte.
    @see BaseType
    */
class Byte : public BaseType {
protected:
    dods_byte d_buf;

public:
    Byte(const string &n);
    Byte(const string &n, const string &d);

    virtual ~Byte() {}

    Byte(const Byte &copy_from);

    Byte &operator=(const Byte &rhs);

    unsigned int width(bool = false) const override { return sizeof(dods_byte); }

    int64_t width_ll(bool = false) const override { return sizeof(dods_byte); }

    BaseType *ptr_duplicate() override;

    // DAP2
    bool serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval) override;
    bool deserialize(UnMarshaller &um, DDS *, bool) override;

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    unsigned int val2buf(void *val, bool reuse = false) override;
    unsigned int buf2val(void **val) override;

    virtual bool set_value(const dods_byte value);
    virtual dods_byte value() const;

    void print_val(FILE *out, string space = "", bool print_decl_p = true) override;
    void print_val(ostream &out, string space = "", bool print_decl_p = true) override;

    bool ops(BaseType *b, int op) override;
    bool d4_ops(BaseType *b, int op) override;
    std::vector<BaseType *> *transform_to_dap2(AttrTable *parent_attr_table) override;

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _byte_h
