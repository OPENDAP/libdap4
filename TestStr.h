// -*- C++ -*-

// Interface for TestStr type. See TestByte.h
//
// jhrg 1/12/95

/* $Log: TestStr.h,v $
/* Revision 1.1  1995/01/19 20:20:55  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestStr_h
#define _TestStr_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <limits.h>

#include "Str.h"

class TestStr: public Str {
public:
    TestStr(const String &n = (char *)0);
    virtual ~TestStr() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(String dataset, String var_name, String constraint);
};

typedef TestStr * TestStrPtr;

#endif

