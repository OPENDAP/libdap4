
// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestSequence.cc,v $
// Revision 1.5  1995/05/10 17:35:31  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.4  1995/03/04  14:38:08  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:46  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:59:00  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:51  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "TestSequence.h"
#ifdef NEVER
#include "Test.h"

String testsequence = "TestSequence";
#endif

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
