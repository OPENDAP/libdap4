
// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestStructure.cc,v $
// Revision 1.5  1995/05/10 17:35:34  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.4  1995/03/04  14:38:10  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:48  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:59:05  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:56  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "TestStructure.h"
#ifdef NEVER
#include "Test.h"

String teststructure = "TestStructure";
#endif

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

// For this `Test' class, run the read mfunc for each of variables which
// comprise the structure. 
//
// To use this mfunc you must make sure that the instnace of TestStructure
// actually has fields defined (using add_var()).
//
// NB: In general it won't work to just pass the constraint expression down
// to the variable's read mfunc; you will have to parse that c expr and send
// only the relavent parts.

bool
TestStructure::read(String dataset, String var_name, String constraint)
{
    for (Pix p = first_var(); p; next_var(p)) {
	if (!var(p)->read(dataset, var(p)->get_var_name(), constraint))
	    return false;
    }

    return true;
}
