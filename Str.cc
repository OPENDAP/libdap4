
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Str.
//
// jhrg 9/7/94

// $Log: Str.cc,v $
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

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <strstream.h>

#include "Str.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "expr.h"
#include "expr.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

Str::Str(const String &n) : BaseType(n, dods_str_c), _buf("")
{
}

unsigned int
Str::length()
{
    return _buf.length();
}

// return the number of bytes that the value of a Str object occupies when
// that value is accessed using read_val().

unsigned int
Str::width()
{
    return sizeof(String);
}

// serialize and deserialize manage memory using malloc and free since, in
// some cases, they must let the xdr library (bundled with the CPU) do the
// allocation and that library will always use malloc/free.

bool
Str::serialize(const String &dataset, DDS &dds, bool ce_eval, bool flush)
{
    int error;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!xdr_str(xdrout(), _buf))
	return false;

    if (flush)
	return expunge();

    return true;
}

// deserialize the String on stdin and put the result in BUF.

bool
Str::deserialize(bool)
{
    return (bool)xdr_str(xdrin(), _buf);
}

// Copy information in the object's internal buffers into the memory pointed
// to by VAL. If *VAL is null, then allocate memory for the value (a string
// in this case).
//
// NB: return the size of the thing val points to (sizeof val), not the
// length of the string. Thus if there is an array of of strings (i.e., (char
// *)s), then the return value of this mfunc can be used to advance to the
// next char * in that array. This weirdness is needed because C programs
// which will need to interface to libraries built using this toolkit will
// not know about g++ Strings and will need to use the C representation for
// strings, but here in the toolkit I use the String class to cut down on
// memory management problems.

unsigned int
Str::buf2val(void **val)
{
    assert(val);

    if (!*val) 
	(String *)*val = new String(_buf);
    else
	*(String *)*val = _buf;

    return sizeof(String);
}

// Copy data in VAL to _BUF.
//
// Returns the number of bytes needed for _BUF (which is a pointer in this
// case). 

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
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

static bool
string_ops(String &i1, String &i2, int op)
{
    switch (op) {
      case EQUAL:
	return i1 == i2;
      case NOT_EQUAL:
	return i1 != i2;
      case GREATER:
	return i1 > i2;
      case GREATER_EQL:
	return i1 >= i2;
      case LESS:
	return i1 < i2;
      case LESS_EQL:
	return i1 <= i2;
      case REGEXP: {
	  Regex r = (const char *)i2;
	  return i1.matches(r);
      }
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
Str::ops(BaseType &b, int op)
{
    String a1, a2;

    if (!read_p()) {
	cerr << "This value not yet read!" << endl;
	return false;
    }
    else {
	String *a1p = &a1;
	buf2val((void **)&a1p);
    }

    if (!b.read_p()) {
	cerr << "Arg value not yet read!" << endl;
	return false;
    }
    else switch (b.type()) {
      case dods_byte_c:
      case dods_int32_c: {
	  dods_int32 i;
	  dods_int32 *ip = &i;
	  b.buf2val((void **)&ip);
	  strstream int_str;
	  int_str << i;
#ifdef NEVER
	  char *int_str;
	  sprintf(int_str, "%d", i);
#endif
	  a2 = int_str.str();
	  break;
      }
      case dods_float64_c: {
	  double d;
	  double *dp = &d;
	  b.buf2val((void **)&dp);
	  strstream flt_str;
	  flt_str << d;
#ifdef NEVER
	  char *flt_str;
	  sprintf(flt_str, "%lf", d);
#endif
	  a2 = flt_str.str();
	  break;
      }
      case dods_str_c: {
	  String *sp = &a2;
	  b.buf2val((void **)&sp);
	  break;
      }
      default:
	return false;
	break;
    }

    return string_ops(a1, a2, op);
}
