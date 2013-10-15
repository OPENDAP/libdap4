
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

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "BaseTypeFactory.h"
#include "debug.h"

namespace libdap {

BaseType *
BaseTypeFactory::NewVariable(Type type, const string &name) const
{
	switch (type) {
	case dods_byte_c:
		return NewByte(name);
	case dods_int16_c:
		return NewInt16(name);
	case dods_uint16_c:
		return NewUInt16(name);
	case dods_int32_c:
		return NewInt32(name);
	case dods_uint32_c:
		return NewUInt32(name);
	case dods_float32_c:
		return NewFloat32(name);
	case dods_float64_c:
		return NewFloat64(name);

	case dods_str_c:
		return NewStr(name);
	case dods_url_c:
		return NewUrl(name);

	case dods_array_c:
		return NewArray(name);
	case dods_structure_c:
		return NewStructure(name);
	case dods_sequence_c:
		return NewSequence(name);
	case dods_grid_c:
		return NewGrid(name);
	default:
		throw InternalErr(__FILE__, __LINE__, "Unknow type");
	}
}
Byte *
BaseTypeFactory::NewByte(const string &n) const
{
    return new Byte(n);
}

Int16 *
BaseTypeFactory::NewInt16(const string &n) const
{
    return new Int16(n);
}

UInt16 *
BaseTypeFactory::NewUInt16(const string &n) const
{
    return new UInt16(n);
}

Int32 *
BaseTypeFactory::NewInt32(const string &n) const
{
    DBG(cerr << "Inside BaseTypeFactory::NewInt32" << endl);
    return new Int32(n);
}

UInt32 *
BaseTypeFactory::NewUInt32(const string &n) const
{
    return new UInt32(n);
}

Float32 *
BaseTypeFactory::NewFloat32(const string &n) const
{
    return new Float32(n);
}

Float64 *
BaseTypeFactory::NewFloat64(const string &n) const
{
    return new Float64(n);
}

Str *
BaseTypeFactory::NewStr(const string &n) const
{
    return new Str(n);
}

Url *
BaseTypeFactory::NewUrl(const string &n) const
{
    return new Url(n);
}

Array *
BaseTypeFactory::NewArray(const string &n , BaseType *v) const
{
    return new Array(n, v);
}

Structure *
BaseTypeFactory::NewStructure(const string &n) const
{
    return new Structure(n);
}

Sequence *
BaseTypeFactory::NewSequence(const string &n) const
{
    DBG(cerr << "Inside BaseTypeFactory::NewSequence" << endl);
    return new Sequence(n);
}

Grid *
BaseTypeFactory::NewGrid(const string &n) const
{
    return new Grid(n);
}

} // namespace libdap
