
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Byte.
//
// jhrg 9/7/94

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Byte.cc,v 1.42 2001/08/24 17:46:22 jimg Exp $"};

#include <stdlib.h>
#include <assert.h>

#include "Byte.h"
#include "Int16.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "expr.tab.h"
#include "Operators.h"
#include "dods-limits.h"
#include "InternalErr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::cerr;
using std::endl;

// NB: Even though Byte is a cardinal type, xdr_char is *not* used to
// transport Byte arrays over the network. Instead, Byte is a special case
// handled in Array.

Byte::Byte(const string &n) : BaseType(n, dods_byte_c)
{
}

Byte::Byte(const Byte &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}
    
Byte &
Byte::operator=(const Byte &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int
Byte::width()
{
    return sizeof(dods_byte);
}

// Serialize the contents of member _BUF (the object's internal buffer, used
// to hold data) and write the result to stdout. If FLUSH is true, write the
// contents of the output buffer to the kernel. FLUSH is false by default. If
// CE_EVAL is true, evaluate the current constraint expression; only send
// data if the CE evaluates to true.
//
// NB: See the comment in BaseType re: why we don't use XDR_CODER here
//
// Returns: false if a failure to read, send or flush is detected, true
// otherwise. 

bool
Byte::serialize(const string &dataset, DDS &dds, XDR *sink, bool ce_eval)
{
  // Jose Garcia
  // Since the read method is virtual and implemented outside
  // libdap++ if we can not read the data that is the problem 
  // of the user or of whoever wrote the surrogate library
  // implemeting read therefore it is an internal error.
#if 0
    if (!read_p() && !read(dataset))
	throw InternalErr("can not read data");
#endif
    // The read() mfunc returns true to indicate that it should be called
    // again, false to indicate that all the data were read. Note that errors
    // are signaled with eitehr an Error or InternalErr exception. 10/5/2000
    // jhrg.
    try {
      if (!read_p())
	read(dataset);
    }
    catch (Error &e) {
      return false;
    }

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!xdr_char(sink, (char *)&_buf))
	return false;

    return true;
}

// deserialize the char on stdin and put the result in _BUF.

bool
Byte::deserialize(XDR *source, DDS *, bool)
{
    unsigned int num = xdr_char(source, (char *)&_buf);

    return (num != 0);
}

// Store the value referenced by VAL in the object's internal buffer. REUSE
// has no effect because this class does not dynamically allocate storage for
// the internal buffer.
//
// Returns: size in bytes of the value's representation.

unsigned int
Byte::val2buf(void *val, bool)
{
  // Jose Garcia
  // This method is public therefore and I believe it has being designed
  // to be use by read which must be implemented on the surrogated library,
  // thus if the pointer val is NULL, is an Internal Error. 
  if(!val)
    throw InternalErr("the incoming pointer does not contain any data.");

    _buf = *(dods_byte *)val;

    return width();
}

unsigned int
Byte::buf2val(void **val)
{
  // Jose Garcia
  // The same comment justifying throwing an Error in val2buf applies here.
  if (!val)
    throw InternalErr("NULL pointer");
  
    if (!*val)
	*val = new dods_byte;

    *(dods_byte *)*val = _buf;

    return width();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Byte::print_val(ostream &os, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << (int)_buf << ";" << endl;
    }
    else 
	os << (int)_buf;
}

