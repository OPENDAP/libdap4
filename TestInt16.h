
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// TestInt16 interface. See TestByte.h for more info.
//
// 3/22/99 jhrg

/* 
 * $Log: TestInt16.h,v $
 * Revision 1.2  1999/04/29 02:29:32  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.1  1999/03/24 23:40:06  jimg
 * Added
 *
 */

#ifndef _TestInt16_h
#define _TestInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int16.h"

class TestInt16: public Int16 {
public:
    TestInt16(const string &n = "");
    virtual ~TestInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

#endif

