
/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// Implementation for TestArray. See TestByte.cc
//
// jhrg 1/12/95

// $Log: TestArray.cc,v $
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
// `config_dap.h' so that other libraries could have header files which were
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

#include <assert.h>

#include "TestArray.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

Array *
NewArray(const String &n, BaseType *v)
{
    return new TestArray(n, v);
}

BaseType *
TestArray::ptr_duplicate()
{
    return new TestArray(*this);
}

TestArray::TestArray(const String &n, BaseType *v) : Array(n, v)
{
}

TestArray::~TestArray()
{
}

// This read mfunc does some strange things to get a value - a real program
// would never get values this way. For testing this is OK.

bool
TestArray::read(const String &dataset, int &error)
{
    if (read_p())
	return true;

    int i;

    // run read() on the contained variable to get, via the read() mfuncs
    // defined in the other Test classes, a value in the *contained* object.
    var()->read(dataset, error);

    unsigned int array_len = length(); // elements in the array

    switch (var()->type()) {
      case dods_byte_c:
      case dods_int32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c: {

	// String and Url are grouped with dods_byte, ... because val2buf works
	// for these types.

	unsigned int elem_wid = var()->width(); // size of an element

	char *tmp = new char[width()];
	void *elem_val = 0;	// NULL init gets read_val() to alloc space

	var()->buf2val(&elem_val); // internal buffer to ELEM_VAL

	for (i = 0; i < array_len; ++i)
	    memcpy(tmp + i * elem_wid, elem_val, elem_wid);


	val2buf(tmp);

	delete[] elem_val;	// alloced in read_val()
	delete[] tmp;		// alloced above

	break;
      }

      case dods_list_c:
      case dods_structure_c:
      case dods_sequence_c:
      case dods_function_c:
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
	    
	    elem->read(dataset, error);

	    // now load the new instance in the array.

	    set_vec(i, elem);
	}

	break;

    }

    set_read_p(true);

    return true;
}
