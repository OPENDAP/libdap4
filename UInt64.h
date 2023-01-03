
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

#ifndef _uint64_h
#define _uint64_h 1


#ifndef _dods_datatypes_h
#include "dods-datatypes.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif

namespace libdap
{

/** @brief Holds a 64-bit unsigned integer.

    @see BaseType */

class UInt64: public BaseType
{
	unsigned int val2buf(void *val, bool)  override {
    	set_value(*reinterpret_cast<dods_uint64*>(val));
    	return sizeof(dods_uint64);
    }
    //virtual unsigned int buf2val(void **)  { throw InternalErr(__FILE__, __LINE__, "Not implemented for UInt64"); }
    unsigned int buf2val(void **) override;
    void print_val(FILE *, string, bool)  override { throw InternalErr(__FILE__, __LINE__, "Not implemented for UInt64"); }

protected:
    dods_uint64 d_buf;

public:
    UInt64(const string &n);
    UInt64(const string &n, const string &d);
    virtual ~UInt64()
    {}

    UInt64(const UInt64 &copy_from);

    UInt64 &operator=(const UInt64 &rhs);

    BaseType *ptr_duplicate() override;

    unsigned int width(bool = false) const override
    {
        return sizeof(dods_uint64);
    }

    int64_t width_ll(bool = false) const override
    {
        return sizeof(dods_uint64);
    }

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    virtual dods_uint64 value() const;
    virtual bool set_value(dods_uint64 val);

    void print_val(ostream &out, string space = "",  bool print_decl_p = true) override;

    bool ops(BaseType *b, int op) override;
    bool d4_ops(BaseType *b, int op) override;
    std::vector<BaseType *> *transform_to_dap2(AttrTable *parent_attr_table) override;

    bool is_dap4_projected(std::vector<string> &inventory) override;

    void dump(ostream &strm) const  override;
};

} // namespace libdap

#endif // _uint64_h

