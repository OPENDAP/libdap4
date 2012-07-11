
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.


#include <string>

#include "BaseType.h"

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

#include "Str.h"
#include "Url.h"

#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "DAP4BaseTypeFactory.h"
#include "debug.h"

namespace libdap {

Byte *
DAP4BaseTypeFactory::NewByte(const string &n) const
{
    return new Byte(n);
}

Byte *
DAP4BaseTypeFactory::NewUInt8(const string &n) const
{
    Byte *b = new Byte(n);
    b->set_type(dods_uint8_c);
    return b;
}

Int8 *
DAP4BaseTypeFactory::NewInt8(const string &n) const
{
    return new Int8(n);
}

Int16 *
DAP4BaseTypeFactory::NewInt16(const string &n) const
{
    return new Int16(n);
}

UInt16 *
DAP4BaseTypeFactory::NewUInt16(const string &n) const
{
    return new UInt16(n);
}

Int32 *
DAP4BaseTypeFactory::NewInt32(const string &n) const
{
    DBG(cerr << "Inside DAP4BaseTypeFactory::NewInt32" << endl);
    return new Int32(n);
}

UInt32 *
DAP4BaseTypeFactory::NewUInt32(const string &n) const
{
    return new UInt32(n);
}

Int64 *
DAP4BaseTypeFactory::NewInt64(const string &n) const
{
    DBG(cerr << "Inside DAP4BaseTypeFactory::NewInt64" << endl);
    return new Int64(n);
}

UInt64 *
DAP4BaseTypeFactory::NewUInt64(const string &n) const
{
    return new UInt64(n);
}

Float32 *
DAP4BaseTypeFactory::NewFloat32(const string &n) const
{
    return new Float32(n);
}

Float64 *
DAP4BaseTypeFactory::NewFloat64(const string &n) const
{
    return new Float64(n);
}

Str *
DAP4BaseTypeFactory::NewStr(const string &n) const
{
    return new Str(n);
}

Url *
DAP4BaseTypeFactory::NewUrl(const string &n) const
{
    return new Url(n);
}

Url *
DAP4BaseTypeFactory::NewURL(const string &n) const
{
    Url *u = new Url(n);
    u->set_type(dods_url4_c);
    return u;
}

Array *
DAP4BaseTypeFactory::NewArray(const string &n , BaseType *v) const
{
    return new Array(n, v);
}

Structure *
DAP4BaseTypeFactory::NewStructure(const string &n) const
{
    return new Structure(n);
}

Sequence *
DAP4BaseTypeFactory::NewSequence(const string &n) const
{
    DBG(cerr << "Inside DAP4BaseTypeFactory::NewSequence" << endl);
    return new Sequence(n);
}

Grid *
DAP4BaseTypeFactory::NewGrid(const string &n) const
{
    return new Grid(n);
}

} // namespace libdap
