
// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestStructure.cc,v $
// Revision 1.1  1995/01/19 20:20:56  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "TestStructure.h"

Structure *
NewStructure(const String &n)
{
    return new TestStructure(n);
}

BaseType *
TestStructure::ptr_duplicate()
{
    return new TestStructure(*this);
}

TestStructure::TestStructure(const String &n) : Structure(n)
{
}

TestStructure::~TestStructure()
{
}

bool
TestStructure::read(String dataset, String var_name, String constraint)
{
}
