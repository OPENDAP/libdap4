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

#ifndef TYPE_H_
#define TYPE_H_

namespace libdap {

/** <b>Part</b> names the parts of multi-section constructor types.
    For example, the <b>Grid</b> class has an <i>array</i> and
    the array <i>maps</i>. Use the <tt>nil</tt> value for data types that
    don't have separate parts.

    \code
    enum Part {
    nil,
    array,
    maps
    };
    \endcode

    @brief Names the parts of multi-section constructor data types.
    @see Grid
    @see BaseType
*/

enum Part {
    nil,   // nil is for types that don't have parts...
    array,
    maps
};

/** <b>Type</b> identifies the data type stored in a particular type
    class. All the DODS Data Access Protocol (DAP) types inherit from
    the BaseType class.

    \code
    enum Type {
    dods_null_c,
    dods_byte_c,
    dods_int16_c,
    dods_uint16_c,
    dods_int32_c,
    dods_uint32_c,
    dods_float32_c,
    dods_float64_c,
    dods_str_c,
    dods_url_c,

    dods_structure_c,
    dods_array_c,
    dods_sequence_c,

    dods_grid_c,

    dods_char_c,
    dods_int8_c,
    dods_uint8_c,

    dods_int64_c,
    dods_uint64_c,
    dods_enum_c,
    dods_opaque_c,
    dods_group_c

    };
    \endcode

    @brief Identifies the data type.
    @see BaseType
*/

enum Type {
    dods_null_c,
    dods_byte_c,
    dods_int16_c,
    dods_uint16_c,
    dods_int32_c,  // Added `dods_' to fix clash with IRIX 5.3.
    dods_uint32_c,
    dods_float32_c,
    dods_float64_c,
    dods_str_c,
    dods_url_c,

    dods_structure_c,
    dods_array_c,
    dods_sequence_c,

    // Not used for DAP4
    dods_grid_c,

    // Added for DAP4
    dods_char_c,	// a synonym for UInt8 (and Byte)
    dods_int8_c,
    dods_uint8_c,

    dods_int64_c,
    dods_uint64_c,
    dods_enum_c,
    dods_opaque_c,
    dods_group_c
};

} // namespace libdap

#endif /* TYPE_H_ */
