// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Templates for relational operations.
//
// jhrg 3/24/99

// $Log: Operators.h,v $
// Revision 1.5  2000/07/09 21:57:09  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.4  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.3.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
//
// Revision 1.3  1999/05/04 19:47:21  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.2  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.1  1999/03/24 23:37:37  jimg
// Added
//

#ifndef __operators_h
#define __operators_h

#include "Regex.h"		// GNU Regex class used for string =~ op.

static unsigned
dods_max(int i1, int i2)
{
    return (unsigned)((i1 < i2) ? i1 : i2);
}

/** Compare two numerical types, both of which are either signed or unsigned.
    This class is one implementation of the comparison policy used by
    rops.
    
    @see rops
    @see USCmp
    @SUCmp */
template<class T1, class T2> class Cmp {
 public:
    static bool eq(T1 v1, T2 v2) {return v1 == v2;}
    static bool ne(T1 v1, T2 v2) {return v1 != v2;}
    static bool gr(T1 v1, T2 v2) {return v1 > v2;}
    static bool ge(T1 v1, T2 v2) {return v1 >= v2;}
    static bool lt(T1 v1, T2 v2) {return v1 < v2;}
    static bool le(T1 v1, T2 v2) {return v1 <= v2;}
    static bool re(T1 v1, T2 v2) {
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
template<class UT1, class T2> class USCmp {
 public:
    static bool eq(UT1 v1, T2 v2) {return v1 == dods_max(0, v2);}
    static bool ne(UT1 v1, T2 v2) {return v1 != dods_max(0, v2);}
    static bool gr(UT1 v1, T2 v2) {return v1 > dods_max(0, v2);}
    static bool ge(UT1 v1, T2 v2) {return v1 >= dods_max(0, v2);}
    static bool lt(UT1 v1, T2 v2) {return v1 < dods_max(0, v2);}
    static bool le(UT1 v1, T2 v2) {return v1 <= dods_max(0, v2);}
    static bool re(UT1 v1, T2 v2) {
	cerr << "Illegal operation" << endl;
	return false;
    }
};

/** Compare two numerical types, the first one signed and the second
    unsigned. If the signed argument is negative, zero is used in the
    comparison. This class is one implementation of the comparison policy
    used by rops. This class is here to make writing the Byte::ops, ...
    member functions simpler. It is not necessary since the functions could
    twiddle the order of arguments to rops and use #USCmp#. Having this class
    make Byte:ops, ... simper to read and write.

    @see Byte::ops
    @see USCmp
    @see Cmp
    @see ops */
template<class T1, class UT2> class SUCmp {
 public:
    static bool eq(T1 v1, UT2 v2) {return dods_max(0, v1) == v2;}
    static bool ne(T1 v1, UT2 v2) {return dods_max(0, v1) != v2;}
    static bool gr(T1 v1, UT2 v2) {return dods_max(0, v1) > v2;}
    static bool ge(T1 v1, UT2 v2) {return dods_max(0, v1) >= v2;}
    static bool lt(T1 v1, UT2 v2) {return dods_max(0, v1) < v2;}
    static bool le(T1 v1, UT2 v2) {return dods_max(0, v1) <= v2;}
    static bool re(T1 v1, UT2 v2) {
	cerr << "Illegal operation" << endl;
	return false;
    }
};

/** Compare two string types.
    This class is one implementation of the comparison policy used by
    rops.
    
    @see rops */
template<class T1, class T2> class StrCmp {
 public:
    static bool eq(T1 v1, T2 v2) {return v1 == v2;}
    static bool ne(T1 v1, T2 v2) {return v1 != v2;}
    static bool gr(T1 v1, T2 v2) {return v1 > v2;}
    static bool ge(T1 v1, T2 v2) {return v1 >= v2;}
    static bool lt(T1 v1, T2 v2) {return v1 < v2;}
    static bool le(T1 v1, T2 v2) {return v1 <= v2;}
    static bool re(T1 v1, T2 v2) {
	Regex r(v2.c_str());
	return r.match(v1.c_str(), v1.length()) > 0;
    }
};

/** This template function is used to compare two values of two instances of
    the DODS simple types (Byte, ..., Str). The function does not take the
    DODS objects as arguments; the caller must access the values of those
    objects and pass them to this function. The reason for this is that all
    the possible functions that could be generated from this template would
    have to be explicitly listed as friend functions in each of the DODS
    simple type classes. In the current implementation, only the simple type
    classes must be friends - to see why, look at Byte::ops and note that it
    accesses the #_buf# member of Int16, ..., Float64 and thus must be a
    friend of those classes.

    NB: This would all be simpler if: 1) g++ supported template friend
    functions (without explicit listing of all the template's arguments). 2)
    we did not have unsigned types.

    @param T1 The type of #a#.
    @param T2 The type of #b#.
    @param C A class which implements the policy used for comparing #a# and
    #b#. 
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

#endif // __operators_h
