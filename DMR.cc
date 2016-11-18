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
#include <io.h>
#include <process.h>
#include <fstream>
#else
#include <unistd.h>    // for alarm and dup
#include <sys/wait.h>
#endif

#include <cassert>

#include <iostream>
#include <sstream>

//#define DODS_DEBUG
//#define DODS_DEBUG2

#include "D4Group.h"
#include "BaseType.h"
#include "Array.h"
#include "DMR.h"
#include "XMLWriter.h"
#include "D4BaseTypeFactory.h"
#include "D4Attributes.h"

#include "DDS.h"	// Included so DMRs can be built using a DDS for 'legacy' handlers

#include "debug.h"

/**
 * DapXmlNamespaces
 *
 * TODO  Replace all uses of the following variables with calls to DapXmlNamespaces
 */
const string c_xml_xsi = "http://www.w3.org/2001/XMLSchema-instance";
const string c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

const string c_default_dap40_schema_location = "http://xml.opendap.org/dap/dap4.0.xsd";

const string c_dap40_namespace = "http://xml.opendap.org/ns/DAP/4.0#";

const string c_dap_40_n_sl = c_dap40_namespace + " " + c_default_dap40_schema_location;

using namespace std;

namespace libdap {

void
DMR::m_duplicate(const DMR &dmr)
{
    // This is needed because we use the factory to make a new instance of the root group
    assert(dmr.OK());

    d_factory = dmr.d_factory; // Shallow copy here

    d_name = dmr.d_name;
    d_filename = dmr.d_filename;

    d_dap_major = dmr.d_dap_major;
    d_dap_minor = dmr.d_dap_minor;
    d_dap_version = dmr.d_dap_version;       // String version of the protocol

    d_dmr_version = dmr.d_dmr_version;

    d_request_xml_base = dmr.d_request_xml_base;

    d_namespace = dmr.d_namespace;

    d_max_response_size = dmr.d_max_response_size;

    // Deep copy, using ptr_duplicate()
    // d_root can only be a D4Group, so the thing returned by ptr_duplicate() must be a D4Group.
    d_root = static_cast<D4Group*>(dmr.d_root->ptr_duplicate());
    DBG(cerr << "dmr.d_root: " << dmr.d_root << endl);
    DBG(cerr << "d_root (from ptr_dup(): " << d_root << endl);

    //d_root = static_cast<D4Group*>(dmr.d_factory->NewVariable(dods_group_c, dmr.d_root->name()));
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
DMR::DMR(D4BaseTypeFactory *factory, const string &name)
        : d_factory(factory), d_name(name), d_filename(""),
          d_dap_major(4), d_dap_minor(0),
          d_dmr_version("1.0"), d_request_xml_base(""),
          d_namespace(c_dap40_namespace), d_max_response_size(0), d_root(0)
{
    // sets d_dap_version string and the two integer fields too
    set_dap_version("4.0");
}

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
        : d_factory(factory), d_name(dds.get_dataset_name()),
          d_filename(dds.filename()), d_dap_major(4), d_dap_minor(0),
          d_dmr_version("1.0"), d_request_xml_base(""),
          d_namespace(c_dap40_namespace), d_max_response_size(0), d_root(0)
{
    // sets d_dap_version string and the two integer fields too
    set_dap_version("4.0");

    build_using_dds(dds);
#if 0
    for (DDS::Vars_iter i = dds.var_begin(), e = dds.var_end(); i != e; ++i) {
    	BaseType *new_var = (*i)->transform_to_dap4(root() /*group*/, root() /*container*/);
    	// If the variable being transformed is a Grid,
    	// then Grid::transform_to_dap4() will add all the arrays to the
    	// container (root() in this case) and return null, indicating that
    	// this code does not need to do anything to add the transformed variable.
    	if (new_var)
    		root()->add_var_nocopy(new_var);
    }

    // Now copy the global attributes
    root()->attributes()->transform_to_dap4(dds.get_attr_table());
#endif
}

/**
 * Make a DMR which uses the given BaseTypeFactory to create variables.
 *
 * @note The default DAP version is 4.0 - use the DDS class to make DAP2
 * things. The default DMR version is 1.0
 */
DMR::DMR()
        : d_factory(0), d_name(""), d_filename(""), d_dap_major(4), d_dap_minor(0),
          d_dap_version("4.0"), d_dmr_version("1.0"), d_request_xml_base(""),
          d_namespace(c_dap40_namespace), d_max_response_size(0), d_root(0)
{
    // sets d_dap_version string and the two integer fields too
    set_dap_version("4.0");
}

/** The DMR copy constructor. */
DMR::DMR(const DMR &rhs) : DapObj()
{
    m_duplicate(rhs);
}

/** Delete a DMR. The BaseType factory is not freed, while the contained
 * group is.
 */
DMR::~DMR()
{
#if 1
    delete d_root;
#endif
}

DMR &
DMR::operator=(const DMR &rhs)
{
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
void DMR::build_using_dds(DDS &dds)
{
	set_name(dds.get_dataset_name());
	set_filename(dds.filename());

	for (DDS::Vars_iter i = dds.var_begin(), e = dds.var_end(); i != e; ++i) {
		BaseType *new_var = (*i)->transform_to_dap4(root() /*group*/, root() /*container*/);
		// If the variable being transformed is a Grid,
		// then Grid::transform_to_dap4() will add all the arrays to the
		// container (root() in this case) and return null, indicating that
		// this code does not need to do anything to add the transformed variable.
		if (new_var) root()->add_var_nocopy(new_var);
	}

	// Now copy the global attributes
	root()->attributes()->transform_to_dap4(dds.get_attr_table());
}

D4Group *
DMR::root()
{
	if (!d_root) d_root = static_cast<D4Group*>(d_factory->NewVariable(dods_group_c, "/"));
	return d_root;
}

/**
 * Given the DAP protocol version, parse that string and set the DMR fields.
 *
 * @param v The version string.
 */
void
DMR::set_dap_version(const string &v)
{
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
 */
long
DMR::request_size(bool constrained)
{
    return d_root->request_size(constrained);
}

/**
 * Print the DAP4 DMR object.
 *
 * @param xml use this XMLWriter to build the XML.
 * @param constrained Should the DMR be subject to a constraint? Defaults to
 * False
 */
void
DMR::print_dap4(XMLWriter &xml, bool constrained)
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Dataset") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Dataset element");

#if 0
    // Reintroduce these if they are really useful. jhrg 4/15/13
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xml",
            (const xmlChar*) c_xml_namespace.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xml");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns:xsi", (const xmlChar*) c_xml_xsi.c_str())
            < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:xsi");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xsi:schemaLocation",
            (const xmlChar*) c_dap_40_n_sl.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns:schemaLocation");
#endif

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xmlns", (const xmlChar*) get_namespace().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xmlns");

    if (!request_xml_base().empty()) {
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "xml:base",
                (const xmlChar*)request_xml_base().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for xml:base");
    }

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "dapVersion",  (const xmlChar*)dap_version().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "dmrVersion", (const xmlChar*)dmr_version().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for dapVersion");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    root()->print_dap4(xml, constrained);

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end the top-level Group element");
}


/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
DMR::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "DMR::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    strm << DapIndent::LMarg << "factory: " << (void *)d_factory << endl ;
    strm << DapIndent::LMarg << "name: " << d_name << endl ;
    strm << DapIndent::LMarg << "filename: " << d_filename << endl ;
    strm << DapIndent::LMarg << "protocol major: " << d_dap_major << endl;
    strm << DapIndent::LMarg << "protocol minor: " << d_dap_minor << endl;

    DapIndent::UnIndent() ;
}

} // namespace libdap
