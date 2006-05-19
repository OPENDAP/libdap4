
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for Float64.
//
// jhrg 9/7/94


#include "config.h"

static char rcsid[] not_used = {"$Id$"};

#include <stdlib.h>

#include <iomanip>

#include "Float64.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
//#include "ce_expr.tab.h"
#include "Operators.h"
#include "dods-limits.h"
#include "InternalErr.h"


using std::cerr;
using std::endl;

/** The Float64 constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    @param n A string containing the name of the variable to be
    created. 

*/
Float64::Float64(const string &n) 
    : BaseType(n, dods_float64_c, (xdrproc_t)XDR_FLOAT64)
{
}

Float64::Float64(const Float64 &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}
    
BaseType *
Float64::ptr_duplicate()
{
    return new Float64(*this);
}

Float64 &
Float64::operator=(const Float64 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int
Float64::width()
{
    return sizeof(dods_float64);
}

bool
Float64::serialize(const string &dataset, ConstraintEvaluator &eval, DDS &dds,
                   XDR *sink, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
	read(dataset);		// read() throws Error and InternalErr

    if (ce_eval && !eval.eval_selection(dds, dataset))
	return true;

    dds.timeout_off();

    if (!xdr_double(sink, &_buf))
	throw Error(
"Network I/O Error. Could not send float 64 data.\n\
This may be due to a bug in libdap, on the server or a\n\
problem with the network connection.");

    return true;
}

bool
Float64::deserialize(XDR *source, DDS *, bool)
{
    if (!xdr_double(source, &_buf))
	throw Error(
"Network I/O Error. Could not read float 64 data. This may be due to a\n\
bug in libdap or a problem with the network connection.");

    return false;
}

unsigned int
Float64::val2buf(void *val, bool)
{
    // Jose Garcia
    // This method is public therefore and I believe it has being designed
    // to be use by read which must be implemented on the surrogated library,
    // thus if the pointer val is NULL, is an Internal Error. 
    if(!val)
	throw InternalErr(__FILE__, __LINE__, 
			  "The incoming pointer does not contain any data.");

    _buf = *(dods_float64 *)val;

    return width();
}

unsigned int
Float64::buf2val(void **val)
{
    // Jose Garcia
    // The same comment justifying throwing an Error in val2buf applies here.
    if (!val)
	throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    if (!*val)
	*val = new dods_float64;

    *(dods_float64 *)*val =_buf;

    return width();
}

/** Return the value of the Float64 held by this instance. This is more
    convenient than the general interface provided by buf2val, but its use
    requires a downcase from BaseType to Float64.

    @return The dods_float32 value. */
dods_float64
Float64::value()
{
    return _buf;
}

void
Float64::print_val(FILE *out, string space, bool print_decl_p)
{
    // FIX: need to set precision in the printing somehow.
    // os.precision(DODS_DBL_DIG);

    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = %.15g;\n", _buf ) ;
    }
    else 
	fprintf( out, "%.15g", _buf ) ;
}

bool
Float64::ops(BaseType *b, int op, const string &dataset)
{
    // Extract the Byte arg's value.
    if (!read_p() && !read(dataset)) {
      // Jose Garcia
      // Since the read method is virtual and implemented outside
      // libdap++ if we can not read the data that is the problem 
      // of the user or of whoever wrote the surrogate library
      // implemeting read therefore it is an internal error.
      throw InternalErr(__FILE__, __LINE__, "This value not read!");
    }

    // Extract the second arg's value.
    if (!b->read_p() && !b->read(dataset)) {
      // Jose Garcia
      // Since the read method is virtual and implemented outside
      // libdap++ if we can not read the data that is the problem 
      // of the user or of whoever wrote the surrogate library
      // implemeting read therefore it is an internal error.
      throw InternalErr(__FILE__, __LINE__, "This value not read!");
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
