// -*- C++ -*-

// Interface for TestFloat64 type. See TestByte.h
//
// jhrg 1/12/95

/* $Log: TestFloat64.h,v $
/* Revision 1.1  1995/01/19 20:20:40  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestFloat64_h
#define _TestFloat64_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Float64.h"

class TestFloat64: public Float64 {
public:
    TestFloat64(const String &n = (char *)0);
    virtual ~TestFloat64() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(String dataset, String var_name, String constraint);
};

typedef TestFloat64 * TestFloat64Ptr;

#endif

