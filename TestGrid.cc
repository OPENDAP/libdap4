
// (c) COPYRIGHT URI/MIT 1995-1996,1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// implementation for TestGrid. See TestByte.
//
// jhrg 1/13/95

// $Log: TestGrid.cc,v $
// Revision 1.14  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.13.14.1  2000/02/17 05:03:14  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.13  1999/04/29 02:29:32  jimg
// Merge of no-gnu branch
//
// Revision 1.12.14.1  1999/02/02 21:57:03  jimg
// String to string version
//
// Revision 1.12  1996/05/31 23:30:19  jimg
// Updated copyright notice.
//
// Revision 1.11  1996/05/22 18:05:23  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.10  1996/04/05 00:21:51  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.9  1995/12/09  01:07:15  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.8  1995/12/06  19:55:22  jimg
// Changes read() member function from three arguments to two.
//
// Revision 1.7  1995/08/26  00:31:55  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.6  1995/07/09  21:29:14  jimg
// Added copyright notice.
//
// Revision 1.5  1995/05/10  17:35:29  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.4  1995/03/04  14:38:04  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:42  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:58:55  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:44  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#include <Pix.h>

#include "TestGrid.h"

Grid *
NewGrid(const string &n)
{
    return new TestGrid(n);
}

BaseType *
TestGrid::ptr_duplicate()
{
    return new TestGrid(*this);
}

TestGrid::TestGrid(const string &n) : Grid(n)
{
}

TestGrid::~TestGrid()
{
}

bool
TestGrid::read(const string &dataset)
{
    if (read_p())
	return true;

    array_var()->read(dataset);

    for (Pix p = first_map_var(); p; next_map_var(p)) {
	if (!map_var(p)->read(dataset))
	    return false;
    }

    set_read_p(true);

    return true;
}
