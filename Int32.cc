
// Implementation for Int32. Quite some implementation, huh?
//
// jhrg 9/7/94

// $Log: Int32.cc,v $
// Revision 1.1  1994/09/09 15:38:45  jimg
// Child class of BaseType -- used in the future to hold specific serialization
// information for integers. Should this be a class that uses BaseType?
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "Int32.h"

Int32::Int32(const String &n, const String &t, const unsigned int d, 
	     const int *s)
    : BaseType(n, t, d, s)
{
}
