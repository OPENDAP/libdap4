
// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Sequence.cc,v $
// Revision 1.4  1994/12/08 15:55:58  dan
// Added data transmission member functions serialize() and deserialize()
// Modified size() member function to return cumulative size of all members.
// Fixed the duplicate() member function to correctly access the data members
// of the sequence being duplicated.
//
// Revision 1.3  1994/10/17  23:34:49  jimg
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

    Sequence &cs = (Sequence)s; // cast away const
    
    for (Pix p = cs.first_var(); p; cs.next_var(p))
	add_var(cs.var(p)->ptr_duplicate());
}

// protected

BaseType *
Sequence::ptr_duplicate()
{
    return new Sequence(*this);
}

// This ctor is silly -- in order to add fields to a Structure or Sequence,
// you must use add_var (a mfunc of Structure).

Sequence::Sequence(const String &n, const String &t, FILE *in, FILE *out)
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

unsigned int
Sequence::size()
{
  unsigned int sz = 0;
  for( Pix p = first_var(); p; next_var(p))
    sz += var(p)->size();

  return sz;
}

bool
Sequence::serialize(bool flush, unsigned int num)
{
    bool status;

    for (Pix p = first_var(); p; next_var(p)) 
      {
	if ( !(status = var(p)->serialize(true)) ) break;
      }
    if (status && flush)
	status = expunge();

    return status;
}

unsigned int
Sequence::deserialize()
{
    unsigned int num, sz = 0;

    for (Pix p = first_var(); p; next_var(p)) {
      if ( !(num = var(p)->deserialize()) ) break;
      else sz += num;
    }
    return num ? sz : (unsigned int)FALSE;
}
