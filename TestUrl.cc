
// Implementation for TestUrl. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestUrl.cc,v $
// Revision 1.1  1995/01/19 20:20:58  jimg
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
