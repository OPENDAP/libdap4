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

#include "D4Attributes.h"
#include "D4AttributeType.h"
#include "InternalErr.h"

#include "AttrTable.h"

#include "util.h"
#include "debug.h"

namespace libdap {

/** Convert an AttrType to it's string representation.
 @param at The Attribute Type.
 @return The type's string representation */
string D4AttributeTypeToString(D4AttributeType at)
{
    switch(at) {
        case attr_null_c:
            return "null";

        case attr_byte_c:
            return "Byte";

        case attr_int16_c:
            return "Int16";

        case attr_uint16_c:
            return "UInt16";

        case attr_int32_c:
            return "Int32";

        case attr_uint32_c:
            return "UInt32";

        case attr_float32_c:
            return "Float32";

        case attr_float64_c:
            return "Float64";

        case attr_str_c:
            return "String";

        case attr_url_c:
            return "Url";

        // Added for DAP4
        case attr_int8_c:
            return "Int8";

        case attr_uint8_c:
            return "UInt8";

        case attr_int64_c:
            return "Int64";

        case attr_uint64_c:
            return "UInt64";

        case attr_enum_c:
            return "Enum";

        case attr_opaque_c:
            return "Opaque";

        // These are specific to attributes while the other types are
        // also supported by the variables. jhrg 4/17/13
        case attr_container_c:
            return "Container";

        case attr_otherxml_c:
            return "OtherXML";

        default:
            throw InternalErr(__FILE__, __LINE__, "Unsupported attribute type");
    }
}

D4AttributeType StringToD4AttributeType(string s)
{
    downcase(s);

    if (s == "container")
        return attr_container_c;

    else if (s == "byte")
        return attr_byte_c;
    else if (s == "int8")
        return attr_int8_c;
    else if (s == "uint8")
        return attr_uint8_c;
    else if (s == "int16")
        return attr_int16_c;
    else if (s == "uint16")
        return attr_uint16_c;
    else if (s == "int32")
        return attr_int32_c;
    else if (s == "uint32")
        return attr_uint32_c;
    else if (s == "int64")
        return attr_int64_c;
    else if (s == "uint64")
        return attr_uint64_c;

    else if (s == "float32")
        return attr_float32_c;
    else if (s == "float64")
        return attr_float64_c;

    else if (s == "string")
        return attr_str_c;
    else if (s == "url")
        return attr_url_c;
    else if (s == "otherxml")
        return attr_otherxml_c;
    else
        return attr_null_c;
}

void
D4Attribute::m_duplicate(const D4Attribute &src)
{
    d_name = src.d_name;
    d_type = src.d_type;
    d_values = src.d_values;
    if (src.d_attributes)
        d_attributes = new D4Attributes(*src.d_attributes);
    else
        d_attributes = 0;
}

D4Attribute::D4Attribute(const D4Attribute &src)
{
    m_duplicate(src);
}

D4Attribute::~D4Attribute()
{
    delete d_attributes;
}

D4Attribute &
D4Attribute::operator=(const D4Attribute &rhs)
{
    if (this == &rhs) return *this;
    m_duplicate(rhs);
    return *this;
}

D4Attributes *
D4Attribute::attributes()
{
    if (!d_attributes) d_attributes = new D4Attributes();
    return d_attributes;
}

/** @brief copy attributes from DAP2 to DAP4
 *
 * Given a DAP2 AttrTable, copy all of its attributes into a DAP4 D4Attributes
 * object.
 *
 * @param at Read the DAP2 attributes from here.
 */
void
D4Attributes::transform_to_dap4(AttrTable &at)
{
	// for every attribute in at, copy it to this.
	for (AttrTable::Attr_iter i = at.attr_begin(), e = at.attr_end(); i != e; ++i) {
		string name = at.get_name(i);
		AttrType type = at.get_attr_type(i);

		switch (type) {
		case Attr_container: {
			D4Attribute *a = new D4Attribute(name, attr_container_c);
			D4Attributes *attributes = a->attributes(); // allocates a new object
			attributes->transform_to_dap4(*at.get_attr_table(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_byte: {
			D4Attribute *a = new D4Attribute(name, attr_byte_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_int16: {
			D4Attribute *a = new D4Attribute(name, attr_int16_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_uint16: {
			D4Attribute *a = new D4Attribute(name, attr_uint16_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_int32: {
			D4Attribute *a = new D4Attribute(name, attr_int32_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_uint32: {
			D4Attribute *a = new D4Attribute(name, attr_uint32_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_float32: {
			D4Attribute *a = new D4Attribute(name, attr_byte_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_float64: {
			D4Attribute *a = new D4Attribute(name, attr_float32_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_string: {
			D4Attribute *a = new D4Attribute(name, attr_str_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_url: {
			D4Attribute *a = new D4Attribute(name, attr_url_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		case Attr_other_xml: {
			D4Attribute *a = new D4Attribute(name, attr_otherxml_c);
			a->add_value_vector(*at.get_attr_vector(i));
			add_attribute_nocopy(a);
			break;
		}
		default:
			throw InternalErr(__FILE__, __LINE__, "Unknown DAP2 attribute type in D4Attributes::copy_from_dap2()");
		}
	}
}

D4Attribute *
D4Attributes::find_depth_first(const string &name, D4AttributesIter i)
{
    if (i == attribute_end())
        return 0;
    else if ((*i)->name() == name)
        return *i;
    else if ((*i)->type() == attr_container_c)
        return find_depth_first(name, (*i)->attributes()->attribute_begin());
    else
        return find_depth_first(name, ++i);
}

D4Attribute *
D4Attributes::find(const string &name)
{
    return find_depth_first(name, attribute_begin());
}

/** Return a pointer to the D4Attribute object that has the given FQN.
 * @note A FQN for an attribute is a series of names separated by dots.
 */
D4Attribute *
D4Attributes::get(const string &fqn)
{
    // name1.name2.name3
    // name1
    // name1.name2
    size_t pos = fqn.find('.');
    string part = fqn.substr(0, pos);
    string rest= "";

    if (pos != string::npos)
        rest = fqn.substr(pos + 1);

    DBG(cerr << "part: '" << part << "'; rest: '" << rest << "'" << endl);

    if (!part.empty()) {
        if (!rest.empty()) {
            D4AttributesIter i = attribute_begin();
            while (i != attribute_end()) {
                if ((*i)->name() == part && (*i)->type() == attr_container_c)
                    return (*i)->attributes()->get(rest);
                ++i;
            }
        }
        else {
            D4AttributesIter i = attribute_begin();
            while (i != attribute_end()) {
                if ((*i)->name() == part)
                    return (*i);
                ++i;
            }
        }
    }

    return 0;
}

void
D4Attribute::print_dap4(XMLWriter &xml) const
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Attribute") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Attribute element");
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "type", (const xmlChar*) D4AttributeTypeToString(type()).c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for type");

    switch (type()) {
        case attr_container_c:
            if (!d_attributes)
                throw InternalErr(__FILE__, __LINE__, "Null Attribute container");
            d_attributes->print_dap4(xml);
            break;

        case attr_otherxml_c:
            if (num_values() != 1)
                throw Error("OtherXML attributes cannot be vector-valued.");
            if (xmlTextWriterWriteRaw(xml.get_writer(), (const xmlChar*) value(0).c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write OtherXML value");
            break;

        default: {
            // Assume only valid types make it into instances
            D4AttributeCIter i = d_values.begin();//value_begin();
            while (i != d_values.end()) {
                if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Value") < 0)
                    throw InternalErr(__FILE__, __LINE__, "Could not write value element");

                if (xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) (*i++).c_str()) < 0)
                    throw InternalErr(__FILE__, __LINE__, "Could not write attribute value");

                if (xmlTextWriterEndElement(xml.get_writer()) < 0)
                    throw InternalErr(__FILE__, __LINE__, "Could not end value element");
            }

            break;
        }
    }

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end Attribute element");
}

void
D4Attributes::print_dap4(XMLWriter &xml) const
{
    if (empty())
        return;

    D4AttributesCIter i = d_attrs.begin();
    while (i != d_attrs.end()) {
        (*i++)->print_dap4(xml);
    }
}

} // namespace libdap

