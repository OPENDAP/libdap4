
// Implementation for TestFloat64. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestFloat64.cc,v $
// Revision 1.2  1995/01/19 21:58:53  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:39  jimg
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

bool
TestFloat64::read_val(void *stuff)
{
    return true;
}

