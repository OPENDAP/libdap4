
// -*- C++ -*-

// Interface for the class Sequence. A sequence contains a single set of
// variables, all at the same lexical level just like a strucuture (and like
// a structure, it may contain other ctor types...). Unlike a structure, a
// sequence defines a pattern that is repeated N times for a sequence of N
// elements. Thus, Sequence { String name; Int32 age; } person; means a
// sequence of N persons which each contain a name and age. The sequence can
// be arbitraily long (i.e., you don't know N by looking at the sequence
// declaration. The variables contained in a sequence are stored by instances
// of this class in a SLList of BaseType pointers.
//
// jhrg 9/14/94

#ifndef _Sequence_h
#define _Sequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <SLList.h>
#include "Structure.h"

class Sequence: public Structure {
private:

public:
    Sequence(const String &n = (char *)0, const String &t = "Sequence");
    // use the default copy ctor, and op=
    virtual ~Sequence() {}
};

#endif
