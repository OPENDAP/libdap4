
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// TestUInt16 interface. See TestByte.h for more info.
//
// 3/22/99 jhrg

/* 
 * $Log: TestUInt16.h,v $
 * Revision 1.1  1999/03/24 23:40:06  jimg
 * Added
 *
 */

#ifndef _TestUInt16_h
#define _TestUInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt16.h"

class TestUInt16: public UInt16 {
public:
    TestUInt16(const String &n = (char *)0);
    virtual ~TestUInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
};

#endif

