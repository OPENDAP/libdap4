
// Implementation for TestArray. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestArray.cc,v $
// Revision 1.1  1995/01/19 20:20:34  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "TestArray.h"

Array *
NewArray(const String &n, BaseType *v)
{
    return new TestArray(n, v);
}

BaseType *
TestArray::ptr_duplicate()
{
    return new TestArray(*this);
}

TestArray::TestArray(const String &n, BaseType *v) : Array(n, v)
{
}

TestArray::~TestArray()
{
}

bool
TestArray::read(String dataset, String var_name, String constraint)
{
}

