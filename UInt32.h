
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

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for the UInt32 (unsigned int 32) type.
//
// jhrg 9/7/94

#ifndef _uint32_h
#define _uint32_h 1

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

/** @brief Holds a 32-bit unsigned integer.

    @see BaseType */

class UInt32 : public BaseType {
protected:
    /// Stored scalar 32-bit unsigned integer value.
    dods_uint32 d_buf;

public:
    UInt32(const string &n);
    UInt32(const string &n, const string &d);
    ~UInt32() override {}

    /**
     * @brief Copy-constructs from another 32-bit unsigned integer variable.
     *
     * @param copy_from Source instance.
     */
    UInt32(const UInt32 &copy_from);

    /**
     * @brief Assigns from another 32-bit unsigned integer variable.
     *
     * @param rhs Source instance.
     * @return This instance after assignment.
     */
    UInt32 &operator=(const UInt32 &rhs);

    BaseType *ptr_duplicate() override;

    unsigned int width(bool = false) const override { return sizeof(dods_uint32); }

    /**
     * @brief Returns the storage width in bytes.
     *
     * @param constrained Ignored for scalar values.
     * @return Number of bytes used by this value.
     */
    int64_t width_ll(bool = false) const override { return sizeof(dods_uint32); }

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
     * @brief Returns the current value.
     *
     * @return Stored unsigned 32-bit value.
     */
    virtual dods_uint32 value() const;
    /**
     * @brief Sets the current value.
     *
     * @param val New unsigned 32-bit value.
     * @return True when the value is accepted.
     */
    virtual bool set_value(dods_uint32 val);

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

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _uint32_h
