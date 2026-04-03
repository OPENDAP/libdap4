
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

#ifndef _int64_h
#define _int64_h 1

#ifndef _dods_datatypes_h
#include "dods-datatypes.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

namespace libdap {

class ConstraintEvaluator;
class Marshaller;
class UnMarshaller;

/** @brief Holds a64-bit signed integer.

    @see BaseType
    */

class Int64 : public BaseType {
    unsigned int val2buf(void *val, bool) override {
        set_value(*reinterpret_cast<dods_int64 *>(val));
        return sizeof(dods_int64);
    }
    // virtual unsigned int buf2val(void **) { throw InternalErr(__FILE__, __LINE__, "Not implemented for Int64"); }
    unsigned int buf2val(void **) override;
    void print_val(FILE *, string, bool, bool) override {
        throw InternalErr(__FILE__, __LINE__, "Not implemented for Int64");
    }

protected:
    /// Stored scalar 64-bit signed integer value.
    dods_int64 d_buf;

public:
    Int64(const string &n);
    Int64(const string &n, const string &d);

    /**
     * @brief Copy-constructs from another 64-bit signed integer variable.
     *
     * @param copy_from Source instance.
     */
    Int64(const Int64 &copy_from);

    /**
     * @brief Assigns from another 64-bit signed integer variable.
     *
     * @param rhs Source instance.
     * @return This instance after assignment.
     */
    Int64 &operator=(const Int64 &rhs);

    ~Int64() override;

    BaseType *ptr_duplicate() override;

    unsigned int width(bool = false) const override { return sizeof(dods_int64); }

    /**
     * @brief Returns the storage width in bytes.
     *
     * @param constrained Ignored for scalar values.
     * @return Number of bytes used by this value.
     */
    int64_t width_ll(bool = false) const override { return sizeof(dods_int64); }

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    /**
     * @brief Sets the current value.
     *
     * @param i New signed 64-bit value.
     * @return True when the value is accepted.
     */
    virtual bool set_value(dods_int64 i);
    /**
     * @brief Returns the current value.
     *
     * @return Stored signed 64-bit value.
     */
    virtual dods_int64 value() const;

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
    std::vector<BaseType *> *transform_to_dap2(AttrTable *parent_attr_table, bool show_shared_dims = false) override;

    bool is_dap4_projected(std::vector<string> &inventory) override;

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _int64_h
