
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Str.
//
// jhrg 9/7/94

// $Log: Str.cc,v $
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
// Added __unused__ to definition of char rcsid[].
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
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.12  1995/05/10  13:45:30  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.11  1995/03/16  17:26:41  jimg
// Moved include of config_dap.h to top of includes.
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: Str.cc,v 1.35 1998/10/21 16:41:29 jimg Exp $"};

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <strstream.h>

#include "Str.h"
#include "DDS.h"
#include "util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

String escattr(String s);

Str::Str(const String &n) : BaseType(n, dods_str_c), _buf("")
{
}

unsigned int
Str::length()
{
    return _buf.length();
}

unsigned int
Str::width()
{
    return sizeof(String);
}

bool
Str::serialize(const String &dataset, DDS &dds, XDR *sink, bool ce_eval = true)
{
    int error;

    DBG(cerr << "Entering (" << this->name() << " [" << this << "])" << endl);

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!xdr_str(sink, _buf))
	return false;
    DBG(cerr << "Exiting: buf = " << _buf << endl);

    return true;
}

// deserialize the String on stdin and put the result in BUF.

bool
Str::deserialize(XDR *source, DDS *, bool)
{
    return (bool)xdr_str(source, _buf);
}

// Copy information in the object's internal buffers into the memory pointed
// to by VAL. If *VAL is null, then allocate memory for the value (a string
// in this case).
//
// NB: return the size of the thing val points to (sizeof val), not the
// length of the string. Thus if there is an array of of strings, then the
// return value of this mfunc can be used to advance to the next string in
// that array. This weirdness is needed because C programs which will need to
// interface to libraries built using this toolkit will not know about g++
// Strings and will need to use the C representation for strings, but here in
// the toolkit I use the String class to cut down on memory management
// problems.

unsigned int
Str::buf2val(void **val)
{
    assert(val);

    if (*val)
	delete *val;

    *val = new String(_buf);

    return sizeof(String);
}

// Copy data in VAL to _BUF.
//
// Returns the number of bytes needed for _BUF.

unsigned int
Str::val2buf(void *val, bool)
{
    assert(val);

    _buf = *(String *)val;

    return sizeof(String);
}

void 
Str::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = \"" << escattr(_buf) << "\";" << endl;
    }
    else 
      os << '"' << escattr(_buf) << '"';
}

bool
Str::ops(BaseType &b, int op, const String &dataset)
{
    String a2;
    int error; 

    if (!read_p() && !read(dataset, error)) {
	assert("This value not read!" && false);
	cerr << "This value not read!" << endl;
	return false;
    }

    if (!b.read_p() && !read(dataset, error)) {
	assert("Arg value not read!" && false);
	cerr << "Arg value not read!" << endl;
	return false;
    }
    else 
	switch (b.type()) {
	  case dods_byte_c:
	  case dods_int32_c: {
	      dods_int32 i;
	      dods_int32 *ip = &i;
	      b.buf2val((void **)&ip);
	      strstream int_str;
	      int_str << i;
	      a2 = int_str.str();
	      int_str.freeze(0);
	      break;
	  }
	  case dods_uint32_c: {
	      dods_uint32 ui;
	      dods_uint32 *uip = &ui;
	      b.buf2val((void **)&uip);
	      strstream uint_str;
	      uint_str << ui;
	      a2 = uint_str.str();
	      uint_str.freeze(0);
	      break;
	  }
	  case dods_float64_c: {
	      double d;
	      double *dp = &d;
	      b.buf2val((void **)&dp);
	      strstream flt_str;
	      flt_str << d;
	      a2 = flt_str.str();
	      flt_str.freeze(0);
	      break;
	  }
	  case dods_str_c: {
	      String *sp = 0;
	      b.buf2val((void **)&sp);
	      a2 = *sp;
	      break;
	  }
	  default:
	    return false;
	    break;
	}

    return string_ops(_buf, a2, op);
}
