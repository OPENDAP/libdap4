
// -*- C++ -*-

// Base class for the variables in a dataset. This is used to store the
// type-invariant information that describes a variable as given in the DODS
// API.
//
// jhrg 9/6/94

/* $Log: BaseType.h,v $
/* Revision 1.4  1994/10/17 23:30:47  jimg
/* Added ptr_duplicate virtual mfunc. Child classes can also define this
/* to copy parts that BaseType does not have (and allocate correctly sized
/* pointers.
/* Removed protected mfunc error() -- use errmsg library instead.
/* Added formatted printing of types (works with DDS::print()).
/*
 * Revision 1.3  1994/09/23  14:34:44  jimg
 * Added mfunc check_semantics().
 * Moved definition of dtor to BaseType.cc.
 *
 * Revision 1.2  1994/09/15  21:08:56  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.1  1994/09/09  15:28:42  jimg
 * Class for base type variables. Int32, ... inherit from this class.
 */

#ifndef _Base_Type_h
#define _Base_Type_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream.h>
#include <String.h>

class BaseType {
private:
    String name;		// name of the variable
    String type;		// Name of the instance's type

    void duplicate(const BaseType &bt);

public:
    BaseType(const String &n = (char *)0, const String &t = (char *)0);
    BaseType(const BaseType &copy_from);
    virtual ~BaseType() {}

    BaseType &operator=(const BaseType &rhs);
    virtual BaseType *ptr_duplicate(); // alloc new BaseType and dup THIS.

    String get_var_name() const;
    void set_var_name(const String &n);

    String get_var_type() const;
    void set_var_type(const String &t);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual bool check_semantics(bool all = false);
};

typedef BaseType * BaseTypePtr;

#endif 
