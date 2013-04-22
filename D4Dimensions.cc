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

#include <sstream>

#include "D4Dimensions.h"
#include "Error.h"
#include "InternalErr.h"
#include "XMLWriter.h"

namespace libdap {

void
D4Dimension::set_size(const string &size)
{
    if (size == "*") {
        set_varying(true);
    }
    else {
        unsigned long value = 0;
        istringstream iss(size);
        iss >> value;

        // First test if the stream is OK, then look to see if we read all
        // of the chars.
        if (!iss || !iss.eof())
            throw Error("Invalid value '" + size + "' passed to D4Dimension::set_size.");
        set_size(value);
    }
}

void
D4Dimension::print_dap4(XMLWriter &xml) const
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"Dimension") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Dimension element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    if (d_varying) {
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "size", (const xmlChar*)"*") < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for size (varying)");
    }
    else {
        ostringstream oss;
        oss << d_size;
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "size", (const xmlChar*)oss.str().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for size");
    }

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end Dimension element");
}

void
D4Dimensions::print_dap4(XMLWriter &xml) const
{
    D4DimensionsCIter i = d_dims.begin();
    while (i != d_dims.end()) {
        (*i++)->print_dap4(xml);
    }
}

} /* namespace libdap */
