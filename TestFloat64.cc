
// Implementation for TestFloat64. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestFloat64.cc,v $
// Revision 1.1  1995/01/19 20:20:39  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestFloat64.h"

Float64 *
NewFloat64(const String &n)
{
    return new TestFloat64(n);
}

TestFloat64::TestFloat64(const String &n) : Float64(n)
{
}

BaseType *
TestFloat64::ptr_duplicate()
{
    return new TestFloat64(*this); // Copy ctor calls duplicate to do the work
}
 
bool
TestFloat64::read(String dataset, String var_name, String constraint)
{
    buf = 99.999;

    return true;
}
