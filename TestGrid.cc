
// implementation for TestGrid. See TestByte.
//
// jhrg 1/13/95

// $Log: TestGrid.cc,v $
// Revision 1.4  1995/03/04 14:38:04  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:42  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:58:55  jimg
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
#include "Test.h"

String testgrid = "TestGrid";

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
