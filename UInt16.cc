
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Int32.
//
// jhrg 9/7/94

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: UInt16.cc,v 1.16 2001/08/24 17:46:22 jimg Exp $"};

#include <stdlib.h>
#include <assert.h>

#include "UInt16.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "expr.tab.h"
#include "Operators.h"
#include "dods-limits.h"
#include "debug.h"
#include "InternalErr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
#include <xdr.h>
#endif

using std::cerr;
using std::endl;

UInt16::UInt16(const string &n) 
    : BaseType(n, dods_uint16_c, (xdrproc_t)XDR_UINT16)
{
}

UInt16::UInt16(const UInt16 &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}
    
UInt16 &
UInt16::operator=(const UInt16 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int
UInt16::width()
{
    return sizeof(dods_uint16);
}

bool
UInt16::serialize(const string &dataset, DDS &dds, XDR *sink,
		  bool ce_eval)
{
  // Jose Garcia
  // Since the read method is virtual and implemented outside
  // libdap++ if we can not read the data that is the problem 
  // of the user or of whoever wrote the surrogate library
  // implemeting read therefore it is an internal error.
#if 0
    if (!read_p() && !read(dataset))
	throw InternalErr(__FILE__, __LINE__, "Cannot read data.");
#endif
    try {
      if (!read_p())
	read(dataset);
    }
    catch (Error &e) {
      return false;
    }

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!XDR_UINT16(sink, &_buf))
	return false;

    return true;
}

bool
UInt16::deserialize(XDR *source, DDS *, bool)
{
    unsigned int num = XDR_UINT16(source, &_buf);

    return (num > 0);		/* make the return value a boolean */
}

unsigned int
UInt16::val2buf(void *val, bool)
{
    // Jose Garcia
    // This method is public therefore and I believe it has being designed
    // to be use by read which must be implemented on the surrogated library,
    // thus if the pointer val is NULL, is an Internal Error. 
    if(!val)
	throw InternalErr(__FILE__, __LINE__, 
			  "The incoming pointer does not contain any data.");

    _buf = *(dods_uint16 *)val;

    return width();
}

unsigned int
UInt16::buf2val(void **val)
{
    // Jose Garcia
    // The same comment justifying throwing an Error in val2buf applies here.
    if (!val)
	throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    if (!*val)
	*val = new dods_uint16;

    *(dods_uint16 *)*val =_buf;

    return width();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
UInt16::print_val(ostream &os, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

bool
UInt16::ops(BaseType *b, int op, const string &dataset)
{
    // Extract the Byte arg's value.
    if (!read_p() && !read(dataset)) {
	// Jose Garcia
	// Since the read method is virtual and implemented outside
	// libdap++ if we can not read the data that is the problem 
	// of the user or of whoever wrote the surrogate library
	// implemeting read therefore it is an internal error.
	throw InternalErr(__FILE__, __LINE__, "This value was not read!");
    }

    // Extract the second arg's value.
    if (!b->read_p() && !b->read(dataset)) {
	// Jose Garcia
	// Since the read method is virtual and implemented outside
	// libdap++ if we can not read the data that is the problem 
	// of the user or of whoever wrote the surrogate library
	// implemeting read therefore it is an internal error.
	throw InternalErr(__FILE__, __LINE__, "This value was not read!");
    }

    switch (b->type()) {
      case dods_byte_c:
	return rops<dods_uint16, dods_byte, Cmp<dods_uint16, dods_byte> >
	    (_buf, dynamic_cast<Byte *>(b)->_buf, op);
      case dods_int16_c:
	return rops<dods_uint16, dods_int16, USCmp<dods_uint16, dods_int16> >
	    (_buf, dynamic_cast<Int16 *>(b)->_buf, op);
      case dods_uint16_c:
	return rops<dods_uint16, dods_uint16, Cmp<dods_uint16, dods_uint16> >
	    (_buf, dynamic_cast<UInt16 *>(b)->_buf, op);
      case dods_int32_c:
	return rops<dods_uint16, dods_int32, USCmp<dods_uint16, dods_int32> >
	    (_buf, dynamic_cast<Int32 *>(b)->_buf, op);
      case dods_uint32_c:
	return rops<dods_uint16, dods_uint32, Cmp<dods_uint16, dods_uint32> >
	    (_buf, dynamic_cast<UInt32 *>(b)->_buf, op);
      case dods_float32_c:
	return rops<dods_uint16, dods_float32, Cmp<dods_uint16, dods_float32> >
	    (_buf, dynamic_cast<Float32 *>(b)->_buf, op);
      case dods_float64_c:
	return rops<dods_uint16, dods_float64, Cmp<dods_uint16, dods_float64> >
	    (_buf, dynamic_cast<Float64 *>(b)->_buf, op);
      default:
	return false;
    }
}

// $Log: UInt16.cc,v $
// Revision 1.16  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.15.4.2  2001/08/18 00:14:23  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.15.4.1  2001/07/28 01:10:42  jimg
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
// Revision 1.15  2000/10/06 01:26:05  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.14  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.13  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.12  2000/08/29 21:22:54  jimg
// Merged with 3.1.9
//
// Revision 1.11  2000/08/22 19:42:28  jimg
// Fixed the assert() in buf2val(). It tested _buf and when that contained 0
// it would fail. This was probably copied from other methods where _buf is
// a pointer.
//
// Revision 1.10  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.9  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.8.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
//
// Revision 1.8.14.2  2000/02/17 05:03:16  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.8.14.1  2000/01/28 22:14:06  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.8  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.7  1999/03/24 23:37:15  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.6.6.2  1999/02/05 09:32:35  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.6.6.1  1999/02/02 21:57:04  jimg
// String to string version
//
// Revision 1.6  1998/03/19 23:30:29  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.5  1997/09/22 22:40:54  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.4  1996/12/02 23:10:29  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.3  1996/10/18 16:49:55  jimg
// Changed variable types used for comparison operations from 16 to 32 bit
// integers.
//
// Revision 1.2  1996/10/01 16:29:16  jimg
// Changed instances of XDT_INT16 to XDR_UINT16.
//
// Revision 1.1  1996/08/26 20:17:49  jimg
// Added.
//

