
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

// Implementation for Byte.
//
// jhrg 9/7/94


#include "config.h"

static char rcsid[] not_used =
    { "$Id$"
    };

#include "Byte.h"
//#include "Int16.h"
#include "DDS.h"
#include "Operators.h"

#include "util.h"
#include "parser.h"
#include "dods-limits.h"
#include "InternalErr.h"

using std::cerr;
using std::endl;

namespace libdap {

/** The Byte constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications.

    @brief The Byte constructor.
    @param n A string containing the name of the variable to be
    created.

*/
Byte::Byte(const string & n): BaseType(n, dods_byte_c)
{}

/** This Byte constructor requires the name of the variable to be created
    and the name of the dataset from which this variable is being created.
    This constructor is used in server-side processing, loading structure in
    from a dataset.

    @brief The Byte server-side constructor.
    @param n A string containing the name of the variable to be created.
    @param d A string containing the name of the dataset from which the
    variable is being created.
*/
Byte::Byte(const string &n, const string &d): BaseType(n, d, dods_byte_c)
{}

Byte::Byte(const Byte & copy_from): BaseType(copy_from)
{
    _buf = copy_from._buf;
}

BaseType *Byte::ptr_duplicate()
{
    return new Byte(*this);
}

Byte & Byte::operator=(const Byte & rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast < BaseType & >(*this) = rhs;

    _buf = rhs._buf;

    return *this;
}

unsigned int Byte::width()
{
    return sizeof(dods_byte);
}

/** Serialize the contents of member _BUF (the object's internal
    buffer, used to hold data) and write the result to stdout. If
    FLUSH is true, write the contents of the output buffer to the
    kernel. FLUSH is false by default. If CE_EVAL is true, evaluate
    the current constraint expression; only send data if the CE
    evaluates to true.

    @return False if a failure to read, send or flush is detected, true
    otherwise.
*/
bool Byte::serialize(ConstraintEvaluator & eval, DDS & dds,
		     Marshaller &m, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
        read();          // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    m.put_byte( _buf ) ;

    return true;
}

/** @brief Deserialize the char on stdin and put the result in
    <tt>_BUF</tt>.
*/
bool Byte::deserialize(UnMarshaller &um, DDS *, bool)
{
    um.get_byte( _buf ) ;

    return false;
}

/** Store the value referenced by <i>val</i> in the object's internal
    buffer. <i>reuse</i> has no effect because this class does not
    dynamically allocate storage for the internal buffer.

    @return The size (in bytes) of the value's representation.  */
unsigned int Byte::val2buf(void *val, bool)
{
    // Jose Garcia
    // This method is public therefore and I believe it has being designed
    // to be use by read which must be implemented on the surrogate library,
    // thus if the pointer val is NULL, is an Internal Error.
    if (!val)
        throw InternalErr("the incoming pointer does not contain any data.");

    _buf = *(dods_byte *) val;

    return width();
}

unsigned int Byte::buf2val(void **val)
{
    // Jose Garcia
    // The same comment justifying throwing an Error in val2buf applies here.
    if (!val)
        throw InternalErr("NULL pointer");

    if (!*val)
        *val = new dods_byte;

    *(dods_byte *) * val = _buf;

    return width();
}

/** Set the value of this instance.
    @param value The value
    @return Always returns true; the return type of bool is for compatibility
    with the Passive* subclasses written by HAO. */
bool Byte::set_value(dods_byte value)
{
    _buf = value;
    set_read_p(true);

    return true;
}

/** Get the value of this instance.
    @return The value. */
dods_byte Byte::value() const
{
    return _buf;
}
//#if FILE_METHODS
void Byte::print_val(FILE * out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        fprintf(out, " = %d;\n", (int) _buf);
    }
    else
        fprintf(out, "%d", (int) _buf);
}
//#endif
void Byte::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
	out << " = " << (int)_buf << ";\n" ;
    }
    else
	out << (int)_buf ;
}

bool Byte::ops(BaseType * b, int op)
{

    // Extract the Byte arg's value.
    if (!read_p() && !read()) {
        cerr << "This value not read!" << endl;
        // Jose Garcia
        // Since the read method is virtual and implemented outside
        // libdap++ if we cannot read the data that is the problem
        // of the user or of whoever wrote the surrogate library
        // implementing read therefore it is an internal error.
        throw InternalErr("This value not read!");
    }
    // Extract the second arg's value.
    if (!b || !b->read_p() && !b->read()) {
        cerr << "This value not read!" << endl;
        // Jose Garcia
        // Since the read method is virtual and implemented outside
        // libdap++ if we cannot read the data that is the problem
        // of the user or of whoever wrote the surrogate library
        // implementing read therefore it is an internal error.
        throw InternalErr("This value not read!");
    }

    switch (b->type()) {
    case dods_byte_c:
        return rops < dods_byte, dods_byte, Cmp < dods_byte, dods_byte > >
               (_buf, dynamic_cast < Byte * >(b)->_buf, op);
    case dods_int16_c:
        return rops < dods_byte, dods_int16, USCmp < dods_byte,
               dods_int16 > > (_buf, dynamic_cast < Int16 * >(b)->_buf, op);
    case dods_uint16_c:
        return rops < dods_byte, dods_uint16, Cmp < dods_byte,
               dods_uint16 > > (_buf, dynamic_cast < UInt16 * >(b)->_buf, op);
    case dods_int32_c:
        return rops < dods_byte, dods_int32, USCmp < dods_byte,
               dods_int32 > > (_buf, dynamic_cast < Int32 * >(b)->_buf, op);
    case dods_uint32_c:
        return rops < dods_byte, dods_uint32, Cmp < dods_byte,
               dods_uint32 > > (_buf, dynamic_cast < UInt32 * >(b)->_buf, op);
    case dods_float32_c:
        return rops < dods_byte, dods_float32, Cmp < dods_byte,
               dods_float32 > > (_buf, dynamic_cast < Float32 * >(b)->_buf,
                                 op);
    case dods_float64_c:
        return rops < dods_byte, dods_float64, Cmp < dods_byte,
               dods_float64 > > (_buf, dynamic_cast < Float64 * >(b)->_buf,
                                 op);
    default:
        return false;
    }
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and information about this
 * instance.
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void Byte::dump(ostream & strm) const
{
    strm << DapIndent::LMarg << "Byte::dump - ("
    << (void *) this << ")" << endl;
    DapIndent::Indent();
    BaseType::dump(strm);
    strm << DapIndent::LMarg << "value: " << _buf << endl;
    DapIndent::UnIndent();
}

} // namespace libdap
