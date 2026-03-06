
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

// #ifndef _str_h
#include "Str.h"
// #endif

namespace libdap {

class BaseType;

/** @brief Legacy maximum URL length used by older protocol constraints. */
const unsigned int max_url_len = 255;

/** @brief Holds an Internet address (URL).

    @see BaseType
    @see Str */
class Url : public Str {

public:
    Url(const std::string &n);
    Url(const std::string &n, const std::string &d);
    /**
     * @brief Constructs a URL variable with an explicit type tag.
     *
     * Use this when callers need to preserve a specific runtime type marker.
     *
     * @param n Variable name.
     * @param t Explicit libdap type tag.
     */
    Url(const std::string &n, Type t);
    /**
     * @brief Constructs a URL variable with declaration and explicit type.
     *
     * @param n Variable name.
     * @param d Declaration string.
     * @param t Explicit libdap type tag.
     */
    Url(const std::string &n, const std::string &d, Type t);
    ~Url() override {}

    BaseType *ptr_duplicate() override;
};

} // namespace libdap

#endif // _url_h
