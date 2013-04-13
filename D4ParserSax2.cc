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

#define DODS_DEBUG 1
#define DODS_DEBUG2 1

#include <iostream>
#include <sstream>

#include <cstring>
#include <cstdarg>

#include "BaseType.h"

#if 0
#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Int64.h"
#include "UInt64.h"

#include "Float32.h"
#include "Float64.h"

#include "Str.h"
#include "Url.h"

#include "Constructor.h"

#include "Structure.h"

#endif

#include "D4Group.h"

#if 0
#include "Array.h"
#include "Sequence.h"
#include "Grid.h"
#endif

#include "D4ParserSax2.h"

#include "util.h"
#include "debug.h"

namespace libdap {

static const not_used char *states[] = {
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

        "inside_grid",
        "inside_map",

        "inside_structure",
        "inside_sequence",

        "parser_unknown",
        "parser_error" };

static bool is_not(const char *name, const char *tag)
{
    return strcmp(name, tag) != 0;
}

void D4ParserSax2::push_state(D4ParserSax2::ParseState state)
{
    s.push(state);
}

D4ParserSax2::ParseState D4ParserSax2::get_state() const
{
    return s.top();
}

void D4ParserSax2::pop_state()
{
    s.pop();
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
        dmr_fatal_error(this, "Required attribute '%s' not found.", attr.c_str());
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

/** Given that an \c Attribute tag has just been read, determine whether the
 element is a container or a simple type, set the state and, for a simple
 type record the type and name for use when \c value elements are found.

 @note Modified to discriminate between OtherXML and the older DAP2.0
 attribute types (container, Byte, ...).

 @param attrs The array of XML attribute values */
void D4ParserSax2::process_attribute_helper(const xmlChar **attrs, int nb_attributes)
{
    // These methods set the state to parser_error if a problem is found.
    transfer_xml_attrs(attrs, nb_attributes);

    bool error = !(check_required_attribute(string("name")) && check_required_attribute(string("type")));
    if (error)
        return;

    if (xml_attrs["type"].value == "Container") {
        push_state(inside_attribute_container);

        AttrTable *child;
        AttrTable *parent = at_stack.top();

        child = parent->append_container(xml_attrs["name"].value);
        at_stack.push(child); // save.
        DBG2(cerr << "Pushing at" << endl);
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
}

bool D4ParserSax2::process_dimension_def(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (strcmp(name, "Dimension") == 0) {
        transfer_xml_attrs(attrs, nb_attributes);

        if (!(check_required_attribute("name") && check_required_attribute("size")))
            return false;

        // This getter allocates a new object if needed.
        dim_def()->set_name(xml_attrs["name"].value);
        if (xml_attrs["size"].value == "*")
            dim_def()->set_varying(true);
        else {
            long size;
            istringstream iss(xml_attrs["size"].value);
            iss >> skipws >> size;
            dim_def()->set_size(size);
        }

        return true;
    }

    return false;
}

bool D4ParserSax2::process_dimension(const char *name, const xmlChar **attrs, int nb_attributes)
{
}

bool D4ParserSax2::process_group(const char *name, const xmlChar **attrs, int nb_attributes)
{
}

/** Check to see if the current tag is either an \c Attribute or an \c Alias
 start tag. This method is a glorified macro...

 @param name The start tag name
 @param attrs The tag's XML attributes
 @return True if the tag was an \c Attribute or \c Alias tag */
inline bool D4ParserSax2::process_attribute(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (strcmp(name, "Attribute") == 0) {
        process_attribute_helper(attrs, nb_attributes);
        // next state: inside_attribtue or inside_attribute_container
        return true;
    }

    return false;
}

/** Check to see if the current tag is an \c Enumeration start tag.

 @param name The start tag name
 @param attrs The tag's XML attributes
 @return True if the tag was an \c Enumeration */
inline bool D4ParserSax2::process_enum_def(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (strcmp(name, "Enumeration") == 0) {
        transfer_xml_attrs(attrs, nb_attributes);

        if (!(check_required_attribute("name") && check_required_attribute("basetype")))
            return false;

        Type t = get_type(xml_attrs["basetype"].value.c_str());
        if (!is_integer_type(t)) {
            dmr_fatal_error(this, "Error: The Enumeration '%s' must have an integer type, instead the type '%s' was used.",
                    xml_attrs["name"].value.c_str(), xml_attrs["basetype"].value.c_str());
            // TODO Why not return false here as with the test above?
            // So that the parse can continue, make the type UInt64
            t = dods_uint64_c;
        }

        // This getter allocates a new object if needed.
        enum_def()->set_name(xml_attrs["name"].value);
        enum_def()->set_type(t);

        return true;
    }

    return false;
}

inline bool D4ParserSax2::process_enum_const(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (strcmp(name, "EnumConst") == 0) {
        // These methods set the state to parser_error if a problem is found.
        transfer_xml_attrs(attrs, nb_attributes);

        if (!(check_required_attribute("name") && check_required_attribute("value")))
            return false;

        istringstream iss(xml_attrs["value"].value);
        long long value = 0;
        iss >> skipws >> value;
        if (iss.fail() || iss.bad()) {
            dmr_fatal_error(this, "Error: Expected an integer value for an Enumeration constant, got '%s' instead.",
                    xml_attrs["value"].value.c_str());
        }
        else if (!enum_def()->is_valid_enum_value(value))
            dmr_fatal_error(this, "Error: In an Enumeration constant, the value '%s' cannot fit in a variable of type '%s'.",
                    xml_attrs["value"].value.c_str(), type_name(d_enum_def->type()).c_str());

        else {
            // unfortunate choice of names... args are 'label' and 'value'
            enum_def()->add_value(xml_attrs["name"].value, value);
        }

        return true;
    }

    return false;
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
    else if (strcmp(name, "Group") == 0) {
        process_variable_helper(dods_group_c, inside_group, attrs, nb_attributes);
        return true;
    }
    else
        return false;
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

    push_state(s);

    if (check_required_attribute("name")) {
        BaseType *btp = dmr()->factory()->NewVariable(t, xml_attrs["name"].value);
        if (!btp)
            dmr_fatal_error(this, "Internal parser error; could not instantiate the variable '%s'.",
                    xml_attrs["name"].value.c_str());

        // Once we make the new variable, we not only load it on to the
        // BaseType stack, we also load its AttrTable on the AttrTable stack.
        // The attribute processing software always operates on the AttrTable
        // at the top of the AttrTable stack (at_stack).
        bt_stack.push(btp);
        at_stack.push(&btp->get_attr_table());
    }
}

void D4ParserSax2::finish_variable(const char *tag, Type t, const char *expected)
{
    if (strcmp(tag, expected) != 0) {
        D4ParserSax2::dmr_fatal_error(this, "Expected an end tag for a %s; found '%s' instead.", expected, tag);
        return;
    }

    pop_state();

    BaseType *btp = bt_stack.top();

    bt_stack.pop();
    at_stack.pop();

    if (btp->type() != t) {
        D4ParserSax2::dmr_fatal_error(this, "Internal error: Expected a %s variable.", expected);
        return;
    }
    // Once libxml2 validates, this can go away. 05/30/03 jhrg
#if 0
    if (t == dods_array_c && dynamic_cast<Array *>(btp)->dimensions() == 0) {
        D4ParserSax2::dmr_fatal_error(this, "No dimension element included in the Array '%s'.", btp->name().c_str());
        return;
    }
#endif
    BaseType *parent = bt_stack.top();

    // TODO Remove is_vector_type() and do something more apropos for DAP4
    if (!(parent->is_vector_type() || parent->is_constructor_type())) {
        D4ParserSax2::dmr_fatal_error(this, "Tried to add the array variable '%s' to a non-constructor type (%s %s).",
                tag, bt_stack.top()->type_name().c_str(), bt_stack.top()->name().c_str());
        return;
    }

    parent->add_var(btp);
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

    DBG2(cerr << "Parser state: " << states[parser->get_state()] << endl);
}

/** Clean up after finishing a parse.
 @param p The SAX parser  */
void D4ParserSax2::dmr_end_document(void * p)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    DBG2(cerr << "Ending state == " << states[parser->get_state()] << endl);

    if (parser->get_state() != parser_start)
        D4ParserSax2::dmr_fatal_error(parser, "The document contained unbalanced tags.");

    // If we've found any sort of error, don't make the DMR; intern() will
    // take care of the error.
    if (parser->get_state() == parser_error)
        return;

    // The root group should be on the stack
    if (parser->bt_stack.top()->type() != dods_group_c) {
        DBG(cerr << "Whoa! A Group should be on the stack!");
        D4ParserSax2::dmr_fatal_error(parser, "The document contained unbalanced variables on the stack: Expected a Group a TOS.");
    }

    parser->bt_stack.pop();     // leave the stack 'clean'
}

void D4ParserSax2::dmr_start_element(void *p, const xmlChar *l, const xmlChar *prefix, const xmlChar *URI,
        int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int /*nb_defaulted*/,
        const xmlChar **attributes)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    const char *localname = (const char *) l;

    DBG2(cerr << "start element: " << localname << ", state: " << states[parser->get_state()] << endl);

    switch (parser->get_state()) {
        case parser_start:
            if (strcmp(localname, "Dataset") == 0) {
                parser->root_ns = URI ? (const char *) URI : "";
                parser->transfer_xml_attrs(attributes, nb_attributes);

                // Set values in/for the DDS
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
                parser->bt_stack.push(parser->dmr()->root());

                parser->push_state(inside_dataset);
            }
            else
                D4ParserSax2::dmr_fatal_error(parser,
                        "Expected DMR to start with a Dataset element; found '%s' instead.", localname);
            break;

            // Note that it's possible to have an empty Dataset. The elements
            // are the same as those for a Group, but for the Dataset, the
            // root group is implicit.
        case inside_dataset:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (parser->process_enum_def(localname, attributes, nb_attributes))
                parser->push_state(inside_enum_def);
            else if (parser->process_dimension_def(localname, attributes, nb_attributes))
                parser->push_state(inside_dim_def);
            else if (parser->process_group(localname, attributes, nb_attributes))
                break;
            else
                break;
            break;

            // The state 'inside_group' means we have just parsed the start
            // of a Group element, but none of the child elements
        case inside_group:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (parser->process_enum_def(localname, attributes, nb_attributes))
                parser->push_state(inside_enum_def);
            else if (parser->process_dimension_def(localname, attributes, nb_attributes))
                parser->push_state(inside_dim_def);
            else if (parser->process_group(localname, attributes, nb_attributes))
                break;

            break;
#if 0
            else
                D4ParserSax2::dmr_fatal_error(parser,
                        "Expected an Attribute, or variable element; found '%s' instead.", localname);
            break;
#endif
        case inside_attribute_container:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an Attribute or Alias element; found '%s' instead.",
                        localname);
            break;

        case inside_attribute:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "value") == 0)
                parser->push_state(inside_attribute_value);
            else
                dmr_fatal_error(parser, "Expected an 'Attribute', 'Alias' or 'value' element; found '%s' instead.",
                        localname);
            break;

        case inside_attribute_value:
            // FIXME
            break;

        case inside_other_xml_attribute:
            DBGN(cerr << endl << "\t inside_other_xml_attribute: " << localname << endl);

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
                dmr_fatal_error(parser, "Expected an 'EnumConst' element; found '%s' instead.", localname);
            break;

        case inside_enum_const:
            // Nothing to do; this element has no content
            break;

        case inside_simple_type:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                dmr_fatal_error(parser, "Expected an 'Attribute' or 'Alias' element; found '%s' instead.", localname);
            break;

        case inside_array:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (is_not(localname, "Array") && parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "dimension") == 0) {
                parser->process_dimension(localname, attributes, nb_attributes);
                // next state: inside_dimension
            }
            else
                dmr_fatal_error(parser, "Expected an 'Attribute' or 'Alias' element; found '%s' instead.", localname);
            break;

        case inside_dimension:
            dmr_fatal_error(parser,
                    "Internal parser error; unexpected state, inside dimension while processing element '%s'.",
                    localname);
            break;

        case inside_structure:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (parser->process_variable(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::dmr_fatal_error(parser,
                        "Expected an Attribute, Alias or variable element; found '%s' instead.", localname);
            break;

        case inside_sequence:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (parser->process_variable(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::dmr_fatal_error(parser,
                        "Expected an Attribute, Alias or variable element; found '%s' instead.", localname);
            break;

        case inside_grid:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "Array") == 0)
                parser->process_variable_helper(dods_array_c, inside_array, attributes, nb_attributes);
            else if (strcmp(localname, "Map") == 0)
                parser->process_variable_helper(dods_array_c, inside_map, attributes, nb_attributes);
            else
                D4ParserSax2::dmr_fatal_error(parser,
                        "Expected an Attribute, Alias or variable element; found '%s' instead.", localname);
            break;

        case inside_map:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (is_not(localname, "Array") && is_not(localname, "Sequence") && is_not(localname, "Grid")
                    && parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "dimension") == 0) {
                parser->process_dimension(localname, attributes, nb_attributes);
                // next state: inside_dimension
            }
            else
                dmr_fatal_error(parser,
                        "Expected an 'Attribute', 'Alias', variable or 'dimension' element; found '%s' instead.",
                        localname);
            break;

        case parser_unknown:
            // *** Never used? If so remove/error
            parser->push_state(parser_unknown);
            break;

        case parser_error:
            break;
    }

    DBGN(cerr << " ... " << states[parser->get_state()] << endl);
}

