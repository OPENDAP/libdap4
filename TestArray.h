
// -*- C++ -*-

// Interface definition for TestArray. See TestByte.h for more information
//
// jhrg 1/12/95

/* $Log: TestArray.h,v $
/* Revision 1.1  1995/01/19 20:20:36  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestArray_h
#define _TestArray_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Array.h"

class TestArray: public Array {
public:
    TestArray(const String &n = (char *)0, BaseType *v = 0);
    virtual ~TestArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(String dataset, String var_name, String constraint);
};

#endif


