
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
#if 0
#include "TestByte.h"
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
#endif

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
#if 0
        case dods_int64_c:
            return NewInt64(name);
        case dods_uint64_c:
            return NewUInt64(name);
#endif
        case dods_float32_c:
            return NewFloat32(name);
        case dods_float64_c:
            return NewFloat64(name);
        case dods_str_c:
            return NewStr(name);
        case dods_url_c:
            return NewUrl(name);
#if 0
            // Noooo ...not adding a new type just to change the case of two letters.
            // jhrg 8/15/13
        case dods_url4_c:
            return NewURL(name);
#endif
        case dods_structure_c:
            return NewStructure(name);

            // FIXME Array and Sequence are back in. jhrg 8/15/13
        case dods_array_c:
            throw InternalErr(__FILE__, __LINE__, "Array is not part of DAP4.");
        case dods_sequence_c:
            throw InternalErr(__FILE__, __LINE__, "Sequence is not part of DAP4.");
        case dods_grid_c:
            throw InternalErr(__FILE__, __LINE__, "Grid is not part of DAP4.");
#if 0
        case dods_enum_c:
            return  NewEnum(name);
#endif
        case dods_array4_c:
            throw InternalErr(__FILE__, __LINE__, "Array4 not impl yet.");

        case dods_group_c:
            return NewGroup(name);

        default:
            return 0;
    }
}
