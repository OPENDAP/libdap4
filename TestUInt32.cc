
// (c) COPYRIGHT URI/MIT 1996,1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestUInt32. See TestByte.cc
//
// jhrg 10/27/96

// $Log: TestUInt32.cc,v $
// Revision 1.3  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.2.6.1  2000/08/02 20:58:26  jimg
// Included the header config_dap.h in this file. config_dap.h has been
// removed from all of the DODS header files.
//
// Revision 1.2  1999/04/29 02:29:33  jimg
// Merge of no-gnu branch
//
// Revision 1.1.14.1  1999/02/02 21:57:04  jimg
// String to string version
//
// Revision 1.1  1996/10/28 23:06:40  jimg
// Added.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"
#include "TestUInt32.h"

UInt32 *
NewUInt32(const string &n)
{
    return new TestUInt32(n);
}

TestUInt32::TestUInt32(const string &n) : UInt32(n)
{
}

BaseType *
TestUInt32::ptr_duplicate()
{
    return new TestUInt32(*this);
}

bool
TestUInt32::read(const string &, int &)
{
    if (read_p())
	return true;

    _buf = 0xf0000000;		// about 4 billion

    set_read_p(true);
    
    return true;
}