bool
Byte::ops(BaseType *b, int op, const string &dataset)
{
    
    // Extract the Byte arg's value.
    if (!read_p() && !read(dataset)) {
      cerr << "This value not read!" << endl;
      // Jose Garcia
      // Since the read method is virtual and implemented outside
      // libdap++ if we can not read the data that is the problem 
      // of the user or of whoever wrote the surrogate library
      // implemeting read therefore it is an internal error.
      throw InternalErr("This value not read!");
    }

    // Extract the second arg's value.
    if (!b->read_p() && !b->read(dataset)) {
      cerr << "This value not read!" << endl;
      // Jose Garcia
      // Since the read method is virtual and implemented outside
      // libdap++ if we can not read the data that is the problem 
      // of the user or of whoever wrote the surrogate library
      // implemeting read therefore it is an internal error.
      throw InternalErr("This value not read!");
    }

    switch (b->type()) {
      case dods_byte_c:
	return rops<dods_byte, dods_byte, Cmp<dods_byte, dods_byte> >
	    (_buf, dynamic_cast<Byte *>(b)->_buf, op);
      case dods_int16_c:
	return rops<dods_byte, dods_int16, USCmp<dods_byte, dods_int16> >
	    (_buf, dynamic_cast<Int16 *>(b)->_buf, op);
      case dods_uint16_c:
	return rops<dods_byte, dods_uint16, Cmp<dods_byte, dods_uint16> >
	    (_buf, dynamic_cast<UInt16 *>(b)->_buf, op);
      case dods_int32_c:
	return rops<dods_byte, dods_int32, USCmp<dods_byte, dods_int32> >
	    (_buf, dynamic_cast<Int32 *>(b)->_buf, op);
      case dods_uint32_c:
	return rops<dods_byte, dods_uint32, Cmp<dods_byte, dods_uint32> >
	    (_buf, dynamic_cast<UInt32 *>(b)->_buf, op);
      case dods_float32_c:
	return rops<dods_byte, dods_float32, Cmp<dods_byte, dods_float32> >
	    (_buf, dynamic_cast<Float32 *>(b)->_buf, op);
      case dods_float64_c:
	return rops<dods_byte, dods_float64, Cmp<dods_byte, dods_float64> >
	    (_buf, dynamic_cast<Float64 *>(b)->_buf, op);
      default:
	return false;
    }
}

