// DapIndent.cc

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

// Methods for the class DapIndent - an indentation class to support
// debugging and the dump methods.

#include "config.h"

#include "DapIndent.h"

namespace libdap {

string DapIndent::_indent ;

void
DapIndent::Indent()
{
    _indent += "    " ;
}

void
DapIndent::UnIndent()
{
    if (_indent.length() == 0)
        return ;
    if (_indent.length() == 4)
        _indent = "" ;
    else
        _indent = _indent.substr(0, _indent.length() - 4) ;
}

void
DapIndent::Reset()
{
    _indent = "" ;
}

const string &
DapIndent::GetIndent()
{
    return _indent ;
}

void
DapIndent::SetIndent(const string &indent)
{
    _indent = indent ;
}

ostream &
DapIndent::LMarg(ostream &strm)
{
    strm << _indent ;
    return strm ;
}

} // namespace libdap

