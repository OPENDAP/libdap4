
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


#include <string>

#include "TestByte.h"

#include "TestInt8.h"

#include "TestInt16.h"
#include "TestUInt16.h"
#include "TestInt32.h"
#include "TestUInt32.h"

#include "TestInt64.h"
#include "TestUInt64.h"

#include "TestFloat32.h"
#include "TestFloat64.h"

#include "TestStr.h"
#include "TestUrl.h"

#include "TestArray.h"
#include "TestStructure.h"

#include "TestD4Sequence.h"
#include "TestD4Group.h"

#include "D4TestTypeFactory.h"

#include "debug.h"

BaseType *D4TestTypeFactory::NewVariable(Type t, const string &name) const
{
    switch (t) {
        case dods_byte_c:
            return NewByte(name);

        case dods_uint8_c:
            return NewUInt8(name);
        case dods_int8_c:
            return NewInt8(name);

        case dods_int16_c:
            return NewInt16(name);
        case dods_uint16_c:
            return NewUInt16(name);
        case dods_int32_c:
            return NewInt32(name);
        case dods_uint32_c:
            return NewUInt32(name);

        case dods_int64_c:
            return NewInt64(name);
        case dods_uint64_c:
            return NewUInt64(name);

        case dods_float32_c:
            return NewFloat32(name);
        case dods_float64_c:
            return NewFloat64(name);

        case dods_str_c:
            return NewStr(name);
        case dods_url_c:
            return NewURL(name);

#if 0
        case dods_enum_c:
            return NewEnum(name);

        case dods_opaque_c:
        	return NewOpaque(name);
#endif

        case dods_array_c:
            return NewArray(name);

        case dods_structure_c:
            return NewStructure(name);

        case dods_sequence_c:
            return NewD4Sequence(name);

        case dods_group_c:
            return NewGroup(name);

        default:
            throw InternalErr(__FILE__, __LINE__, "Unimplemented type in DAP4.");
    }
}


Byte *
D4TestTypeFactory::NewByte(const string &n) const
{
    return new TestByte(n);
}

Byte *
D4TestTypeFactory::NewUInt8(const string &n) const
{
    Byte *b = new TestByte(n);
    b->set_type(dods_uint8_c);
    return b;
}

Int8 *
D4TestTypeFactory::NewInt8(const string &n) const
{
    return new TestInt8(n);
}

Int16 *
D4TestTypeFactory::NewInt16(const string &n) const
{
    return new TestInt16(n);
}

UInt16 *
D4TestTypeFactory::NewUInt16(const string &n) const
{
    return new TestUInt16(n);
}

Int32 *
D4TestTypeFactory::NewInt32(const string &n) const
{
    DBG(cerr << "Inside DAP4BaseTypeFactory::NewInt32" << endl);
    return new TestInt32(n);
}

UInt32 *
D4TestTypeFactory::NewUInt32(const string &n) const
{
    return new TestUInt32(n);
}

Int64 *
D4TestTypeFactory::NewInt64(const string &n) const
{
    DBG(cerr << "Inside DAP4BaseTypeFactory::NewInt64" << endl);
    return new TestInt64(n);
}

UInt64 *
D4TestTypeFactory::NewUInt64(const string &n) const
{
    return new TestUInt64(n);
}

Float32 *
D4TestTypeFactory::NewFloat32(const string &n) const
{
    return new TestFloat32(n);
}

Float64 *
D4TestTypeFactory::NewFloat64(const string &n) const
{
    return new TestFloat64(n);
}

Str *
D4TestTypeFactory::NewStr(const string &n) const
{
    return new TestStr(n);
}

Url *
D4TestTypeFactory::NewUrl(const string &n) const
{
    return new TestUrl(n);
}

/** Note that this method is called NewURL - URL in caps.
 */
Url *
D4TestTypeFactory::NewURL(const string &n) const
{
    return NewUrl(n);
}

#if 0
/** For an Enum, both it's name and type must be know before use. This
 * factory method uses "" and dods_null_c as the default values for name
 * and type, respectively.
 *
 * @name The name of the Enum
 * @type The Enum's element type.
 * @return A new Enum variable
 */
D4Enum *
D4TestTypeFactory::NewEnum(const string &name, Type type) const
{
    return new TestD4Enum(name, type);
}
#endif

Array *
D4TestTypeFactory::NewArray(const string &n, BaseType *v) const
{
	return new TestArray(n, v, true /* is_dap4 */);
}

Structure *
D4TestTypeFactory::NewStructure(const string &n) const
{
    return new TestStructure(n);
}

D4Sequence *
D4TestTypeFactory::NewD4Sequence(const string &n) const
{
    return new TestD4Sequence(n);
}

D4Group *
D4TestTypeFactory::NewGroup(const string &n) const
{
    return new TestD4Group(n);
}

