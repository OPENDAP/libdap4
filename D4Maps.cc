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

#include "config.h"

#include "XMLWriter.h"
#include "InternalErr.h"
#include "D4Group.h"
#include "Array.h"
#include "D4Dimensions.h"
#include "D4Maps.h"

using namespace libdap;

Array* D4Map::array(D4Group *root)
{
    if (d_array)
        return d_array;
    else {
        d_array = dynamic_cast<Array*>(root->find_var(d_array_path));
        if (!d_array)
            throw InternalErr(__FILE__, __LINE__,
                              std::string("Failed to find an array at: ").append(d_array_path));
        return d_array;
    }
}

void
D4Map::print_dap4(XMLWriter &xml)
{
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Map") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write Map element");

	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name",
                                    (const xmlChar*)(d_array ? d_array->FQN().c_str(): d_name.c_str())) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end Map element");

}

D4Maps&
D4Maps::operator=(const D4Maps &rhs)
{
	if (this == &rhs) return *this;
	m_duplicate(rhs);
	return *this;
}
