
// Implementation for TestInt32. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestInt32.cc,v $
// Revision 1.1  1995/01/19 20:20:47  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestInt32.h"

Int32 *
NewInt32(const String &n)
{
    return new TestInt32(n);
}

TestInt32::TestInt32(const String &n) : Int32(n)
{
}

BaseType *
TestInt32::ptr_duplicate()
{
    return new TestInt32(*this);
}

bool
TestInt32::read(String dataset, String var_name, String constraint)
{
    buf = 123456789;
    
    return true;
}
