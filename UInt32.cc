
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Int32.
//
// jhrg 9/7/94

// $Log: UInt32.cc,v $
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

static char rcsid[] __unused__ = {"$Id: UInt32.cc,v 1.7 1997/09/22 22:40:20 jimg Exp $"};

#include <stdlib.h>
#include <assert.h>

#include "UInt32.h"
#include "DDS.h"
#include "util.h"
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
    assert(_buf && val);

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

// Evaluate OP with both operands cast to unsigned.

bool
UInt32::ops(BaseType &b, int op, const String &dataset)
{
    dods_uint32 a1, ua2;
    int error; 

    if (!read_p() && !read(dataset, error)) {
	assert("This value not read!" && false);
	cerr << "This value not read!" << endl;
	return false;
    }
    else {
	dods_uint32 *a1p = &a1;
	buf2val((void **)&a1p);
    }

    if (!b.read_p() && !read(dataset, error)) {
	assert("Arg value not read!" && false);
	cerr << "Arg value not read!" << endl;
	return false;
    }
    else switch (b.type()) {
      case dods_byte_c:
      case dods_int32_c: 
      case dods_uint32_c: {
	dods_uint32 *a2p = &ua2;
	b.buf2val((void **)&a2p);
	break;
      }
      case dods_float64_c: {
	double d;
	double *dp = &d;
	b.buf2val((void **)&dp);
	ua2 = (dods_uint32)d;
	break;
      }
      case dods_str_c: {
	String s;
	String *sp = &s;
	b.buf2val((void **)&sp);

	char *ptr;
	const char *cp = (const char *)s;
	ua2 = (unsigned long)strtol(cp, &ptr, 0);

	if (ua2 == 0 && cp == ptr) {
	    cerr << "`" << s << "' is not an integer value" << endl;
	    return false;
	}
	break;
      }
      default:
	return false;
	break;
    }

    return int_ops(a1, ua2, op);
}
