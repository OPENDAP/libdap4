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
#include "DMR.h"
#include "D4BaseTypeFactory.h"

#include "debug.h"

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
	// This is needed because we use the factory to make a new instance of the rot group
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

    // TODO Deep copy, using ptr_duplicate() instead?
    d_root = static_cast<D4Group*>(dmr.d_factory->NewVariable(dods_group_c, dmr.d_root->name()));
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
          d_dmr_version("1.0"), d_request_xml_base(""),
          d_namespace(""), d_max_response_size(0), d_root(0)
{
    // sets d_dap_version string and the two integer fields too
    set_dap_version("4.0");
}

/**
 * Make a DMR which uses the given BaseTypeFactory to create variables.
 *
 * @note The default DAP version is 4.0 - use the DDS class to make DAP2
 * things. The default DMR version is 1.0
 */
DMR::DMR()
        : d_factory(0), d_name(""), d_filename(""),
          d_dmr_version("1.0"), d_request_xml_base(""),
          d_namespace(""), d_max_response_size(0), d_root(0)
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
    delete d_root;
}

DMR &
DMR::operator=(const DMR &rhs)
{
    if (this == &rhs)
        return *this;

    m_duplicate(rhs);

    return *this;
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
