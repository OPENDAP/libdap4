
// Implementation for the class Structure
//
// jhrg 9/14/94

#ifdef _GNUG_
#pragma implementation
#endif

#include "Structure.h"

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
    if (vars.empty())
	return NULL;

    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->get_var_name() == name)
	    return vars(p);
}

Pix
Structure::first_var()
{
    return vars.first();
}

void
Structure::next_var(Pix &p)
{
    vars.next(p);
}

BaseType *
Structure::var(Pix p)
{
    return vars(p);
}

void
Structure::print_decl(bool print_semi)
{
    cout << get_var_type() << " {" << endl;
    for (Pix p = first_var(); p; next_var(p))
	var(p)->print_decl();
    cout << "} " << get_var_name();
    if (print_semi)
	cout << ";" << endl;
}
    
