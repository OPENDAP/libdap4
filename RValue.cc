
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1996-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// This file contains mfuncs defined for struct rvalue (see expr.h) that
// *cannot* be included in that struct's declaration because their
// definitions must follow *both* rvalue's and func_rvalue's declarations.
// jhrg 3/4/96

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: RValue.cc,v 1.14 2004/02/19 19:42:52 jimg Exp $"};

#include <assert.h>

#include <iostream>

#include "BaseType.h"
#include "expr.h"
#include "RValue.h"
#include "DDS.h"

using namespace std;

rvalue::rvalue(BaseType *bt): value(bt), func(0), args(0)
{
}

rvalue::rvalue(btp_func f, vector<rvalue *> *a) : value(0), func(f), args(a)
{
}

rvalue::rvalue(): value(0), func(0), args(0)
{
}

rvalue::~rvalue() 
{
    // Deleting the BaseType pointers in value and args is a bad idea since
    // those might be variables in the dataset. The DDS dtor will take care
    // of deleting them. The constants wrapped in BaseType objects should be
    // pushed on the list of CE-allocated temp objects which the DDS also
    // frees. 
}

string
rvalue::value_name()
{
    assert(value);

    return value->name();
}

// Return the BaseType * to a value for an rvalue.
// NB: this must be defined after the struct func_rvalue (since it uses
// func_rvalue's bvalue() mfunc. 

/** Return the BaseType * that contains a value for a given rvalue. If the
    rvalue is a BaseType *, ensures that the read mfunc has been called. If
    the rvalue is a func_rvalue, evaluates the func_rvalue and returns the
    result. The functions referenced by func_rvalues must encapsulate their
    return values in BaseType *s. 

    @note If the BaseType pointer is a Sequence, calling read loads the first
    value; because the read_p property is set code that iteratively reads
    rows of the Sequence will work *if* it first checks read_p. In other
    words, all code that reads rows of Sequences should check read_p. If that
    property is true, then the code should assume that one rows worth of data
    has already been read. 
*/
BaseType *
rvalue::bvalue(const string &dataset, DDS &dds) 
{
    if (value) {
	if (!value->read_p())
	    value->read(dataset);

	return value;
    }
    else if (func) {
	int argc = args->size();
	// Add space for null terminator
#ifdef WIN32
	BaseType **argv = (new (BaseType*)) + argc + 1;
#else
	BaseType **argv = new (BaseType*)[argc + 1];
#endif

	int index = 0;
	for (Args_iter i = args->begin(); i != args->end(); i++)
	{
	    assert(*i) ;
	    argv[index++] = (*i)->bvalue(dataset, dds);
	}

	argv[index] = 0;		// Add null terminator
	BaseType *ret_val = (*func)(argc, argv, dds);

#ifdef WIN32
	delete *argv; *argv = 0; // Hmmm... 02/03/04 jhrg 
#else
	delete[] argv; argv = 0;
#endif

	return ret_val;
    }
    else {
	return 0;
    }
}


/** Build an argument list suitable for calling a <tt>btp_func</tt>,
    <tt>bool_func</tt>, and so on. Since this takes an <tt>rvalue_list</tt> and
    not an rvalue, it is a function rather than a class
    member. 

    This function performs a common task but does not fit within the RValue
    class well. It is used by Clause and expr.y. */
BaseType **
build_btp_args(rvalue_list *args, DDS &dds)
{
    int argc = 0;

    if (args)
	argc = args->size();

    // Add space for a null terminator
    BaseType **argv = new (BaseType *[argc + 1]);

    string dataset = dds.filename();
		
    int index = 0;
    if (argc) {
	for (rvalue::Args_iter i = args->begin(); i != args->end(); i++) {
	    argv[index++] = (*i)->bvalue(dataset, dds);
	}
    }

    argv[index] = 0;		// Add the null terminator.

    return argv;
}

// $Log: RValue.cc,v $
// Revision 1.14  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.12.2.4  2004/02/11 22:26:46  jimg
// Changed all calls to delete so that whenever we use 'delete x' or
// 'delete[] x' the code also sets 'x' to null. This ensures that if a
// pointer is deleted more than once (e.g., when an exception is thrown,
// the method that throws may clean up and then the catching method may
// also clean up) the second, ..., call to delete gets a null pointer
// instead of one that points to already deleted memory.
//
// Revision 1.12.2.3  2004/02/04 00:05:11  jimg
// Memory errors: I've fixed a number of memory errors (leaks, references)
// found using valgrind. Many remain. I need to come up with a systematic
// way of running the tests under valgrind.
//
// Revision 1.13  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.12.2.2  2003/09/06 23:10:40  jimg
// Fixed a bug in rvalue::bvalue() where the send_p property was being set and
// reset before and after the call to read() inside the if(value) statement.
// These calls where a fix to a design flaw in BaseType. I fixed the problem in
// BaseType by adding the in_selection property. This is used to tell read()
// methods that a variable is in the current selection and should be read even
// if it is not in the current projection. See bug 657.
//
// Revision 1.12.2.1  2003/08/30 09:58:36  rmorris
// Changed syntax for allocating an array of objects (BaseType)
// to be something compatible with both VC++ and gnu compilers.
// See build_btp_args().  Fixes several expr-testsuite failures under
// win32.
//
// Revision 1.12  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.10.2.3  2003/04/18 07:14:35  rmorris
// Fixed portability problem with the new operator used in tandem
// with [] in VC++.
//
// Revision 1.10.2.2  2003/04/18 03:29:03  jimg
// Added set/reset of the send_p flag when a variable is read in the bvalue()
// method. If this is not done then all the parts of a constructor type (like
// Grid) won't be read (e.g., the Grid's map vectors are not read). I reset
// send_p after reading in case the variable is an argument to a function and
// not part of the current projection. That's pretty unlikely, but possible.
//
// Revision 1.11  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.10.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.10  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.9  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.7.4.2  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.7.4.1  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.8  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.7  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.6  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.5  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.4.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
//
// Revision 1.4.14.1  2000/02/17 05:03:13  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.4  1999/05/04 19:47:21  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.3  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/01/21 02:54:27  jimg
// Fixed dataset; this variable should be the filename of the dataset, not the
// value returned by get_data_name() which is the DODS name of the dataset.
// This value is not set by the dds.filename() method.
//
// Revision 1.1  1998/10/21 16:14:16  jimg
// Added. Based on code that used to be in expr.h/cc
//
// Revision 1.9  1998/09/17 16:59:09  jimg
// Added a test in bvalue() to prevent non-existent variables/fields from being
// accessed. Instead the member function returns null.
//
// Revision 1.8  1997/02/10 02:32:45  jimg
// Added assert statements for pointers
//
// Revision 1.7  1996/11/27 22:40:22  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.6  1996/08/13 18:55:48  jimg
// Added __unused__ to definition of char rcsid[].
//
// Revision 1.5  1996/05/31 23:31:00  jimg
// Updated copyright notice.
//
// Revision 1.4  1996/05/29 22:08:54  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.3  1996/05/22 18:05:36  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.2  1996/03/05 00:54:35  jimg
// Added dtor for rvalue.
// Modified ctor to work with new btp_rvalue class.
//
// Revision 1.1  1996/02/23 17:30:21  jimg
// Created.
//

