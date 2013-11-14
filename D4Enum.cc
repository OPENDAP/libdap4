
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

#include "D4Group.h"
#include "D4Enum.h"
#include "D4EnumDefs.h"
#include "D4Attributes.h"

#include "Float32.h"
#include "Float64.h"

#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"

#include "Operators.h"
#include "InternalErr.h"
#include "util.h"
#include "debug.h"

using std::cerr;
using std::endl;

namespace libdap {

// Explicit instantiation of the template member function 'value(T *)'.
// This is required in order to have the library contain these member
// function when its own code does not use them. Normally, C++ instantiates
// templates when they are used, and this forces that process so the
// library file contains the various versions of the member function.
//
// NB: I could not get this syntax to work in the header file. jhrg 8/19/13
template void D4Enum::value<dods_byte>(dods_byte *v) const;
template void D4Enum::value<dods_int16>(dods_int16 *v) const;
template void D4Enum::value<dods_uint16>(dods_uint16 *v) const;
template void D4Enum::value<dods_int32>(dods_int32 *v) const;
template void D4Enum::value<dods_uint32>(dods_uint32 *v) const;
template void D4Enum::value<dods_int64>(dods_int64 *v) const;
template void D4Enum::value<dods_uint64>(dods_uint64 *v) const;

template void D4Enum::set_value<dods_byte>(dods_byte v);
template void D4Enum::set_value<dods_int16>(dods_int16 v);
template void D4Enum::set_value<dods_uint16>(dods_uint16 v);
template void D4Enum::set_value<dods_int32>(dods_int32 v);
template void D4Enum::set_value<dods_uint32>(dods_uint32 v);
template void D4Enum::set_value<dods_int64>(dods_int64 v);
template void D4Enum::set_value<dods_uint64>(dods_uint64 v);

void
D4Enum::set_enumeration(D4EnumDef *enum_def) {
    d_enum_def = enum_def;
    d_element_type = enum_def->type();
}

void
D4Enum::compute_checksum(Crc32 &checksum)
{
    switch (d_element_type) {
    case dods_byte_c:
    case dods_uint8_c:
    case dods_int8_c:
        checksum.AddData(reinterpret_cast<uint8_t*>(&d_buf.ui8), sizeof(uint8_t));
        break;
    case dods_uint16_c:
    case dods_int16_c:
        checksum.AddData(reinterpret_cast<uint8_t*>(&d_buf.ui16), sizeof(uint16_t));
        break;
    case dods_uint32_c:
    case dods_int32_c:
        checksum.AddData(reinterpret_cast<uint8_t*>(&d_buf.ui32), sizeof(uint32_t));
        break;
    case dods_uint64_c:
    case dods_int64_c:
        checksum.AddData(reinterpret_cast<uint8_t*>(&d_buf.ui64), sizeof(uint64_t));
        break;

    default:
        assert(!"illegal type for D4Enum");
    }
}


/**
 * @brief Serialize a D4Enum
 * Use the (integer) data type associated with an Enumeration definition to
 * serialize the value of a D4Enum variable. This send just the bits that
 * correspond to the declared type, not all 64-bits of storage used by a
 * scalar D4Enum.
 * @param m
 * @param dmr Unused
 * @param eval Unused
 * @param filter Unused
 * @exception Error is thrown if the value needs to be read and that operation fails.
 */
void
D4Enum::serialize(D4StreamMarshaller &m, DMR &, ConstraintEvaluator &, bool)
{
    if (!read_p())
        read();          // read() throws Error

	switch (d_element_type) {
	case dods_byte_c:
	case dods_uint8_c:
		m.put_byte(d_buf.ui8);
		break;
	case dods_uint16_c:
		m.put_uint16(d_buf.ui16);
		break;
	case dods_uint32_c:
		m.put_uint32(d_buf.ui32);
		break;
	case dods_uint64_c:
		m.put_uint64(d_buf.ui64);
		break;

	case dods_int8_c:
		m.put_int8(d_buf.i8);
		break;
	case dods_int16_c:
		m.put_int16(d_buf.i16);
		break;
	case dods_int32_c:
		m.put_int32(d_buf.i32);
		break;
	case dods_int64_c:
		m.put_int64(d_buf.i64);
		break;
	default:
		assert(!"illegal type for D4Enum");
	}
}

void
D4Enum::deserialize(D4StreamUnMarshaller &um, DMR &)
{
	switch (d_element_type) {
	case dods_byte_c:
	case dods_uint8_c:
		um.get_byte(d_buf.ui8);
		break;
	case dods_uint16_c:
		um.get_uint16(d_buf.ui16);
		break;
	case dods_uint32_c:
		um.get_uint32(d_buf.ui32);
		break;
	case dods_uint64_c:
		um.get_uint64(d_buf.ui64);
		break;

	case dods_int8_c:
		um.get_int8(d_buf.i8);
		break;
	case dods_int16_c:
		um.get_int16(d_buf.i16);
		break;
	case dods_int32_c:
		um.get_int32(d_buf.i32);
		break;
	case dods_int64_c:
		um.get_int64(d_buf.i64);
		break;
	default:
		assert(!"illegal type for D4Enum");
	}
}

unsigned int D4Enum::val2buf(void *val, bool)
{
    if (!val)
        throw InternalErr("The incoming pointer does not contain any data.");

    switch (d_element_type) {
     case dods_byte_c:
     case dods_uint8_c:
         d_buf.ui8 = *(dods_byte*)val;
         break;
     case dods_uint16_c:
         d_buf.ui16 = *(dods_uint16*)val;
         break;
     case dods_uint32_c:
         d_buf.ui32 = *(dods_uint32*)val;
         break;
     case dods_uint64_c:
         d_buf.ui64 = *(dods_uint64*)val;
         break;

     case dods_int8_c:
         d_buf.i8 = *(dods_int8*)val;
         break;
     case dods_int16_c:
         d_buf.i16 = *(dods_int16*)val;
         break;
     case dods_int32_c:
         d_buf.i32 = *(dods_int32*)val;
         break;
     case dods_int64_c:
         d_buf.i64 = *(dods_int64*)val;
         break;
     default:
         assert(!"illegal type for D4Enum");
     }

    return width();
}

unsigned int D4Enum::buf2val(void **val)
{
    if (!val)
        throw InternalErr("NULL pointer");

    switch (d_element_type) {
     case dods_byte_c:
     case dods_uint8_c:
         if (!*val) *val = new dods_byte;
         *(dods_byte *) * val = d_buf.ui8;
         break;
     case dods_uint16_c:
         if (!*val) *val = new dods_uint16;
         *(dods_uint16 *) * val = d_buf.ui16;
         break;
     case dods_uint32_c:
         if (!*val) *val = new dods_uint32;
         *(dods_uint32 *) * val = d_buf.ui32;
         break;
     case dods_uint64_c:
         if (!*val) *val = new dods_uint64;
         *(dods_uint64 *) * val = d_buf.ui64;
         break;

     case dods_int8_c:
         if (!*val) *val = new dods_int8;
         *(dods_int8*) * val = d_buf.i8;
         break;
     case dods_int16_c:
         if (!*val) *val = new dods_int16;
         *(dods_int16 *) * val = d_buf.i16;
         break;
     case dods_int32_c:
         if (!*val) *val = new dods_int32;
         *(dods_int32 *) * val = d_buf.i32;
         break;
     case dods_int64_c:
         if (!*val) *val = new dods_int64;
         *(dods_int64 *) * val = d_buf.i64;
         break;
     default:
         assert(!"illegal type for D4Enum");
     }

    return width();
}

void D4Enum::print_val(ostream &out, string space, bool print_decl_p)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        out << " = ";
    }

    if (is_signed()) {
    	int64_t v;
    	value(&v);
    	out << v;
    }
    else {
    	uint64_t v;
    	value(&v);
    	out << v;
    }

    if (print_decl_p)
    	out << ";" << endl;
}

