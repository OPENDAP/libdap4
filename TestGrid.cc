
// implementation for TestGrid. See TestByte.
//
// jhrg 1/13/95

// $Log: TestGrid.cc,v $
// Revision 1.2  1995/01/19 21:58:55  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:44  jimg
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

bool
TestGrid::read_val(void *stuff)
{
    return true;
}

