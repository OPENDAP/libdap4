
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestUInt16. See TestByte.cc
//
// 3/22/99 jhrg

// $Log: TestUInt16.cc,v $
// Revision 1.1  1999/03/24 23:40:06  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestUInt16.h"

UInt16 *
NewUInt16(const String &n)
{
    return new TestUInt16(n);
}

TestUInt16::TestUInt16(const String &n) : UInt16(n)
{
}

BaseType *
TestUInt16::ptr_duplicate()
{
    return new TestUInt16(*this);
}

bool
TestUInt16::read(const String &, int &)
{
    if (read_p())
	return true;

    _buf = 64000;

    set_read_p(true);
    
    return true;
}
