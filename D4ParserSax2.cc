// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2012 OPeNDAP, Inc.
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

//#define DODS_DEBUG 1

#include <iostream>
#include <sstream>

#include <cstring>
#include <cstdarg>

#include <libxml/parserInternals.h>

#include "DMR.h"

#include "BaseType.h"
#include "D4Group.h"
#include "D4Attributes.h"

#include "D4BaseTypeFactory.h"

#include "D4ParserSax2.h"

#include "util.h"
#include "debug.h"

namespace libdap {

static const char *states[] = {
        "parser_start",

        "inside_dataset",

        // inside_group is the state just after parsing the start of a Group
        // element.
        "inside_group",

        "inside_attribute_container",
        "inside_attribute",
        "inside_attribute_value",
        "inside_other_xml_attribute",

        "inside_enum_def",
        "inside_enum_const",

        "inside_dim_def",

        // This covers Byte, ..., Url, Opaque
        "inside_simple_type",

        "inside_array",
        "inside_dimension",

        "inside_structure",

        "parser_unknown",
        "parser_error",
        "parser_fatal_error"
};

static bool is_not(const char *name, const char *tag)
{
    return strcmp(name, tag) != 0;
}

/** Dump XML attributes to local store so they can be easily manipulated.
 * XML attribute names are always folded to lower case.
 * @param attributes The XML attribute array
 * @param nb_attributes The number of attributes
 */
void D4ParserSax2::transfer_xml_attrs(const xmlChar **attributes, int nb_attributes)
{
    if (!xml_attrs.empty())
        xml_attrs.clear(); // erase old attributes

    // Make a value using the attribute name and the prefix, namespace URI
    // and the value. The prefix might be null.
    unsigned int index = 0;
    for (int i = 0; i < nb_attributes; ++i, index += 5) {
        xml_attrs.insert(map<string, XMLAttribute>::value_type(string((const char *)attributes[index]),
                                                               XMLAttribute(attributes + index + 1)));

        DBG(cerr << "XML Attribute '" << (const char *)attributes[index] << "': "
                << xml_attrs[(const char *)attributes[index]].value << endl);
    }
}

/** Transfer the XML namespaces to the local store so they can be manipulated
 * more easily.
 *
 * @param namespaces Array of xmlChar*
 * @param nb_namespaces The number of namespaces in the array.
 */
void D4ParserSax2::transfer_xml_ns(const xmlChar **namespaces, int nb_namespaces)
{
    // make a value with the prefix and namespace URI. The prefix might be null.
    for (int i = 0; i < nb_namespaces; ++i) {
        namespace_table.insert(map<string, string>::value_type(namespaces[i * 2] != 0 ? (const char *)namespaces[i * 2] : "",
                                                               (const char *)namespaces[i * 2 + 1]));
    }
}

/** Is a required XML attribute present? Attribute names are always lower case.
 * @note To use this method, first call transfer_xml_attrs.
 * @param attr The XML attribute
 * @return True if the XML attribute was present in the last tag, otherwise
 * it sets the global error state and returns false.
 */
bool D4ParserSax2::check_required_attribute(const string & attr)
{
    if (xml_attrs.find(attr) == xml_attrs.end()) {
        dmr_error(this, "Required attribute '%s' not found.", attr.c_str());
        return false;
    }
    else
        return true;
}

/** Is a XML attribute present? Attribute names are always lower case.
 * @note To use this method, first call transfer_xml_attrs.
 * @param attr The XML attribute
 * @return True if the XML attribute was present in the last/current tag,
 * false otherwise.
 */
bool D4ParserSax2::check_attribute(const string & attr)
{
    return (xml_attrs.find(attr) != xml_attrs.end());
}

bool D4ParserSax2::process_dimension_def(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (is_not(name, "Dimension"))
        return false;

    transfer_xml_attrs(attrs, nb_attributes);

    if (!(check_required_attribute("name") && check_required_attribute("size"))) {
        dmr_error(this, "The required attribute 'name' or 'size' was missing from a Dimension element.");
        return false;
    }

    // This getter (dim_def) allocates a new object if needed.
    dim_def()->set_name(xml_attrs["name"].value);
    try {
        dim_def()->set_size(xml_attrs["size"].value);
    }
    catch (Error &e) {
        dmr_error(this, e.get_error_message().c_str());
        return false;
    }

    return true;
}

bool D4ParserSax2::process_dimension(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (is_not(name, "Dim"))
        return false;

    transfer_xml_attrs(attrs, nb_attributes);

    // Do stuff and return true if it's a valid Dim

    return true;
}

bool D4ParserSax2::process_group(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (is_not(name, "Group"))
        return false;

    transfer_xml_attrs(attrs, nb_attributes);

    if (!check_required_attribute("name")) {
        dmr_error(this, "The required attribute 'name' was missing from a Group element.");
        return false;
    }

    BaseType *btp = dmr()->factory()->NewVariable(dods_group_c, xml_attrs["name"].value);
    if (!btp) {
        dmr_fatal_error(this, "Could not instantiate the variable '%s'.", xml_attrs["name"].value.c_str());
        return false;
    }

    // Need to set this to get teh D4Attribute behavior in the type classes
    // shared between DAP2 and DAP4. jhrg 4/18/13
    btp->set_is_dap4(true);
    push_basetype(btp);

    push_attributes(btp->attributes());
    return true;
}

/** Check to see if the current tag is either an \c Attribute or an \c Alias
 start tag. This method is a glorified macro...

 @param name The start tag name
 @param attrs The tag's XML attributes
 @return True if the tag was an \c Attribute or \c Alias tag */
inline bool D4ParserSax2::process_attribute(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (is_not(name, "Attribute"))
        return false;

    // These methods set the state to parser_error if a problem is found.
    transfer_xml_attrs(attrs, nb_attributes);

    // add error
    if (!(check_required_attribute(string("name")) && check_required_attribute(string("type")))) {
        dmr_error(this, "The required attribute 'name' or 'type' was missing from an Attribute element.");
        return false;
    }

    if (xml_attrs["type"].value == "Container") {
        push_state(inside_attribute_container);

        DBG(cerr << "Pushing attribute container " << xml_attrs["name"].value << endl);
        D4Attribute *child = new D4Attribute(xml_attrs["name"].value, attr_container_c);

        D4Attributes *tos = top_attributes();
        // add return
        if (!tos) {
            dmr_fatal_error(this, "Expected an Attribute container on the top of the attribute stack.");
            return false;
        }

        tos->add_attribute_nocopy(child);
        push_attributes(child->attributes());
    }
    else if (xml_attrs["type"].value == "OtherXML") {
        push_state(inside_other_xml_attribute);

        dods_attr_name = xml_attrs["name"].value;
        dods_attr_type = xml_attrs["type"].value;
    }
    else {
        push_state(inside_attribute);

        dods_attr_name = xml_attrs["name"].value;
        dods_attr_type = xml_attrs["type"].value;
    }

    return true;
}

/** Check to see if the current tag is an \c Enumeration start tag.

 @param name The start tag name
 @param attrs The tag's XML attributes
 @return True if the tag was an \c Enumeration */
inline bool D4ParserSax2::process_enum_def(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (is_not(name, "Enumeration"))
        return false;

    transfer_xml_attrs(attrs, nb_attributes);

    if (!(check_required_attribute("name") && check_required_attribute("basetype"))) {
        dmr_error(this, "The required attribute 'name' or 'basetype' was missing from an Enumeration element.");
        return false;
    }

    Type t = get_type(xml_attrs["basetype"].value.c_str());
    if (!is_integer_type(t)) {
        dmr_error(this, "The Enumeration '%s' must have an integer type, instead the type '%s' was used.",
                xml_attrs["name"].value.c_str(), xml_attrs["basetype"].value.c_str());
        return false;
    }

    // This getter allocates a new object if needed.
    enum_def()->set_name(xml_attrs["name"].value);
    enum_def()->set_type(t);

    return true;
}

inline bool D4ParserSax2::process_enum_const(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (is_not(name, "EnumConst"))
        return false;

    // These methods set the state to parser_error if a problem is found.
    transfer_xml_attrs(attrs, nb_attributes);

    if (!(check_required_attribute("name") && check_required_attribute("value"))) {
        dmr_error(this, "The required attribute 'name' or 'value' was missing from an EnumConst element.");
        return false;
    }

    istringstream iss(xml_attrs["value"].value);
    long long value = 0;
    iss >> skipws >> value;
    if (iss.fail() || iss.bad()) {
        dmr_error(this, "Expected an integer value for an Enumeration constant, got '%s' instead.",
                xml_attrs["value"].value.c_str());
    }
    else if (!enum_def()->is_valid_enum_value(value)) {
        dmr_error(this, "In an Enumeration constant, the value '%s' cannot fit in a variable of type '%s'.",
                xml_attrs["value"].value.c_str(), type_name(d_enum_def->type()).c_str());
    }
    else {
        // unfortunate choice of names... args are 'label' and 'value'
        enum_def()->add_value(xml_attrs["name"].value, value);
    }

    return true;
}

/** Check to see if the current element is the start of a variable declaration.
 If so, process it. A glorified macro...
 @param name The start element name
 @param attrs The element's XML attributes
 @return True if the element was a variable */
inline bool D4ParserSax2::process_variable(const char *name, const xmlChar **attrs, int nb_attributes)
{
    Type t = get_type(name);
    if (is_simple_type(t)) {
        process_variable_helper(t, inside_simple_type, attrs, nb_attributes);
        return true;
    }
    else if (strcmp(name, "Structure") == 0) {
        process_variable_helper(dods_structure_c, inside_structure, attrs, nb_attributes);
        return true;
    }
    else {
        return false;
    }
}

/** Given that a tag which opens a variable declaration has just been read,
 create the variable. Once created, push the variable onto the stack of
 variables, push that variable's attribute table onto the attribute table
 stack and update the state of the parser.
 @param t The type of variable to create.
 @param s The next state of the parser (e.g., inside_simple_type, ...)
 @param attrs the attributes read with the tag */
void D4ParserSax2::process_variable_helper(Type t, ParseState s, const xmlChar **attrs, int nb_attributes)
{
    transfer_xml_attrs(attrs, nb_attributes);

    if (check_required_attribute("name")) {
        BaseType *btp = dmr()->factory()->NewVariable(t, xml_attrs["name"].value);
        if (!btp) {
            dmr_fatal_error(this, "Could not instantiate the variable '%s'.", xml_attrs["name"].value.c_str());
            return;
        }

        btp->set_is_dap4(true); // see comment above
        push_basetype(btp);

        push_attributes(btp->attributes());

        push_state(s);
    }
}

// TODO this is called for array and structure only, but 'array' is about to
// get subsumed by the code that processes simple types, so maybe this can
// be retired?
void D4ParserSax2::finish_variable(const char *tag, Type t, const char *expected)
{
    if (strcmp(tag, expected) != 0) {
        D4ParserSax2::dmr_error(this, "Expected an end tag for a %s; found '%s' instead.", expected, tag);
        return;
    }

    BaseType *btp = top_basetype();
    pop_basetype();
    pop_attributes();

    if (btp->type() != t) {
        D4ParserSax2::dmr_error(this, "Expected a %s variable.", expected);
        delete btp;
        return;
    }

    BaseType *parent = top_basetype();
    if (parent && !parent->is_constructor_type()) {
        D4ParserSax2::dmr_error(this, "Tried to add the variable '%s' to a non-constructor type (%s %s).",
                tag, top_basetype()->type_name().c_str(), top_basetype()->name().c_str());
        delete btp;
        return;
    }

    parent->add_var_nocopy(btp);

    pop_state();
}

/** @name SAX Parser Callbacks

 These methods are declared static in the class header. This gives them C
 linkage which allows them to be used as callbacks by the SAX parser
 engine. */
//@{

/** Initialize the SAX parser state object. This object is passed to each
 callback as a void pointer. The initial state is parser_start.

 @param p The SAX parser  */
void D4ParserSax2::dmr_start_document(void * p)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    parser->error_msg = "";
    parser->char_data = "";

