
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for Float32.
//
// 3/22/9 jhrg9

// $Log: Float32.cc,v $
// Revision 1.11  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.10  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.9.20.1  2000/06/02 18:21:27  rmorris
// Mod's for port to Win32.
//
// Revision 1.9  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.8  1999/04/01 22:50:02  jimg
// Switched to DODS type names and fixed float-vs-double bungle
//
// Revision 1.7  1999/03/24 23:40:05  jimg
// Added
//
// Revision 1.5.6.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.5.6.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.5  1998/03/19 23:32:59  jimg
// Removed old code (that was surrounded by #if 0 ... #endif).
//
// Revision 1.4  1997/09/22 23:03:46  jimg
// Added DDS * to deserialize parameters.
//
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

static char rcsid[] not_used = {"$Id: Float32.cc,v 1.11 2000/07/09 22:05:35 rmorris Exp $"};

#include <stdlib.h>
#include <assert.h>

#include "Float32.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "expr.tab.h"
#include "Operators.h"
#include "dods-limits.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
using std::cerr;
using std::endl;
#endif

Float32::Float32(const string &n) 
    : BaseType(n, dods_float32_c, (xdrproc_t)XDR_FLOAT32)
{
}

unsigned int
Float32::width()
{
    return sizeof(dods_float32);
}

bool
Float32::serialize(const string &dataset, DDS &dds, XDR *sink, 
		   bool ce_eval)
{
    int error;

    if (!read_p() && !read(dataset, error))
	return false;

    if (ce_eval && !dds.eval_selection(dataset))
	return true;

    if (!xdr_float(sink, &_buf))
	return false;

    return true;
}

bool
Float32::deserialize(XDR *source, DDS *, bool)
{
    unsigned int num = xdr_float(source, &_buf);

    return (num != 0);
}

unsigned int
Float32::val2buf(void *val, bool)
{
    assert(val);

    _buf = *(dods_float32 *)val;

    return width();
}

unsigned int
Float32::buf2val(void **val)
{
    assert(_buf && val);

    if (!*val)
	*val = new dods_float32;

    *(dods_float32 *)*val =_buf;

    return width();
}

void 
Float32::print_val(ostream &os, string space, bool print_decl_p)
{
    os.precision(DODS_FLT_DIG);

    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = " << _buf << ";" << endl;
    }
    else 
	os << _buf;
}

bool
Float32::ops(BaseType *b, int op, const string &dataset)
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
	return rops<dods_float32, dods_byte, Cmp<dods_float32, dods_byte> >
	    (_buf, dynamic_cast<Byte *>(b)->_buf, op);
      case dods_int16_c:
	return rops<dods_float32, dods_int16, Cmp<dods_float32, dods_int16> >
	    (_buf, dynamic_cast<Int16 *>(b)->_buf, op);
      case dods_uint16_c:
	return rops<dods_float32, dods_uint16, Cmp<dods_float32, dods_uint16> >
	    (_buf, dynamic_cast<UInt16 *>(b)->_buf, op);
      case dods_int32_c:
	return rops<dods_float32, dods_int32, Cmp<dods_float32, dods_int32> >
	    (_buf, dynamic_cast<Int32 *>(b)->_buf, op);
      case dods_uint32_c:
	return rops<dods_float32, dods_uint32, Cmp<dods_float32, dods_uint32> >
	    (_buf, dynamic_cast<UInt32 *>(b)->_buf, op);
      case dods_float32_c:
	return rops<dods_float32, dods_float32, Cmp<dods_float32, dods_float32> >
	    (_buf, dynamic_cast<Float32 *>(b)->_buf, op);
      case dods_float64_c:
	return rops<dods_float32, dods_float64, Cmp<dods_float32, dods_float64> >
	    (_buf, dynamic_cast<Float64 *>(b)->_buf, op);
      default:
	return false;
    }
}
