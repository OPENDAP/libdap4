
// -*- C++ -*-

// This is the interface definition file for the abstract class
// Vector. Vector is the parent class for List and Array.

/* $Log: Vector.h,v $
/* Revision 1.5  1996/04/05 00:22:10  jimg
/* Compiled with g++ -Wall and fixed various warnings.
/*
 * Revision 1.4  1996/03/05 01:03:04  jimg
 * Added ce_eval parameter to serialize() member function.
 * Added vec_resize() member function to class.
 *
 * Revision 1.3  1995/12/09  01:07:34  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.2  1995/12/06  19:49:35  jimg
 * Changed the var() and print_decl() mfuncs. var() now takes an index and
 * returns a pointer to the BaseType object with the correct
 * value. print_decl() takes a new flag - constrained - which causes only
 * those dimensions selected by the current constraint expression to be printed
 *
 * Revision 1.1  1995/11/22  22:30:20  jimg
 * Created.
 */

#ifndef _Vector_h
#define _Vector_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseType.h"
#include "BaseTypeVec.h"

class Vector: public BaseType {
private:
    int _length;		// number of elements in the vector
    BaseType *_var;		// base type of the Vector

    void *_buf;			// array which holds cardinal data

    BaseTypePtrVec _vec;	// used for vectors of all other types

protected:
    // This function copies the private members of Vector.
    void _duplicate(const Vector &v);

public:
    Vector(const String &n = (char *)0, BaseType *v = 0, 
	   const Type &t = d_null_t);
    Vector(const Vector &rhs);

    virtual ~Vector();

    const Vector &operator=(const Vector &rhs);
    virtual BaseType *ptr_duplicate() = 0; 

    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);

    virtual unsigned int width();
    virtual unsigned int length();
    virtual void set_length(int l);

    virtual bool serialize(const String &dataset, DDS &dds, 
			   bool ce_eval = true, bool flush = false);
    virtual bool deserialize(bool reuse = false);

    virtual bool read(const String &dataset) = 0;

    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    void set_vec(int i, BaseType *val);
    void vec_resize(int l);

    virtual BaseType *var(const String &name = (char *)0);

    // Return the BaseType pointer for the given Vector element. The value of
    // the element is copied into the BaseType *. See BaseType::var() for a
    // version of this mfunc that does *not* copy the value.
    virtual BaseType *var(unsigned int i);

    virtual void add_var(BaseType *v, Part p = nil);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool check_semantics(bool all = false);
};

#endif /* _Vector_h */
