
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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

// Implementation for Int32.
//
// jhrg 9/7/94

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Int16.cc,v 1.16 2003/01/23 00:22:24 jimg Exp $"};

#include <stdlib.h>

#include "Int16.h"
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

using std::cerr;
using std::endl;

Int16::Int16(const string &n) : BaseType(n, dods_int16_c, (xdrproc_t)XDR_INT16)
{
}

Int16::Int16(const Int16 &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}
    
BaseType *
Int16::ptr_duplicate()
{
    return new Int16(*this);
}

Int16 &
Int16::operator=(const Int16 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int
Int16::width()
{
    return sizeof(dods_int16);
}

bool
Int16::serialize(const string &dataset, DDS &dds, XDR *sink,
		 bool ce_eval)
{
    if (!read_p())
	read(dataset);		// read() throws Error and InternalErr
  
    if (ce_eval && !dds.eval_selection(dataset))
	return true;
  
    if (!XDR_INT16(sink, &_buf))
	throw Error(
"Network I/O Error. Could not send int 16 data.\n\
This may be due to a bug in DODS, on the server or a\n\
problem with the network connection.");
  
    return true;
}

bool
Int16::deserialize(XDR *source, DDS *, bool)
{
    if (!XDR_INT16(source, &_buf))
	throw Error(
"Network I/O Error. Could not read int 16 data. This may be due to a\n\
bug in DODS or a problem with the network connection.");

    return false;
}

unsigned int
Int16::val2buf(void *val, bool)
{
  // Jose Garcia
  // This method is public therefore and I believe it has being designed
  // to be use by read which must be implemented on the surrogated library,
  // thus if the pointer val is NULL, is  an Internal Error. 
  if(!val)
      throw InternalErr(__FILE__, __LINE__, 
			"The incoming pointer does not contain any data.");

    _buf = *(dods_int16 *)val;

    return width();
}

unsigned int
Int16::buf2val(void **val)
{
  // Jose Garcia
  // The same comment justifying throwing an Error in val2buf applies here.
  if (!val)
      throw InternalErr(__FILE__, __LINE__, "NULL pointer.");  

    if (!*val)
	*val = new dods_int16;

    *(dods_int16 *)*val =_buf;

    return width();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Int16::print_val(ostream &os, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

void 
Int16::print_val(FILE *out, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = %d;\n", _buf ) ;
    }
    else 
	fprintf( out, "%d", _buf ) ;
}

bool
Int16::ops(BaseType *b, int op, const string &dataset)
{

    // Extract the Byte arg's value.
    if (!read_p() && !read(dataset)) {
      // Jose Garcia
      // Since the read method is virtual and implemented outside
      // libdap++ if we can not read the data that is the problem 
      // of the user or of whoever wrote the surrogate library
      // implemeting read therefore it is an internal error.
      throw InternalErr(__FILE__, __LINE__, "This value not read!");
    }

    // Extract the second arg's value.
    if (!b->read_p() && !b->read(dataset)) {
      // Jose Garcia
      // Since the read method is virtual and implemented outside
      // libdap++ if we can not read the data that is the problem 
      // of the user or of whoever wrote the surrogate library
      // implemeting read therefore it is an internal error.
      throw InternalErr(__FILE__, __LINE__, "This value not read!");
    }

    switch (b->type()) {
      case dods_byte_c:
	return rops<dods_int16, dods_byte, SUCmp<dods_int16, dods_byte> >
	    (_buf, dynamic_cast<Byte *>(b)->_buf, op);
      case dods_int16_c:
	return rops<dods_int16, dods_int16, Cmp<dods_int16, dods_int16> >
	    (_buf, dynamic_cast<Int16 *>(b)->_buf, op);
      case dods_uint16_c:
	return rops<dods_int16, dods_uint16, SUCmp<dods_int16, dods_uint16> >
	    (_buf, dynamic_cast<UInt16 *>(b)->_buf, op);
      case dods_int32_c:
	return rops<dods_int16, dods_int32, Cmp<dods_int16, dods_int32> >
	    (_buf, dynamic_cast<Int32 *>(b)->_buf, op);
      case dods_uint32_c:
	return rops<dods_int16, dods_uint32, SUCmp<dods_int16, dods_uint32> >
	    (_buf, dynamic_cast<UInt32 *>(b)->_buf, op);
      case dods_float32_c:
	return rops<dods_int16, dods_float32, Cmp<dods_int16, dods_float32> >
	    (_buf, dynamic_cast<Float32 *>(b)->_buf, op);
      case dods_float64_c:
	return rops<dods_int16, dods_float64, Cmp<dods_int16, dods_float64> >
	    (_buf, dynamic_cast<Float64 *>(b)->_buf, op);
      default:
	return false;
    }
}

// $Log: Int16.cc,v $
// Revision 1.16  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.15  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.11.4.7  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.11.4.6  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.11.4.5  2002/05/22 16:57:51  jimg
// I modified the `data type classes' so that they do not need to be
// subclassed for clients. It might be the case that, for a complex client,
// subclassing is still the best way to go, but you're not required to do
// it anymore.
//
// Revision 1.14  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.11.4.4  2001/10/02 17:01:52  jimg
// Made the behavior of serialize and deserialize uniform. Both methods now
// use Error exceptions to signal problems with network I/O and InternalErr
// exceptions to signal other problems. The return codes, always true for
// serialize and always false for deserialize, are now meaningless. However,
// by always returning a code that means OK, old code should continue to work.
//
// Revision 1.13  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.11.4.3  2001/09/07 00:38:35  jimg
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
// Revision 1.12  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.11.4.2  2001/08/18 00:17:09  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.11.4.1  2001/07/28 01:10:42  jimg
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
// Revision 1.11  2000/10/06 01:26:05  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.10  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.9  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.8  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.7  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.6.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
//
// Revision 1.6.14.2  2000/02/17 05:03:13  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.6.14.1  2000/01/28 22:14:05  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.6  1999/04/29 02:29:30  jimg
// Merge of no-gnu branch
//
// Revision 1.5  1999/03/24 23:37:15  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.4.6.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.4.6.1  1999/02/02 21:57:00  jimg
// String to string version
//
// Revision 1.4  1998/03/19 23:31:06  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.3  1997/09/22 23:00:18  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.2  1996/12/02 23:10:18  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.1  1996/08/26 20:17:47  jimg
// Added.
//

