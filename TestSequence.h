
// -*- C++ -*-

// Interface for the class TestSequence. See TestByte.h
//
// jhrg 1/12/95

/* $Log: TestSequence.h,v $
/* Revision 1.3  1995/02/10 02:33:59  jimg
/* Modified Test<class>.h and .cc so that they used to new definitions of
/* read_val().
/* Modified the classes read() so that they are more in line with the
/* class library's intended use in a real subclass set.
/*
 * Revision 1.2  1995/01/19  21:59:42  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.1  1995/01/19  20:20:52  jimg
 * Created as an example of subclassing the class hierarchy rooted at
 * BaseType.
 *
 */

#ifndef _TestSequence_h
#define _TestSequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Sequence.h"

class TestSequence: public Sequence {
public:
    TestSequence(const String &n = (char *)0);
    virtual ~TestSequence();

    virtual BaseType *ptr_duplicate();

    virtual bool read(String dataset, String var_name, String constraint);
};

#endif
