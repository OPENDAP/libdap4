
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996,1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// TestUInt32 interface. See TestByte.h for more info.
//
// jhrg 10/27/96

/* 
 * $Log: TestUInt32.h,v $
 * Revision 1.3  1999/04/29 02:29:33  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.2.10.1  1999/02/02 21:57:04  jimg
 * String to string version
 *
 * Revision 1.2  1997/08/11 18:19:29  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.1  1996/10/28 23:10:05  jimg
 * Added.
 */

#ifndef _TestUInt32_h
#define _TestUInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt32.h"

class TestUInt32: public UInt32 {
public:
    TestUInt32(const string &n = "");
    virtual ~TestUInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

#endif