    parser->push_state(parser_start);

    parser->push_attributes(parser->dmr()->root()->attributes());

    if (parser->debug()) cerr << "Parser start state: " << states[parser->get_state()] << endl;
}

/** Clean up after finishing a parse.
 @param p The SAX parser  */
void D4ParserSax2::dmr_end_document(void * p)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    if (parser->debug()) cerr << "Parser end state: " << states[parser->get_state()] << endl;

    if (parser->get_state() != parser_start)
        D4ParserSax2::dmr_error(parser, "The document contained unbalanced tags.");

    // If we've found any sort of error, don't make the DMR; intern() will
    // take care of the error.
    if (parser->get_state() == parser_error || parser->get_state() == parser_fatal_error)
        return;

    // The root group should be on the stack
    if (parser->top_basetype()->type() != dods_group_c) {
        D4ParserSax2::dmr_error(parser, "The document did not contain a valid root Group.");
    }

    parser->pop_basetype();     // leave the stack 'clean'
}

void D4ParserSax2::dmr_start_element(void *p, const xmlChar *l, const xmlChar *prefix, const xmlChar *URI,
        int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int /*nb_defaulted*/,
        const xmlChar **attributes)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    const char *localname = (const char *) l;

    if (parser->debug()) cerr << "Start element " << localname << " (state " << states[parser->get_state()] << ")" << endl;

    switch (parser->get_state()) {
        case parser_start:
            if (is_not(localname, "Dataset"))
                D4ParserSax2::dmr_error(parser, "Expected DMR to start with a Dataset element; found '%s' instead.", localname);

            parser->root_ns = URI ? (const char *) URI : "";
            parser->transfer_xml_attrs(attributes, nb_attributes);

            if (parser->check_required_attribute(string("name")))
                parser->dmr()->set_name(parser->xml_attrs["name"].value);

            if (parser->check_attribute("dapVersion"))
                parser->dmr()->set_dap_version(parser->xml_attrs["dapVersion"].value);

            if (parser->check_attribute("dmrVersion"))
                parser->dmr()->set_dap_version(parser->xml_attrs["dmrVersion"].value);

            if (parser->check_attribute("base"))
                parser->dmr()->set_request_xml_base(parser->xml_attrs["base"].value);

            if (!parser->root_ns.empty())
                parser->dmr()->set_namespace(parser->root_ns);

            // Push the root Group on the stack
            parser->push_basetype(parser->dmr()->root());

            parser->push_state(inside_dataset);

            break;

            // Both inside dataset and inside group can have the same stuff.
            // The difference is that the Dataset holds the root group, which
            // must be present; other groups are optional
        case inside_dataset:
        case inside_group:
            if (parser->process_enum_def(localname, attributes, nb_attributes))
                parser->push_state(inside_enum_def);
            else if (parser->process_dimension_def(localname, attributes, nb_attributes))
                parser->push_state(inside_dim_def);
            else if (parser->process_group(localname, attributes, nb_attributes))
                parser->push_state(inside_group);
            else if (parser->process_variable(localname, attributes, nb_attributes))
                // This will push either inside_simple_type or inside_structure
                // onto the parser state stack.
               break;
            else if (parser->process_attribute(localname, attributes, nb_attributes))
                // This will push either inside_attribute, inside_attribute_container
                // or inside_otherxml_attribute onto the parser state stack
                break;
            else
                D4ParserSax2::dmr_error(parser, "Expected an Attribute, Enumeration, Dimension, Group or variable element; found '%s' instead.", localname);
            break;

        case inside_attribute_container:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::dmr_error(parser, "Expected an Attribute element; found '%s' instead.", localname);
            break;

        case inside_attribute:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "value") == 0)
                parser->push_state(inside_attribute_value);
            else
                dmr_error(parser, "Expected an 'Attribute' or 'value' element; found '%s' instead.", localname);
            break;

        case inside_attribute_value:
            // Attribute values are processed by the end element code.
            break;

        case inside_other_xml_attribute:
            parser->other_xml_depth++;

            // Accumulate the elements here
            parser->other_xml.append("<");
            if (prefix) {
                parser->other_xml.append((const char *) prefix);
                parser->other_xml.append(":");
            }
            parser->other_xml.append(localname);

            if (nb_namespaces != 0) {
                parser->transfer_xml_ns(namespaces, nb_namespaces);

                for (map<string, string>::iterator i = parser->namespace_table.begin();
                        i != parser->namespace_table.end(); ++i) {
                    parser->other_xml.append(" xmlns");
                    if (!i->first.empty()) {
                        parser->other_xml.append(":");
                        parser->other_xml.append(i->first);
                    }
                    parser->other_xml.append("=\"");
                    parser->other_xml.append(i->second);
                    parser->other_xml.append("\"");
                }
            }

            if (nb_attributes != 0) {
                parser->transfer_xml_attrs(attributes, nb_attributes);
                for (XMLAttrMap::iterator i = parser->xml_attr_begin(); i != parser->xml_attr_end(); ++i) {
                    parser->other_xml.append(" ");
                    if (!i->second.prefix.empty()) {
                        parser->other_xml.append(i->second.prefix);
                        parser->other_xml.append(":");
                    }
                    parser->other_xml.append(i->first);
                    parser->other_xml.append("=\"");
                    parser->other_xml.append(i->second.value);
                    parser->other_xml.append("\"");
                }
            }

            parser->other_xml.append(">");
            break;

        case inside_enum_def:
            // process an EnumConst element
            if (parser->process_enum_const(localname, attributes, nb_attributes))
                parser->push_state(inside_enum_const);
            else
                dmr_error(parser, "Expected an 'EnumConst' element; found '%s' instead.", localname);
            break;

        case inside_enum_const:
            // No content; nothing to do
            break;

        case inside_simple_type:
            // TODO Add support for arrays here because this is where
            // dimensions will appear.
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                dmr_error(parser, "Expected an 'Attribute' element; found '%s' instead.", localname);
            break;

        case inside_array:
            if (is_not(localname, "Array") && parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "dimension") == 0) {
                parser->process_dimension(localname, attributes, nb_attributes);
                // next state: inside_dimension
            }
            else if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                dmr_error(parser, "Expected an 'Attribute' element; found '%s' instead.", localname);
            break;

        case inside_dim_def:
            // No content; nothing to do
            break;

        case inside_dimension:
            // No content.
            break;

        case inside_structure:
            if (parser->process_variable(localname, attributes, nb_attributes))
                // This will push either inside_simple_type or inside_structure
                // onto the parser state stack.
                break;
            else if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::dmr_error(parser, "Expected an Attribute or variable element; found '%s' instead.", localname);
            break;

        case parser_unknown:
            // FIXME?
            // *** Never used? If so remove/error
            parser->push_state(parser_unknown);
            break;

        case parser_error:
        case parser_fatal_error:
            break;
    }

    if (parser->debug()) cerr << "Start element exit state: " << states[parser->get_state()] << endl;
}

