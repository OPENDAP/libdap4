
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestFloat32. See TestByte.cc
//
// 3/22/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"
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
TestFloat32::read(const string &)
{
    if (read_p())
	return true;

    _buf = (float)99.999;

    set_read_p(true);

    return true;
}

// $Log: TestFloat32.cc,v $
// Revision 1.6  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.5  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.4  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.2.6.1  2000/08/02 20:58:26  jimg
// Included the header config_dap.h in this file. config_dap.h has been
// removed from all of the DODS header files.
//
// Revision 1.3  2000/07/18 03:04:09  rmorris
// Inserted simple trivial cast that makes VC++ happier.
//
// Revision 1.2.14.1  2000/02/17 05:03:14  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.2  1999/04/29 02:29:32  jimg
// Merge of no-gnu branch
//
// Revision 1.1  1999/03/24 23:40:05  jimg
// Added
//

