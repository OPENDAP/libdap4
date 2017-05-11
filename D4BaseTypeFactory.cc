
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2005 OPeNDAP, Inc.
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

#include "config.h"

#include <string>

#include "BaseType.h"
#include "Type.h"

#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"

#include "Int64.h"
#include "UInt64.h"

#include "Float32.h"
#include "Float64.h"

#include "D4Enum.h"

#include "Str.h"
#include "Url.h"

#include "D4Opaque.h"

#include "Array.h"

#include "Structure.h"
#include "D4Sequence.h"

#include "D4Group.h"

#include "D4BaseTypeFactory.h"
#include "debug.h"

namespace libdap {

BaseType *D4BaseTypeFactory::NewVariable(Type t, const string &name) const
{
    switch (t) {
        case dods_byte_c:
            return NewByte(name);
        case dods_char_c:
        	return NewChar(name);
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

        case dods_enum_c:
            return NewEnum(name);

        case dods_str_c:
            return NewStr(name);
        case dods_url_c:
            return NewURL(name);

        case dods_opaque_c:
            return NewOpaque(name);

        case dods_structure_c:
            return NewStructure(name);

        case dods_sequence_c:
            return NewD4Sequence(name);

        case dods_array_c:
            return NewArray(name);

        case dods_group_c:
            return NewGroup(name);

        default:
            throw InternalErr(__FILE__, __LINE__, "Unimplemented type in DAP4");
    }
}

Byte *
D4BaseTypeFactory::NewByte(const string &n) const
{
    Byte *b = new Byte(n);
    b->set_is_dap4(true);
    return b;
}

// Use the type constants specific to Char and UInt8 so the print reps will
// match the server's idea of the types.
Byte *
D4BaseTypeFactory::NewChar(const string &n) const
{
    Byte *b = new Byte(n);
    b->set_type(dods_char_c);
    b->set_is_dap4(true);
    return b;
}

Byte *
D4BaseTypeFactory::NewUInt8(const string &n) const
{
    Byte *b = new Byte(n);
    b->set_type(dods_uint8_c);
    b->set_is_dap4(true);
    return b;
}

Int8 *
D4BaseTypeFactory::NewInt8(const string &n) const
{
    Int8 *b = new Int8(n);
    b->set_is_dap4(true);
    return b;
}

Int16 *
D4BaseTypeFactory::NewInt16(const string &n) const
{
    Int16 *b = new Int16(n);
    b->set_is_dap4(true);
    return b;
}

UInt16 *
D4BaseTypeFactory::NewUInt16(const string &n) const
{
    UInt16 *b = new UInt16(n);
    b->set_is_dap4(true);
    return b;
}

Int32 *
D4BaseTypeFactory::NewInt32(const string &n) const
{
    DBG(cerr << "Inside DAP4BaseTypeFactory::NewInt32" << endl);
    Int32 *b = new Int32(n);
    b->set_is_dap4(true);
    return b;
}

UInt32 *
D4BaseTypeFactory::NewUInt32(const string &n) const
{
    UInt32 *b = new UInt32(n);
    b->set_is_dap4(true);
    return b;
}

Int64 *
D4BaseTypeFactory::NewInt64(const string &n) const
{
    DBG(cerr << "Inside DAP4BaseTypeFactory::NewInt64" << endl);
    Int64 *b = new Int64(n);
    b->set_is_dap4(true);
    return b;
}

UInt64 *
D4BaseTypeFactory::NewUInt64(const string &n) const
{
    UInt64 *b = new UInt64(n);
    b->set_is_dap4(true);
    return b;
}

Float32 *
D4BaseTypeFactory::NewFloat32(const string &n) const
{
    Float32 *b = new Float32(n);
    b->set_is_dap4(true);
    return b;
}

Float64 *
D4BaseTypeFactory::NewFloat64(const string &n) const
{
    Float64 *b = new Float64(n);
    b->set_is_dap4(true);
    return b;
}

/**
 * Enums need a name and the name of an enumeration that was defined by the
 * dataset. If the later is not known, it must be set before the enum is used.
 * @param name
 * @param enum_name
 * @return
 */
D4Enum *
D4BaseTypeFactory::NewEnum(const string &name, Type type) const
{
    return new D4Enum(name, type);
}


Str *
D4BaseTypeFactory::NewStr(const string &n) const
{
    Str *b = new Str(n);
    b->set_is_dap4(true);
    return b;
}

Url *
D4BaseTypeFactory::NewUrl(const string &n) const
{
    Url *b = new Url(n);
    b->set_is_dap4(true);
    return b;
}

D4Opaque *
D4BaseTypeFactory::NewOpaque(const string &n) const
{
    return new D4Opaque(n);
}

/** Note that this method is called NewURL - URL in caps.
 */
Url *
D4BaseTypeFactory::NewURL(const string &n) const
{
    Url *b = new Url(n);
    b->set_is_dap4(true);
    return b;
}

Array *
D4BaseTypeFactory::NewArray(const string &n, BaseType *v) const
{
	return new Array(n, v, true /* is_dap4 */);
}

Structure *
D4BaseTypeFactory::NewStructure(const string &n) const
{
    Structure *b = new Structure(n);
    b->set_is_dap4(true);
    return b;
}

D4Sequence *
D4BaseTypeFactory::NewD4Sequence(const string &n) const
{
	return new D4Sequence(n);
}

D4Group *
D4BaseTypeFactory::NewGroup(const string &n) const
{
    return new D4Group(n);
}

} // namespace libdap
