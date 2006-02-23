
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
 
// (c) COPYRIGHT URI/MIT 1995-1996,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for TestArray. See TestByte.cc
//
// jhrg 1/12/95

#include "config.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestArray.h"
#include "TestCommon.h"


using std::cerr;
using std::endl;

extern int test_variable_sleep_interval;

void
TestArray::_duplicate(const TestArray &ts)
{
    d_series_values = ts.d_series_values;
}

BaseType *
TestArray::ptr_duplicate()
{
    return new TestArray(*this);
}

TestArray::TestArray(const string &n, BaseType *v) : Array(n, v),
        d_series_values(false)
{
}

TestArray::TestArray(const TestArray &rhs) : Array(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestArray::~TestArray()
{
}

TestArray &
TestArray::operator=(const TestArray &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Array &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

// This read mfunc does some strange things to get a value - a real program
// would never get values this way. For testing this is OK.

bool
TestArray::read(const string &dataset)
{
    if (read_p())
		return true;

    if (test_variable_sleep_interval > 0)
		sleep(test_variable_sleep_interval);

    unsigned i;

    // run read() on the contained variable to get, via the read() mfuncs
    // defined in the other Test classes, a value in the *contained* object.
    var()->read(dataset);

    unsigned int array_len = length(); // elements in the array

    switch (var()->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c: {

	// String and Url are grouped with dods_byte, ... because val2buf works
	// for these types.

	unsigned int elem_wid = var()->width(); // size of an element

	char *tmp = new char[width()];
	// elem_val was a void pointer; delete complained. 6/4/2001 jhrg
	char *elem_val = 0;	// NULL init gets read_val() to alloc space

	// Added cast as temporary fix. 6/4/2001 jhrg
	var()->buf2val((void **)&elem_val); // internal buffer to ELEM_VAL

	for (i = 0; i < array_len; ++i)
	    memcpy(tmp + i * elem_wid, elem_val, elem_wid);


	val2buf(tmp);

	delete elem_val; elem_val = 0; // alloced in buf2val()
	delete[] tmp; tmp = 0;	// alloced above

	break;
      }

      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	
	// Arrays of Structure, ... must load each element into the array 
	// manually. Because these are stored as C++/DODS objects, there is
	// no need to manipulate blocks of memory by hand as in the above
	// case. 
        // NB: Strings are handled like Byte, etc. because, even though they
	// are represented using C++ objects they are *not* represented using
	// objects defined by DODS, while Structure, etc. are.

	for (i = 0; i < array_len; ++i) {

	    // Create a new object that is a copy of `var()' (whatever that
	    // is). The copy will have the value read in by the read() mfunc
	    // executed before this switch stmt.

	    BaseType *elem = var()->ptr_duplicate(); 

	    // read values into the new instance.
	    
	    elem->read(dataset);

	    // now load the new instance in the array.

	    set_vec(i, elem);
	}

	break;
	
      case dods_array_c:
      case dods_null_c:
      default:
	throw InternalErr(__FILE__, __LINE__, "Bad DODS data type");
	break;
    }

    set_read_p(true);

    return true;
}

void
TestArray::set_series_values(bool sv)
{
    dynamic_cast<TestCommon&>(*var()).set_series_values(sv);    
    d_series_values = sv;
}

// $Log: TestArray.cc,v $
// Revision 1.34  2005/03/30 23:12:01  jimg
// Modified to use the new factory class.
//
// Revision 1.33  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.29.2.4  2005/01/18 23:21:44  jimg
// All Test* classes now handle copy and assignment correctly.
//
// Revision 1.29.2.3  2005/01/14 19:38:37  jimg
// Added support for returning cyclic values.
//
// Revision 1.32  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.29.2.2  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.31  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.29.2.1  2003/07/23 23:56:36  jimg
// Now supports a simple timeout system.
//
// Revision 1.30  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.29  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.28  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.27.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.27  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.26  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.24.4.3  2001/08/18 00:15:27  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.24.4.2  2001/07/28 01:10:42  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.25  2001/06/15 23:49:02  jimg
// Merged with release-3-2-4.
//
// Revision 1.24.4.1  2001/06/05 06:49:19  jimg
// Added the Constructor class which is to Structures, Sequences and Grids
// what Vector is to Arrays and Lists. This should be used in future
// refactorings (I thought it was going to be used for the back pointers).
// Introduced back pointers so children can refer to their parents in
// hierarchies of variables.
// Added to Sequence methods to tell if a child sequence is done
// deserializing its data.
// Fixed the operator=() and copy ctors; removed redundency from
// _duplicate().
// Changed the way serialize and deserialize work for sequences. Now SOI and
// EOS markers are written for every `level' of a nested Sequence. This
// should fixed nested Sequences. There is still considerable work to do
// for these to work in all cases.
//
// Revision 1.24  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.23  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.22  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.21.14.1  2000/02/17 05:03:14  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.21  1999/05/04 19:47:22  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.20  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.19  1998/11/10 00:57:44  jimg
// Fixed errant delete [] (should have been plain delete).
//
// Revision 1.18.14.1  1999/02/02 21:57:02  jimg
// String to string version
//
// Revision 1.18  1996/12/02 18:21:17  jimg
// Added case for unit32 to ops() member functon.
//
// Revision 1.17  1996/08/13 20:50:39  jimg
// Changed definition of the read member function.
//
// Revision 1.16  1996/05/31 23:30:08  jimg
// Updated copyright notice.
//
// Revision 1.15  1996/05/22 18:05:16  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.14  1996/05/14 15:38:41  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.13  1996/04/05 21:59:29  jimg
// Misc Changes for release 2.0.1 of the core software - for developers.
//
// Revision 1.12  1996/04/05 00:21:43  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.11  1996/03/05 18:57:28  jimg
// Fixed problems with variable scoping in for and switch statements.
//
// Revision 1.10  1995/12/09  01:07:04  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.9  1995/12/06  19:55:15  jimg
// Changes read() member function from three arguments to two.
//
// Revision 1.8  1995/08/23  00:50:01  jimg
// Fixed the read() member function so that it works correctly for arrays/lists
// of Structure, ... types.
//
// Revision 1.7  1995/07/09  21:29:07  jimg
// Added copyright notice.
//
// Revision 1.6  1995/05/10  13:45:34  jimg
// Changed the name of the configuration header file from `config.h' to
// `config.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.5  1995/03/16  17:32:27  jimg
// Fixed bugs with read()s new & delete calls.
//
// Revision 1.4  1995/03/04  14:38:00  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:37  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:58:50  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:34  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//

