
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

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for Float32.
//
// 3/22/99 jhrg


#include <iomanip>

#include "config.h"

static char rcsid[] not_used =
    {"$Id$"
    };

#include "Float32.h"
#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "Operators.h"
#include "dods-limits.h"
#include "InternalErr.h"


using std::cerr;
using std::endl;

namespace libdap {

/** The Float32 constructor accepts only the name of the variable. The
    name may be omitted, which will create a nameless variable. This
    may be adequate for some applications.

    @param n A string containing the name of the variable to be
    created.
*/
Float32::Float32(const string &n)
        : BaseType(n, dods_float32_c)
{}

/** The Float32 server-side constructor accepts the name of the variable and
    the dataset name from which this instance is created.

    @param n A string containing the name of the variable to be created.
    @param d A string containing the name of the dataset from which this
    variable is created
*/
Float32::Float32(const string &n, const string &d)
        : BaseType(n, d, dods_float32_c)
{}

Float32::Float32(const Float32 &copy_from) : BaseType(copy_from)
{
    _buf = copy_from._buf;
}

BaseType *
Float32::ptr_duplicate()
{
    return new Float32(*this);
}

Float32 &
Float32::operator=(const Float32 &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<BaseType &>(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int
Float32::width()
{
    return sizeof(dods_float32);
}

bool
Float32::serialize(ConstraintEvaluator &eval, DDS &dds,
                   Marshaller &m, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
        read();  // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    m.put_float32( _buf ) ;

    return true;
}

bool
Float32::deserialize(UnMarshaller &um, DDS *, bool)
{
    um.get_float32( _buf ) ;

    return false;
}

unsigned int
Float32::val2buf(void *val, bool)
{
    // Jose Garcia This method is public I believe it has been
    // designed to be used by read which must be implemented in a
    // subclass, thus if the pointer val is NULL, is an Internal
    // Error.
    // Changed to InternalErr. jhrg
    if (!val)
        throw InternalErr(__FILE__, __LINE__,
                          "The incoming pointer does not contain any data.");

    _buf = *(dods_float32 *)val;

    return width();
}

unsigned int
Float32::buf2val(void **val)
{
    // Jose Garcia
    // The same comment justifying throwing an Error in val2buf applies here.
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    if (!*val)
        *val = new dods_float32;

    *(dods_float32 *)*val = _buf;

    return width();
}

bool
Float32::set_value(dods_float32 f)
{
    _buf = f;
    set_read_p(true);

    return true;
}

/** Return the value of the Float32 held by this instance. This is more
    convenient than the general interface provided by buf2val, but its use
    requires a cast from BaseType to Float32.

    @return The dods_float32 value. */
dods_float32
Float32::value() const
{
    return _buf;
}
//#if FILE_METHODS
void
Float32::print_val(FILE *out, string space, bool print_decl_p)
{
    // FIX: need to set precision in the printing somehow.
    // os.precision(DODS_FLT_DIG);

    if (print_decl_p) {
        print_decl(out, space, false);
        fprintf(out, " = %.6g;\n", _buf) ;
    }
    else
        fprintf(out, "%.6g", _buf) ;
}
//#endif
void
Float32::print_val(ostream &out, string space, bool print_decl_p)
{
    // FIX: need to set precision in the printing somehow.
    // os.precision(DODS_FLT_DIG);

    if (print_decl_p) {
        print_decl(out, space, false);
	out << " = " << std::setprecision( 6 ) << _buf << ";\n" ;
    }
    else
	out << std::setprecision( 6 ) << _buf ;
}

bool
Float32::ops(BaseType *b, int op)
{
    // Get this instance's value
    if (!read_p() && !read()) {
        // Jose Garcia Since the read method is virtual and
        // implemented outside libdap++ if we cannot read the data
        // that is the problem of whomever wrote the implementation of
        // read and therefore it is an internal error.
        throw InternalErr(__FILE__, __LINE__, "This value not read!");
    }

    // Extract the second arg's value.
    if (!b || !b->read_p() && !b->read()) {
        throw InternalErr(__FILE__, __LINE__, "This value not read!");
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

/** @brief dumps information about this object

   Displays the pointer value of this instance and information about this
   instance.

   @param strm C++ i/o stream to dump the information to
   @return void
 */
void
Float32::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Float32::dump - (" << (void *)this << ")"
	 << endl ;
    DapIndent::Indent() ;
    BaseType::dump(strm) ;
    strm << DapIndent::LMarg << "value: " << _buf << endl ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

