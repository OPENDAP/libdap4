
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

#include "TestByte.h"

#include "TestInt8.h"

#include "TestInt16.h"
#include "TestUInt16.h"
#include "TestInt32.h"
#include "TestUInt32.h"

#include "TestFloat32.h"
#include "TestFloat64.h"

#include "TestStr.h"
#include "TestUrl.h"

#include "TestArray.h"
#include "TestStructure.h"

#include "TestD4Group.h"

#include "D4BaseTypeFactory.h"

using namespace libdap ;

/** A factory for the TestByte, ..., TestGrid types.
    @author James Gallagher */
class D4TestTypeFactory : public D4BaseTypeFactory {
public:
    D4TestTypeFactory() {}
    virtual ~D4TestTypeFactory() {}

    virtual BaseType *NewVariable(Type t, const string &name) const;

    virtual BaseTypeFactory *ptr_duplicate() const {
        return new D4TestTypeFactory;
    }

    virtual Byte *NewByte(const string &n = "") const { return new TestByte(n); }

    virtual Byte *NewUInt8(const string &n = "") const { return new TestByte(n); }
    virtual Int8 *NewInt8(const string &n = "") const { return new TestInt8(n); }

    virtual Int16 *NewInt16(const string &n = "") const { return new TestInt16(n); }
    virtual UInt16 *NewUInt16(const string &n = "") const { return new TestUInt16(n); }
    virtual Int32 *NewInt32(const string &n = "") const { return new TestInt32(n); }
    virtual UInt32 *NewUInt32(const string &n = "") const { return new TestUInt32(n); }

    // TODO Add 64bit ints

    virtual Float32 *NewFloat32(const string &n = "") const { return new TestFloat32(n); }
    virtual Float64 *NewFloat64(const string &n = "") const { return new TestFloat64(n); }

    virtual Str *NewStr(const string &n = "") const { return new TestStr(n); }
    virtual Url *NewUrl(const string &n = "") const { return new TestUrl(n); }

    virtual Array *NewArray(const string &n = "", BaseType *v = 0) const { return new TestArray(n, v); }
    virtual Structure *NewStructure(const string &n = "") const { return new TestStructure(n); }

    virtual D4Group *NewGroup(const string &n = "") const { return new TestD4Group(n); }

};

#endif // d4_test_type_factory_h