// $Log: Byte.cc,v $
// Revision 1.42  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.41.4.2  2001/08/18 00:19:18  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.41.4.1  2001/07/28 01:10:41  jimg
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
// Revision 1.41  2000/10/06 01:26:04  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.40  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.39  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.38  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.37  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.36.20.1  2000/06/02 18:14:42  rmorris
// Mod for port to win32.
//
// Revision 1.36.14.1  2000/01/28 22:14:04  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.36  1999/04/29 02:29:27  jimg
// Merge of no-gnu branch
//
// Revision 1.35  1999/03/24 23:37:14  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.34.2.2  1999/02/05 09:32:33  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.34.2.1  1999/02/02 21:56:55  jimg
// String to string version
//
// Revision 1.34  1998/08/06 16:10:39  jimg
// Added cast in call to abs. I'm not sure why...
//
// Revision 1.33  1998/03/11 00:30:35  jimg
// Fixed a bug in buf2val(...) where the assert failed when the value held by
// the variable was 0 (which is a perfectly valid value).
//
// Revision 1.32  1997/12/31 21:46:50  jimg
// Added casts in serialize and deserialize to get rid of warnings about
// signed-ness of buffers passed to XDR functions.
//
// Revision 1.31  1997/10/09 22:19:14  jimg
// Resolved conflicts in merge of 2.14c to trunk.
//
// Revision 1.30  1997/10/04 00:32:53  jimg
// Release 2.14c fixes
//
// Revision 1.29  1997/09/22 23:08:54  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.28.6.1  1997/08/20 22:10:41  jimg
// Changed the cast in Byte::print_val from (unsigned int) to (int) so that if
// dods_byte is signed (i.e., if char is signed or dods_byte gets redefined
// somehow) values will still be within the 0 to 255/-128 to 127 range. What
// happened was that dods_byte was signed on Solaris and the cast to unsigned
// int made numbers like -100 balloon up to 4 billion+.
//
// Revision 1.28  1996/12/02 23:10:05  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.27  1996/12/02 18:19:02  jimg
// Added case for uint32 to ops() member function.
//
// Revision 1.26  1996/08/13 17:15:41  jimg
// Added _unused_ to the static global rcsid to remove warning when building
// with gcc -Wall.
// Moved the byte_ops function to util.cc.
//
// Revision 1.25  1996/06/04 21:33:12  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.24  1996/05/31 23:29:27  jimg
// Updated copyright notice.
//
// Revision 1.23  1996/05/29 22:08:34  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.22  1996/05/16 22:49:42  jimg
// Dan's changes for version 2.0. Added a parameter to read that returns
// an error code so that EOF can be distinguished from an actual error when
// reading sequences. This *may* be replaced by an error member function
// in the future.
//
// Revision 1.21  1996/05/14 15:38:18  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.20  1996/05/06 18:34:07  jimg
// Replaced calls to atof and atoi with calls to strtol and strtod.
//
// Revision 1.19  1996/04/05 00:21:24  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.18  1996/03/05 18:42:55  jimg
// Fixed serialize so that expunge() is always called when the member function
// finishes and FLUSH is true.
// Added ops member function and byte_ops interface function.
//
// Revision 1.17  1996/02/02 00:31:01  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.16  1995/12/09  01:06:35  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.15  1995/12/06  21:35:13  jimg
// Changed read() from three to two parameters.
// Removed store_val() and read_val() (use buf2val() and val2buf()).
//
// Revision 1.14  1995/08/26  00:31:27  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.13  1995/08/22  23:57:48  jimg
// Removed deprecated member functions.
// Changed read_val/Store_val to buf2val/val2buf.
//
// Revision 1.12.2.2  1995/09/27 21:25:56  jimg
// Fixed casts in serialize and deserialize.
//
// Revision 1.12.2.1  1995/09/27  19:07:00  jimg
// Add casts to `cast away' const and unsigned in places where we call various
// xdr functions (which don't know about, or use, const or unsigned.
//
// Revision 1.12  1995/07/09  21:28:53  jimg
// Added copyright notice.
//
// Revision 1.11  1995/06/28  17:10:18  dan
// Modified constructor to explicitly cast return type to xdrproc_t, resolves
// problem associated with <rpc/xdr.h> under OSF 3.X.
//
// Revision 1.10  1995/05/10  15:33:55  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.9  1995/05/10  13:45:10  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.8  1995/03/16  17:26:37  jimg
// Moved include of config_dap.h to top of includes.
// Added TRACE_NEW switched dbnew debugging includes.
//
// Revision 1.7  1995/03/04  14:34:41  jimg
// Major modifications to the transmission and representation of values:
// Added card() virtual function which is true for classes that
// contain cardinal types (byte, int float, string).
// Changed the representation of Str from the C rep to a C++
// class represenation.
// Chnaged read_val and store_val so that they take and return
// types that are stored by the object (e.g., inthe case of Str
// an URL, read_val returns a C++ String object).
// Modified Array representations so that arrays of card()
// objects are just that - no more storing strings, ... as
// C would store them.
// Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// an array of a structure is represented as an array of Structure
// objects).
//
// Revision 1.6  1995/02/10  02:22:41  jimg
// Added DBMALLOC includes and switch to code which uses malloc/free.
// Private and protected symbols now start with `_'.
// Added new accessors for name and type fields of BaseType; the old ones
// will be removed in a future release.
// Added the store_val() mfunc. It stores the given value in the object's
// internal buffer.
// Made both List and Str handle their values via pointers to memory.
// Fixed read_val().
// Made serialize/deserialize handle all malloc/free calls (even in those
// cases where xdr initiates the allocation).
// Fixed print_val().
//
// Revision 1.5  1995/01/19  20:05:16  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// references.
//
// Revision 1.4  1995/01/11  15:54:26  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.3  1994/11/29  20:06:32  jimg
// Added mfuncs for data transmission.
// Made the xdr_coder function pointer xdr_bytes() while (de)serialize() uses
// xdr_char().
// Removed `type' from ctor parameter list.
// Added FILE *in and *out to parameter list (they default to stdin/out).
//
// Revision 1.2  1994/09/23  14:36:06  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:38  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/09  15:38:45  jimg
// Child class of BaseType -- used in the future to hold specific serialization
// information for integers. Should this be a class that uses BaseType?
//

