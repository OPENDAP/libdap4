// This may look like C code, but it is really -*- C++ -*-

// Virtual base class for the variables in a dataset. This is used to store
// the type-invariant information that describes a variable as given in the
// DODS API.
//
// jhrg 9/6/94

// $Log: BaseType.h,v $
// Revision 1.1  1994/09/09 15:28:42  jimg
// Class for base type variables. Int32, ... inherit from this class.
//

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
    unsigned int dimensions;	// number of dimensions
    int *shape;			// array that gives the size of each dimension

    int shape_size;		// current size of the array `shape'

//    static String null_str;	// default arg for ctor params

public:
    BaseType(const String &n = (char *)0, const String &t = (char *)0, 
	     const unsigned int d = 0, const int *s = 0);
    BaseType(const BaseType &copy_from);
    virtual ~BaseType();

    BaseType &operator=(const BaseType &rhs);

    // because these return references, they can be used to `get' or `set'
    // the fields.
    String &get_name() { return name; }
    String &get_type() { return type; }
    unsigned int &get_dimensions() { return dimensions; }
    int *get_shape() { return shape; }

    void add_shape_dim(unsigned int size);
    virtual void print();
};

typedef BaseType * BaseTypePtr;

#endif 