void D4ParserSax2::dmr_end_element(void *p, const xmlChar *l, const xmlChar *prefix, const xmlChar *URI)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    const char *localname = (const char *) l;

    DBG2(cerr << "End element " << localname << " (state "
            << states[parser->get_state()] << ")" << endl);

    switch (parser->get_state()) {
        case parser_start:
            dmr_fatal_error(parser,
                    "Internal parser error; unexpected state, inside start state while processing element '%s'.",
                    localname);
            break;

        case inside_dataset:
            if (strcmp(localname, "Dataset") == 0)
                parser->pop_state();
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end Dataset tag; found '%s' instead.", localname);
            break;

        case inside_group:
            if (strcmp(localname, "Group") == 0)
                parser->pop_state();
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end Group tag; found '%s' instead.", localname);
            break;

        case inside_attribute_container:
            if (strcmp(localname, "Attribute") == 0) {
                parser->pop_state();
                parser->at_stack.pop(); // pop when leaving a container.
            }
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end Attribute tag; found '%s' instead.", localname);
            break;

        case inside_attribute:
            if (strcmp(localname, "Attribute") == 0)
                parser->pop_state();
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end Attribute tag; found '%s' instead.", localname);
            break;

        case inside_attribute_value:
            if (strcmp(localname, "value") == 0) {
                parser->pop_state();
                AttrTable *atp = parser->at_stack.top();
                atp->append_attr(parser->dods_attr_name, parser->dods_attr_type, parser->char_data);
                parser->char_data = ""; // Null this after use.
            }
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end value tag; found '%s' instead.", localname);

            break;

        case inside_other_xml_attribute: {
            if (strcmp(localname, "Attribute") == 0 && parser->root_ns == (const char *) URI) {

                DBGN(cerr << endl << "\t Popping the 'inside_other_xml_attribute' state"
                        << endl);

                parser->pop_state();

                AttrTable *atp = parser->at_stack.top();
                atp->append_attr(parser->dods_attr_name, parser->dods_attr_type, parser->other_xml);

                parser->other_xml = ""; // Null this after use.
            }
            else {
                DBGN(cerr << endl << "\t inside_other_xml_attribute: " << localname
                        << ", depth: " << parser->other_xml_depth << endl);
                if (parser->other_xml_depth == 0)
                    D4ParserSax2::dmr_fatal_error(parser,
                            "Expected an OtherXML attribute to end! Instead I found '%s'", localname);
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
            if (strcmp(localname, "Enumeration") == 0) {
                BaseType *btp = parser->bt_stack.top();
                if (!btp || btp->type() != dods_group_c)
                    D4ParserSax2::dmr_fatal_error(parser, "Expected a Group to be the current item, while finishing up an %s.", localname);
                else {
                    // copy the pointer; not a deep copy
                    parser->dmr()->root()->enum_defs()->add_enum_nocopy(parser->enum_def());
                    // Set the enum_def to null; next call to enum_def() will
                    // allocate a new object
                    parser->clear_enum_def();
                    parser->pop_state();
                }
            }
            else {
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end Enumeration tag; found '%s' instead.", localname);
            }
            break;

        case inside_enum_const:
            if (strcmp(localname, "EnumConst") == 0)
                parser->pop_state();
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end EnumConst tag; found '%s' instead.", localname);
            break;

        case inside_dim_def: {
            if (strcmp(localname, "Dimension") != 0)
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end Enumeration tag; found '%s' instead.",
                        localname);

            // Grab the thing on the TOS but don't pop
            BaseType *btp = parser->bt_stack.top();
            if (!btp || btp->type() != dods_group_c)
                D4ParserSax2::dmr_fatal_error(parser,
                        "Expected a Group to be the current item, while finishing up an %s.", localname);

            // copy the pointer; not a deep copy
            parser->dmr()->root()->dims()->add_dim_nocopy(parser->dim_def());
            // Set the dim_def to null; next call to dim_def() will
            // allocate a new object
            parser->clear_dim_def();
            parser->pop_state();
            break;
        }

        case inside_simple_type:
            if (is_simple_type(get_type(localname))) {
                parser->pop_state();
                BaseType *btp = parser->bt_stack.top();
                parser->bt_stack.pop();
                parser->at_stack.pop();

                BaseType *parent = parser->bt_stack.top();

                // NB: This works because we seed the stack with a dummy
                // structure instance at the start of the parse. When the
                // parse is complete the variables in that structure will
                // be transferred to the DDS. Or maybe someday we will really
                // use a Structure instance in DDS...
                if (parent->is_vector_type() || parent->is_constructor_type())
                    parent->add_var(btp);
                else
                    D4ParserSax2::dmr_fatal_error(parser,
                            "Tried to add the simple-type variable '%s' to a non-constructor type (%s %s).", localname,
                            parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
            }
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end tag for a simple type; found '%s' instead.",
                        localname);
            break;

        case inside_array:
            parser->finish_variable(localname, dods_array_c, "Array");
            break;

        case inside_dimension:
            if (strcmp(localname, "dimension") == 0)
                parser->pop_state();
            else
                D4ParserSax2::dmr_fatal_error(parser, "Expected an end dimension tag; found '%s' instead.", localname);
            break;

        case inside_structure:
            parser->finish_variable(localname, dods_structure_c, "Structure");
            break;

        case inside_sequence:
            parser->finish_variable(localname, dods_sequence_c, "Sequence");
            break;

        case inside_grid:
            parser->finish_variable(localname, dods_grid_c, "Grid");
            break;

        case inside_map:
            parser->finish_variable(localname, dods_array_c, "Map");
            break;

        case parser_unknown:
            parser->pop_state();
            break;

        case parser_error:
            break;
    }

    DBGN(cerr << " ... " << states[parser->get_state()] << endl);
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
            DBG2(cerr << "Characters: '" << parser->char_data << "'" << endl);
            break;

        case inside_other_xml_attribute:
            parser->other_xml.append((const char *) (ch), len);
            DBG2(cerr << "Other XML Characters: '" << parser->other_xml << "'" << endl);
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
            D4ParserSax2::dmr_fatal_error(parser, "Found a CData block but none are allowed by DAP.");

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

    parser->push_state(parser_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = xmlSAX2GetLineNumber(parser->context);

    parser->error_msg += "At line " + long_to_string(line) + ": ";
    parser->error_msg += string(str) + string("\n");
}

//@}

void D4ParserSax2::cleanup_parse()
{
    try {
        if (!context->wellFormed)
            throw D4ParseError(string("\nThe DMR is not a well formed XML document.\n") + error_msg);
        else if (!context->valid)
            throw D4ParseError(string("\nThe DMR is not a valid document.\n") + error_msg);
        else if (get_state() == parser_error)
            throw D4ParseError(string("\nError parsing DMR response.\n") + error_msg);
    }
    catch (...) {
        context->sax = NULL;
        xmlFreeParserCtxt(context);

        delete d_enum_def;
        d_enum_def = 0;

        throw;
    }

    context->sax = NULL; // Not sure this is needed
    xmlFreeParserCtxt(context);

    delete d_enum_def;
    d_enum_def = 0;
}

/**
 * @brief Read the DDX from a stream instead of a file.
 *
 * This method reads and parses the DDX from a stream. When/if it encounters
 * the <blob> element is records the value of the  CID attribute and returns
 * it using the cid value-result parameter. When the boundary marker is
 * found, the parser will read that and return (if it is found before the
 * DDX is completely parsed).
 *
 * @param f The input stream
 * @param dest_ddmr Value-result parameter. Pass a DMR in and the information
 * in the DMR will be added to it.
 */
void D4ParserSax2::intern(istream &f, DMR *dest_dmr)
{
    // Code example from libxml2 docs re: read from a stream.

    if (!f.good())
        throw InternalErr(__FILE__, __LINE__, "Input stream not open or read error");
    if (!dest_dmr)
        throw InternalErr(__FILE__, __LINE__, "DMR object is null");

    const int size = 1024;
    char chars[size];

    f.getline(chars, size);
    int res = f.gcount();
    if (res > 0) {
        DBG(cerr << "line: (" << res << "): " << chars << endl);
#if 0
        // Originally this and block below were used to alloc the context
        // object for libxml2. In tracing down memory leaks, I wound up with
        // what's in the code now, although 256 bytes are still leaked from
        // inside xmlCreatePushParserCtxt according to valgrind.
        context = xmlCreatePushParserCtxt(NULL, NULL, chars, res - 1, "stream");
#endif
        d_dmr = dest_dmr; // dump values here

        xmlSAXHandler ddx_sax_parser;
        memset(&ddx_sax_parser, 0, sizeof(xmlSAXHandler));

        ddx_sax_parser.getEntity = &D4ParserSax2::dmr_get_entity;
        ddx_sax_parser.startDocument = &D4ParserSax2::dmr_start_document;
        ddx_sax_parser.endDocument = &D4ParserSax2::dmr_end_document;
        ddx_sax_parser.characters = &D4ParserSax2::dmr_get_characters;
        ddx_sax_parser.ignorableWhitespace = &D4ParserSax2::dmr_ignoreable_whitespace;
        ddx_sax_parser.cdataBlock = &D4ParserSax2::dmr_get_cdata;
        ddx_sax_parser.warning = &D4ParserSax2::dmr_fatal_error;
        ddx_sax_parser.error = &D4ParserSax2::dmr_fatal_error;
        ddx_sax_parser.fatalError = &D4ParserSax2::dmr_fatal_error;
        ddx_sax_parser.initialized = XML_SAX2_MAGIC;
        ddx_sax_parser.startElementNs = &D4ParserSax2::dmr_start_element;
        ddx_sax_parser.endElementNs = &D4ParserSax2::dmr_end_element;

        context = xmlCreatePushParserCtxt(&ddx_sax_parser, this, chars, res - 1, "stream");

#if 0
       context->sax = &ddx_sax_parser;
       context->userData = this;
#endif
        context->validate = true;

        f.getline(chars, size);
        while ((f.gcount() > 0)) {
            DBG(cerr << "line: (" << f.gcount() << "): " << chars << endl);
            xmlParseChunk(context, chars, f.gcount() - 1, 0);
            f.getline(chars, size);
        }
        // This call ends the parse.
        xmlParseChunk(context, chars, 0, 1/*terminate*/);

        cleanup_parse();
    }
}

/** Parse a DMR document stored in a string.

 @param document Read the DMR from this string.
 @param dest_dmr Value/result parameter; dumps the information to this DMR
 instance.
 @exception D4ParseError Thrown if the XML document could not be
 read or parsed.
 */
void D4ParserSax2::intern(const string &document, DMR *dest_dmr)
{
    istringstream iss(document);
    intern(iss, dest_dmr);
}

} // namespace libdap
