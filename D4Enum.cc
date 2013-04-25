
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <cassert>
#include <sstream>

#include "Byte.h"           // synonymous with UInt8 and Char
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "D4Enum.h"
//#include "UD4Enum.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"

#if 0
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"
#endif

#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"

#include "DDS.h"
#include "util.h"
#include "parser.h"
#include "Operators.h"
#include "dods-limits.h"
#include "debug.h"
#include "util.h"
#include "InternalErr.h"

using std::cerr;
using std::endl;

namespace libdap {

bool
D4Enum::serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval)
{
    dds.timeout_on();

    if (!read_p())
        read();  // read() throws Error and InternalErr

#if EVAL
    if (ce_eval && !eval.eval_selection(dds, dataset()))
        return true;
#endif

    dds.timeout_off();

    assert(typeid(m) == typeid(D4StreamMarshaller));

    static_cast<D4StreamMarshaller*>(&m)->put_int64( d_buf ) ;

    return true;
}

bool
D4Enum::deserialize(UnMarshaller &um, DDS *, bool)
{
    assert(typeid(um) == typeid(D4StreamUnMarshaller));

    static_cast<D4StreamUnMarshaller*>(&um)->get_int64( &d_buf ) ;

    return false;
}

virtual void value(dods_byte *v) const;
virtual void value(dods_int16 *v) const;
virtual void value(dods_uint16 *v) const;
virtual void value(dods_int32 *v) const;
virtual void value(dods_uint32 *v) const;
virtual void value(dods_int64 *v) const;
virtual void value(dods_uint64 *v) const;

virtual void set_value(dods_byte v) ;
virtual void set_value(dods_int16 v) ;
virtual void set_value(dods_uint16 v) ;
virtual void set_value(dods_int32 v) ;
virtual void set_value(dods_uint32 v) ;
virtual void set_value(dods_int64 v) ;
virtual void set_value(dods_uint64 v) ;

void D4Enum::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        out << " = " << (dods_int64) d_buf << ";\n";
    }
    else
        out << (int) d_buf;
}

/** Write the XML representation of this variable. This method is used to
    build the DDX XML response.
    @param out Destination output stream
    @param space Use this to indent child declarations. Default is "".
    @param constrained If true, only print this if it's part part of the
    current projection. Default is False. */
void
BaseType::print_xml_writer(XMLWriter &xml, bool constrained)
{
    if (constrained && !send_p())
        return;

    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)Enum) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Enum element");

    if (!d_name.empty())
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    if (is_integer_type(d_element_type)
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "enum", (const xmlChar*)type_name(d_element_type).c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    if (get_attr_table().get_size() > 0)
        get_attr_table().print_xml_writer(xml);

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end Enum element");
}


bool
D4Enum::ops(BaseType *b, int op)
{
    // Get the arg's value.
    if (!read_p() && !read())
        throw InternalErr(__FILE__, __LINE__, "This value not read!");

    // Get the second arg's value.
    if (!b->read_p() && !b->read())
        throw InternalErr(__FILE__, __LINE__, "This value not read!");

    switch (b->type()) {
        case dods_int8_c:
            return Cmp<dods_int64, dods_int8>(op, d_buf, static_cast<Int8*>(b)->value());
        case dods_byte_c:
            return SUCmp<dods_int64, dods_byte>(op, d_buf, static_cast<Byte*>(b)->value());
        case dods_int16_c:
            return Cmp<dods_int64, dods_int16>(op, d_buf, static_cast<Int16*>(b)->value());
        case dods_uint16_c:
            return SUCmp<dods_int64, dods_uint16>(op, d_buf, static_cast<UInt16*>(b)->value());
        case dods_int32_c:
            return Cmp<dods_int64, dods_int32>(op, d_buf, static_cast<Int32*>(b)->value());
        case dods_uint32_c:
            return SUCmp<dods_int64, dods_uint32>(op, d_buf, static_cast<UInt32*>(b)->value());
        case dods_int64_c:
            return Cmp<dods_int64, dods_int64>(op, d_buf, static_cast<D4Enum*>(b)->value());
        case dods_uint64_c:
            return SUCmp<dods_int64, dods_uint64>(op, d_buf, static_cast<UD4Enum*>(b)->value());
        case dods_float32_c:
            return Cmp<dods_int64, dods_float32>(op, d_buf, static_cast<Float32*>(b)->value());
        case dods_float64_c:
            return Cmp<dods_int64, dods_float64>(op, d_buf, static_cast<Float64*>(b)->value());
        default:
            return false;
    }

    return false;
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
D4Enum::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "D4Enum::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    BaseType::dump(strm) ;
    strm << DapIndent::LMarg << "value: " << d_buf << endl ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

