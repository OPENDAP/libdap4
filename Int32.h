
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

// Interface for Int32 type.
//
// jhrg 9/7/94

#ifndef _int32_h
#define _int32_h 1

#ifndef _dods_datatypes_h
#include "dods-datatypes.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

#if 0
#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif
#endif

namespace libdap {

class DMR;
class ConstraintEvaluator;

/** @brief Holds a 32-bit signed integer.

    @see BaseType
    */

class Int32 : public BaseType {
protected:
    /// Stored scalar 32-bit signed integer value.
    dods_int32 d_buf;

public:
    Int32(const string &n);
    Int32(const string &n, const string &d);

    /**
     * @brief Copy-constructs from another 32-bit signed integer variable.
     *
     * @param copy_from Source instance.
     */
    Int32(const Int32 &copy_from);

    /**
     * @brief Assigns from another 32-bit signed integer variable.
     *
     * @param rhs Source instance.
     * @return This instance after assignment.
     */
    Int32 &operator=(const Int32 &rhs);

    ~Int32() override;

    BaseType *ptr_duplicate() override;

    unsigned int width(bool = false) const override { return sizeof(dods_int32); }

    /**
     * @brief Returns the storage width in bytes.
     *
     * @param constrained Ignored for scalar values.
     * @return Number of bytes used by this value.
     */
    int64_t width_ll(bool = false) const override { return sizeof(dods_int32); }

    // DAP2
    bool serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval = true) override;
    bool deserialize(UnMarshaller &um, DDS *dds, bool reuse = false) override;

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    unsigned int val2buf(void *val, bool reuse = false) override;
    unsigned int buf2val(void **val) override;

    /**
     * @brief Sets the current value.
     *
     * @param i New signed 32-bit value.
     * @return True when the value is accepted.
     */
    virtual bool set_value(dods_int32 i);
    /**
     * @brief Returns the current value.
     *
     * @return Stored signed 32-bit value.
     */
    virtual dods_int32 value() const;

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

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _int32_h
