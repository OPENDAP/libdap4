
// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Sequence.cc,v $
// Revision 1.8  1995/02/10 02:23:02  jimg
// Added DBMALLOC includes and switch to code which uses malloc/free.
// Private and protected symbols now start with `_'.
// Added new accessors for name and type fields of BaseType; the old ones
// will be removed in a future release.
// Added the store_val() mfunc. It stores the given value in the object's
// internal buffer.
// Made both List and Str handle their values via pointers to memory.
// Fixed read_val().
// Made serialize/deserialize handle all malloc/free calls (even in those
// cases where xdr initiates the allocation).
// Fixed print_val().
//
// Revision 1.7  1995/01/19  20:05:26  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.6  1995/01/11  15:54:53  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.5  1994/12/15  21:21:13  dan
// Modified Sequence class to directly inherit from class BaseType
// Modified constructors to reflect new inheritance.
//
// Revision 1.4  1994/12/08  15:55:58  dan
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

#include <assert.h>

#include "debug.h"
#include "Sequence.h"
#include "util.h"

void
Sequence::_duplicate(const Sequence &s)
{
    set_name(s.name());

    Sequence &cs = (Sequence)s; // cast away const
    
    for (Pix p = cs.first_var(); p; cs.next_var(p))
	add_var(cs.var(p)->ptr_duplicate());
}

// This ctor is silly -- in order to add fields to a Structure or Sequence,
// you must use add_var (a mfunc of Structure).

Sequence::Sequence(const String &n) 
    : BaseType( n, "Sequence", (xdrproc_t)NULL) 
{
    set_name(n);
}

Sequence::Sequence(const Sequence &rhs)
{
    _duplicate(rhs);
}

Sequence::~Sequence()
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	delete _vars(p);
}

const Sequence &
Sequence::operator=(const Sequence &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

// NB: Part p defaults to nil for this class

void 
Sequence::add_var(BaseType *bt, Part p)
{
    _vars.append(bt);
}

BaseType *
Sequence::var(const String &name)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	if (_vars(p)->name() == name)
	    return _vars(p);

    return 0;
}

Pix
Sequence::first_var()
{
    return _vars.first();
}

void
Sequence::next_var(Pix &p)
{
    if (!_vars.empty() && p)
	_vars.next(p);
}

BaseType *
Sequence::var(Pix p)
{
    if (!_vars.empty() && p)
	return _vars(p);
    else 
	return NULL;
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
Sequence::serialize(bool flush)
{
    bool status;

    for (Pix p = first_var(); p; next_var(p)) {
	if ( !(status = var(p)->serialize(false)) ) break;
    }

    if (status && flush)
	status = expunge();

    return status;
}

unsigned int
Sequence::deserialize(bool reuse)
{
    unsigned int num, sz = 0;

    for (Pix p = first_var(); p; next_var(p)) {
	sz += num = var(p)->deserialize(reuse);
	if (num == 0) 
	    return (unsigned int)false;
    }

    return sz;
}

unsigned int
Sequence::store_val(void *val, bool reuse)
{
    assert(val);
    
    unsigned int pos = 0;
    for (Pix p = first_var(); p; next_var(p))
	pos += var(p)->store_val(val + pos, reuse);

    return pos;
}

unsigned int
Sequence::read_val(void **val)
{
    assert(val);

    if (!*val)
	*val = new char[size()];
    
    unsigned int pos = 0;
    for (Pix p = first_var(); p; next_var(p))
	pos += var(p)->read_val((void **)*val + pos);

    return pos;
}

void
Sequence::print_decl(ostream &os, String space, bool print_semi)
{
    os << space << type() << " {" << endl;
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->print_decl(os, space + "    ");
    os << space << "} " << name();
    if (print_semi)
	os << ";" << endl;
}

void 
Sequence::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";
    for (Pix p = _vars.first(); p; _vars.next(p), p && os << ", ") {
	_vars(p)->print_val(os, "", false);
    }
    os << " }";

    if (print_decl_p)
	os << ";";
}

bool
Sequence::check_semantics(bool all)
{
    if (!BaseType::check_semantics())
	return false;

    if (!unique(_vars, (const char *)name(), (const char *)type()))
	return false;

    if (all) 
	for (Pix p = _vars.first(); p; _vars.next(p))
	    if (!_vars(p)->check_semantics(true))
		return false;

    return true;
}
