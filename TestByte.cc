
// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestByte. See the comments in TestByte.h
// For each of the `variable classes' (e.g., Byte, ... Array, ... Grid) you
// *must* define a ctor, dtor, ptr_duplicate and read mfunc. In addition, you
// must edit the definition of New<class name> so that it creates the correct
// type of object. for example, edit NewByte() so that it creates and returns
// a TestByte pointer (see util.cc).
//
// jhrg 1/12/95

// $Log: TestByte.cc,v $
// Revision 1.14  1996/08/13 20:50:40  jimg
// Changed definition of the read member function.
//
// Revision 1.13  1996/05/31 23:30:10  jimg
// Updated copyright notice.
//
// Revision 1.12  1996/05/22 18:05:18  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.11  1996/04/04 18:05:07  jimg
// Merged changes from version 1.1.1.
//
// Revision 1.10  1995/12/09  01:07:07  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.9  1995/12/06  19:55:17  jimg
// Changes read() member function from three arguments to two.
//
// Revision 1.8  1995/08/26  00:31:51  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.7.2.1  1996/02/23 21:37:27  jimg
// Updated for new configure.in.
// Fixed problems on Solaris 2.4.
//
// Revision 1.7  1995/07/09  21:29:09  jimg
// Added copyright notice.
//
// Revision 1.6  1995/05/10  17:35:25  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.5  1995/03/16  17:42:43  jimg
// Minor fixes.
//
// Revision 1.4  1995/03/04  14:38:01  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:38  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:58:51  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:37  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "TestByte.h"

// The NewByte `helper function' creates a pointer to the a TestByte and
// returns that pointer. It takes the same arguments as the class's ctor. If
// any of the variable classes are subclassed (e.g., to make a new Byte like
// HDFByte) then the corresponding function here, and in the other class
// definition files, needs to be changed so that it creates an instnace of
// the new (sub)class. Continuing the earlier example, that would mean that
// NewByte() would return a HDFByte, not a Byte.
//
// It is important that these function's names and return types do not change
// - they are called by the parser code (for the dds, at least) so if their
// names changes, that will break.
//
// The declarations for these fuctions (in util.h) should *not* need
// changing. 

Byte *
NewByte(const String &n)
{
    return new TestByte(n);
}

TestByte::TestByte(const String &n) : Byte(n)
{
}

BaseType *
TestByte::ptr_duplicate()
{
    return new TestByte(*this);
}

bool
TestByte::read(const String &, int &)
{
    if (read_p())
	return true;

    _buf = 255;

    set_read_p(true);
    
    return true;
}
