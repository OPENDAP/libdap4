// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2026 OPeNDAP, Inc.
// Author: NAthan Potter <ndp@opendap.org>
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
//
// cerealization_patch.h
//

#ifndef CEREALIZATION_PATCH_H
#define CEREALIZATION_PATCH_H

#define CEREALIZATION_PATCH_ATTR_NAME "hyrax_dap"
#define CEREALIZATION_PATCH_ATTR_VALUE "4.0"

#include "InternalErr.h"
#include "XMLWriter.h"
namespace libdap {

/**
 *
 * Adds the serialization patch attribute to the current element.
 * @param xml
 * @param dap4_namespace_name
 */
static void add_serialization_patch_attribute(libdap::XMLWriter &xml, const string &) {

    /*
    TODO - If we don't need a namespace prefix we can drop this bit.
    if (!dap4_namespace_name.empty()) {
        // Oddly, the dap namespace prefix is not defined by default. We have to add it to
        // namespace prefix our special attribute.
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)"xmlns:dap",
                                        (const xmlChar *)dap4_namespace_name.c_str()) < 0)
            throw libdap::InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:dap");
    }
    */

    // TODO - Is this really just a hyrax thing? Rethink this attribute name!
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)CEREALIZATION_PATCH_ATTR_NAME,
                                    (const xmlChar *)CEREALIZATION_PATCH_ATTR_VALUE) < 0)
        throw libdap::InternalErr(__FILE__, __LINE__, "Could not write attribute for " CEREALIZATION_PATCH_ATTR_NAME);
}

} // namespace libdap

#endif // CEREALIZATION_PATCH_H
