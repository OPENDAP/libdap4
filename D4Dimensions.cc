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

#include <sstream>

#include "XMLWriter.h"
#include "D4Dimensions.h"
#include "D4Group.h"

#include "Error.h"
#include "InternalErr.h"

namespace libdap {

void
D4Dimension::set_size(const string &size)
{
	unsigned long value = 0;
	istringstream iss(size);
	iss >> value;

	// First test if the stream is OK, then look to see if we read all
	// of the chars.
	if (!iss || !iss.eof()) throw Error("Invalid value '" + size + "' passed to D4Dimension::set_size.");
	set_size(value);
}

/**
 * @brief Get the FQN for the dimension
 * @return The D4Dimension as a fully qualified name.
 */
string
D4Dimension::fully_qualified_name() const
{
	string name = d_name;

	// d_parent is the D4Dimensions container and its parent is the Group where
	// this Dimension is defined.
	D4Group *grp = d_parent->parent();
	while (grp) {
		// The root group is named "/" (always); this avoids '//name'
		name = (grp->name() == "/") ? "/" + name : grp->name() + "/" + name;

		grp = static_cast<D4Group*>(grp->get_parent());
	}

	return name;
}

/**
 * @brief Print the Dimension declaration.
 * Print the Dimension in a form suitable for use in a Group definition/declaration.
 * @see print_dap4(XMLWriter &xml, bool print_fqn)
 * @param xml Print to this XMLWriter instance
 */
void
D4Dimension::print_dap4(XMLWriter &xml) const
{
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Dimension") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write Dimension element");

	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
#if 0
	// Use FQNs when things are referenced, not when they are defined
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)fully_qualified_name().c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
#endif
	ostringstream oss;
	if (d_constrained)
	    oss << (d_c_stop - d_c_start) / d_c_stride + 1;
	else
	    oss << d_size;
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "size", (const xmlChar*) oss.str().c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for size");

	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end Dimension element");
}

// Note that in order for this to work the second argument must not be a reference.
// jhrg 8/20/13
static bool
dim_name_eq(D4Dimension *d, const string name)
{
	return d->name() == name;
}

D4Dimension *
D4Dimensions::find_dim(const string &name)
{
	D4DimensionsIter d = find_if(d_dims.begin(), d_dims.end(), bind2nd(ptr_fun(dim_name_eq), name));
	return (d != d_dims.end()) ? *d: 0;
}

void
D4Dimensions::print_dap4(XMLWriter &xml, bool constrained) const
{
    D4DimensionsCIter i = d_dims.begin();
    while (i != d_dims.end()) {
        if (!constrained || parent()->find_first_var_that_uses_dimension(*i))
            (*i)->print_dap4(xml);
        ++i;
    }
}

} /* namespace libdap */
