
// -*- C++ -*-

// Class for array variables. The dimensions of the array are stored in the
// list SHAPE. 
//
// jhrg 9/6/94

/* $Log: Array.h,v $
/* Revision 1.2  1994/09/23 14:31:37  jimg
/* Added check_semantics mfunc.
/* Added sanity checking for access to shape list (is it empty?).
/* Added cvs log listing to Array.cc.
/*
 */

#ifndef _Array_h
#define _Array_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <SLList.h>
#include "CtorType.h"

class Array: public CtorType {
private:
    BaseType *var_ptr;		// var that is an array
    SLList<int> shape;		// list of dimension sizes (i.e., the shape)

public:
    Array(const String &n = (char *)0, const String &t = "Array",
	  BaseType *v = 0);
    // use the depfault copy ctor and op=
    virtual ~Array() { }

    virtual BaseType *var(const String &name = (char *)0);
    virtual void add_var(BaseType *v, Part p = nil);

    void append_dim(int dim);

    Pix first_dim();
    void next_dim(Pix &p);
    int dim(Pix p);

    int dimensions();

    virtual void print_decl(bool print_semi);
    virtual bool check_semantics(bool all = false);
};

#endif
