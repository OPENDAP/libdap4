
// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Structure.cc,v $
// Revision 1.4  1994/11/22 14:06:10  jimg
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

#include <string.h>
#include <stdlib.h>

#include "Structure.h"
#include "util.h"

// private

void
Structure::duplicate(const Structure &s)
{
    set_var_name(s.get_var_name());
    set_var_type(s.get_var_type());
    
    Structure &cs = (Structure)s; // cast away const

    for (Pix p = cs.vars.first(); p; cs.vars.next(p))
	vars.append(cs.vars(p)->ptr_duplicate());
}

// protected

BaseType *
Structure::ptr_duplicate()
{
    return new Structure(*this);
}

Structure::Structure(const String &n, const String &t)
{
    set_var_name(n);
    set_var_type(t);
}

Structure::Structure(const Structure &rhs)
{
    duplicate(rhs);
}

Structure::~Structure()
{
    for (Pix p = vars.first(); p; vars.next(p))
	delete vars(p);
}

const Structure &
Structure::operator=(const Structure &rhs)
{
    if (this == &rhs)
	return *this;

    duplicate(rhs);

    return *this;
}

// NB: Part p defaults to nil for this class

void 
Structure::add_var(BaseType *bt, Part p)
{
    vars.append(bt);
}

BaseType *
Structure::var(const String &name)
{
    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->get_var_name() == name)
	    return vars(p);

    return 0;
}

Pix
Structure::first_var()
{
    return vars.first();
}

void
Structure::next_var(Pix &p)
{
    if (!vars.empty() && p)
	vars.next(p);
}

BaseType *
Structure::var(Pix p)
{
    if (!vars.empty() && p)
	return vars(p);
    else
	return NULL;
}

void
Structure::print_decl(ostream &os, String space, bool print_semi)
{
    os << space << get_var_type() << " {" << endl;
    for (Pix p = vars.first(); p; vars.next(p))
	vars(p)->print_decl(os, space + "    ");
    os << space << "} " << get_var_name();
    if (print_semi)
	os << ";" << endl;
}

// To seamantically OK, a structure's members must have unique names.
//
// Returns: true if the structure is OK, false if not.

bool
Structure::check_semantics(bool all)
{
    if (!BaseType::check_semantics())
	return false;

    if (!unique(vars, (const char *)get_var_name(),
		(const char *)get_var_type()))
	return false;

    if (all) 
	for (Pix p = vars.first(); p; vars.next(p))
	    if (!vars(p)->check_semantics(true))
		return false;

    return true;
}

