// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for TestFloat32 type. See TestByte.h
//
// 3/22/99 jhrg

/* 
 * $Log: TestFloat32.h,v $
 * Revision 1.2  1999/04/29 02:29:32  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.1  1999/03/24 23:40:05  jimg
 * Added
 *
 */

#ifndef _TestFloat32_h
#define _TestFloat32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Float32.h"

class TestFloat32: public Float32 {
public:
    TestFloat32(const string &n = (char *)0);
    virtual ~TestFloat32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

#endif

