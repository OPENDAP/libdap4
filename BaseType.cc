
// Implementation for BaseType.
//
// jhrg 9/6/94

// $Log: BaseType.cc,v $
// Revision 1.2  1994/09/15 21:08:36  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is not represented.
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
    
BaseType::~BaseType()
{
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
