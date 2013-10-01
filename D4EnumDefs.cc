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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "D4EnumDefs.h"

#include <sstream>

#include "util.h"

namespace libdap {

void enumValues::print_value(XMLWriter &xml, const enumValues::value &ev) const
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"EnumConst") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write EnumConst element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)ev.item.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    ostringstream oss;
    oss << ev.num;
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "value", (const xmlChar*)oss.str().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for value");

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end EnumConst element");
}

void enumValues::print(XMLWriter &xml) const
{
    vector<enumValues::value>::const_iterator i = d_values.begin();
    while(i != d_values.end()) {
        print_value(xml, *i++);
    }
}

void D4EnumDefs::print_enum(XMLWriter &xml, const D4EnumDefs::enumeration &e) const
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"Enumeration") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Enumeration element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)e.name.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "basetype", (const xmlChar*)type_name(e.type).c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    // print each of e.values
    e.values.print(xml);

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end Enumeration element");
}

void D4EnumDefs::print(XMLWriter &xml) const
{
    vector<D4EnumDefs::enumeration>::const_iterator i = d_enums.begin();
    while (i != d_enums.end()) {
        print_enum(xml, *i++);
    }
}

} /* namespace libdap */
