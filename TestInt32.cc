
// Implementation for TestInt32. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestInt32.cc,v $
// Revision 1.5  1995/05/10 17:35:30  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.4  1995/03/04  14:38:06  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:44  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:58:57  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:47  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestInt32.h"
#ifdef NEVER
#include "Test.h"

String testint32 = "TestInt32";
#endif

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
    _buf = 123456789;
    
    return true;
}
