
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Float64.
//
// jhrg 9/7/94

// $Log: Float64.cc,v $
// Revision 1.35  1999/03/24 23:37:14  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.34  1999/03/19 17:40:52  jimg
// Added a call to ios::precision in read(). This sets the precision to 15. The
// default value, 6, was rounding some values.
//
// Revision 1.33  1998/03/19 23:31:22  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.32  1998/03/11 00:31:00  jimg
// Fixed a bug in buf2val(...) where the assert failed when the value held by
// the variable was 0 (which is a perfectly valid value).
//
// Revision 1.31  1997/09/22 23:03:12  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.30  1996/12/02 23:10:15  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.29  1996/12/02 18:21:14  jimg
// Added case for unit32 to ops() member functon.
//
// Revision 1.28  1996/08/13 18:23:42  jimg
// Moved float64_ops to util.cc.
//
// Revision 1.27  1996/06/04 21:33:25  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.26  1996/05/31 23:29:40  jimg
// Updated copyright notice.
//
// Revision 1.25  1996/05/29 22:08:38  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.24  1996/05/16 22:49:44  jimg
// Dan's changes for version 2.0. Added a parameter to read that returns
// an error code so that EOF can be distinguished from an actual error when
// reading sequences. This *may* be replaced by an error member function
// in the future.
//
// Revision 1.23  1996/05/14 15:38:22  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.22  1996/05/06 18:34:35  jimg
// Replaced calls to atof and atoi with calls to strtol and strtod.
//
// Revision 1.21  1996/04/05 00:21:29  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.20  1996/04/04 18:37:02  jimg
// Merged changes from version 1.1.1.
// Removed unneeded include files.
//
// Revision 1.19  1996/03/05 18:24:53  jimg
// Added ce_eval to serailize member function.
// Fixed serialize so that expunge() is always called when the member function
// finishes and FLUSH is true.
// Added ops member function and double_ops function.
//
// Revision 1.18  1996/02/02 00:31:02  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.17  1995/12/09  01:06:40  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.16  1995/12/06  21:35:20  jimg
// Changed read() from three to two parameters.
// Removed store_val() and read_val() (use buf2val() and val2buf()).
//
// Revision 1.15  1995/08/26  00:31:29  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.14  1995/08/22  23:57:50  jimg
// Removed deprecated member functions.
// Changed read_val/Store_val to buf2val/val2buf.
//
// Revision 1.13.2.1  1995/09/29 19:28:01  jimg
// Fixed problems with xdr.h on an SGI.
// Fixed conflict of d_int32_t (which was in an enum type defined by BaseType) on
// the SGI.
//
// Revision 1.13  1995/07/09  21:28:56  jimg
// Added copyright notice.
//
// Revision 1.12  1995/05/10  15:33:57  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.11  1995/05/10  13:45:14  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.10  1995/03/16  17:26:38  jimg
// Moved include of config_dap.h to top of includes.
// Added TRACE_NEW switched dbnew debugging includes.
//
// Revision 1.9  1995/03/04  14:34:42  jimg
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
// Revision 1.8  1995/02/10  02:22:47  jimg
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
// Revision 1.7  1995/01/19  20:05:18  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.6  1995/01/11  15:54:31  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.5  1994/12/09  21:35:58  jimg
// Used the XDR_INT32 and XDR_FLOAT64 symbols defined in config.h.
//
// Revision 1.4  1994/11/29  20:10:39  jimg
// Added functions for data transmission.
// Added boolean parameter to serialize which, when true, causes the output
// buffer to be flushed. The default value is false.
// Added FILE *in and *out parameters to the ctor. The default values are
// stdin/out.
// Removed the `type' parameter from the ctor.
//
// Revision 1.3  1994/11/22  14:05:46  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.2  1994/09/23  14:36:10  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:40  jimg
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

static char rcsid[] __unused__ = {"$Id: Float64.cc,v 1.35 1999/03/24 23:37:14 jimg Exp $"};

#include <stdlib.h>
#include <assert.h>

#include <iomanip.h>

#include "Float64.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "expr.tab.h"
#include "Operators.h"
#include "dods-limits.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

Float64::Float64(const String &n) 
: BaseType(n, dods_float64_c, (xdrproc_t)XDR_FLOAT64)
{
}

unsigned int
Float64::width()
{
    return sizeof(double);
}

bool
Float64::serialize(const String &dataset, DDS &dds, XDR *sink, 
		   bool ce_eval = true)
{
    int error;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!xdr_double(sink, &_buf))
	return false;

    return true;
}

bool
Float64::deserialize(XDR *source, DDS *, bool)
{
    unsigned int num = xdr_double(source, &_buf);

    return num;
}

unsigned int
Float64::val2buf(void *val, bool)
{
    assert(val);

    _buf = *(double *)val;

    return width();
}

unsigned int
Float64::buf2val(void **val)
{
    assert(val);

    if (!*val)
	*val = new double;

    *(double *)*val =_buf;

    return width();
}

void 
Float64::print_val(ostream &os, String space, bool print_decl_p)
{
    os.precision(DODS_DBL_DIG);

    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

bool
Float64::ops(BaseType *b, int op, const String &dataset)
{
    int error = 0;

    // Extract the Byte arg's value.
    if (!read_p() && (!read(dataset, error) || error)) {
	assert("This value not read!" && false);
	cerr << "This value not read!" << endl;
	return false;
    }

    // Extract the second arg's value.
    if (!b->read_p() && (!b->read(dataset, error) || error)) {
	assert("This value not read!" && false);
	cerr << "This value not read!" << endl;
	return false;
    }

    switch (b->type()) {
      case dods_byte_c:
	return rops<dods_float64, dods_byte, Cmp<dods_float64, dods_byte> >
	    (_buf, dynamic_cast<Byte *>(b)->_buf, op);
      case dods_int16_c:
	return rops<dods_float64, dods_int16, Cmp<dods_float64, dods_int16> >
	    (_buf, dynamic_cast<Int16 *>(b)->_buf, op);
      case dods_uint16_c:
	return rops<dods_float64, dods_uint16, Cmp<dods_float64, dods_uint16> >
	    (_buf, dynamic_cast<UInt16 *>(b)->_buf, op);
      case dods_int32_c:
	return rops<dods_float64, dods_int32, Cmp<dods_float64, dods_int32> >
	    (_buf, dynamic_cast<Int32 *>(b)->_buf, op);
      case dods_uint32_c:
	return rops<dods_float64, dods_uint32, Cmp<dods_float64, dods_uint32> >
	    (_buf, dynamic_cast<UInt32 *>(b)->_buf, op);
      case dods_float32_c:
	return rops<dods_float64, dods_float32, Cmp<dods_float64, dods_float32> >
	    (_buf, dynamic_cast<Float32 *>(b)->_buf, op);
      case dods_float64_c:
	return rops<dods_float64, dods_float64, Cmp<dods_float64, dods_float64> >
	    (_buf, dynamic_cast<Float64 *>(b)->_buf, op);
      default:
	return false;
    }
}
