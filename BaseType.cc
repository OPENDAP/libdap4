
// Implementation for BaseType.
//
// jhrg 9/6/94

// $Log: BaseType.cc,v $
// Revision 1.5  1994/11/22 14:05:26  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.4  1994/10/17  23:30:46  jimg
// Added ptr_duplicate virtual mfunc. Child classes can also define this
// to copy parts that BaseType does not have (and allocate correctly sized
// pointers.
// Removed protected mfunc error() -- use errmsg library instead.
// Added formatted printing of types (works with DDS::print()).
//
// Revision 1.3  1994/09/23  14:34:42  jimg
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

#include <stdio.h>		// for stdin and stdout

#include "BaseType.h"

// Private copy mfunc

void
BaseType::duplicate(const BaseType &bt)
{
    name = bt.name;
    type = bt.type;
    _xdr_coder = bt._xdr_coder;	// just copy this function pointer

    xdrstdio_create(xdrin, stdin, XDR_DECODE);
    xdrstdio_create(xdrout, stdout, XDR_ENCODE);
}

// Public mfuncs

// Note that the ctor (as well as the copy ctor via duplicate)
// open/initialize the (XDRS *)s XDRIN and XDROUT to reference sdtin and
// stdout. This means that writing to std{in,out} must work correctly, and
// probably means that is must be OK to mix calls to cout/cin with calls that
// write to std{out,in} (it is for g++ with libg++ at version 2.6 or
// greater).

BaseType::BaseType(const String &n, const String &t, xdrproc_t xdr) 
    : name(n), type(t), _xdr_coder(xdr)
{
    xdrstdio_create(xdrin, stdin, XDR_DECODE);
    xdrstdio_create(xdrout, stdout, XDR_ENCODE);
} 

BaseType::BaseType(const BaseType &copy_from)
{
    duplicate(copy_from);
}
    
BaseType::~BaseType()
{
    xdr_destroy(xdrin);
    xdr_destroy(xdrout);
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
BaseType::get_var_name() const
{ 
    return name; 
}

void 
BaseType::set_var_name(const String &n)
{ 
    name = n; 
}

String
BaseType::get_var_type() const
{
    return type;
}

void
BaseType::set_var_type(const String &t)
{
    type = t;
}

// Using this mfunc, objects that contain a (BaseType *) can get the xdr
// function used to serialize the object.

xdrproc_t
BaseType::xdr_coder()
{
    return _xdr_coder;
}

// send a printed representation of the variable's declaration to cout. If
// print_semi is true, append a semicolon and newline.

void 
BaseType::print_decl(ostream &os, String space, bool print_semi)
{
    os << space << type << " " << name;
    if (print_semi)
	os << ";" << endl;
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


