
// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Sequence.cc,v $
// Revision 1.3  1994/10/17 23:34:49  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:48:31  jimg
// Fixed some errors in comments.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "debug.h"
#include "Sequence.h"

// private

void
Sequence::duplicate(const Sequence &s)
{
    set_var_name(s.get_var_name());
    set_var_type(s.get_var_type());
    
    for (Pix p = first_var(); p; next_var(p))
	add_var(var(p)->ptr_duplicate());
}

// protected

BaseType *
Sequence::ptr_duplicate()
{
    return new Sequence(*this);
}

// This ctor is silly -- in order to add fields to a Structure or Sequence,
// you must use add_var (a mfunc of Structure).

Sequence::Sequence(const String &n, const String &t)
{
    set_var_name(n);
    set_var_type(t);
}

Sequence::Sequence(const Sequence &rhs)
{
    duplicate(rhs);
}

// NB: since Sequence is a child of Structure, Structure's dtor will be
// called and will delete the pointers in Structure's mvar `var'.

Sequence::~Sequence()
{
    DBG(cerr << "Entering Sequence dtor" << endl);
}

const Sequence &
Sequence::operator=(const Sequence &rhs)
{
    if (this == &rhs)
	return *this;

    duplicate(rhs);

    return *this;
}
