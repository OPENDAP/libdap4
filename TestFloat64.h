// -*- C++ -*-

// Interface for TestFloat64 type. See TestByte.h
//
// jhrg 1/12/95

/* $Log: TestFloat64.h,v $
/* Revision 1.2  1995/01/19 21:59:31  jimg
/* Added read_val from dummy_read.cc to the sample set of sub-class
/* implementations.
/* Changed the declaration of readVal in BaseType so that it names the
/* mfunc read_val (to be consistant with the other mfunc names).
/* Removed the unnecessary duplicate declaration of the abstract virtual
/* mfuncs read and (now) read_val from the classes Byte, ... Grid. The
/* declaration in BaseType is sufficient along with the decl and definition
/* in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
/*
 * Revision 1.1  1995/01/19  20:20:40  jimg
 * Created as an example of subclassing the class hierarchy rooted at
 * BaseType.
 *
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
    virtual bool read_val(void *stuff);
};

typedef TestFloat64 * TestFloat64Ptr;

#endif

