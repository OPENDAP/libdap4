
// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestSequence.cc,v $
// Revision 1.1  1995/01/19 20:20:51  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "TestSequence.h"

Sequence *
NewSequence(const String &n)
{
    return new TestSequence(n);
}

// protected

BaseType *
TestSequence::ptr_duplicate()
{
    return new TestSequence(*this);
}

// public

TestSequence::TestSequence(const String &n) : Sequence(n)
{
}

TestSequence::~TestSequence()
{
}

bool 
TestSequence::read(String dataset, String var_name, String constraint)
{
}
