
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestInt16. See TestByte.cc
//
// 3/22/99 jhrg

// $Log: TestInt16.cc,v $
// Revision 1.1  1999/03/24 23:40:06  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestInt16.h"

Int16 *
NewInt16(const String &n)
{
    return new TestInt16(n);
}

TestInt16::TestInt16(const String &n) : Int16(n)
{
}

BaseType *
TestInt16::ptr_duplicate()
{
    return new TestInt16(*this);
}

bool
TestInt16::read(const String &, int &)
{
    if (read_p())
	return true;

    _buf = 32000;

    set_read_p(true);
    
    return true;
}
