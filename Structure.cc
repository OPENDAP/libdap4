
// Implementation for the class Structure
//
// jhrg 9/14/94

// $Log: Structure.cc,v $
// Revision 1.2  1994/09/23 14:45:26  jimg
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

Structure::Structure(const String &n, const String &t)
{
    set_var_name(n);
    set_var_type(t);
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
    if (!vars.empty() && !p)
	return vars(p);
}

void
Structure::print_decl(bool print_semi)
{
    cout << get_var_type() << " {" << endl;
    for (Pix p = vars.first(); p; vars.next(p))
	vars(p)->print_decl();
    cout << "} " << get_var_name();
    if (print_semi)
	cout << ";" << endl;
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

