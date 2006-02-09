
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

// Implementation for Str.
//
// jhrg 9/7/94


#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdlib.h>

#include "Str.h"
#include "DDS.h"
#include "parser.h"
#include "expr.tab.h"
#include "Operators.h"
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"
#include "debug.h"


using std::cerr;
using std::endl;

string escattr(string s);

/** The Str constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    @param n A string containing the name of the variable to be
    created. 

*/
Str::Str(const string &n) : BaseType(n, dods_str_c), _buf("")
{
}

Str::Str(const Str &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}
    
BaseType *
Str::ptr_duplicate()
{
    return new Str(*this);
}

Str &
Str::operator=(const Str &rhs)
{
    if (this == &rhs)
	return *this;

    // Call BaseType::operator=.
    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int
Str::length()
{
    return _buf.length();
}

unsigned int
Str::width()
{
    return sizeof(string);
}

bool
Str::serialize(const string &dataset, DDS &dds, XDR *sink, bool ce_eval)
{

    DBG(cerr << "Entering (" << this->name() << " [" << this << "])" << endl);

    dds.timeout_on();

    if (!read_p())
      read(dataset);

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    dds.timeout_off();

    if (!xdr_str(sink, _buf))
	throw Error(
"Network I/O Error. Could not send string data.\n\
This may be due to a bug in libdap, on the server or a\n\
problem with the network connection.");

    DBG(cerr << "Exiting: buf = " << _buf << endl);

    return true;
}

// deserialize the string on stdin and put the result in BUF.

bool
Str::deserialize(XDR *source, DDS *, bool)
{
    if (xdr_str(source, _buf) != 1)
	throw Error(
"Network I/O Error. Could not read string data. This may be due to a\n\
bug in libdap or a problem with the network connection.");

    return false;
}

/** Read the object's value and put a copy in the C++ string object
    referenced by \e **val. If \e *val is null, this method will allocate
    a string object using new and store the result there. If \e *val 
    is not null, it will assume that \e *val references a string object
    and put the value there.
    
    @param val A pointer to null or to a string object.
    @return The sizeof(string*)
    @exception InternalErr Thrown if \e val is null. */
unsigned int
Str::buf2val(void **val)
{
    // Jose Garcia
    // The same comment justifying throwing an Error in val2buf applies here.
    if(!val)
	throw InternalErr(__FILE__, __LINE__, 
			  "No place to store a reference to the data.");
#if 0
    // This statement break the case where the caller allocates a string
    // object and passes the address of a pointer to that object for val.
    if (*val)
	delete static_cast<string *>(*val);
#endif
    // If *val is null, then the caller has not allocated storage for the 
    // value; we must. If there is storage there, assume it is a string and
    // assign _buf's value to that storage.
    if (!*val)
        *val = new string(_buf);
    else
        *static_cast<string*>(*val) = _buf;
        
    return sizeof(string*);
}

/** Store the value referenced by \e val in this object. Even though the
    type of \e val is \c void*, this method assumes the type is \c string*.
    Note that the value is copied so the caller if free to throw away/reuse
    the actual parameter once this call has returned.
    
    @param val A pointer to a C++ string object.
    @param reuse Not used by this version of the method.
    @exception IntenalErr if \e val is null.
    @return The width of the pointer. */
unsigned int
Str::val2buf(void *val, bool)
{
    // Jose Garcia
    // This method is public therefore and I believe it has being designed
    // to be use by read which must be implemented on the surrogated library,
    // thus if the pointer val is NULL, is an Internal Error. 
    if (!val)
	throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    _buf = *static_cast<string*>(val);

    return sizeof(string*);
}
#if 0
void 
Str::print_val(ostream &os, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = \"" << escattr(_buf) << "\";" << endl;
    }
    else 
        os << '"' << escattr(_buf) << '"';
}
#endif
void 
Str::print_val(FILE *out, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = \"%s\";\n", escattr(_buf).c_str() ) ;
    }
    else 
      fprintf( out, "\"%s\"", escattr(_buf).c_str() ) ;
}

bool
Str::ops(BaseType *b, int op, const string &dataset)
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
      throw InternalErr(__FILE__, __LINE__, "Argument value was not read!");
    }

    switch (b->type()) {
      case dods_str_c:
	return rops<string, string, StrCmp<string, string> >
	    (_buf, dynamic_cast<Str *>(b)->_buf, op);
      case dods_url_c:
	return rops<string, string, StrCmp<string, string> >
	    (_buf, dynamic_cast<Url *>(b)->_buf, op);
      default:
	return false;
    }
}

