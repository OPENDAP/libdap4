// -*- C++ -*-

// Interface for TestUrl type. See TestByte.h
//
// jhrg 1/12/95

/* $Log: TestUrl.h,v $
/* Revision 1.1  1995/01/19 20:20:59  jimg
/* Created as an example of subclassing the class hierarchy rooted at
/* BaseType.
/*
 */

#ifndef _TestUrl_h
#define _TestUrl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Url.h"

class TestUrl: public Url {
public:
    TestUrl(const String &n = (char *)0);
    virtual ~TestUrl() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(String dataset, String var_name, String constraint);
};

typedef TestUrl * TestUrlPtr;

#endif

