
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for Byte.
//
// jhrg 9/7/94


#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdlib.h>

#include "Byte.h"
#include "Int16.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "expr.tab.h"
#include "Operators.h"
#include "dods-limits.h"
#include "InternalErr.h"


using std::cerr;
using std::endl;

/** The Byte constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    \note Even though Byte is a cardinal type, xdr_char is <i>not</i>
    used to transport Byte arrays over the network. Instead, Byte is
    a special case handled in Array.

    @brief The Byte constructor.
    @param n A string containing the name of the variable to be
    created. 

*/

Byte::Byte(const string &n) : BaseType(n, dods_byte_c)
{
}

Byte::Byte(const Byte &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}
    
BaseType *
Byte::ptr_duplicate()
{
    return new Byte(*this);
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

/** Serialize the contents of member _BUF (the object's internal
    buffer, used to hold data) and write the result to stdout. If
    FLUSH is true, write the contents of the output buffer to the
    kernel. FLUSH is false by default. If CE_EVAL is true, evaluate
    the current constraint expression; only send data if the CE
    evaluates to true. 

    @note See the comment in BaseType about why we don't use XDR_CODER
    here. 

    @return False if a failure to read, send or flush is detected, true
    otherwise. 
*/
bool
Byte::serialize(const string &dataset, DDS &dds, XDR *sink, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
	read(dataset);		// read() throws Error and InternalErr

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    dds.timeout_off();

    if (!xdr_char(sink, (char *)&_buf))
	throw Error(
"Network I/O Error. Could not send byte data.\n\
This may be due to a bug in DODS, on the server or a\n\
problem with the network connection.");

    return true;
}

/** @brief Deserialize the char on stdin and put the result in
    <tt>_BUF</tt>. 
*/
bool
Byte::deserialize(XDR *source, DDS *, bool)
{
    if (!xdr_char(source, (char *)&_buf))
	throw Error(
"Network I/O Error. Could not read byte data. This may be due to a\n\
bug in DODS or a problem with the network connection.");

    return false;
}

/** Store the value referenced by <i>val</i> in the object's internal
    buffer. <i>reuse</i> has no effect because this class does not
    dynamically allocate storage for the internal buffer.

    @return The size (in bytes) of the value's representation.  */
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
#if 0
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
#endif

void 
Byte::print_val(FILE *out, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = %d;\n", (int)_buf ) ;
    }
    else 
	fprintf( out, "%d", (int)_buf ) ;
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
// Revision 1.51  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.49.2.2  2004/07/02 20:41:51  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.50  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.49.2.1  2003/07/25 06:04:28  jimg
// Refactored the code so that DDS:send() is now incorporated into
// DODSFilter::send_data(). The old DDS::send() is still there but is
// depracated.
// Added 'smart timeouts' to all the variable classes. This means that
// the new server timeouts are active only for the data read and CE
// evaluation. This went inthe BaseType::serialize() methods because it
// needed to time both the read() calls and the dds::eval() calls.
//
// Revision 1.49  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.48  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.47.2.1  2003/02/21 00:10:06  jimg
// Repaired copyright.
//
// Revision 1.47  2003/01/23 00:22:23  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.46  2003/01/10 19:46:39  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.41.4.7  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.41.4.6  2002/08/08 06:54:56  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.45  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.41.4.5  2002/05/22 16:57:51  jimg
// I modified the `data type classes' so that they do not need to be
// subclassed for clients. It might be the case that, for a complex client,
// subclassing is still the best way to go, but you're not required to do
// it anymore.
//
// Revision 1.44  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.41.4.4  2001/10/02 17:01:52  jimg
// Made the behavior of serialize and deserialize uniform. Both methods now
// use Error exceptions to signal problems with network I/O and InternalErr
// exceptions to signal other problems. The return codes, always true for
// serialize and always false for deserialize, are now meaningless. However,
// by always returning a code that means OK, old code should continue to work.
//
// Revision 1.43  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.41.4.3  2001/09/07 00:38:34  jimg
// Sequence::deserialize(...) now reads all the sequence values at once.
// Its call semantics are the same as the other classes' versions. Values
// are stored in the Sequence object using a vector<BaseType *> for each
// row (those are themselves held in a vector). Three new accessor methods
// have been added to Sequence (row_value() and two versions of var_value()).
// BaseType::deserialize(...) now always returns true. This matches with the
// expectations of most client code (the seqeunce version returned false
// when it was done reading, but all the calls for sequences must be changed
// anyway). If an XDR error is found, deserialize throws InternalErr.
//
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
// Failed to change `config.h' to `config.h' in these files.
//
// Revision 1.9  1995/05/10  13:45:10  jimg
// Changed the name of the configuration header file from `config.h' to
// `config.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.8  1995/03/16  17:26:37  jimg
// Moved include of config.h to top of includes.
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

