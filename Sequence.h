
// -*- C++ -*-

// Interface for the class Sequence. A sequence contains a single set of
// variables, all at the same lexical level just like a strucuture (and like
// a structure, it may contain other ctor types...). Unlike a structure, a
// sequence defines a pattern that is repeated N times for a sequence of N
// elements. Thus, Sequence { String name; Int32 age; } person; means a
// sequence of N persons where each contain a name and age. The sequence can
// be arbitraily long (i.e., you don't know N by looking at the sequence
// declaration.
//
// jhrg 9/14/94

/* $Log: Sequence.h,v $
/* Revision 1.7  1995/01/18 18:40:25  dan
/* Declared member function 'readVal', defined in dummy_read.cc
/*
 * Revision 1.6  1995/01/11  15:54:54  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.5  1994/12/15  21:21:54  dan
 * Modified class Sequence inheritance hierarchy, now directly inherits
 * from class BaseType.
 *
 * Revision 1.4  1994/11/22  14:06:04  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:50  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:48:32  jimg
 * Fixed some errors in comments.
 */

#ifndef _Sequence_h
#define _Sequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <SLList.h>
#include "BaseType.h"

#include "config.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

class Sequence: public BaseType {
private:
    SLList<BaseTypePtr> vars;

    void duplicate(const Sequence &s);

public:
    Sequence(const String &n = (char *)0);
    Sequence(const Sequence &rhs);
    virtual ~Sequence();

    const Sequence &operator=(const Sequence &rhs);
    virtual BaseType *ptr_duplicate();

    virtual unsigned int size();
    virtual bool read(String dataset, String var_name, String constraint);
    virtual bool readVal(void *stuff);

    virtual bool serialize(bool flush = false, unsigned int num = 0);
    virtual unsigned int deserialize();

    virtual BaseType *var(const String &name);
    virtual void add_var(BaseType *, Part p = nil);

    Pix first_var();
    void next_var(Pix &p);
    BaseType *var(Pix p);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual void print_val(ostream &os, String space = "");
    virtual bool check_semantics(bool all = false);
};

#endif
