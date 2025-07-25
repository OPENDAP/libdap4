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

#ifdef WIN32
#include <fstream>
#include <io.h>
#include <process.h>
#endif

#include <iostream>
#include <memory>
#include <sstream>

#include "Array.h"
#include "BaseType.h"
#include "D4Attributes.h"
#include "D4BaseTypeFactory.h"
#include "D4Group.h"
#include "DMR.h"
#include "Grid.h"
#include "XMLWriter.h"

#include "DDS.h" // Included so DMRs can be built using a DDS for 'legacy' handlers

#include "DapIndent.h"
#include "debug.h"

using namespace std;

namespace libdap {

/**
 * @brief Copy the contents of the given DMR into this one.
 * This is defined because the we perform a deep copy of the root group object.
 * @param dmr
 */
void DMR::m_duplicate(const DMR &dmr) {
    d_factory = dmr.d_factory; // Shallow copy here

    d_name = dmr.d_name;
    d_filename = dmr.d_filename;

    d_dap_major = dmr.d_dap_major;
    d_dap_minor = dmr.d_dap_minor;
    d_dap_version = dmr.d_dap_version; // String version of the protocol

    d_dmr_version = dmr.d_dmr_version;

    d_request_xml_base = dmr.d_request_xml_base;

    d_namespace = dmr.d_namespace;

    d_max_response_size_kb = dmr.d_max_response_size_kb;

    d_ce_empty = dmr.d_ce_empty;

    d_use_dap4_checksums = dmr.d_use_dap4_checksums;

    // Deep copy, using ptr_duplicate()
    // d_root can only be a D4Group, so the thing returned by ptr_duplicate() must be a D4Group.
    d_root = static_cast<D4Group *>(dmr.d_root->ptr_duplicate());
    DBG(cerr << "dmr.d_root: " << dmr.d_root << endl);
    DBG(cerr << "d_root (from ptr_dup(): " << d_root << endl);
}

/**
 * Make a DMR which uses the given BaseTypeFactory to create variables.
 *
 * @note The default DAP version is 4.0 - use the DDS class to make DAP2
 * things. The default DMR version is 1.0
 *
 * @param factory The D4BaseTypeFactory to use when creating instances of
 * DAP4 variables. The caller must ensure the factory's lifetime is at least
 * that of the DMR instance.
 * @param name The name of the DMR - usually derived from the name of the
 * pathname or table name of the dataset.
 */
DMR::DMR(D4BaseTypeFactory *factory, const string &name) : d_factory(factory), d_name(name) {}

/** @brief Build a DMR using a DAP2 DDS.
 *
 * Given a DDS from code written for DAP2, build a DAP4 DMR object. This
 * works because DAP4 subsumes DAP2, but there are a few quirks... For
 * each variable in the DDS, transform it to the equivalent DAP4 variable
 * type and then copy the variable's attributes. Most types convert easily.
 * Types that need special treatment are:
 * Array: DAP2 array dimensions must be morphed to DAP4
 * Sequence: Make a D4Sequence
 * Grid: Make a coverage; assume Grids with the same dimension names
 * have 'shared dimensions' and that maps with the same names are shared too.
 *
 * @note Assume that a DDS has only a root group. This is not actually
 * true for a DDS from the HDF5 handler, because it has Groups encoded
 * into the variable names. jhrg 3/18/14
 *
 * @param factory Factory class used to make new variables
 * @param dds Get the variables to convert from this DAP2 DDS.
 * @see BaseType::transform_to_dap4()
 */
DMR::DMR(D4BaseTypeFactory *factory, DDS &dds)
    : d_factory(factory), d_name(dds.get_dataset_name()), d_filename(dds.filename()) {
    build_using_dds(dds);
}

/** The DMR copy constructor. */
DMR::DMR(const DMR &rhs) : DapObj() { m_duplicate(rhs); }

/** Delete a DMR. The BaseType factory is not freed, while the contained
 * group is.
 */
DMR::~DMR() { delete d_root; }

DMR &DMR::operator=(const DMR &rhs) {
    if (this == &rhs)
        return *this;

    m_duplicate(rhs);

    return *this;
}

/**
 * If we have a DDS that includes Attributes, use it to build the DMR. This
 * will copy all of the variables in the DDS into the DMR using BaseType::transform_to_dap4(),
 * so the actual types added can be controlled by code that specializes
 * the various type classes.
 *
 * @param dds Read variables and Attributes from this DDS
 */
void DMR::build_using_dds(DDS &dds) {
    set_name(dds.get_dataset_name());
    set_filename(dds.filename());

    D4Group *root_grp = root();
    for (DDS::Vars_iter i = dds.var_begin(), e = dds.var_end(); i != e; ++i) {
        BaseType *d4_var = root()->var((*i)->name());
        // Don't add duplicate variables. We have to make this check
        // because some of the child variables may add arrays
        // to the root object. For example, this happens in
        // Grid with the Map Arrays - ndp - 05/08/17
        if (!d4_var) {
            // no variable of this name is in the root group at this point. Add it.
            DBG(cerr << __func__ << "() - Transforming top level variable: " << " (" << (*i)->type_name() << ":'"
                     << (*i)->name() << "':" << (void *)(*i) << ") (root:" << root_grp << ")" << endl;);
            (*i)->transform_to_dap4(root_grp, root_grp);
            DBG(cerr << __func__ << "() - top level variable: '" << (*i)->name() << "' (type:" << (*i)->type_name()
                     << ") Transformed" << endl;);
        } else {
            DBG(cerr << __func__ << "() - Skipping variable: " << d4_var->type_name() << " " << d4_var->name()
                     << " because a variable with" << " this name already exists in the root group." << endl;);
        }
    }

    // Now copy the global attributes
    root()->attributes()->transform_to_dap4(dds.get_attr_table());
}

/**
 * @brief Build a DDS from a DMR
 *
 * Build a DDS from a DMR, collapsing DAP4 Groups, transforming Arrays to Grids
 * as needed, and moving the attributes around to match the new variables. All
 * of the variables in the returned DDS object are newly allocated with separate
 * lifetimes from the objects in the DMR. They are made using ptr_duplicate()
 * so that all of the variables mirror specializations for the various types.
 * That is, if the HDF5 handler built the DMR, then the resulting DDS will hold
 * instances of H5Int32, etc.
 *
 * @note The caller is responsible for deleting the resulting DDS object.
 *
 * @return A pointer to the newly allocated DDS.
 */
DDS *DMR::getDDS() {
    DBG(cerr << __func__ << "() - BEGIN" << endl);

    BaseTypeFactory btf;
    DDS *dds = new DDS(&btf, name());
    dds->filename(filename());

    // Now copy the global attributes
    // TODO Make this a unique_ptr<> and let the compiler delete it. jhrg 6/17/19
    // change made jhrg 2/4/22
#if 1
    unique_ptr<vector<BaseType *>> top_vars(root()->transform_to_dap2(&(dds->get_attr_table()) /*, true*/));
    for (vector<BaseType *>::iterator i = top_vars->begin(), e = top_vars->end(); i != e; i++) {
        dds->add_var_nocopy(*i);
    }
#else
    vector<BaseType *> *top_vars = root()->transform_to_dap2(&(dds->get_attr_table()) /*, true*/);
    for (vector<BaseType *>::iterator i = top_vars->begin(), e = top_vars->end(); i != e; i++) {
        dds->add_var_nocopy(*i);
    }
    delete top_vars;
#endif
    DBG(cerr << __func__ << "() - END" << endl);

    dds->set_factory(0);
    return dds;
}

/**
 * Get the root group for this DMR. This accessor allocates the root group
 * if one does not exist using the factory class bound to this DMR
 *
 * @return A pointer to the root group.
 */
D4Group *DMR::root() {
    if (!d_root)
        d_root = static_cast<D4Group *>(d_factory->NewVariable(dods_group_c, "/"));
    return d_root;
}

/**
 * Given the DAP protocol version, parse that string and set the DMR fields.
 *
 * @param v The version string.
 */
void DMR::set_dap_version(const string &v) {
    istringstream iss(v);

    int major = -1, minor = -1;
    char dot;
    if (!iss.eof() && !iss.fail())
        iss >> major;
    if (!iss.eof() && !iss.fail())
        iss >> dot;
    if (!iss.eof() && !iss.fail())
        iss >> minor;

    if (major == -1 || minor == -1 or dot != '.')
        throw InternalErr(__FILE__, __LINE__, "Could not parse dap version. Value given: " + v);

    d_dap_version = v;

    d_dap_major = major;
    d_dap_minor = minor;

    // Now set the related XML constants. These might be overwritten if
    // the DMR instance is being built from a document parse, but if it's
    // being constructed by a server the code to generate the XML document
    // needs these values to match the DAP version information.
    switch (d_dap_major) {
    case 4:
        d_namespace = c_dap40_namespace;
        break;
    default:
        d_namespace = "";
        break;
    }
}

/** Get the size of a response, in kilobytes. This method looks at the
 * variables in the DMR a computes the number of bytes in the response.
 *
 * @note This version of the method does a poor job with Arrays that
 * have varying dimensions.
 *
 * @param constrained Should the size of the whole DMR be used or should the
 * current constraint be taken into account?
 * @return The size of the request in kilobytes
 * @deprecated "Use DMR::request_size_kb()"
 */
//[[deprecated("Use DMR::request_size_kb()")]]
long DMR::request_size(bool constrained) { return d_root->request_size(constrained); }

/** Get the size of a response, in kilobytes. This method looks at the
 * variables in the DMR a computes the number of bytes in the response.
 *
 * @note This version of the method does a poor job with Arrays that
 * have varying dimensions.
 *
 * @param constrained Should the size of the whole DMR be used or should the
 * current constraint be taken into account?
 * @return The size of the request in kilobytes
 */
uint64_t DMR::request_size_kb(bool constrained) { return d_root->request_size_kb(constrained); }

/**
 * Print the DAP4 DMR object.
 *
 * @param xml use this XMLWriter to build the XML.
 * @param constrained Should the DMR be subject to a constraint? Defaults to
 * False
 */
void DMR::print_dap4(XMLWriter &xml, bool constrained) {
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar *)"Dataset") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Dataset element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)"xmlns",
                                    (const xmlChar *)get_namespace().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns");

    if (!request_xml_base().empty()) {
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)"xml:base",
                                        (const xmlChar *)request_xml_base().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xml:base");
    }

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)"dapVersion",
                                    (const xmlChar *)dap_version().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)"dmrVersion",
                                    (const xmlChar *)dmr_version().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)"name", (const xmlChar *)name().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    root()->print_dap4(xml, constrained);

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end the top-level Group element");
}

