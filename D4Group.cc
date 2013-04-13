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

#include "D4Group.h"

namespace libdap {

void D4Group::m_duplicate(const D4Group &g)
{
    // dims; deep copy
    d_dims = new D4Dimensions(*g.d_dims);

    // enums; deep copy
    d_enum_defs = new D4EnumDefs(*g.d_enum_defs);

    // groups
    groupsCIter i = g.d_groups.begin();
    while(i != g.d_groups.end()) {
        D4Group *g = (*i++)->ptr_duplicate();
        this->d_groups.push_back(g);
    }

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
    m_duplicate(rhs);
}

D4Group::~D4Group()
{
    //for_each(d_groups.begin(), d_groups.end(), group_delete);

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
D4Group::print_dap4(XMLWriter &xml, bool constrained)
{
    if (!name().empty()) {
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

    // Print the Group body if this method is called on either a named group
    // or the root group.
    // enums
    if (!enum_defs()->empty())
        enum_defs()->print_dap4(xml);

    // dims
    if (!dims()->empty())
        dims()->print_dap4(xml);

    // TODO Note that the order of the parts of a Group are different here
    // than in the rng grammar.

    // FIXME The attributes in DAP4 are broken... Change them to fit the
    // pattern established by D4Dimensions and D4EnumDefs
#if 0
    // attributes
    get_attr_table().print_dap4(xml);
#endif
    // variables
    Constructor::Vars_iter v = var_begin();
    while (v != var_end()) {
        (*v++)->print_dap4(xml, constrained);
        //++v;
    }

    // groups
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->print_dap4(xml, constrained);

    if (!name().empty()) {
        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end " + type_name() + " element");
    }
}

} /* namespace libdap */