void D4ParserSax2::dmr_end_element(void *p, const xmlChar *l, const xmlChar *prefix, const xmlChar *URI)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    const char *localname = (const char *) l;

    if (parser->debug()) cerr << "End element " << localname << " (state " << states[parser->get_state()] << ")" << endl;

    switch (parser->get_state()) {
        case parser_start:
            dmr_fatal_error(parser, "Unexpected state, inside start state while processing element '%s'.", localname);
            break;

        case inside_dataset:
            if (is_not(localname, "Dataset"))
                D4ParserSax2::dmr_error(parser, "Expected an end Dataset tag; found '%s' instead.", localname);

            parser->pop_state();
            break;

        case inside_group: {
            if (is_not(localname, "Group")) {
                D4ParserSax2::dmr_error(parser, "Expected an end tag for a Group; found '%s' instead.", localname);
                break;
            }

            // Pop the stack, the Group should be there. If an error is found,
            // make sure to delete the BaseType*.
            BaseType *btp = parser->top_basetype();
            parser->pop_basetype();
            if (btp->type() != dods_group_c) {
                D4ParserSax2::dmr_error(parser, "Expected to find a Group element on the stack.");
                delete btp;
            }

            // Link the group we just poped to the thing on the stack, which
            // should also be a group.
            BaseType *parent = parser->top_basetype();
            if (parent && !parent->type() == dods_group_c) {
                D4ParserSax2::dmr_error(parser, "Tried to add a Group to a non-group type '%s %s' (will continue with parse).",
                        parser->top_basetype()->type_name().c_str(), parser->top_basetype()->name().c_str());
                delete btp;
            }
            else
                static_cast<D4Group*>(parent)->add_group_nocopy(static_cast<D4Group*>(btp));

            parser->pop_state();
            break;
        }

        case inside_attribute_container:
            if (is_not(localname, "Attribute"))
                D4ParserSax2::dmr_error(parser, "Expected an end Attribute tag; found '%s' instead.", localname);

            parser->pop_state();
            parser->pop_attributes();
            break;

        case inside_attribute:
            if (is_not(localname, "Attribute"))
                D4ParserSax2::dmr_error(parser, "Expected an end Attribute tag; found '%s' instead.", localname);

            parser->pop_state();
            break;

        case inside_attribute_value: {
            if (is_not(localname, "value"))
                D4ParserSax2::dmr_error(parser, "Expected an end value tag; found '%s' instead.", localname);

            parser->pop_state();

            // The old code added more values using the name and type as
            // indexes to find the correct attribute. Use get() for that
            // now. Or fix this code to keep a pointer to the to attribute...
            D4Attributes *attrs = parser->top_attributes();
            D4Attribute *attr = attrs->get(parser->dods_attr_name);
            if (!attr) {
                attr = new D4Attribute(parser->dods_attr_name, StringToD4AttributeType(parser->dods_attr_type));
                attrs->add_attribute_nocopy(attr);
            }
            attr->add_value(parser->char_data);

            parser->char_data = ""; // Null this after use.
            break;
        }

        case inside_other_xml_attribute: {
            if (strcmp(localname, "Attribute") == 0 && parser->root_ns == (const char *) URI) {
                parser->pop_state();

                // The old code added more values using the name and type as
                // indexes to find the correct attribute. Use get() for that
                // now. Or fix this code to keep a pointer to the to attribute...
                D4Attributes *attrs = parser->top_attributes();
                D4Attribute *attr = attrs->get(parser->dods_attr_name);
                if (!attr) {
                    attr = new D4Attribute(parser->dods_attr_name, StringToD4AttributeType(parser->dods_attr_type));
                    attrs->add_attribute_nocopy(attr);
                }
                attr->add_value(parser->other_xml);

                parser->other_xml = ""; // Null this after use.
            }
            else {
                if (parser->other_xml_depth == 0) {
                    D4ParserSax2::dmr_error(parser, "Expected an OtherXML attribute to end! Instead I found '%s'", localname);
                    break;
                }
                parser->other_xml_depth--;

                parser->other_xml.append("</");
                if (prefix) {
                    parser->other_xml.append((const char *) prefix);
                    parser->other_xml.append(":");
                }
                parser->other_xml.append(localname);
                parser->other_xml.append(">");
            }
            break;
        }

        case inside_enum_def:
            if (is_not(localname, "Enumeration"))
                D4ParserSax2::dmr_error(parser, "Expected an end Enumeration tag; found '%s' instead.", localname);

            if (!parser->top_basetype() || parser->top_basetype()->type() != dods_group_c)
                D4ParserSax2::dmr_error(parser, "Expected a Group to be the current item, while finishing up an Enumeration.");
            else {
                // copy the pointer; not a deep copy
                parser->dmr()->root()->enum_defs()->add_enum_nocopy(parser->enum_def());
                // Set the enum_def to null; next call to enum_def() will
                // allocate a new object
                parser->clear_enum_def();
                parser->pop_state();
            }
            break;

        case inside_enum_const:
            if (is_not(localname, "EnumConst"))
                D4ParserSax2::dmr_error(parser, "Expected an end EnumConst tag; found '%s' instead.", localname);

            parser->pop_state();
            break;

        case inside_dim_def: {
            if (is_not(localname, "Dimension"))
                D4ParserSax2::dmr_error(parser, "Expected an end Dimension tag; found '%s' instead.",  localname);

            if (!parser->top_basetype() || parser->top_basetype()->type() != dods_group_c)
                D4ParserSax2::dmr_error(parser, "Expected a Group to be the current item, while finishing up an Dimension.");

            // copy the pointer; not a deep copy
            parser->dmr()->root()->dims()->add_dim_nocopy(parser->dim_def());
            // Set the dim_def to null; next call to dim_def() will
            // allocate a new object. Calling 'clear' is important because
            // the cleanup method will free dim_def if it's not null and
            // we just copied the pointer in the add_dim_nocopy() call
            // above.
            parser->clear_dim_def();
            parser->pop_state();
            break;
        }

        case inside_simple_type:
            if (is_simple_type(get_type(localname))) {
                parser->pop_state();
                BaseType *btp = parser->top_basetype();
                parser->pop_basetype();
                parser->pop_attributes();

                BaseType *parent = parser->top_basetype();

               if (parent && parent->is_constructor_type())
                    parent->add_var_nocopy(btp);
                else {
                    D4ParserSax2::dmr_error(parser,
                            "Tried to add the simple-type variable '%s' to a non-constructor type (%s %s).", localname,
                            parser->top_basetype()->type_name().c_str(), parser->top_basetype()->name().c_str());
                    // since the BaseType* was popped and not copied anywhere,
                    // it must be deleted.
                    delete btp;
                }
            }
            else
                D4ParserSax2::dmr_error(parser, "Expected an end tag for a simple type; found '%s' instead.", localname);
            break;

        case inside_array:
            parser->finish_variable(localname, dods_array_c, "Array");
            break;

        case inside_dimension:
            if (is_not(localname, "dimension"))
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end Dimension tag; found '%s' instead.", localname);

            parser->pop_state();
            break;

        case inside_structure:
            parser->finish_variable(localname, dods_structure_c, "Structure");
            break;

        case parser_unknown:
            parser->pop_state();
            break;

        case parser_error:
        case parser_fatal_error:
            break;
    }

    if (parser->debug()) cerr << "End element exit state: " << states[parser->get_state()] << endl;
}

