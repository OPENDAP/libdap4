
// (c) COPYRIGHT URI/MIT 1996-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This file contains mfuncs defined for struct rvalue (see expr.h) that
// *cannot* be included in that struct's declaration because their
// definitions must follow *both* rvalue's and func_rvalue's declarations.
// jhrg 3/4/96

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: RValue.cc,v 1.9 2003/01/10 19:46:40 jimg Exp $"};

#include <assert.h>

#include "BaseType.h"
#include "expr.h"
#include "RValue.h"
#include "DDS.h"

rvalue::rvalue(BaseType *bt): value(bt), func(0), args(0)
{
}

rvalue::rvalue(btp_func f, rvalue_list *a) : value(0), func(f), args(a)
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
    rvalue is a BaseType *, ensures that the read mfunc has been
    called. If the rvalue is a func_rvalue, evaluates the func_rvalue and
    returns the result.
      
    NB: The functions referenced by func_rvalues must encapsulate their
    return values in BaseType *s. */
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
	//  MS Visual C++ 6.0 doesn't allow arithmetic expressions in []
	//  except in the left-most [] with the new operator.
	BaseType **argv = new BaseType*[argc + 1];
#else
	BaseType *argv[argc + 1];
#endif

	int index = 0;
	for (Args_iter i = args->begin(); i != args->end(); i++)
	{
	    assert(*i) ;
	    argv[index++] = (*i)->bvalue(dataset, dds);
	}

	argv[index] = 0;		// Add null terminator
	return (*func)(argc, argv, dds);
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
#ifdef WIN32
	//  MS Visual C++ 6.0 doesn't allow arithmetic expressions in []
	//  except in the left-most [] with the new operator.
	BaseType **argv = new BaseType*[argc + 1];
#else
    BaseType **argv = new (BaseType *)[argc + 1];
#endif
    string dataset = dds.filename();
		
    int index = 0;
    if (argc) {
	for (rvalue::Args_iter i = args->begin(); i != args->end(); i++)
	{
	    argv[index++] = (*i)->bvalue(dataset, dds);
	}
    }

    argv[index] = 0;		// Add the null terminator.

    return argv;
}

// $Log: RValue.cc,v $
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

