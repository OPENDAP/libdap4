
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

// Interface for PassiveFloat32 type.
//
// pwest 11/04/03

#ifndef _passivefloat32_h
#define _passivefloat32_h 1


#include "Float32.h"
#include "dods-datatypes.h"

/** @brief Holds a 32-bit (double precision) floating point value.

    @see BaseType
    */

class PassiveFloat32: public Float32
{
public:
    PassiveFloat32(const string &n = "");

    virtual ~PassiveFloat32();

    PassiveFloat32(const PassiveFloat32 &copy_from);

    PassiveFloat32 &operator=(const PassiveFloat32 &rhs);

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset) ;

    virtual bool set_value(dods_float32 val) ;
};

#endif // _passivefloat32_h