/** Process/accumulate character data. This may be called more than once for
 one logical clump of data. Only save character data when processing
 'value' elements; throw away all other characters. */
void D4ParserSax2::dmr_get_characters(void * p, const xmlChar * ch, int len)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    switch (parser->get_state()) {
        case inside_attribute_value:
            parser->char_data.append((const char *) (ch), len);
            DBG(cerr << "Characters: '" << parser->char_data << "'" << endl);
            break;

        case inside_other_xml_attribute:
            parser->other_xml.append((const char *) (ch), len);
            DBG(cerr << "Other XML Characters: '" << parser->other_xml << "'" << endl);
            break;

        default:
            break;
    }
}

/** Read whitespace that's not really important for content. This is used
 only for the OtherXML attribute type to preserve formating of the XML.
 Doing so makes the attribute value far easier to read.
 */
void D4ParserSax2::dmr_ignoreable_whitespace(void *p, const xmlChar *ch, int len)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    switch (parser->get_state()) {
        case inside_other_xml_attribute:
            parser->other_xml.append((const char *) (ch), len);
            break;

        default:
            break;
    }
}

/** Get characters in a cdata block. DAP does not use CData, but XML in an
 OtherXML attribute (the value of that DAP attribute) might use it. This
 callback also allows CData when the parser is in the 'parser_unknown'
 state since some future DAP element might use it.
 */
