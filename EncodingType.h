
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

#ifndef _encodingtype_h
#define _encodingtype_h

namespace libdap
{

/** libdap understands two types of encoding: x-plain and deflate, which
    correspond to plain uncompressed data and data compressed with zlib's LZW
    algorithm respectively.

    <code>
     enum EncodingType {
       unknown_enc,
       deflate,
       x_plain,
       gzip,
       binary
     };
    </code>

    @brief The type of encoding used on the current stream. */

enum EncodingType {
    unknown_enc,
    deflate,
    x_plain,
    gzip,
    binary
};

} // namespace libdap

#endif
