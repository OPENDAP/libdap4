
/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// Implementation for Byte.
//
// jhrg 9/7/94

// $Log: Byte.cc,v $
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

#include <stdlib.h>		// for atoi()
#include <assert.h>

#include "Byte.h"
#include "DDS.h"
#include "parser.h"
#include "expr.tab.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

// NB: Even though Byte is a cardinal type, xdr_char is *not* used to
// transport Byte arrays over the network. Instead, Byte is a special case
// handled in Array.

Byte::Byte(const String &n) : BaseType(n, d_byte_t)
{
}

unsigned int
Byte::width()
{
    return sizeof(byte);
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
Byte::serialize(const String &dataset, DDS &dds, bool ce_eval, bool flush)
{
    if (!read_p() && !read(dataset))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!xdr_char(xdrout(), &_buf))
	return false;

    if (flush)
	return expunge();

    return true;
}

// deserialize the char on stdin and put the result in _BUF.

bool
Byte::deserialize(bool)
{
    unsigned int num = xdr_char(xdrin(), &_buf);

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

    _buf = *(byte *)val;

    return width();
}

unsigned int
Byte::buf2val(void **val)
{
    assert(_buf && val);

    if (!*val)
	*val = new byte;

    *(byte *)*val = _buf;

    return width();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
Byte::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << (unsigned int)_buf << ";" << endl;
    }
    else 
	os << (unsigned int)_buf;
}

static bool
byte_ops(int i1, int i2, int op)
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
      case REGEXP:
	cerr << "Regexp not valid for byte values" << endl;
	return false;
      default:
	cerr << "Unknown operator" << endl;
	return false;
    }
}

bool
Byte::ops(BaseType &b, int op)
{
    int32 a1, a2;
    if (!read_p()) {
	cerr << "This value not yet read!" << endl;
	return false;
    }
    else {
	int32 *a1p = &a1;
	buf2val((void **)&a1p);
    }

    if (!b.read_p()) {
	cerr << "Arg value not yet read!" << endl;
	return false;
    }
    else switch (b.type()) {
      case d_byte_t:
      case d_int32_t: {
	int32 *a2p = &a2;
	b.buf2val((void **)&a2p);
	break;
      }
      case d_float64_t: {
	double d;
	double *dp = &d;
	b.buf2val((void **)&dp);
	a2 = (int32)d;
	break;
      }
      case d_str_t: {
	String s;
	String *sp = &s;
	b.buf2val((void **)&sp);
	a2 = atoi((const char *)s);
	break;
      }
      default:
	return false;
	break;
    }

    return byte_ops(a1, a2, op);
}
