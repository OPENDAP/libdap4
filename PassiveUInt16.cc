
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

// Implementation for PassiveUInt16.
//
// pwest 11/04/03


#include "config.h"

static char rcsid[] not_used =
    {"$Id$"
    };

#include <stdlib.h>

#include "PassiveUInt16.h"
#include "InternalErr.h"
#include "debug.h"


using std::cerr;
using std::endl;

/** The PassiveUInt16 constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications.

    @param n A string containing the name of the variable to be
    created.
*/
PassiveUInt16::PassiveUInt16(const string &n) : UInt16(n)
{}

PassiveUInt16::PassiveUInt16(const PassiveUInt16 &copy_from) : UInt16(copy_from)
{}

BaseType *
PassiveUInt16::ptr_duplicate()
{
    return new PassiveUInt16(*this);
}

PassiveUInt16::~PassiveUInt16()
{
    DBG(cerr << "~PassiveUInt16" << endl);
}

PassiveUInt16 &
PassiveUInt16::operator=(const PassiveUInt16 &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

bool
PassiveUInt16::read(const string &)
{
    if (!read_p()) {
        val2buf(&_buf) ;
        set_read_p(true) ;
    }

    return true ;
}

bool
PassiveUInt16::set_value(dods_uint16 val)
{
    _buf = val ;
    return read("") ;
}
