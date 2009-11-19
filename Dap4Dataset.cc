// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2009 OPeNDAP, Inc.
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

#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>

//#define DODS_DEBUG
//#define DODS_DEBUG2

#include "Dap4Dataset.h"

#include "Error.h"
#include "InternalErr.h"
#include "escaping.h"

#include "debug.h"

using namespace std;

namespace libdap {

void Dap4Dataset::m_clone(const Dap4Dataset &rhs)
{
    //d_default_group = new Dap4Group(*d_default_group);
    d_no_default_group = rhs.d_no_default_group;
}

/** Make a Dap4Dataset which uses the given BaseTypeFactory to create variables.
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method.
    @param factory BaseTypeFactory which instantiates Byte, ..., Grid. The
    caller is responsible for freeing the object \e after deleting this Dap4Dataset.
    Can also be set using set_factory(). Never delete until just before
    deleting the Dap4Dataset itself unless you intend to replace the factory with a
    new instance.
    @param n The name of the data set. Can also be set using
    set_dataset_name(). */
Dap4Dataset::Dap4Dataset(BaseTypeFactory *factory, const string &name)
    : DDS(factory, name), d_no_default_group(false)//, d_default_group(0)
{
    //d_default_group = new Dap4Group();

    set_dap_major(4);
    set_dap_minor(0);
}

/** The Dap4Dataset copy constructor. */
Dap4Dataset::Dap4Dataset(const Dap4Dataset &rhs) : DDS(rhs)
{
    m_clone(rhs);
}

Dap4Dataset::~Dap4Dataset()
{
    //delete d_default_group;
    //d_default_group = 0;
}

Dap4Dataset &Dap4Dataset::operator=(const Dap4Dataset &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<DDS &>(*this) = rhs; // run parent (DDS) assignment operator
    m_clone(rhs);

    return *this;
}

/** @brief Adds a copy of the Group to the Dap4Dataset.
    Using the ptr_duplicate() method, perform a deep copy on the Group
    referenced by \e btp and adds the result to this Dap4Dataset.
    @note The copy will not copy data values.
    @param btp Source variable.
    @exception InternalErr if the pointer is null
    @exception Error if the BaseType is not a Group */
void Dap4Dataset::add_var(BaseType *btp)
{
    if (!btp)
        throw InternalErr(__FILE__, __LINE__,
                          "Trying to add a BaseType object with a NULL pointer.");
    if (btp->type() == dap4_group_c)
	d_no_default_group = true;

    if (d_no_default_group && btp->type() != dap4_group_c)
	throw Error("Only Groups can be added to a DAP4 Dataset");

    DDS::add_var(btp);
}

/** @brief Print a constrained Dap4Dataset to the specified ostream.

    Print those parts (variables) of the Dap4Dataset structure to OS that
    are marked to be sent after evaluating the constraint
    expression.

    \note This function only works for scalars at the top level.

    @returns true.
*/

class VariablePrintXMLStrm : public unary_function<BaseType *, void>
{
    ostream &d_out;
    bool d_constrained;
public:
    VariablePrintXMLStrm(ostream &out, bool constrained)
            : d_out(out), d_constrained(constrained)
    {}
    void operator()(BaseType *bt)
    {
        bt->print_xml(d_out, "    ", d_constrained);
    }
};

/** Print an XML representation of this Dap4Dataset. This method is used to generate
    the part of the DDX response. The \c Dataset tag is \e not written by
    this code. The caller of this method must handle writing that and
    including the \c dataBLOB tag.

    @param out Destination ostream.
    @param constrained True if the output should be limited to just those
    variables that are in the projection of the current constraint
    expression.
    @param blob The dataBLOB href. */
void
Dap4Dataset::print_xml(ostream &out, bool constrained, const string &blob)
{
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    out << "<Dataset name=\"" << id2xml(get_dataset_name()) << "\"\n";

    out << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";

    out << "xsi:schemaLocation=\"" << c_dap_namespace << "  "
	    << c_default_dap_schema_location << "\"\n";

    out << "xmlns:grddl=\"http://www.w3.org/2003/g/data-view#\"\n";
    out << "grddl:transformation=\"" << c_grddl_transformation_dap32 << "\"\n";

    out << "xmlns=\"" << c_dap_namespace << "\"\n";
    out << "xmlns:dap=\"" << c_dap_namespace << "\"\n";

    out << "dapVersion=\"" << get_dap_major() << "." << get_dap_minor() << "\"";

    if (!get_request_xml_base().empty()) {
	out << "\n";
	out << "xmlns:xml=\"" << c_xml_namespace << "\"\n";
	out << "xml:base=\"" << get_request_xml_base() << "\"";
    }

    // Close the Dataset element
    out << ">\n";

    get_attr_table().print_xml(out, "    ", constrained);

    out << "\n";


    for_each(var_begin(), var_end(), VariablePrintXMLStrm(out, constrained));

    out << "\n";

    // Only print this for the 2.0, 3.0 and 3.1 versions - which are essentially
    // the same.
    // For DAP 3.2 and greater, use the new syntax and value. The 'blob' is
    // actually the CID of the MIME part that holds the data.
    if (get_dap_major() == 2 && get_dap_minor() == 0) {
	out << "    <dataBLOB href=\"\"/>\n";
    }
    else if (!blob.empty() && (get_dap_major() == 3 && get_dap_minor() >= 2)
	    || get_dap_major() >= 4) {
	out << "    <blob href=\"cid:" << blob << "\"/>\n";
    }

    out << "</Dataset>\n";
}

bool Dap4Dataset::m_groups_only() {
    for (Vars_iter i = var_begin(); i != var_end(); i++)
        if (!(*i)->type() == dap4_group_c)
            return false;
    return true;
}

/** @brief Check the semantics of each of the variables represented in the
    Dap4Dataset.

    Check the semantics of the Dap4Dataset describing a complete dataset. If ALL is
    true, check not only the semantics of THIS->TABLE, but also recursively
    all ctor types in the THIS->TABLE. By default, ALL is false since parsing
    a Dap4Dataset input file runs semantic checks on all variables (but not the
    dataset itself.

    @return TRUE if the conventions for the Dap4Dataset are not violated, FALSE
    otherwise.
    @param all If true, recursively check the individual members of
    compound variables.
    @see BaseType::check_semantics */
bool
Dap4Dataset::check_semantics(bool all)
{
    if (DDS::check_semantics(all) && m_groups_only())
	return true;

    return false;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
Dap4Dataset::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Dap4Dataset::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;

    DDS::dump(strm);

    DapIndent::UnIndent() ;
}

} // namespace libdap