// $Log: Str.cc,v $
// Revision 1.58  2005/04/07 22:32:47  jimg
// Updated doxygen comments: fixed errors; updated comments about set_read_p.
// Removed the VirtualCtor classes. Added a README about the factory
// classes.
//
// Revision 1.57  2005/03/02 21:23:43  jimg
// RemovedConnections.cc/h; now in nc3-dods.
//
// Revision 1.56  2005/02/14 22:19:57  jimg
// Fixed handling of strings. No longer frees storage quite a capriciously.
//
// Revision 1.55  2005/02/10 23:42:55  jimg
// Replaced old style cast with static_cast in val2buf.
//
// Revision 1.54  2004/09/16 15:21:47  jimg
// Fixed the return values of buf2val() and val2buf(). They now return the size
// of a std::string pointer, std::string.
//
// Revision 1.53  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.51.2.2  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.52  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.51.2.1  2003/07/25 06:04:28  jimg
// Refactored the code so that DDS:send() is now incorporated into
// DODSFilter::send_data(). The old DDS::send() is still there but is
// depracated.
// Added 'smart timeouts' to all the variable classes. This means that
// the new server timeouts are active only for the data read and CE
// evaluation. This went inthe BaseType::serialize() methods because it
// needed to time both the read() calls and the dds::eval() calls.
//
// Revision 1.51  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.50  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.49.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.49  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.48  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.43.4.8  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.43.4.7  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.47  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.43.4.6  2002/05/22 16:57:51  jimg
// I modified the `data type classes' so that they do not need to be
// subclassed for clients. It might be the case that, for a complex client,
// subclassing is still the best way to go, but you're not required to do
// it anymore.
//
// Revision 1.46  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.43.4.5  2001/10/02 17:01:52  jimg
// Made the behavior of serialize and deserialize uniform. Both methods now
// use Error exceptions to signal problems with network I/O and InternalErr
// exceptions to signal other problems. The return codes, always true for
// serialize and always false for deserialize, are now meaningless. However,
// by always returning a code that means OK, old code should continue to work.
//
// Revision 1.45  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.43.4.4  2001/09/25 20:29:17  jimg
// Added include debug.h
//
// Revision 1.43.4.3  2001/09/07 00:38:35  jimg
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
// Revision 1.44  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.43.4.2  2001/08/18 00:16:00  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.43.4.1  2001/07/28 01:10:42  jimg
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
// Revision 1.43  2000/10/06 01:26:05  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.42  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.41  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.40  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.39  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.38.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
//
// Revision 1.38.14.2  2000/02/17 05:03:14  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.38.14.1  2000/01/28 22:14:06  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.38  1999/05/04 19:47:22  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.37  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.36  1999/03/24 23:37:15  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.35  1998/10/21 16:41:29  jimg
// Added some instrumentation (using DBG). This might be useful later on...
//
// Revision 1.34  1998/10/19 19:32:13  jimg
// Fixed a bug in ops(): buf2val was used incorrectly and string_ops never
// got the right strings. since the values were always "" for both arguments,
// any regex match returned true! Also, removed one call to buf2val in ops()
// since its OK to pass the member _buf to string_ops().
//
// Revision 1.33  1998/09/17 17:16:25  jimg
// Fixed errant comments.
//
// Revision 1.32  1998/09/10 19:17:58  jehamby
// Change Str::print_val() to quote Strings when printing them (so geturl can
// generate less ambiguous output).
//
// Revision 1.31.6.2  1999/02/05 09:32:35  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.31.6.1  1999/02/02 21:57:01  jimg
// String to string version
//
// Revision 1.31  1998/03/19 23:30:49  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.30  1998/02/05 20:13:56  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.29  1997/09/22 22:45:43  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.28  1996/12/02 23:10:25  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.27  1996/12/02 18:21:16  jimg
// Added case for unit32 to ops() member functon.
//
// Revision 1.26  1996/08/13 18:36:53  jimg
// Added not_used to definition of char rcsid[].
// Moved str_ops() to util.cc
//
// Revision 1.25  1996/06/04 21:33:42  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.24  1996/05/31 23:30:01  jimg
// Updated copyright notice.
//
// Revision 1.23  1996/05/29 22:08:49  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.22  1996/05/16 22:49:50  jimg
// Dan's changes for version 2.0. Added a parameter to read that returns
// an error code so that EOF can be distinguished from an actual error when
// reading sequences. This *may* be replaced by an error member function
// in the future.
//
// Revision 1.21  1996/05/14 15:38:36  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.20  1996/04/05 00:21:39  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.19  1996/03/05 17:43:08  jimg
// Added ce_eval to serailize member function.
// Added relational operators (the ops member function and string_ops function).
//
// Revision 1.18  1995/12/09  01:06:57  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.17  1995/12/06  21:35:23  jimg
// Changed read() from three to two parameters.
// Removed store_val() and read_val() (use buf2val() and val2buf()).
//
// Revision 1.16  1995/08/26  00:31:46  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.15  1995/08/23  00:18:30  jimg
// Now uses newer function names.
// Uses the new xdr_str() function in util.cc
//
// Revision 1.14  1995/07/09  21:29:04  jimg
// Added copyright notice.
//
// Revision 1.13  1995/05/10  15:34:05  jimg
// Failed to change `config.h' to `config.h' in these files.
//
// Revision 1.12  1995/05/10  13:45:30  jimg
// Changed the name of the configuration header file from `config.h' to
// `config.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.11  1995/03/16  17:26:41  jimg
// Moved include of config.h to top of includes.
// Added TRACE_NEW switched dbnew debugging includes.
//
// Revision 1.10  1995/03/04  14:34:50  jimg
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
// Revision 1.9  1995/02/10  02:22:49  jimg
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
// Revision 1.8  1995/01/19  20:05:19  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.7  1995/01/11  15:54:33  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.6  1994/12/22  04:32:23  reza
// Changed the default type to String (from Str) to match DDS parser.
//
// Revision 1.5  1994/12/14  19:18:00  jimg
// Added mfunc len(). Replaced size() with a mfunc that returns the size of
// a pointer to a string (this simplifies Structure, ...).
//
// Revision 1.4  1994/12/14  18:04:33  jimg
// Changed definition of size() so that it returns the number of bytes in
// the string.
//
// Revision 1.3  1994/11/29  20:16:32  jimg
// Added mfunc for data transmission.
// Uses special xdr function for serialization and xdr_coder.
// Removed `type' parameter from ctor.
// Added FILE *in and *out to ctor parameter list.
//
// Revision 1.2  1994/09/23  14:36:12  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:48  jimg
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

