
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestUInt32. See TestByte.cc
//
// jhrg 10/27/96

// $Log: TestUInt32.cc,v $
// Revision 1.1  1996/10/28 23:06:40  jimg
// Added.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestUInt32.h"

UInt32 *
NewUInt32(const String &n)
{
    return new TestUInt32(n);
}

TestUInt32::TestUInt32(const String &n) : UInt32(n)
{
}

BaseType *
TestUInt32::ptr_duplicate()
{
    return new TestUInt32(*this);
}

bool
TestUInt32::read(const String &, int &)
{
    if (read_p())
	return true;

    _buf = 0xf0000000;		// about 4 billion

    set_read_p(true);
    
    return true;
}

