
// Implementation for TestStr. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestStr.cc,v $
// Revision 1.2  1995/01/19 21:59:03  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:53  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestStr.h"

Str *
NewStr(const String &n)
{
    return new TestStr(n);
}

TestStr::TestStr(const String &n) : Str(n)
{
}

BaseType *
TestStr::ptr_duplicate()
{
    return new TestStr(*this);
}

bool
TestStr::read(String dataset, String var_name, String constraint)
{
    const char str_test[]={"Silly test string: one, two, ..."};

    if (buf) {			// ctor sets BUF to 0
	free(buf);
	buf = 0;
    }

    buf = (char *)malloc(strlen(str_test)+1);
    strcpy(buf, str_test);

    return true;
}

bool
TestStr::read_val(void *stuff)
{
    return true;
}

