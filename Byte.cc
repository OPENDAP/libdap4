
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Byte.
//
// jhrg 9/7/94

// $Log: Byte.cc,v $
// Revision 1.30  1997/10/04 00:32:53  jimg
// Release 2.14c fixes
//
<<<<<<< Byte.cc
// Revision 1.29  1997/09/22 23:08:54  jimg
// Added DDS * to deserialize parameters.
//
=======
// Revision 1.28.6.1  1997/08/20 22:10:41  jimg
// Changed the cast in Byte::print_val from (unsigned int) to (int) so that if
// dods_byte is signed (i.e., if char is signed or dods_byte gets redefined
// somehow) values will still be within the 0 to 255/-128 to 127 range. What
// happened was that dods_byte was signed on Solaris and the cast to unsigned
// int made numbers like -100 balloon up to 4 billion+.
//
>>>>>>> 1.28.6.1
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

<<<<<<< Byte.cc
static char rcsid[] __unused__ = {"$Id: Byte.cc,v 1.30 1997/10/04 00:32:53 jimg Exp $"};
=======
static char rcsid[] __unused__ = {"$Id: Byte.cc,v 1.30 1997/10/04 00:32:53 jimg Exp $"};
>>>>>>> 1.28.6.1

#include <stdlib.h>
#include <assert.h>

#include "Byte.h"
#include "DDS.h"
#include "util.h"
#include "dods-limits.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

// NB: Even though Byte is a cardinal type, xdr_char is *not* used to
// transport Byte arrays over the network. Instead, Byte is a special case
// handled in Array.

Byte::Byte(const String &n) : BaseType(n, dods_byte_c)
{
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
Byte::serialize(const String &dataset, DDS &dds, XDR *sink, 
		bool ce_eval = true)
{
    int error;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!xdr_char(sink, &_buf))
	return false;

    return true;
}

// deserialize the char on stdin and put the result in _BUF.

bool
Byte::deserialize(XDR *source, DDS *, bool)
{
    unsigned int num = xdr_char(source, &_buf);

    return num;
}

// Store the value referenced by VAL in the object's internal buffer. REUSE
// has no effect because this class does not dynamically allocate storage for
// the internal buffer.
//
// Returns: size in bytes of the value's representation.

unsigned int
Byte::val2buf(void *val, bool)
{
    assert(val);

    _buf = *(dods_byte *)val;

    return width();
}

unsigned int
Byte::buf2val(void **val)
{
    assert(_buf && val);

    if (!*val)
	*val = new dods_byte;

    *(dods_byte *)*val = _buf;

    return width();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Byte::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << (int)_buf << ";" << endl;
    }
    else 
	os << (int)_buf;
}

bool
Byte::ops(BaseType &b, int op, const String &dataset)
{
    dods_int32 a1, a2;
    int error;

    if (!read_p() && !read(dataset, error)) {
	assert("This value not read!" && false);
	cerr << "This value not read!" << endl;
	return false;
    }
    else {
	dods_int32 *a1p = &a1;
	buf2val((void **)&a1p);
    }

    if (!b.read_p() && !b.read(dataset, error)) {
	assert("Arg value not read!" && false);
	cerr << "Arg value not read!" << endl;
	return false;
    }
    else 
	switch (b.type()) {
	  case dods_byte_c:
	  case dods_int32_c: {
	      dods_int32 *a2p = &a2;
	      b.buf2val((void **)&a2p);
	      break;
	  }
	  case dods_uint32_c: {
	      dods_int32 *a2p = &a2;
	      b.buf2val((void **)&a2p);
	      a2 = abs(a2);
	      break;
	  }
	  case dods_float64_c: {
	      double d;
	      double *dp = &d;
	      b.buf2val((void **)&dp);
	      a2 = (dods_int32)d;
	      break;
	  }
	  case dods_str_c: {
	      String s;
	      String *sp = &s;
	      b.buf2val((void **)&sp);

	      char *ptr;
	      const char *cp = (char *)(const char *)s;
	      long v = strtol(cp, &ptr, 0);

	      if (v == 0 && cp == ptr) {
		  cerr << "`" << s << "' is not an integer value" << endl;
		  return false;
	      }
	      if (v > DODS_INT_MAX || v < DODS_INT_MIN) {
		  cerr << "`" << v << "' is not a integer value" << endl;
		  return false;
	      }

	      a2 = v;
	      break;
	  }
	  default:
	    return false;
	    break;
	}

    return byte_ops(a1, a2, op);
}
