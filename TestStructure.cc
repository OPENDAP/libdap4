
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestStructure.cc,v $
// Revision 1.16  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.15  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.14.6.1  2000/08/02 20:58:26  jimg
// Included the header config_dap.h in this file. config_dap.h has been
// removed from all of the DODS header files.
//
// Revision 1.14.14.1  2000/02/17 05:03:15  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.14  1999/04/29 02:29:33  jimg
// Merge of no-gnu branch
//
// Revision 1.13.14.1  1999/02/02 21:57:03  jimg
// String to string version
//
// Revision 1.13  1996/05/31 23:30:34  jimg
// Updated copyright notice.
//
// Revision 1.12  1996/05/22 18:05:30  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.11  1996/03/05 18:49:23  jimg
// Fixed Errors in comments.
//
// Revision 1.10  1995/12/09  01:07:28  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.9  1995/12/06  19:55:29  jimg
// Changes read() member function from three arguments to two.
//
// Revision 1.8  1995/08/26  00:32:00  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.7  1995/08/23  00:44:37  jimg
// Updated to use the newer member functions.
//
// Revision 1.6  1995/07/09  21:29:20  jimg
// Added copyright notice.
//
// Revision 1.5  1995/05/10  17:35:34  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.4  1995/03/04  14:38:10  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:48  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:59:05  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:56  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"
#include "TestStructure.h"

Structure *
NewStructure(const string &n)
{
    return new TestStructure(n);
}

BaseType *
TestStructure::ptr_duplicate()
{
    return new TestStructure(*this);
}

TestStructure::TestStructure(const string &n) : Structure(n)
{
}

TestStructure::~TestStructure()
{
}

// For this `Test' class, run the read mfunc for each of variables which
// comprise the structure. 

bool
TestStructure::read(const string &dataset)
{
    if (read_p())
	return true;

    for (Pix p = first_var(); p; next_var(p)) {
	if (!var(p)->read(dataset))
	    return false;
    }

    set_read_p(true);

    return true;
}