/** Write the XML representation of this variable. This method is used to
    build the DDX XML response.
    @param out Destination output stream
    @param space Use this to indent child declarations. Default is "".
    @param constrained If true, only print this if it's part part of the
    current projection. Default is False. */
void
D4Enum::print_xml_writer(XMLWriter &xml, bool constrained)
{
    if (constrained && !send_p())
        return;

    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"Enum") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Enum element");

    if (!name().empty())
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");


    string path = d_enum_def->name();
    // Not every D4EnumDef is a member of an instance of D4EnumDefs - the D4EnumDefs instance
    // holds a reference to the D4Group that holds the Enum definitions.
    // TODO Should this be changed - so the EnumDef holds a reference to its parent Group?
    if (d_enum_def->parent()) {
    	// print the FQN for the enum def; D4Group::FQN() includes the trailing '/'
    	path = static_cast<D4Group*>(d_enum_def->parent()->parent())->FQN() + path;
    }
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "enum", (const xmlChar*)path.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for enum");

    attributes()->print_dap4(xml);

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
#if 0
            // FIXME
        case dods_int64_c:
            return Cmp<dods_int64, dods_int64>(op, d_buf, static_cast<D4Enum*>(b)->value());
        case dods_uint64_c:
            return SUCmp<dods_int64, dods_uint64>(op, d_buf, static_cast<D4Enum*>(b)->value());
#endif
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
    strm << DapIndent::LMarg << "value: " << d_buf.ui64 << endl ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

