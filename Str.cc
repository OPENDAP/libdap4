
// Implementation for Str. Quite some implementation, huh?
//
// jhrg 9/7/94

// $Log: Str.cc,v $
// Revision 1.1  1994/09/15 21:08:48  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is not represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/09  15:38:45  jimg
// Child class of BaseType -- used in the future to hold specific serialization
// information for integers. Should this be a class that uses BaseType?
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "Str.h"

Str::Str(const String &n, const String &t) : BaseType(n, t)
{
}