void D4ParserSax2::dmr_get_cdata(void *p, const xmlChar *value, int len)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    switch (parser->get_state()) {
        case inside_other_xml_attribute:
            parser->other_xml.append((const char *) (value), len);
            break;

        case parser_unknown:
            break;

        default:
            D4ParserSax2::dmr_error(parser, "Found a CData block but none are allowed by DAP4.");

            break;
    }
}

/** Handle the standard XML entities.

 @param parser The SAX parser
 @param name The XML entity. */
xmlEntityPtr D4ParserSax2::dmr_get_entity(void *, const xmlChar * name)
{
    return xmlGetPredefinedEntity(name);
}

/** Process an XML fatal error. Note that SAX provides for warnings, errors
 and fatal errors. This code treats them all as fatal errors since there's
 typically no way to tell a user about the error since there's often no
 user interface for this software.

 @note This static function does not throw an exception or otherwise
 alter flow of control except for altering the parser state.

 @param p The SAX parser
 @param msg A printf-style format string. */
void D4ParserSax2::dmr_fatal_error(void * p, const char *msg, ...)
{
    va_list args;
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    parser->push_state(parser_fatal_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = xmlSAX2GetLineNumber(parser->context);

    parser->error_msg += "At line " + long_to_string(line) + ": " + string(str);
}

void D4ParserSax2::dmr_error(void *p, const char *msg, ...)
{
    va_list args;
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    parser->push_state(parser_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = xmlSAX2GetLineNumber(parser->context);

    parser->error_msg += "At line " + long_to_string(line) + ": " + string(str);
}
//@}
#if 0
/** Helper to delete allocated stuff. */
void D4ParserSax2::delete_parser_locals()
{
    context->sax = NULL;
    xmlFreeParserCtxt(context);

    delete d_enum_def;
    d_enum_def = 0;

    delete d_dim_def;
    d_dim_def = 0;
}
#endif
/** Clean up after a parse operation. If the parser encountered an error,
 * throw either an Error or InternalErr object.
 */
void D4ParserSax2::cleanup_parse()
{
    bool wellFormed = context->wellFormed;
    bool valid = context->valid;

    context->sax = NULL;
    xmlFreeParserCtxt(context);

    delete d_enum_def;
    d_enum_def = 0;

    delete d_dim_def;
    d_dim_def = 0;

    // If there's an error, there may still be items on the stack at the
    // end of the parse.
    while (!btp_stack.empty()) {
        delete top_basetype();
        pop_basetype();
    }

    if (!wellFormed)
        throw Error("Error: The DMR was not well formed. " + error_msg);
    else if (!valid)
        throw Error("Error: The DMR was not valid." + error_msg);
    else if (get_state() == parser_error)
        throw Error("Error: " + error_msg);
    else if (get_state() == parser_fatal_error)
        throw InternalErr("Internal Error: " + error_msg);
}

/**
 * Read the DMR from a stream instead of a file.
 *
 * @param f The input stream
 * @param dest_dmr Value-result parameter. Pass a pointer to a DMR in and
 * the information in the DMR will be added to it.
 * @param debug If true, ouput helpful debugging messages, False by default.
 *
 * @exception Error Thrown if the XML document could not be read or parsed.
 * @exception InternalErr Thrown if an internal error is found.
 */
void D4ParserSax2::intern(istream &f, DMR *dest_dmr, bool debug)
{
    d_debug = debug;

    // Code example from libxml2 docs re: read from a stream.

    if (!f.good())
        throw Error("Input stream not open or read error");
    if (!dest_dmr)
        throw InternalErr(__FILE__, __LINE__, "DMR object is null");

    const int size = 1024;
    char chars[size];

    f.getline(chars, size);
    int res = f.gcount();
    if (res > 0) {
        if (debug) cerr << "line: (" << res << "): " << chars << endl;
        d_dmr = dest_dmr; // dump values here

        xmlSAXHandler ddx_sax_parser;
        memset(&ddx_sax_parser, 0, sizeof(xmlSAXHandler));

        ddx_sax_parser.getEntity = &D4ParserSax2::dmr_get_entity;
        ddx_sax_parser.startDocument = &D4ParserSax2::dmr_start_document;
        ddx_sax_parser.endDocument = &D4ParserSax2::dmr_end_document;
        ddx_sax_parser.characters = &D4ParserSax2::dmr_get_characters;
        ddx_sax_parser.ignorableWhitespace = &D4ParserSax2::dmr_ignoreable_whitespace;
        ddx_sax_parser.cdataBlock = &D4ParserSax2::dmr_get_cdata;
        ddx_sax_parser.warning = &D4ParserSax2::dmr_error;
        ddx_sax_parser.error = &D4ParserSax2::dmr_error;
        ddx_sax_parser.fatalError = &D4ParserSax2::dmr_fatal_error;
        ddx_sax_parser.initialized = XML_SAX2_MAGIC;
        ddx_sax_parser.startElementNs = &D4ParserSax2::dmr_start_element;
        ddx_sax_parser.endElementNs = &D4ParserSax2::dmr_end_element;

        context = xmlCreatePushParserCtxt(&ddx_sax_parser, this, chars, res - 1, "stream");
        context->validate = true;

        f.getline(chars, size);
        while ((f.gcount() > 0)) {
            if (debug) cerr << "line: (" << f.gcount() << "): " << chars << endl;
            xmlParseChunk(context, chars, f.gcount() - 1, 0);
            f.getline(chars, size);
        }
        // This call ends the parse.
        xmlParseChunk(context, chars, 0, 1/*terminate*/);

        cleanup_parse();
    }
}

/** Parse a DMR document stored in a string.
 *
 * @param document Read the DMR from this string.
 * @param dest_dmr Value/result parameter; dumps the information to this DMR
 * instance.
 * @param debug If true, ouput helpful debugging messages, False by default
 *
 * @exception Error Thrown if the XML document could not be read or parsed.
 * @exception InternalErr Thrown if an internal error is found.
 */
void D4ParserSax2::intern(const string &document, DMR *dest_dmr, bool debug)
{
    istringstream iss(document);
    intern(iss, dest_dmr, debug);
}

} // namespace libdap
