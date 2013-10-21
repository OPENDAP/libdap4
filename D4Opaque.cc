// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin D4Opaqueeet, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.


#include "config.h"

#include <sstream>

#include "D4Opaque.h"

#include "DMR.h"
#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"

#include "util.h"
#include "crc.h"

#include "debug.h"

using namespace std;

namespace libdap {

D4Opaque &
D4Opaque::operator=(const D4Opaque &rhs)
{
    if (this == &rhs)
        return *this;

    // Call BaseType::operator=
    dynamic_cast<BaseType &>(*this) = rhs;

    d_buf = rhs.d_buf;

    return *this;
}

void
D4Opaque::compute_checksum(Crc32 &checksum)
{
	checksum.AddData(&d_buf[0], d_buf.length());
}

void
D4Opaque::serialize(D4StreamMarshaller &m, DMR &, ConstraintEvaluator &, bool)
{
    if (!read_p())
        read();          // read() throws Error

    m.put_opaque_dap4( &d_buf[0], d_buf.length() ) ;
}

void
D4Opaque::deserialize(D4StreamUnMarshaller &um, DMR &)
{
    um.get_opaque_dap4( d_buf ) ;
}

/** Read the object's value and put a copy in the C++ string object
    referenced by \e **val. If \e *val is null, this method will allocate
    a string object using new and store the result there. If \e *val
    is not null, it will assume that \e *val references a string object
    and put the value there.

    @param val A pointer to null or to a string object.
    @return The sizeof(string*)
    @exception InternalErr Thrown if \e val is null. */
unsigned int
D4Opaque::buf2val(void **val)
{
	assert(val);

    // If *val is null, then the caller has not allocated storage for the
    // value; we must. If there is storage there, assume it is a string and
    // assign d_buf's value to that storage.
    if (!*val)
        *val = new vector<uint8_t>;
    else
        *static_cast<vector<uint8_t>*>(*val) = d_buf;

    return sizeof(vector<uint8_t>*);
}

/** Store the value referenced by \e val in this object. Even though the
    type of \e val is \c void*, this method assumes the type is \c string*.
    Note that the value is copied so the caller if free to throw away/reuse
    the actual parameter once this call has returned.

    @param val A pointer to a C++ string object.
    @param reuse Not used by this version of the method.
    @exception IntenalErr if \e val is null.
    @return The width of the pointer. */
unsigned int
D4Opaque::val2buf(void *val, bool)
{
    // Jose Garcia
    // This method is public therefore and I believe it has being designed
    // to be use by read which must be implemented on the surrogated library,
    // thus if the pointer val is NULL, is an Internal Error.
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    d_buf = *static_cast<string*>(val);

    return sizeof(string*);
}

/** Set the value of this instance.
    @param value The value
    @return Always returns true; the return type of bool is for compatibility
    with the Passive* subclasses written by HAO. */
bool
D4Opaque::set_value(const string &value)
{
    d_buf = value;
    set_read_p(true);

    return true;
}

/** Get the value of this instance.
    @return The value. */
string
D4Opaque::value() const
{
    return d_buf;
}

void
D4Opaque::print_val(FILE *out, string space, bool print_decl_p)
{
    ostringstream oss;
    print_val(oss, space, print_decl_p);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void
D4Opaque::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
	out << " = \"" << escattr(d_buf) << "\";\n" ;
    }
    else
	out << "\"" << escattr(d_buf) << "\"" ;
}

bool
D4Opaque::ops(BaseType *b, int op)
{
    // Extract the Byte arg's value.
    if (!read_p() && !read()) {
        // Jose Garcia
        // Since the read method is virtual and implemented outside
        // libdap++ if we cannot read the data that is the problem
        // of the user or of whoever wrote the surrogate library
        // implemeting read therefore it is an internal error.
        throw InternalErr(__FILE__, __LINE__, "This value was not read!");
    }

    // Extract the second arg's value.
    if (!b || !(b->read_p() || b->read())) {
        // Jose Garcia
        // Since the read method is virtual and implemented outside
        // libdap++ if we cannot read the data that is the problem
        // of the user or of whoever wrote the surrogate library
        // implemeting read therefore it is an internal error.
        throw InternalErr(__FILE__, __LINE__, "Argument value was not read!");
    }

    switch (b->type()) {
    case dods_str_c:
        return D4OpaqueCmp<string, string>(op, d_buf, static_cast<D4Opaque*>(b)->value());
    case dods_url_c:
        return D4OpaqueCmp<string, string>(op, d_buf, static_cast<Url*>(b)->value());
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
void
D4Opaque::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "D4Opaque::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    BaseType::dump(strm) ;
    strm << DapIndent::LMarg << "value: " << d_buf << endl ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

