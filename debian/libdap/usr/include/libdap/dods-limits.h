
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

// (c) COPYRIGHT URI/MIT 1996,1998,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Limits for DAP2. Use these *instead* of <limits.h> since DAP2 needs to see
// the same values on all machines.

#ifndef _dods_limits_h
#define _dods_limits_h

#if 0
// The old values. I switched to hex notation when 64-bit ints were added
// to DAP4.
#define DODS_CHAR_BIT 8
#define DODS_SCHAR_MIN -128
#define DODS_SCHAR_MAX 127
#define DODS_UCHAR_MAX 255U
#define DODS_UCHAR_MIN 0

#define DODS_SHRT_MIN -32768
#define DODS_SHRT_MAX 32767
#define DODS_USHRT_MAX 65535U

#define DODS_INT_MIN -2147483648
// was (-2147483647 - 1)
#define DODS_INT_MAX 2147483647
#define DODS_UINT_MAX 4294967295U

#define DODS_LONG_MIN -2147483648
// was (-2147483647 - 1)
#define DODS_LONG_MAX 2147483647
#define DODS_ULONG_MAX 4294967295UL
#endif

#define DODS_CHAR_BIT 8
#define DODS_SCHAR_MIN (-0x7f-1)
#define DODS_SCHAR_MAX 0x7f
#define DODS_UCHAR_MAX 0xffU
#define DODS_UCHAR_MIN 0

#define DODS_SHRT_MIN (-0x7fff-1)
#define DODS_SHRT_MAX 0x7fff
#define DODS_USHRT_MAX 0xffffU

#define DODS_INT_MIN (-0x7fffffff-1)
#define DODS_INT_MAX 0x7fffffff
#define DODS_UINT_MAX 0xffffffffUL

#define DODS_LONG_MIN DODS_INT_MIN
#define DODS_LONG_MAX DODS_INT_MAX
#define DODS_ULONG_MAX DODS_UINT_MAX

#define DODS_LLONG_MIN (-0x7fffffffffffffffLL-1)
#define DODS_LLONG_MAX 0x7fffffffffffffffLL
#define DODS_ULLONG_MAX 0xffffffffffffffffULL

#define DODS_MAX_ARRAY_INDEX 0x1fffffffffffffffULL

#define DODS_DBL_DIG 15 /* digits of precision of a "double" */
#define DODS_DBL_MAX 1.7976931348623157E+308 /* max decimal value of a */
/* "double" */
#define DODS_DBL_MIN 2.2250738585072014E-308 /* min decimal value of a */
/* "double" */
#define DODS_FLT_DIG 6  /* digits of precision of a "float" */
#define DODS_FLT_MAX 3.402823466E+38F  /* max decimal value of a "float" */
#define DODS_FLT_MIN 1.175494351E-38F  /* min decimal value of a "float" */

/* This constant should not be used. */
#define DODS_MB_LEN_MAX 4

#endif // _dods_limits_h
