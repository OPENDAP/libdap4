
// (c) COPYRIGHT URI/MIT 1995-1996,1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for TestArray. See TestByte.cc
//
// jhrg 1/12/95

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

#include <assert.h>

#include "TestArray.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

Array *
NewArray(const string &n, BaseType *v)
{
    return new TestArray(n, v);
}

BaseType *
TestArray::ptr_duplicate()
{
    return new TestArray(*this);
}

TestArray::TestArray(const string &n, BaseType *v) : Array(n, v)
{
}

TestArray::~TestArray()
{
}

// This read mfunc does some strange things to get a value - a real program
// would never get values this way. For testing this is OK.

bool
TestArray::read(const string &dataset)
{
    if (read_p())
	return true;

    unsigned i;

    // run read() on the contained variable to get, via the read() mfuncs
    // defined in the other Test classes, a value in the *contained* object.
    var()->read(dataset);

    unsigned int array_len = length(); // elements in the array

    switch (var()->type()) {
      case dods_byte_c:
      case dods_int32_c:
      case dods_uint32_c:
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

	delete elem_val;	// alloced in buf2val()
	delete[] tmp;		// alloced above

	break;
      }

      case dods_list_c:
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
	assert(false);
	cerr << "__FILE__:__LINE__ Bad DODS data type" << endl;
	break;
    }

    set_read_p(true);

    return true;
}

// $Log: TestArray.cc,v $
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