/**
 * @brief Scans the inventory of projected variables and their attributes for projected DAP4 types.
 * As it locates projected DAP4 types it adds a description of each to the vector inventory
 * @param inventory
 * @return true when dap4 types have been requested, false otherwise.
 */
bool DMR::is_dap4_projected(std::vector<string> &inventory) {
    bool has_dap4 = root()->attributes()->has_dap4_types(root()->FQN(), inventory);

    for (const auto var : root()->variables()) {
        has_dap4 |= var->is_dap4_projected(inventory);
    }
    for (const auto grp : root()->groups()) {
        has_dap4 |= grp->is_dap4_projected(inventory);
    }
    return has_dap4;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void DMR::dump(ostream &strm) const {
    strm << DapIndent::LMarg << "DMR::dump - (" << (void *)this << ")" << endl;
    DapIndent::Indent();
    strm << DapIndent::LMarg << "factory: " << (void *)d_factory << endl;
    strm << DapIndent::LMarg << "name: " << d_name << endl;
    strm << DapIndent::LMarg << "filename: " << d_filename << endl;
    strm << DapIndent::LMarg << "protocol major: " << d_dap_major << endl;
    strm << DapIndent::LMarg << "protocol minor: " << d_dap_minor << endl;

    DapIndent::UnIndent();
}

} // namespace libdap
