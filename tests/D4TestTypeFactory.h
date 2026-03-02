
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef d4_test_type_factory_h
#define d4_test_type_factory_h

#include <string>

#include "D4BaseTypeFactory.h"

using namespace libdap;

/** A factory for the DAP4 TestByte, ...,  types.
    @author James Gallagher */
class D4TestTypeFactory : public D4BaseTypeFactory {
public:
    D4TestTypeFactory() {}
    ~D4TestTypeFactory() override {}

    BaseTypeFactory *ptr_duplicate() const override { return new D4TestTypeFactory; }

    BaseType *NewVariable(Type t, const string &name) const override;

    Byte *NewByte(const string &n = "") const override;

    // New for DAP4
    Int8 *NewInt8(const string &n = "") const override;
    Byte *NewUInt8(const string &n = "") const override;
    Byte *NewChar(const string &n = "") const override;

    Int16 *NewInt16(const string &n = "") const override;
    UInt16 *NewUInt16(const string &n = "") const override;
    Int32 *NewInt32(const string &n = "") const override;
    UInt32 *NewUInt32(const string &n = "") const override;

    // New for DAP4
    Int64 *NewInt64(const string &n = "") const override;
    UInt64 *NewUInt64(const string &n = "") const override;

    Float32 *NewFloat32(const string &n = "") const override;
    Float64 *NewFloat64(const string &n = "") const override;

    D4Enum *NewEnum(const string &n = "", Type type = dods_null_c) const override;

    Str *NewStr(const string &n = "") const override;
    Url *NewUrl(const string &n = "") const override;
    Url *NewURL(const string &n = "") const override;

    D4Opaque *NewOpaque(const string &n = "") const override;

    Array *NewArray(const string &n = "", BaseType *v = 0) const override;

    Structure *NewStructure(const string &n = "") const override;
    D4Sequence *NewD4Sequence(const string &n = "") const override;

    D4Group *NewGroup(const string &n = "") const override;
};

#endif // d4_test_type_factory_h
