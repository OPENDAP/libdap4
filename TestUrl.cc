
// Implementation for TestUrl. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestUrl.cc,v $
// Revision 1.2  1995/01/19 21:59:06  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:58  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestUrl.h"

Url *
NewUrl(const String &n)
{
    return new TestUrl(n);
}

TestUrl::TestUrl(const String &n) : Url(n)
{
}

BaseType *
TestUrl::ptr_duplicate()
{
    return new TestUrl(*this);
}

bool
TestUrl::read(String dataset, String var_name, String constraint)
{
    const char url_test[]={"http://dcz.gso.uri.edu/avhrr-archive/archive.html"};

    if (_buf) {			// ctor sets BUF to 0
	free(_buf);
	_buf = 0;
    }

    _buf = (char *)malloc(strlen(url_test)+1);
    strcpy(_buf, url_test);

    return true;
}

bool
TestUrl::read_val(void *stuff)
{
    return true;
}
