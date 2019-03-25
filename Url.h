
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Url type.
//
// jhrg 9/7/94

#ifndef _url_h
#define _url_h 1


#include <string>

#if 0
#ifndef _dods_limits_h
#include "dods-limits.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif
#endif


// #ifndef _str_h
#include "Str.h"
//#endif

namespace libdap
{

class BaseType;

const unsigned int max_url_len = 255;

/** @brief Holds an Internet address (URL).

    @see BaseType
    @see Str */
class Url: public Str
{

#if 0
private:
string _buf;

// This enables methods of Str to access _buf in this class.
friend class Str;
#endif

public:
    Url(const std::string &n);
    Url(const std::string &n, const std::string &d);
    Url(const std::string &n, Type t);
    Url(const std::string &n, const std::string &d, Type t);
    virtual ~Url()
    {}

    virtual BaseType *ptr_duplicate();
};

} // namespace libdap

#endif // _url_h

