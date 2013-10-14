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

//#define DODS_DEBUG

#include "XMLWriter.h"
#include "D4Attributes.h"
#include "D4Dimensions.h"
#include "D4Group.h"
#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"

#include "debug.h"

#define D4_ATTR 1

namespace libdap {

void D4Group::m_duplicate(const D4Group &g)
{
	DBG(cerr << "In D4Group::m_duplicate for " << g.name() << endl);

    // dims; deep copy, this is the parent
    d_dims = new D4Dimensions(*g.d_dims);
    d_dims->set_parent(this);

    // enums; deep copy
    d_enum_defs = new D4EnumDefs(*g.d_enum_defs);

    // groups
    groupsCIter i = g.d_groups.begin();
    while(i != g.d_groups.end()) {
        D4Group *g = (*i++)->ptr_duplicate();
        add_group_nocopy(g);
        // Using push_back failed to set the group's parent pointer. jhrg 9/3/13
        //this->d_groups.push_back(g);
    }

    DBG(cerr << "Exiting D4Group::m_duplicate" << endl);
}

/** The D4Group constructor requires only the name of the variable
    to be created. The name may be omitted, which will create a
    nameless variable. This may be adequate for some applications.

    @param n A string containing the name of the variable.
*/
D4Group::D4Group(const string &name)
    : Constructor(name, dods_group_c, /*is_dap4*/true), d_dims(0), d_enum_defs(0)
{}

/** The D4Group server-side constructor requires the name of the variable
    to be created and the dataset name from which this variable is being
    created. Used on server-side handlers.

    @param n A string containing the name of the variable.
    @param d A string containing the name of the dataset.
*/
D4Group::D4Group(const string &name, const string &dataset)
    : Constructor(name, dataset, dods_group_c, /*is_dap4*/true), d_dims(0), d_enum_defs(0)
{}

/** The D4Group copy constructor. */
D4Group::D4Group(const D4Group &rhs) : Constructor(rhs), d_dims(0), d_enum_defs(0)
{
	DBG(cerr << "In D4Group::copy_ctor for " << rhs.name() << endl);
    m_duplicate(rhs);
}

D4Group::~D4Group()
{
    delete d_dims;
    delete d_enum_defs;

    groupsIter i = d_groups.begin();
    while(i != d_groups.end())
        delete *i++;
}

D4Group *
D4Group::ptr_duplicate()
{
    return new D4Group(*this);
}

D4Group &
D4Group::operator=(const D4Group &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<Constructor &>(*this) = rhs; // run Constructor=

    m_duplicate(rhs);

    return *this;
}

// Note that in order for this to work the second argument must not be a reference.
// jhrg 8/20/13
static bool
name_eq(D4Group *g, const string name)
{
	return g->name() == name;
}

D4Group *
D4Group::find_child_grp(const string &grp_name)
{
	groupsIter g = find_if(grp_begin(), grp_end(), bind2nd(ptr_fun(name_eq), grp_name));
	return (g != grp_end()) ? *g: 0;
}

/**
 * @brief Find the dimension using a path.
 * Using the DAP4 name syntax, lookup a dimension. The dimension must
 * be defined before it is used. The \c path argument may be either an
 * absolute path or a relative path. Note that the name syntax does not
 * provide for paths to contain an 'up one level' symbol.
 * @param path The path to the dimension
 * @return A pointer to the D4Dimension object.
 */
D4Dimension *
D4Group::find_dim(const string &path)
{
	string lpath = path;		// get a mutable copy

	// special-case for the root group
	if (lpath[0] == '/') {
		if (name() != "/")
			throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
		else
			lpath = lpath.substr(1);
	}

	string::size_type pos = lpath.find('/');
	if (pos == string::npos) {
		// name looks like 'bar'
		return dims()->find_dim(lpath);
	}

	// name looks like foo/bar/baz where foo an bar must be groups
	string grp_name = lpath.substr(0, pos);
	lpath = lpath.substr(pos + 1);

	D4Group *grp = find_child_grp(grp_name);
	return (grp != 0) ? grp->find_dim(lpath): 0;
}

/** Compute the size of all of the variables in this group and it's children,
 * in kilobytes
 *
 * @param constrained Should the current constraint be taken into account?
 * @return The size in kilobytes
 */
long
D4Group::request_size(bool constrained)
{
    long long size = 0;
    // variables
    Constructor::Vars_iter v = var_begin();
    while (v != var_end()) {
        if (constrained) {
            if ((*v)->send_p())
                size += (*v)->width(constrained);
        }
        else {
            size += (*v)->width(constrained);
        }

        ++v;
    }

    // groups
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        size += (*g++)->request_size(constrained);

    return size / 1024;
}

void
D4Group::set_read_p(bool state)
{
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->set_read_p(state);

    Constructor::set_read_p(state);
}

void
D4Group::set_send_p(bool state)
{
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->set_send_p(state);

    Constructor::set_send_p(state);
}

/**
 * @brief Serialize a Group
 * @param m The DAP4 Stream Marshaller. This object serializes the data values and
 * writes checksums (using CRC32) for the top level variables in every Group for which
 * one or more variables are sent. The DAP4 Marshaller object can be made so that only
 * the checksums are written.
 * @param dmr Unused
 * @param eval Unused
 * @param filter Unused
 * @exception Error is thrown if the value needs to be read and that operation fails.
 */
void
D4Group::serialize(D4StreamMarshaller &m, DMR &dmr, ConstraintEvaluator &eval, bool filter)
{
#if 0
    // This will call Constructor read which will, for everything but a Sequence,
    // read all of the data in one shot. However, the serialize() methods for the
    // Arrays, Structures, etc., also have read() calls in them and those can be
    // used to control how long the data are in memory, e.g., limiting the lifetime
    // of a large array and avoiding having overlapping arrays when they are not
    // needed. For a sequence read() has different semantics. It is called once
    // for every instance and the read_p flag is not used.
    if (!read_p())
        read();  // read() throws Error
#endif

    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->serialize(m, dmr, eval, filter);

    // Specialize how the top-level variables in any Group are sent; include
    // a checksum for them. A subset operation might make an interior set of
    // variables, but the parent structure will still be present and the checksum
    // will be computed for that structure. In other words, DAP4 does not try
    // to sort out which variables are the 'real' top-level variables and instead
    // simply computes the CRC for whatever appears as a variable in the root
    // group.
	for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
		// Only send the stuff in the current subset.
		if ((*i)->send_p()) {
			m.reset_checksum();

			(*i)->serialize(m, dmr, eval, filter);

			DBG(cerr << "Wrote CRC32: " << m.get_checksum() << " for " << (*i)->name() << endl);
			m.put_checksum();
		}
	}
}

