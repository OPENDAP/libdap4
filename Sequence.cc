
// Implementation for the class Structure
//
// jhrg 9/14/94

#ifdef _GNUG_
#pragma implementation
#endif

#include "Sequence.h"

Sequence::Sequence(const String &n, const String &t)
{
    set_var_name(n);
    set_var_type(t);
}
