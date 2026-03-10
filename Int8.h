
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

#ifndef _int8_h
#define _int8_h 1

#ifndef _dods_datatypes_h
#include "dods-datatypes.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

namespace libdap {

/** @brief Holds an 8-bit signed integer value. */

class Int8 : public BaseType {
    // This is used in BaseType *Vector::var(unsigned int i)
    unsigned int val2buf(void *val, bool) override {
        set_value(*reinterpret_cast<dods_int8 *>(val));
        return sizeof(dods_int8);
    }
    unsigned int buf2val(void **) override {
        throw InternalErr(__FILE__, __LINE__, "buf2val: Not implemented for Int8");
    }
    void print_val(FILE *, string, bool, bool) override {
        throw InternalErr(__FILE__, __LINE__, "print_val: Not implemented for Int8");
    }

protected:
    /// Stored scalar 8-bit signed integer value.
    dods_int8 d_buf;

public:
    Int8(const string &n);
    Int8(const string &n, const string &d);
    ~Int8() override {}

    /**
     * @brief Copy-constructs from another 8-bit signed integer variable.
     *
     * @param copy_from Source instance.
     */
    Int8(const Int8 &copy_from);

    /**
     * @brief Assigns from another 8-bit signed integer variable.
     *
     * @param rhs Source instance.
     * @return This instance after assignment.
     */
    Int8 &operator=(const Int8 &rhs);

    BaseType *ptr_duplicate() override;

    unsigned int width(bool = false) const override { return sizeof(dods_int8); }

    /**
     * @brief Returns the storage width in bytes.
     *
     * @param constrained Ignored for scalar values.
     * @return Number of bytes used by this value.
     */
    int64_t width_ll(bool constrained = false) const override { return sizeof(dods_int8); }

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    /**
     * @brief Returns the current value.
     *
     * @return Stored signed 8-bit value.
     */
    virtual dods_int8 value() const;
    /**
     * @brief Sets the current value.
     *
     * @param val New signed 8-bit value.
     * @return True when the value is accepted.
     */
    virtual bool set_value(dods_int8 val);

    /**
     * @brief Writes this value using C++ stream output.
     *
     * @param out Output stream.
     * @param space Indentation prefix.
     * @param print_decl_p True to include declaration text.
     * @param is_root_grp True when printing in the root group context.
     */
    void print_val(ostream &out, string space = "", bool print_decl_p = true, bool is_root_grp = true) override;

    bool ops(BaseType *b, int op) override;
    bool d4_ops(BaseType *b, int op) override;
    /**
     * @brief Converts this DAP4 value into equivalent DAP2 value objects.
     *
     * @param parent_attr_table Destination attribute table for converted metadata.
     * @param show_shared_dims True to include shared-dimension annotations.
     * @return A heap-allocated list of DAP2 values corresponding to this object.
     */
    std::vector<BaseType *> *transform_to_dap2(AttrTable *parent_attr_table, bool show_shared_dims = false) override;

    bool is_dap4_projected(std::vector<std::string> &inventory) override;

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _int16_h
