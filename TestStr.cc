
// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestStr. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestStr.cc,v $
// Revision 1.14  1996/05/31 23:30:31  jimg
// Updated copyright notice.
//
// Revision 1.13  1996/05/22 18:05:29  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.12  1996/05/14 15:38:44  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.11  1996/04/05 00:22:01  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.10  1995/12/09  01:07:25  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.9  1995/12/06  19:55:28  jimg
// Changes read() member function from three arguments to two.
//
// Revision 1.8  1995/08/26  00:31:59  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.7  1995/08/23  00:44:35  jimg
// Updated to use the newer member functions.
//
// Revision 1.6  1995/07/09  21:29:19  jimg
// Added copyright notice.
//
// Revision 1.5  1995/05/10  17:35:33  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.4  1995/03/04  14:38:09  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:47  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:59:03  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:53  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>

#include <String.h>

#include "TestStr.h"

Str *
NewStr(const String &n)
{
    return new TestStr(n);
}

TestStr::TestStr(const String &n) : Str(n)
{
}

BaseType *
TestStr::ptr_duplicate()
{
    return new TestStr(*this);
}

bool
TestStr::read(const String &dataset, int &error)
{
    if (read_p())
	return true;

    String dods_str_test="Silly test string: one, two, ...";
    
    (void) val2buf(&dods_str_test);

    set_read_p(true);

    return true;
}
