
// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Structure.cc,v $
// Revision 1.10  1995/03/16 17:29:12  jimg
// Added include config.h to top of include list.
// Added TRACE_NEW switched dbnew includes.
// Fixed bug in read_val() where **val was passed incorrectly to
// subordinate read_val() calls.
//
// Revision 1.9  1995/03/04  14:34:51  jimg
// Major modifications to the transmission and representation of values:
// 	Added card() virtual function which is true for classes that
// 	contain cardinal types (byte, int float, string).
// 	Changed the representation of Str from the C rep to a C++
// 	class represenation.
// 	Chnaged read_val and store_val so that they take and return
// 	types that are stored by the object (e.g., inthe case of Str
// 	an URL, read_val returns a C++ String object).
// 	Modified Array representations so that arrays of card()
// 	objects are just that - no more storing strings, ... as
// 	C would store them.
// 	Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// 	an array of a structure is represented as an array of Structure
// 	objects).
//
// Revision 1.8  1995/02/10  02:22:59  jimg
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
// Revision 1.7  1995/01/19  20:05:24  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.6  1995/01/11  15:54:49  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.5  1994/12/16  15:16:39  dan
// Modified Structure class removing inheritance from class CtorType
// and directly inheriting from class BaseType to alloc calling
// BaseType's constructor directly.
//
// Revision 1.4  1994/11/22  14:06:10  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.3  1994/10/17  23:34:47  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:45:26  jimg
// Added mfunc check_semantics().
// Added sanity checking on the variable list (is it empty?).
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "Structure.h"
#include "util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void
Structure::_duplicate(const Structure &s)
{
    set_name(s.name());
    
    Structure &cs = (Structure)s; // cast away const

    for (Pix p = cs._vars.first(); p; cs._vars.next(p))
	_vars.append(cs._vars(p)->ptr_duplicate());
}

Structure::Structure(const String &n) 
    : BaseType( n, "Structure", (xdrproc_t)NULL)
{
    set_name(n);
}

Structure::Structure(const Structure &rhs)
{
    _duplicate(rhs);
}

Structure::~Structure()
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	delete _vars(p);
}

const Structure &
Structure::operator=(const Structure &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

// NB: Part p defaults to nil for this class

void 
Structure::add_var(BaseType *bt, Part p)
{
    _vars.append(bt);
}

bool
Structure::card()
{
    return false;
}

unsigned int
Structure::size()		// deprecated
{
    return width();
}

unsigned int
Structure::width()
{
    unsigned int sz = 0;

    for (Pix p = first_var(); p; next_var(p))
	sz += var(p)->size();

    return sz;
}

bool
Structure::serialize(bool flush)
{
    bool status;

    for (Pix p = first_var(); p; next_var(p)) 
	if ( !(status = var(p)->serialize(false)) ) 
	    break;

    if ( status && flush )
	status = expunge();

    return status;
}

bool
Structure::deserialize(bool reuse)
{
    unsigned int num, sz = 0;

    for (Pix p = first_var(); p; next_var(p)) {
	sz += num = var(p)->deserialize(reuse);
	if (num == 0) 
	    return (unsigned int)false;
    }

    return sz;
}

// This mfunc assumes that val contains values for all the elements of the
// strucuture in the order those elements are declared.

unsigned int
Structure::store_val(void *val, bool reuse)
{
    assert(val);
    
    unsigned int pos = 0;
    for (Pix p = first_var(); p; next_var(p))
	pos += var(p)->store_val(val + pos, reuse);

    return pos;
}

unsigned int
Structure::read_val(void **val)
{
    assert(val);

    if (!*val)
	*val = new char[size()];

    unsigned int pos = 0;
    void *tval;

    for (Pix p = first_var(); p; next_var(p)) {
	tval = *val + pos;
	pos += var(p)->read_val(&tval);
    }

    return pos;
}

BaseType *
Structure::var(const String &name)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	if (_vars(p)->name() == name)
	    return _vars(p);

    return 0;
}

Pix
Structure::first_var()
{
    return _vars.first();
}

void
Structure::next_var(Pix &p)
{
    if (!_vars.empty() && p)
	_vars.next(p);
}

BaseType *
Structure::var(Pix p)
{
    if (!_vars.empty() && p)
	return _vars(p);
    else 
      return NULL;
}

void
Structure::print_decl(ostream &os, String space, bool print_semi)
{
    os << space << type() << " {" << endl;
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->print_decl(os, space + "    ");
    os << space << "} " << name();
    if (print_semi)
	os << ";" << endl;
}

// print the values of the contained variables

void 
Structure::print_val(ostream &os, String space, bool print_decl_p)
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
	os << ";" << endl;
}

bool
Structure::check_semantics(bool all)
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

