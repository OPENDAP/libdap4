
// Implementation for BaseType.
//
// jhrg 9/6/94

// $Log: BaseType.cc,v $
// Revision 1.3  1994/09/23 14:34:42  jimg
// Added mfunc check_semantics().
// Moved definition of dtor to BaseType.cc.
//
// Revision 1.2  1994/09/15  21:08:36  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/09  15:28:41  jimg
// Class for base type variables. Int32, ... inherit from this class.

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>		// for abort()

#include <String.h>

#include "BaseType.h"

// Private copy mfunc

void
BaseType::duplicate(const BaseType &bt)
{
    name = bt.name;
    type = bt.type;
}

void
BaseType::error(const String &msg)
{
    cout << msg;
    abort();
}

BaseType::BaseType(const String &n, const String &t) : name(n), type(t)
{
} 

BaseType::BaseType(const BaseType &copy_from)
{
    duplicate(copy_from);
}
    
BaseType &
BaseType::operator=(const BaseType &rhs)
{
    if (this == &rhs)
	return *this;

    duplicate(rhs);

    return *this;
}

String 
BaseType::get_var_name()
{ 
    return name; 
}

void 
BaseType::set_var_name(const String &n)
{ 
    name = n; 
}

String
BaseType::get_var_type()
{
    return type;
}

void
BaseType::set_var_type(const String &t)
{
    type = t;
}

// send a printed representation of the variable's declaration to cout. If
// print_semi is true, append a semicolon and newline.

void 
BaseType::print_decl(bool print_semi)
{
    cout << type << " " << name;
    if (print_semi)
	cout << ";" << endl;
}

// Compares the object's current state with the semantics of a particular
// type. This will typically be defined in ctor classes (which have
// complicated semantics). For BaseType, an object is semantically correct if
// it has both a non-null name and type.
//
// NB: This is not the same as an invariant -- during the parse objects exist
// but have no name. Also, the bool ALL defaults to false for BaseType. It is
// used by children of CtorType.
//
// Returns: true if the object is semantically correct, false otherwise.

bool
BaseType::check_semantics(bool all)
{
    bool sem = ((const char *)type && (const char *)name);

    if (!sem) 
	cerr << "Every variable must have both a name and a type" << endl;

    return sem;
}
