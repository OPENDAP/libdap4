
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Int32.
//
// jhrg 9/7/94

// $Log: UInt32.cc,v $
// Revision 1.9  1999/03/24 23:37:15  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.8  1998/03/11 00:30:11  jimg
// Fixed a bug in buf2val(...) where the assert failed when the value held by
// the variable was 0 (which is a perfectly valid value).
//
// Revision 1.7  1997/09/22 22:40:20  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.6  1996/12/02 23:10:30  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.5  1996/11/13 19:06:01  jimg
// Fixed the ops() function so that comparisons between UInt32 and Int32, etc.
// will work correctly. Untested.
//
// Revision 1.4  1996/10/28 23:09:14  jimg
// Fixed compile-time bug in ops() where the type names dods_* were used where
// the constants dods_*_c should have been used.
//
// Revision 1.3  1996/10/18 16:52:18  jimg
// Fixed an error in the ctor where dods_int32_c was passed instead of
// dods_uint32_c.
// Changed comparisons so that unsigned 32 bit ints are used when appropriate.
//
// Revision 1.2  1996/08/26 20:17:50  jimg
// Added.
//
// Revision 1.1  1996/08/26 19:45:27  jimg
// Added.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: UInt32.cc,v 1.9 1999/03/24 23:37:15 jimg Exp $"};

#include <stdlib.h>
#include <assert.h>

#include "UInt32.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "expr.tab.h"
#include "Operators.h"
#include "dods-limits.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

UInt32::UInt32(const String &n) 
    : BaseType(n, dods_uint32_c, (xdrproc_t)XDR_UINT32)
{
}

unsigned int
UInt32::width()
{
    return sizeof(dods_uint32);
}

bool
UInt32::serialize(const String &dataset, DDS &dds, XDR *sink, 
		  bool ce_eval = true)
{
    int error;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!XDR_UINT32(sink, &_buf))
	return false;

    return true;
}

bool
UInt32::deserialize(XDR *source, DDS *, bool)
{
    unsigned int num = XDR_UINT32(source, &_buf);

    return (num > 0);		/* make the return value a boolean */
}

unsigned int
UInt32::val2buf(void *val, bool)
{
    assert(val);

    _buf = *(dods_uint32 *)val;

    return width();
}

unsigned int
UInt32::buf2val(void **val)
{
    assert(val);

    if (!*val)
	*val = new dods_uint32;

    *(dods_uint32 *)*val =_buf;

    return width();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
UInt32::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

bool
UInt32::ops(BaseType *b, int op, const String &dataset)
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
	return rops<dods_uint32, dods_byte, Cmp<dods_uint32, dods_byte> >
	    (_buf, dynamic_cast<Byte *>(b)->_buf, op);
      case dods_int16_c:
	return rops<dods_uint32, dods_int16, USCmp<dods_uint32, dods_int16> >
	    (_buf, dynamic_cast<Int16 *>(b)->_buf, op);
      case dods_uint16_c:
	return rops<dods_uint32, dods_uint16, Cmp<dods_uint32, dods_uint16> >
	    (_buf, dynamic_cast<UInt16 *>(b)->_buf, op);
      case dods_int32_c:
	return rops<dods_uint32, dods_int32, USCmp<dods_uint32, dods_int32> >
	    (_buf, dynamic_cast<Int32 *>(b)->_buf, op);
      case dods_uint32_c:
	return rops<dods_uint32, dods_uint32, Cmp<dods_uint32, dods_uint32> >
	    (_buf, dynamic_cast<UInt32 *>(b)->_buf, op);
      case dods_float32_c:
	return rops<dods_uint32, dods_float32, Cmp<dods_uint32, dods_float32> >
	    (_buf, dynamic_cast<Float32 *>(b)->_buf, op);
      case dods_float64_c:
	return rops<dods_uint32, dods_float64, Cmp<dods_uint32, dods_float64> >
	    (_buf, dynamic_cast<Float64 *>(b)->_buf, op);
      default:
	return false;
    }
}
