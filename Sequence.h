
// -*- C++ -*-

// Interface for the class Sequence. A sequence contains a single set of
// variables, all at the same lexical level just like a strucuture (and like
// a structure, it may contain other ctor types...). Unlike a structure, a
// sequence defines a pattern that is repeated N times for a sequence of N
// elements. Thus, Sequence { String name; Int32 age; } person; means a
// sequence of N persons which each contain a name and age. The sequence can
// be arbitraily long (i.e., you don't know N by looking at the sequence
// declaration.
//
// jhrg 9/14/94

/* $Log: Sequence.h,v $
/* Revision 1.3  1994/10/17 23:34:50  jimg
/* Added code to print_decl so that variable declarations are pretty
/* printed.
/* Added private mfunc duplicate().
/* Added ptr_duplicate().
/* Added Copy ctor, dtor and operator=.
/*
 * Revision 1.2  1994/09/23  14:48:32  jimg
 * Fixed some errors in comments.
 */

#ifndef _Sequence_h
#define _Sequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <SLList.h>
#include "Structure.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

class Sequence: public Structure {
private:
    void duplicate(const Sequence &s);

public:
    Sequence(const String &n = (char *)0, const String &t = "Sequence");
    Sequence(const Sequence &rhs);
    virtual ~Sequence();

    const Sequence &operator=(const Sequence &rhs);
    virtual BaseType *ptr_duplicate();
};

#endif
