
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

#ifndef dap4_base_type_factory_h
#define dap4_base_type_factory_h

#include <string>

#include "BaseTypeFactory.h"
#include "Type.h"

// Class declarations; Make sure to include the corresponding headers in the
// implementation file.

namespace libdap {

class Byte;
class Int8;
class Int16;
class UInt16;
class Int32;
class UInt32;
class Int64;
class UInt64;

class Float32;
class Float64;

class Str;
class Url;

class D4Enum;
class D4Opaque;

class Array;

class Structure;
class D4Sequence;

class D4Group;

class BaseType;

/**
 *  Return instances of objects that are to be stored in a DDS for a
 *  DAP4 dataset.
 *
 */
class D4BaseTypeFactory : public BaseTypeFactory {
public:
    D4BaseTypeFactory() {}
    ~D4BaseTypeFactory() override {}

    BaseType *NewVariable(Type t, const string &name) const override;

    BaseTypeFactory *ptr_duplicate() const override { return new D4BaseTypeFactory; }

    Byte *NewByte(const string &n = "") const override;

    // The Int8 types are new for DAP4
    /** @brief Builds an `Int8` variable. @param n Optional variable name. @return Newly allocated variable instance. */
    virtual Int8 *NewInt8(const string &n = "") const;
    /** @brief Builds a DAP4 `Char` alias (`UInt8`) variable. @param n Optional variable name. @return Newly allocated
     * variable instance. */
    virtual Byte *NewChar(const string &n = "") const;
    /** @brief Builds a `UInt8` variable. @param n Optional variable name. @return Newly allocated variable instance. */
    virtual Byte *NewUInt8(const string &n = "") const;

    Int16 *NewInt16(const string &n = "") const override;
    UInt16 *NewUInt16(const string &n = "") const override;
    Int32 *NewInt32(const string &n = "") const override;
    UInt32 *NewUInt32(const string &n = "") const override;

    // New for DAP4
    /** @brief Builds an `Int64` variable. @param n Optional variable name. @return Newly allocated variable instance.
     */
    virtual Int64 *NewInt64(const string &n = "") const;
    /** @brief Builds a `UInt64` variable. @param n Optional variable name. @return Newly allocated variable instance.
     */
    virtual UInt64 *NewUInt64(const string &n = "") const;

    Float32 *NewFloat32(const string &n = "") const override;
    Float64 *NewFloat64(const string &n = "") const override;

    /**
     * @brief Builds a DAP4 enum variable.
     * @param n Optional variable name.
     * @param type Backing integer type for the enum.
     * @return Newly allocated enum variable.
     */
    virtual D4Enum *NewEnum(const string &n = "", Type type = dods_null_c) const;

    Str *NewStr(const string &n = "") const override;
    Url *NewUrl(const string &n = "") const override;
    virtual Url *NewURL(const string &n = "") const;

    /** @brief Builds a `D4Opaque` variable. @param n Optional variable name. @return Newly allocated variable instance.
     */
    virtual D4Opaque *NewOpaque(const string &n = "") const;

    Array *NewArray(const string &n = "", BaseType *v = 0) const override;

    Structure *NewStructure(const string &n = "") const override;
    /** @brief Builds a `D4Sequence` variable. @param n Optional variable name. @return Newly allocated variable
     * instance. */
    virtual D4Sequence *NewD4Sequence(const string &n = "") const;

    /** @brief Builds a `D4Group` variable. @param n Optional group name. @return Newly allocated group instance. */
    virtual D4Group *NewGroup(const string &n = "") const;
};

} // namespace libdap

#endif // dap4_base_type_factory_h
