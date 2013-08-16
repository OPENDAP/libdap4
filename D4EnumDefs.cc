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

#include "D4EnumDefs.h"

#include <sstream>

#include "dods-limits.h"
#include "util.h"

namespace libdap {

/** Test if a particular value is legal for a given type. In a D4EnumDef,
 * all values are actually stored in a long long, but the different
 * enumerations can specify different types like Byte, Int32, ..., and this
 * method is used to test that the values match those types.
 */
bool
D4EnumDef::is_valid_enum_value(long long value)
{
    switch (type()) {
        case dods_int8_c:
            return (value >= DODS_SCHAR_MIN && value <= DODS_SCHAR_MAX);
        case dods_byte_c:
        case dods_uint8_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_UCHAR_MAX);
        case dods_int16_c:
            return (value >= DODS_SHRT_MIN && value <= DODS_SHRT_MAX);
        case dods_uint16_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_USHRT_MAX);
        case dods_int32_c:
            return (value >= DODS_INT_MIN && value <= DODS_INT_MAX);
        case dods_uint32_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_UINT_MAX);
        case dods_int64_c:
            return (value >= DODS_LLONG_MIN && value <= DODS_LLONG_MAX);
        case dods_uint64_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_ULLONG_MAX);
        default:
            return false;
    }
}

#if 0
void D4EnumDefValues::print_value(XMLWriter &xml, const D4EnumDefValues::tuple &ev) const
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"EnumConst") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write EnumConst element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)ev.label.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    ostringstream oss;
    oss << ev.value;
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "value", (const xmlChar*)oss.str().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for value");

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end EnumConst element");
}

void D4EnumDefValues::print_dap4(XMLWriter &xml) const
{
    vector<D4EnumDefValues::tuple>::const_iterator i = d_tuples.begin();
    while(i != d_tuples.end()) {
        print_value(xml, *i++);
    }
}
#endif

void D4EnumDef::print_value(XMLWriter &xml, const D4EnumDef::tuple &tuple) const
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"EnumConst") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write EnumConst element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)tuple.label.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    ostringstream oss;
    oss << tuple.value;
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "value", (const xmlChar*)oss.str().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for value");

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end EnumConst element");
}

void D4EnumDef::print_dap4(XMLWriter &xml) const
{
    vector<D4EnumDef::tuple>::const_iterator i = d_tuples.begin();
    while(i != d_tuples.end()) {
        print_value(xml, *i++);
    }
}

void D4EnumDefs::m_print_enum(XMLWriter &xml, D4EnumDef *e) const
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"Enumeration") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Enumeration element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)e->name().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "basetype", (const xmlChar*)D4type_name(e->type()).c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    // print each of e.values
    e->print_dap4(xml);

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end Enumeration element");
}

void D4EnumDefs::print_dap4(XMLWriter &xml) const
{
    D4EnumDefCIter i = d_enums.begin();
    while (i != d_enums.end()) {
        m_print_enum(xml, *i++);
    }
}

} /* namespace libdap */
