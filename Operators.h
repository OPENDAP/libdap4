
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
#include "ce_expr.tab.h"

using namespace std;

namespace libdap
{

inline unsigned
dods_max(int i1, int i2)
{
    return (unsigned)((i1 > i2) ? i1 : i2);
}

/** Compare two numerical types, both of which are either signed or unsigned.
    This class is one implementation of the comparison policy used by
    rops.

    @see rops
    @see USCmp
    @see SUCmp */
template<class T1, class T2> class Cmp
{
public:
    static bool eq(T1 v1, T2 v2)
    {
        return v1 == v2;
    }
    static bool ne(T1 v1, T2 v2)
    {
        return v1 != v2;
    }
    static bool gr(T1 v1, T2 v2)
    {
        return v1 > v2;
    }
    static bool ge(T1 v1, T2 v2)
    {
        return v1 >= v2;
    }
    static bool lt(T1 v1, T2 v2)
    {
        return v1 < v2;
    }
    static bool le(T1 v1, T2 v2)
    {
        return v1 <= v2;
    }
    static bool re(T1, T2)
    {
        cerr << "Illegal operation" << endl;
        return false;
    }
};

/** Compare two numerical types, the first one unsigned and the second
    signed. If the signed argument is negative, zero is used in the
    comparison. This class is one implementation of the comparison policy
    used by rops.

    @see rops
    @see SUCmp
    @see Cmp */
template<class UT1, class T2> class USCmp
{
public:
    static bool eq(UT1 v1, T2 v2)
    {
        return v1 == dods_max(0, v2);
    }
    static bool ne(UT1 v1, T2 v2)
    {
        return v1 != dods_max(0, v2);
    }
    static bool gr(UT1 v1, T2 v2)
    {
        return v1 > dods_max(0, v2);
    }
    static bool ge(UT1 v1, T2 v2)
    {
        return v1 >= dods_max(0, v2);
    }
    static bool lt(UT1 v1, T2 v2)
    {
        return v1 < dods_max(0, v2);
    }
    static bool le(UT1 v1, T2 v2)
    {
        return v1 <= dods_max(0, v2);
    }
    static bool re(UT1, T2)
    {
        cerr << "Illegal operation" << endl;
        return false;
    }
};

/** Compare two numerical types, the first one signed and the second
    unsigned. If the signed argument is negative, zero is used in the
    comparison. This class is one implementation of the comparison policy
    used by rops. This class is here to make writing the Byte::ops, ...
    member functions simpler. It is not necessary since the functions could
    twiddle the order of arguments to rops and use <tt>USCmp</tt>. Having
    this class make Byte:ops, ... simpler to read and write.

    @see Byte::ops
    @see USCmp
    @see Cmp
    @see ops */
template<class T1, class UT2> class SUCmp
{
public:
    static bool eq(T1 v1, UT2 v2)
    {
        return dods_max(0, v1) == v2;
    }
    static bool ne(T1 v1, UT2 v2)
    {
        return dods_max(0, v1) != v2;
    }
    static bool gr(T1 v1, UT2 v2)
    {
        return dods_max(0, v1) > v2;
    }
    static bool ge(T1 v1, UT2 v2)
    {
        return dods_max(0, v1) >= v2;
    }
    static bool lt(T1 v1, UT2 v2)
    {
        return dods_max(0, v1) < v2;
    }
    static bool le(T1 v1, UT2 v2)
    {
        return dods_max(0, v1) <= v2;
    }
    static bool re(T1, UT2)
    {
        cerr << "Illegal operation" << endl;
        return false;
    }
};

/** Compare two string types.
    This class is one implementation of the comparison policy used by
    rops.

    @see rops */
template<class T1, class T2> class StrCmp
{
public:
    static bool eq(T1 v1, T2 v2)
    {
        return v1 == v2;
    }
    static bool ne(T1 v1, T2 v2)
    {
        return v1 != v2;
    }
    static bool gr(T1 v1, T2 v2)
    {
        return v1 > v2;
    }
    static bool ge(T1 v1, T2 v2)
    {
        return v1 >= v2;
    }
    static bool lt(T1 v1, T2 v2)
    {
        return v1 < v2;
    }
    static bool le(T1 v1, T2 v2)
    {
        return v1 <= v2;
    }
    static bool re(T1 v1, T2 v2)
    {
        Regex r(v2.c_str());
        return r.match(v1.c_str(), v1.length()) > 0;
    }
};

/** This template function is used to compare two values of two instances of
    the DAP2 simple types (Byte, ..., Str). The function does not take the
    DAP2 objects as arguments; the caller must access the values of those
    objects and pass them to this function. The reason for this is that all
    the possible functions that could be generated from this template would
    have to be explicitly listed as friend functions in each of the DAP2
    simple type classes. In the current implementation, only the simple type
    classes must be friends - to see why, look at Byte::ops and note that it
    accesses the <tt>_buf</tt> member of Int16, ..., Float64 and thus must be a
    friend of those classes.

    NB: This would all be simpler if: 1) g++ supported template friend
    functions (without explicit listing of all the template's arguments). 2)
    we did not have unsigned types.

    T1 The type of <tt>a</tt>.

    T2 The type of <tt>b</tt>.

    C A class which implements the policy used for comparing <tt>a</tt>
    and <tt>b</tt>.

    @param a The first argument.
    @param b The second argument.
    @param op The relational operator.
    @see Byte::ops */

template<class T1, class T2, class C>
bool rops(T1 a, T2 b, int op)
{
    switch (op) {
    case SCAN_EQUAL:
        return C::eq(a, b);
    case SCAN_NOT_EQUAL:
        return C::ne(a, b);
    case SCAN_GREATER:
        return C::gr(a, b);
    case SCAN_GREATER_EQL:
        return C::ge(a, b);
    case SCAN_LESS:
        return C::lt(a, b);
    case SCAN_LESS_EQL:
        return C::le(a, b);
    case SCAN_REGEXP:
        return C::re(a, b);
    default:
        cerr << "Unknown operator" << endl;
        return false;
    }
}

} // namespace libdap

#endif // _operators_h
