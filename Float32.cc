
// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Float64.
//
// jhrg 9/7/94

// $Log: Float32.cc,v $
// Revision 1.3  1996/12/02 23:10:12  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.2  1996/12/02 18:21:13  jimg
// Added case for unit32 to ops() member functon.
//
// Revision 1.1  1996/08/26 20:17:53  jimg
// Added.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: Float32.cc,v 1.3 1996/12/02 23:10:12 jimg Exp $"};

#include <stdlib.h>
#include <assert.h>

#include "Float32.h"
#include "DDS.h"
#include "util.h"
#include "dods-limits.h"
#if 0
#include "parser.h"
#include "expr.h"
#include "expr.tab.h"
#endif

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

Float32::Float32(const String &n) 
: BaseType(n, dods_float64_c, (xdrproc_t)XDR_FLOAT64)
{
}

unsigned int
Float32::width()
{
    return sizeof(double);
}

bool
Float32::serialize(const String &dataset, DDS &dds, XDR *sink, 
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
Float32::deserialize(XDR *source, bool)
{
    unsigned int num = xdr_double(source, &_buf);

    return num;
}

unsigned int
Float32::val2buf(void *val, bool)
{
    assert(val);

    _buf = *(double *)val;

    return width();
}

unsigned int
Float32::buf2val(void **val)
{
    assert(_buf && val);

    if (!*val)
	*val = new double;

    *(double *)*val =_buf;

    return width();
}

void 
Float32::print_val(ostream &os, String space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

bool
Float32::ops(BaseType &b, int op, const String &dataset)
{
    double a1, a2;
    int error; 

    if (!read_p() && !read(dataset, &error)) {
	assert("This value not read!" && false);
	cerr << "This value not read!" << endl;
	return false;
    }
    else {
	double *a1p = &a1;
	buf2val((void **)&a1p);
    }

    if (!b.read_p() && !read(dataset, &error)) {
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
	      a2 = i;
	      break;
	  }
	  case dods_uint32_c: {
	      dods_uint32 ui;
	      dods_uint32 *uip = &ui;
	      b.buf2val((void **)&uip);
	      a2 = ui;
	      break;
	  }
	  case dods_float32_c:
	  case dods_float64_c: {
	      double *a2p = &a2;
	      b.buf2val((void **)&a2p);
	      break;
	  }
	  case dods_str_c: {
	      String s;
	      String *sp = &s;
	      b.buf2val((void **)&sp);

	      char *ptr;
	      const char *cp = (const char *)s;
	      a2 = strtod(cp, &ptr);

	      if (a2 == 0.0 && cp == ptr) {
		  cerr << "`" << s << "' is not an float value" << endl;
		  return false;
	      }

	      break;
	  }
	  default:
	    return false;
	    break;
	}

    return double_ops(a1, a2, op);
}
