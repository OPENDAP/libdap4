
// -*- C++ -*-

// Interface definition for TestArray. See TestByte.h for more information
//
// jhrg 1/12/95

/* $Log: TestArray.h,v $
/* Revision 1.4  1995/12/06 19:55:33  jimg
/* Changes read() member function from three arguments to two.
/*
 * Revision 1.3  1995/02/10  02:33:51  jimg
 * Modified Test<class>.h and .cc so that they used to new definitions of
 * read_val().
 * Modified the classes read() so that they are more in line with the
 * class library's intended use in a real subclass set.
 *
 * Revision 1.2  1995/01/19  21:59:28  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.1  1995/01/19  20:20:36  jimg
 * Created as an example of subclassing the class hierarchy rooted at
 * BaseType.
 *
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

    virtual bool read(String dataset, String var_name);
};

#endif


