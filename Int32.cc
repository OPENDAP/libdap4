
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for Int32.
//
// jhrg 9/7/94

#include "config.h"

// #define DODS_DEBUG

#include <sstream>

#include "Byte.h" // synonymous with UInt8 and Char
#include "Float32.h"
#include "Float64.h"
#include "Int16.h"
#include "Int32.h"
#include "Int64.h"
#include "Int8.h"
#include "Str.h"
#include "UInt16.h"
#include "UInt32.h"
#include "UInt64.h"
#include "Url.h"

#include "DDS.h"
#include "Marshaller.h"
#include "UnMarshaller.h"

#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"
#include "DMR.h"

#include "DapIndent.h"
#include "InternalErr.h"
#include "Operators.h"
#include "debug.h"
#include "dods-limits.h"
#include "parser.h"
#include "util.h"

using std::cerr;
using std::endl;

namespace libdap {

/** The Int32 constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications.

    @param n A string containing the name of the variable to be
    created.
*/
Int32::Int32(const string &n) : BaseType(n, dods_int32_c), d_buf(0) {}

/** The Int32 server-side constructor accepts the name of the variable and
    the dataset name from which this instance is created.

    @param n A string containing the name of the variable to be created.
    @param d A string containing the name of the dataset from which this
    variable is created
*/
Int32::Int32(const string &n, const string &d) : BaseType(n, d, dods_int32_c), d_buf(0) {}

Int32::Int32(const Int32 &copy_from) : BaseType(copy_from) { d_buf = copy_from.d_buf; }

BaseType *Int32::ptr_duplicate() { return new Int32(*this); }

Int32::~Int32() { DBG(cerr << "~Int32" << endl); }

Int32 &Int32::operator=(const Int32 &rhs) {
    if (this == &rhs)
        return *this;
    BaseType::operator=(rhs);
    d_buf = rhs.d_buf;
    return *this;
}

bool Int32::serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval) {
#if USE_LOCAL_TIMEOUT_SCHEME
    dds.timeout_on();
#endif
    if (!read_p())
        read(); // read() throws Error and InternalErr

    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#if USE_LOCAL_TIMEOUT_SCHEME
    dds.timeout_off();
#endif
    m.put_int32(d_buf);

    return true;
}

bool Int32::deserialize(UnMarshaller &um, DDS *, bool) {
    um.get_int32(d_buf);

    return false;
}

void Int32::compute_checksum(Crc32 &checksum) { checksum.AddData(reinterpret_cast<uint8_t *>(&d_buf), sizeof(d_buf)); }

/**
 * @brief Serialize an Int8
 * @param m
 * @param dmr Unused
 * @param eval Unused
 * @param filter Unused
 * @exception Error is thrown if the value needs to be read and that operation fails.
 */
void Int32::serialize(D4StreamMarshaller &m, DMR &, /*ConstraintEvaluator &,*/ bool) {
    if (!read_p())
        read(); // read() throws Error

    m.put_int32(d_buf);
}

void Int32::deserialize(D4StreamUnMarshaller &um, DMR &) { um.get_int32(d_buf); }

unsigned int Int32::val2buf(void *val, bool) {
    // Jose Garcia
    // This method is public therefore and I believe it has being designed
    // to be use by read which must be implemented on the surrogated library,
    // thus if the pointer val is NULL, is an Internal Error.
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "The incoming pointer does not contain any data.");

    d_buf = *(dods_int32 *)val;

    return width();
}

unsigned int Int32::buf2val(void **val) {
    // Jose Garcia
    // The same comment justifying throwing an Error in val2buf applies here.
    if (!val)
        throw InternalErr(__FILE__, __LINE__, "NULL pointer.");

    if (!*val)
        *val = new dods_int32;

    *(dods_int32 *)*val = d_buf;

    return width();
}

dods_int32 Int32::value() const { return d_buf; }

bool Int32::set_value(dods_int32 i) {
    d_buf = i;
    set_read_p(true);

    return true;
}

void Int32::print_val(FILE *out, string space, bool print_decl_p) {
    ostringstream oss;
    print_val(oss, space, print_decl_p);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void Int32::print_val(ostream &out, string space, bool print_decl_p) {
    if (print_decl_p) {
        print_decl(out, space, false);
        out << " = " << (int)d_buf << ";\n";
    } else
        out << (int)d_buf;
}

bool Int32::ops(BaseType *b, int op) {
    if (!read_p() && !read()) {
        // Jose Garcia
        // Since the read method is virtual and implemented outside
        // libdap++ if we cannot read the data that is the problem
        // of the user or of whoever wrote the surrogate library
        // implemeting read therefore it is an internal error.
        throw InternalErr(__FILE__, __LINE__, "This value not read!");
    }

    // Extract the second arg's value.
    if (!b || !(b->read_p() || b->read())) {
        // Jose Garcia
        // Since the read method is virtual and implemented outside
        // libdap++ if we cannot read the data that is the problem
        // of the user or of whoever wrote the surrogate library
        // implemeting read therefore it is an internal error.
        throw InternalErr(__FILE__, __LINE__, "This value not read!");
    }

    return d4_ops(b, op);
}

/**
 * @see BaseType::d4_ops(BaseType *, int)
 */
bool Int32::d4_ops(BaseType *b, int op) {
    DBG(cerr << "b->typename(): " << b->type_name() << endl);
    switch (b->type()) {
    case dods_int8_c:
        return Cmp<dods_int32, dods_int8>(op, d_buf, static_cast<Int8 *>(b)->value());
    case dods_byte_c:
        return Cmp<dods_int32, dods_byte>(op, d_buf, static_cast<Byte *>(b)->value());
    case dods_int16_c:
        return Cmp<dods_int32, dods_int16>(op, d_buf, static_cast<Int16 *>(b)->value());
    case dods_uint16_c:
        return Cmp<dods_int32, dods_uint16>(op, d_buf, static_cast<UInt16 *>(b)->value());
    case dods_int32_c:
        return Cmp<dods_int32, dods_int32>(op, d_buf, static_cast<Int32 *>(b)->value());
    case dods_uint32_c:
        return Cmp<dods_int32, dods_uint32>(op, d_buf, static_cast<UInt32 *>(b)->value());
    case dods_int64_c:
        return Cmp<dods_int32, dods_int64>(op, d_buf, static_cast<Int64 *>(b)->value());
    case dods_uint64_c:
        return Cmp<dods_int32, dods_uint64>(op, d_buf, static_cast<UInt64 *>(b)->value());
    case dods_float32_c:
        return Cmp<dods_int32, dods_float32>(op, d_buf, static_cast<Float32 *>(b)->value());
    case dods_float64_c:
        return Cmp<dods_int32, dods_float64>(op, d_buf, static_cast<Float64 *>(b)->value());
    case dods_str_c:
    case dods_url_c:
        throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
    default:
        throw Error(malformed_expr, "Relational operators only work with scalar types.");
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
void Int32::dump(ostream &strm) const {
    strm << DapIndent::LMarg << "Int32::dump - (" << (void *)this << ")" << endl;
    DapIndent::Indent();
    BaseType::dump(strm);
    strm << DapIndent::LMarg << "value: " << d_buf << endl;
    DapIndent::UnIndent();
}

} // namespace libdap
