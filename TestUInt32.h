
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
 * Revision 1.4  2000/09/21 16:22:09  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.3.14.1  2000/02/17 05:03:16  jimg
 * Added file and line number information to calls to InternalErr.
 * Resolved compile-time problems with read due to a change in its
 * parameter list given that errors are now reported using exceptions.
 *
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
    
    virtual bool read(const string &dataset);
};

#endif

