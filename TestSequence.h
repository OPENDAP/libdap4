
// -*- C++ -*-

// Interface for the class TestSequence. See TestByte.h
//
// jhrg 1/12/95

/* $Log: TestSequence.h,v $
/* Revision 1.1  1995/01/19 20:20:52  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
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
