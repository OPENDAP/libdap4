
// -*- C++ -*-

// TestInt32 interface. See TestByte.h for more info.
//
// jhrg 1/12/95

/* $Log: TestInt32.h,v $
/* Revision 1.1  1995/01/19 20:20:48  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestInt32_h
#define _TestInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int32.h"

class TestInt32: public Int32 {
public:
    TestInt32(const String &n = (char *)0);
    virtual ~TestInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(String dataset, String var_name, String constraint);
};

typedef TestInt32 * TestInt32Ptr;

#endif

