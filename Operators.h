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

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Templates for relational operations.
//
// jhrg 3/24/99

#ifndef _operators_h
#define _operators_h

#include "GNURegex.h"  // GNU Regex class used for string =~ op.
#include "parser.h"  // for ID_MAX
#include "ce_expr.tab.hh"

using namespace std;

namespace libdap {

/** Compare two numerical types, both of which are either signed or unsigned.
 This class is one implementation of the comparison policy used by
 rops.

 @see rops
 @see USCmp
 @see SUCmp */
template<class T1, class T2>
bool Cmp(int op, T1 v1, T2 v2)
{
    switch (op) {
        case SCAN_EQUAL:
            return v1 == v2;
        case SCAN_NOT_EQUAL:
            return v1 != v2;
        case SCAN_GREATER:
            return v1 > v2;
        case SCAN_GREATER_EQL:
            return v1 >= v2;
        case SCAN_LESS:
            return v1 < v2;
        case SCAN_LESS_EQL:
            return v1 <= v2;
        case SCAN_REGEXP:
            throw Error("Regular expressions are supported for strings only.");
        default:
            throw Error("Unrecognized operator.");
    }
}

template<class T>
static inline unsigned long long dap_floor_zero(T i)
{
    return (unsigned long long) ((i < 0) ? 0 : i);
}

/** Compare two numerical types, the first one unsigned and the second
 signed. If the signed argument is negative, zero is used in the
 comparison. This class is one implementation of the comparison policy
 used by rops.

 @see rops
 @see SUCmp
 @see Cmp */
template<class UT1, class T2>
bool USCmp(int op, UT1 v1, T2 v2)
{
    switch (op) {
        case SCAN_EQUAL:
            return v1 == dap_floor_zero<T2>(v2);
        case SCAN_NOT_EQUAL:
            return v1 != dap_floor_zero<T2>(v2);
        case SCAN_GREATER:
            return v1 > dap_floor_zero<T2>(v2);
        case SCAN_GREATER_EQL:
            return v1 >= dap_floor_zero<T2>(v2);
        case SCAN_LESS:
            return v1 < dap_floor_zero<T2>(v2);
        case SCAN_LESS_EQL:
            return v1 <= dap_floor_zero<T2>(v2);
        case SCAN_REGEXP:
            throw Error("Regular expressions are supported for strings only.");
        default:
            throw Error("Unrecognized operator.");
    }
}

/** Compare two numerical types, the first one signed and the second
 unsigned. If the signed argument is negative, zero is used in the
 comparison. This class is one implementation of the comparison policy
 used by rops. This class is here to make writing the Byte::ops, ...
 member functions simpler. It is not necessary since the functions could
 twidle the order of arguments to rops and use <tt>USCmp</tt>. Having
 this class make Byte:ops, ... simpler to read and write.

 @see Byte::ops
 @see USCmp
 @see Cmp
 @see ops */
template<class T1, class UT2>
bool SUCmp(int op, T1 v1, UT2 v2)
{
    switch (op) {
        case SCAN_EQUAL:
            return dap_floor_zero<T1>(v1) == v2;
        case SCAN_NOT_EQUAL:
            return dap_floor_zero<T1>(v1) != v2;
        case SCAN_GREATER:
            return dap_floor_zero<T1>(v1) > v2;
        case SCAN_GREATER_EQL:
            return dap_floor_zero<T1>(v1) >= v2;
        case SCAN_LESS:
            return dap_floor_zero<T1>(v1) < v2;
        case SCAN_LESS_EQL:
            return dap_floor_zero<T1>(v1) <= v2;
        case SCAN_REGEXP:
            throw Error("Regular expressions are supported for strings only.");
        default:
            throw Error("Unrecognized operator.");
    }
}

/** Compare two string types.
 This class is one implementation of the comparison policy used by
 rops.

 @see rops */
template<class T1, class T2>
bool StrCmp(int op, T1 v1, T2 v2)
{
    switch (op) {
        case SCAN_EQUAL:
            return v1 == v2;
        case SCAN_NOT_EQUAL:
            return v1 != v2;
        case SCAN_GREATER:
            return v1 > v2;
        case SCAN_GREATER_EQL:
            return v1 >= v2;
        case SCAN_LESS:
            return v1 < v2;
        case SCAN_LESS_EQL:
            return v1 <= v2;
        case SCAN_REGEXP: {
            Regex r(v2.c_str());
            return r.match(v1.c_str(), v1.length()) > 0;
        }
        default:
            throw Error("Unrecognized operator.");
    }
}

} // namespace libdap

#endif // _operators_h
