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

#ifndef d4attributetype_h
#define d4attributetype_h

/** D4AttributeType identifies the data type stored in a particular attribute

    @see D4Attributes
*/

enum D4AttributeType {
    attr_null_c,

    attr_byte_c,
    attr_int16_c,
    attr_uint16_c,
    attr_int32_c,  // Added `attr_' to fix clash with IRIX 5.3.
    attr_uint32_c,
    attr_float32_c,
    attr_float64_c,
    attr_str_c,
    attr_url_c,

    // Added for DAP4
    attr_int8_c,
    attr_uint8_c,

    attr_int64_c,
    attr_uint64_c,

#if 0
    // just use attr_url_c. jhrg 8/15/13
    attr_url4_c,
#endif

    attr_enum_c,
    attr_opaque_c,

    // These are specific to attributes while the other types are
    // also supported by the variables. jhrg 4/17/13
    attr_container_c,
    attr_otherxml_c
};

#endif /* d4attributetype_h */
