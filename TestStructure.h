
// -*- C++ -*-

// Interface for the class TestStructure. See TestByte.h
//
// jhrg 1/12/95

/* $Log: TestStructure.h,v $
/* Revision 1.1  1995/01/19 20:20:57  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestStructure_h
#define _TestStructure_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Structure.h"

class TestStructure: public Structure {
public:
    TestStructure(const String &n = (char *)0);
    virtual ~TestStructure();

    virtual BaseType *ptr_duplicate();

    virtual bool read(String dataset, String var_name, String constraint);
};

#endif
