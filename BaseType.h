
// -*- C++ -*-

// Abstract base class for the variables in a dataset. This is used to store
// the type-invariant information that describes a variable as given in the
// DODS API.
//
// jhrg 9/6/94

/* $Log: BaseType.h,v $
/* Revision 1.5  1994/11/22 14:05:29  jimg
/* Added code for data transmission to parts of the type hierarchy. Not
/* complete yet.
/* Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
/*
 * Revision 1.4  1994/10/17  23:30:47  jimg
 * Added ptr_duplicate virtual mfunc. Child classes can also define this
 * to copy parts that BaseType does not have (and allocate correctly sized
 * pointers.
 * Removed protected mfunc error() -- use errmsg library instead.
 * Added formatted printing of types (works with DDS::print()).
 *
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

#include <rpc/xdr.h>
#include <iostream.h>
#include <String.h>

class BaseType {
private:
    String name;		// name of the variable
    String type;		// Name of the instance's type

    void duplicate(const BaseType &bt);

protected:
    // xdr_coder is used as an argument to xdr procedures that encode groups
    // of things (e.g., xdr_array()). Each leaf class's ctor must set this.
    xdrproc_t _xdr_coder;

    XDR *xdrin;			// xdr pointer for input (from stdin)
    XDR *xdrout;		// xdr pointer for output (to stdout)

public:
    BaseType(const String &n = (char *)0, const String &t = (char *)0,
	     xdrproc_t xdr = NULL);
    BaseType(const BaseType &copy_from);
    virtual ~BaseType();

    BaseType &operator=(const BaseType &rhs);
    virtual BaseType *ptr_duplicate() = 0; // alloc new instance and dup THIS.

    String get_var_name() const;
    void set_var_name(const String &n);

    String get_var_type() const;
    void set_var_type(const String &t);

    xdrproc_t xdr_coder();

    virtual unsigned int size() = 0; // local representation size in bytes

    // Put the data into a local buffer so that it may be serialized: that
    // means read it from the file and into a buffer. The buffer is
    // serialized by the mfunc serialize().
    virtual bool read(String dataset, String var_name, String constraint) = 0;

    // move data to and from the net.
    virtual bool serialize(unsigned int num = 0) = 0; 
    virtual unsigned int deserialize() = 0;

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual void print_val(ostream &os, String space = "") = 0;

    virtual bool check_semantics(bool all = false);
};

typedef BaseType * BaseTypePtr;

#endif 
