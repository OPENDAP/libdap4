
// -*- C++ -*-

// Class for array variables. The dimensions of the array are stored in the
// list SHAPE. 
//
// jhrg 9/6/94

/* $Log: Array.h,v $
/* Revision 1.4  1994/11/22 14:05:22  jimg
/* Added code for data transmission to parts of the type hierarchy. Not
/* complete yet.
/* Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
/*
 * Revision 1.3  1994/10/17  23:34:43  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:31:37  jimg
 * Added check_semantics mfunc.
 * Added sanity checking for access to shape list (is it empty?).
 * Added cvs log listing to Array.cc.
 */

#ifndef _Array_h
#define _Array_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <limits.h>
#include <SLList.h>
#include "CtorType.h"

#include "config.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

const int DODS_MAX_ARRAY = UINT_MAX;

class Array: public CtorType {
private:
    BaseType *var_ptr;		// var that is an array
    SLList<int> shape;		// list of dimension sizes (i.e., the shape)

    void duplicate(const Array &a);

public:
    Array(const String &n = (char *)0, const String &t = "Array",
	  BaseType *v = 0);
    Array(const Array &rhs);
    virtual ~Array();

    const Array &operator=(const Array &rhs);
    virtual BaseType *ptr_duplicate(); // alloc new Array and dup THIS to it

    virtual unsigned int size();

    virtual bool read(String dataset, String var_name, String constraint);

    virtual bool serialize(unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual BaseType *var(const String &name = (char *)0);
    virtual void add_var(BaseType *v, Part p = nil);

    void append_dim(int dim);

    Pix first_dim();
    void next_dim(Pix &p);
    int dim(Pix p);

    int dimensions();

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual void print_val(ostream &os, String space = "");

    virtual bool check_semantics(bool all = false);
};

#endif
