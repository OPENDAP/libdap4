
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Int32.
//
// jhrg 9/7/94

// $Log: UInt16.cc,v $
// Revision 1.1  1996/08/26 20:17:49  jimg
// Added.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: UInt16.cc,v 1.1 1996/08/26 20:17:49 jimg Exp $"};

#include <stdlib.h>
#include <assert.h>

#include "Int32.h"
#include "DDS.h"
#include "util.h"
#include "dods-limits.h"
#if 0
#include "parser.h"
#include "expr.h"
#include "expr.tab.h"
#endif
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

UInt16::UInt16(const String &n) : BaseType(n, dods_uint16_c, (xdrproc_t)XDR_INT16)
{
}

unsigned int
UInt16::width()
{
    return sizeof(dods_uint16);
}

bool
UInt16::serialize(const String &dataset, DDS &dds, XDR *sink,
		 bool ce_eval = true)
{
    int error;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!XDR_INT16(sink, &_buf))
	return false;

    return true;
}

bool
UInt16::deserialize(XDR *source, bool)
{
    unsigned int num = XDR_INT16(source, &_buf);

    return (num > 0);		/* make the return value a boolean */
}

unsigned int
UInt16::val2buf(void *val, bool)
{
    assert(val);

    _buf = *(dods_uint16 *)val;

    return width();
}

unsigned int
UInt16::buf2val(void **val)
{
    assert(_buf && val);

    if (!*val)
	*val = new dods_uint16;

    *(dods_uint16 *)*val =_buf;

    return width();
}

// Print BUF to stdout with its declaration. Intended mostly for debugging.

void 
UInt16::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

bool
UInt16::ops(BaseType &b, int op)
{
    dods_uint16 a1, a2;

    if (!read_p()) {
	cerr << "This value not yet read!" << endl;
	return false;
    }
    else {
	dods_uint16 *a1p = &a1;
	buf2val((void **)&a1p);
    }

    if (!b.read_p()) {
	cerr << "Arg value not yet read!" << endl;
	return false;
    }
    else switch (b.type()) {
      case dods_byte_c:
      case dods_uint16_c:
      case dods_uuint16_c: {	// Might loose data here! jhrg 8/25/96
      case dods_int32_c:
      case dods_uint32_c: 
	dods_uint16 *a2p = &a2;
	b.buf2val((void **)&a2p);
	break;
      }
      case dods_float64_c: {
	double d;
	double *dp = &d;
	b.buf2val((void **)&dp);
	a2 = (dods_uint16)d;
	break;
      }
      case dods_str_c: {
	String s;
	String *sp = &s;
	b.buf2val((void **)&sp);

	char *ptr;
	const char *cp = (const char *)s;
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

    return int_ops(a1, a2, op);
}
