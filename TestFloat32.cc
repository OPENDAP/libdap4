
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestFloat32. See TestByte.cc
//
// 3/22/99 jhrg

// $Log: TestFloat32.cc,v $
// Revision 1.2  1999/04/29 02:29:32  jimg
// Merge of no-gnu branch
//
// Revision 1.1  1999/03/24 23:40:05  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestFloat32.h"

Float32 *
NewFloat32(const string &n)
{
    return new TestFloat32(n);
}

TestFloat32::TestFloat32(const string &n) : Float32(n)
{
}

BaseType *
TestFloat32::ptr_duplicate()
{
    return new TestFloat32(*this); // Copy ctor calls duplicate to do the work
}
 
bool
TestFloat32::read(const string &, int &)
{
    if (read_p())
	return true;

    _buf = 99.999;

    set_read_p(true);

    return true;
}
