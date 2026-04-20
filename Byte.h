
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
    /// Stored scalar byte value.
    dods_byte d_buf;

public:
    Byte(const string &n);
    Byte(const string &n, const string &d);

    ~Byte() override {}

    /**
     * @brief Copy-constructs a byte variable from another instance.
     *
     * The new instance copies the source value and metadata.
     *
     * @param copy_from Source instance to copy.
     */
    Byte(const Byte &copy_from);

    /**
     * @brief Assigns from another byte variable.
     *
     * Replaces this instance's value and metadata with those of `rhs`.
     *
     * @param rhs Source instance.
     * @return This instance after assignment.
     */
    Byte &operator=(const Byte &rhs);

    unsigned int width(bool = false) const override { return sizeof(dods_byte); }

    /**
     * @brief Returns the storage width in bytes.
     *
     * @param constrained Ignored for scalar byte values.
     * @return Number of bytes used by the byte value.
     */
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

    /**
     * @brief Writes this value using C stdio output.
     *
     * @param out Output file stream.
     * @param space Indentation prefix.
     * @param print_decl_p True to include declaration text.
     * @param is_root_grp True when printing in the root group context.
     */
    void print_val(FILE *out, string space = "", bool print_decl_p = true, bool is_root_grp = true) override;
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

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _byte_h
