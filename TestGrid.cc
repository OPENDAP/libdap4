
// implementation for TestGrid. See TestByte.
//
// jhrg 1/13/95

// $Log: TestGrid.cc,v $
// Revision 1.1  1995/01/19 20:20:44  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#include "TestGrid.h"

Grid *
NewGrid(const String &n)
{
    return new TestGrid(n);
}

// protected

BaseType *
TestGrid::ptr_duplicate()
{
    return new TestGrid(*this);
}

// public

TestGrid::TestGrid(const String &n) : Grid(n)
{
}

TestGrid::~TestGrid()
{
}

bool
TestGrid::read(String dataset, String var_name, String constraint)
{
}