void D4Group::deserialize(D4StreamUnMarshaller &um, DMR &dmr)
{
	groupsIter g = d_groups.begin();
	while (g != d_groups.end())
		(*g++)->deserialize(um, dmr);

	// Specialize how the top-level variables in any Group are received; read
	// their checksum and store the value in a magic attribute of the variable
	for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
		(*i)->deserialize(um, dmr);

		D4Attribute *a = new D4Attribute("DAP4_Checksum_CRC32", attr_str_c);
		string crc = um.get_checksum_str();
		a->add_value(crc);
		DBG(cerr << "Read CRC32: " << crc << " for " << (*i)->name() << endl);
		(*i)->attributes()->add_attribute_nocopy(a);
	}
}

void
D4Group::print_dap4(XMLWriter &xml, bool constrained)
{
    if (!name().empty() && name() != "/") {
        // For named groups, if constrained is true only print if this group
        // has variables that are marked for transmission. For the root group
        // this test is not made.
        if (constrained && !send_p())
            return;

        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) type_name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write " + type_name() + " element");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*) name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
    }

    // enums
    if (!enum_defs()->empty())
        enum_defs()->print_dap4(xml);

    // dims
    if (!dims()->empty())
        dims()->print_dap4(xml);

    // groups
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->print_dap4(xml, constrained);

    // variables
    Constructor::Vars_iter v = var_begin();
    while (v != var_end())
        (*v++)->print_dap4(xml, constrained);

#if D4_ATTR
    // attributes
    attributes()->print_dap4(xml);
#endif

    if (!name().empty() && name() != "/") {
        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end " + type_name() + " element");
    }
}

} /* namespace libdap */
