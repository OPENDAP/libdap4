
// This may look like C code, but it is really -*- C++ -*-

// Base class for the variables in a dataset. This is used to store the
// type-invariant information that describes a variable as given in the DODS
// API.
//
// jhrg 9/6/94

/* $Log: BaseType.h,v $
/* Revision 1.2  1994/09/15 21:08:56  jimg
/* Added many classes to the BaseType hierarchy - the complete set of types
/* described in the DODS API design documet is not represented.
/* The parser can parse DDS files.
/* Fixed many small problems with BaseType.
/* Added CtorType.
/*
 * Revision 1.1  1994/09/09  15:28:42  jimg
 * Class for base type variables. Int32, ... inherit from this class.
 */

#ifndef _Base_Type_h
#define _Base_Type_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <String.h>

class BaseType {
private:
    String name;		// name of the variable
    String type;		// Name of the instance's type

    void duplicate(const BaseType &bt);

protected:
    virtual void error(const String &msg = "Error");

public:
    BaseType(const String &n = (char *)0, const String &t = (char *)0);
    BaseType(const BaseType &copy_from);
    virtual ~BaseType();

    BaseType &operator=(const BaseType &rhs);

    String get_var_name();
    void set_var_name(const String &n);

    String get_var_type();
    void set_var_type(const String &t);

    virtual void print_decl(bool print_semi = true);
};

typedef BaseType * BaseTypePtr;

#endif 
