
// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Sequence.cc,v $
// Revision 1.2  1994/09/23 14:48:31  jimg
// Fixed some errors in comments.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "Sequence.h"

Sequence::Sequence(const String &n, const String &t)
{
    set_var_name(n);
    set_var_type(t);
}
