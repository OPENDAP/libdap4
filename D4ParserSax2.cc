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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

#include "D4Group.h"

#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "D4ParserSax2.h"

#include "util.h"
// #include "mime_util.h"
#include "debug.h"

namespace libdap {

static const not_used char *states[] = {
        "parser_start",

        // inside_group is the state just after parsing the start of a Group
        // element.
        "inside_group",

        "inside_attribute_container",
        "inside_attribute",
        "inside_attribute_value",
        "inside_other_xml_attribute",

        "inside_enum_def",
        "inside_enum_const",

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

// Glue the BaseTypeFactory to the enum-based factory defined statically
// here.

BaseType *D4ParserSax2::factory(Type t, const string & name)
{
    switch (t) {
        case dods_byte_c:
            return d_factory->NewByte(name);
        case dods_uint8_c:
            return d_factory->NewUInt8(name);
        case dods_int8_c:
            return d_factory->NewInt8(name);
        case dods_int16_c:
            return d_factory->NewInt16(name);
        case dods_uint16_c:
            return d_factory->NewUInt16(name);
        case dods_int32_c:
            return d_factory->NewInt32(name);
        case dods_uint32_c:
            return d_factory->NewUInt32(name);
        case dods_int64_c:
            return d_factory->NewInt64(name);
        case dods_uint64_c:
            return d_factory->NewUInt64(name);
        case dods_float32_c:
            return d_factory->NewFloat32(name);
        case dods_float64_c:
            return d_factory->NewFloat64(name);
        case dods_str_c:
            return d_factory->NewStr(name);
        case dods_url_c:
            return d_factory->NewUrl(name);
        case dods_url4_c:
            return d_factory->NewURL(name);
        case dods_array_c:
            return d_factory->NewArray(name);
        case dods_structure_c:
            return d_factory->NewStructure(name);
        case dods_sequence_c:
            return d_factory->NewSequence(name);
        case dods_grid_c:
            return d_factory->NewGrid(name);
        case dods_group_c:
            return d_factory->NewGroup(name);
        default:
            return 0;
    }
}

static bool is_valid_enum_value(const Type &t, long long value)
{
    switch (t) {
        case dods_int8_c:
            return (value >= DODS_SCHAR_MIN && value <= DODS_SCHAR_MAX);
        case dods_byte_c:
        case dods_uint8_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_UCHAR_MAX);
        case dods_int16_c:
            return (value >= DODS_SHRT_MIN && value <= DODS_SHRT_MAX);
        case dods_uint16_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_USHRT_MAX);
        case dods_int32_c:
            return (value >= DODS_INT_MIN && value <= DODS_INT_MAX);
        case dods_uint32_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_UINT_MAX);
        case dods_int64_c:
            return (value >= DODS_LLONG_MIN && value <= DODS_LLONG_MAX);
        case dods_uint64_c:
            return (value >= 0 && static_cast<unsigned long long>(value) <= DODS_ULLONG_MAX);
        default:
            return false;
    }
}

static bool is_not(const char *name, const char *tag)
{
    return strcmp(name, tag) != 0;
}

void D4ParserSax2::set_state(D4ParserSax2::ParseState state)
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
 Attribute names are always folded to lower case.
 @param attributes The XML attribute array
 @param nb_attributes Teh number of attributes */
void D4ParserSax2::transfer_xml_attrs(const xmlChar **attributes, int nb_attributes)
{
    if (!xml_attrs.empty())
        xml_attrs.clear(); // erase old attributes

    unsigned int index = 0;
    for (int i = 0; i < nb_attributes; ++i, index += 5) {
        // Make a value using the attribute name and the prefix, namespace URI
        // and the value. The prefix might be null.
        xml_attrs.insert(map<string, XMLAttribute>::value_type(string((const char *) attributes[index]),
                        XMLAttribute(attributes + index + 1)));

        DBG(cerr << "Attribute '" << (const char *)attributes[index] << "': "
                << xml_attrs[(const char *)attributes[index]].value << endl);
    }
}

void D4ParserSax2::transfer_xml_ns(const xmlChar **namespaces, int nb_namespaces)
{
    for (int i = 0; i < nb_namespaces; ++i) {
        // make a value with the prefix and namespace URI. The prefix might be
        // null.
        namespace_table.insert(
                map<string, string>::value_type(namespaces[i * 2] != 0 ? (const char *) namespaces[i * 2] : "",
                        (const char *) namespaces[i * 2 + 1]));
    }
}

/** Is an attribute present? Attribute names are always lower case.
 @note To use this method, first call transfer_xml_attrs.
 @param attr The XML attribute
 @return True if the XML attribute was present in the last tag */
bool D4ParserSax2::check_required_attribute(const string & attr)
{
    map<string, XMLAttribute>::iterator i = xml_attrs.find(attr);
    if (i == xml_attrs.end())
        ddx_fatal_error(this, "Required attribute '%s' not found.", attr.c_str());
    return true;
}

/** Is an attribute present? Attribute names are always lower case.
 @note To use this method, first call transfer_xml_attrs.
 @param attr The XML attribute
 @return True if the XML attribute was present in the last/current tag,
 false otherwise. */
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
        set_state(inside_attribute_container);

        AttrTable *child;
        AttrTable *parent = at_stack.top();

        child = parent->append_container(xml_attrs["name"].value);
        at_stack.push(child); // save.
        DBG2(cerr << "Pushing at" << endl);
    }
    else if (xml_attrs["type"].value == "OtherXML") {
        set_state(inside_other_xml_attribute);

        dods_attr_name = xml_attrs["name"].value;
        dods_attr_type = xml_attrs["type"].value;
    }
    else {
        set_state(inside_attribute);

        dods_attr_name = xml_attrs["name"].value;
        dods_attr_type = xml_attrs["type"].value;
    }
}

/** Given that an \c Enumeration tag has just been read...


 @param attrs The array of XML attribute values */
void D4ParserSax2::process_enum_def_helper(const xmlChar **attrs, int nb_attributes)
{
    // These methods set the state to parser_error if a problem is found.
    transfer_xml_attrs(attrs, nb_attributes);

    bool error = !(check_required_attribute(string("name"))
                   && check_required_attribute(string("basetype")));
    if (error)
        return;

    Type t = get_type(xml_attrs["basetype"].value.c_str());
    if (!is_integer_type(t)) {
        ddx_fatal_error(this, "Error: The Enumeration '%s' must have an integer type, instead the type '%s' was used.",
                xml_attrs["name"].value.c_str(), xml_attrs["basetype"].value.c_str());
        // So that the parse can continue, make the type UInt64
        t = dods_uint64_c;
    }

    d_enum_def = new D4EnumDef(xml_attrs["name"].value, t);

    set_state(inside_enum_def);
}

/** Given that an \c Enumeration tag has just been read...


 @param attrs The array of XML attribute values */
void D4ParserSax2::process_enum_const_helper(const xmlChar **attrs, int nb_attributes)
{
    // These methods set the state to parser_error if a problem is found.
    transfer_xml_attrs(attrs, nb_attributes);

    bool error = !(check_required_attribute(string("name"))
                   && check_required_attribute(string("value")));
    if (error)
        return;

    istringstream iss(xml_attrs["value"].value);
    long long value = 0;
    iss >> skipws >> value;
    if (iss.fail() || iss.bad()) {
        ddx_fatal_error(this, "Error: Expected an integer value for an Enumeration constant, got '%s' instead.",
                xml_attrs["value"].value.c_str());
    }
    else if (!is_valid_enum_value(d_enum_def->get_type(), value))
        ddx_fatal_error(this, "Error: In an Enumeration constant, the value '%s' cannot fit in a variable of type '%s'.",
                xml_attrs["value"].value.c_str(), type_name(d_enum_def->get_type()).c_str());

    else {
        d_enum_def->add_value(xml_attrs["name"].value, value);
    }

    set_state(inside_enum_const);
}

/** Given that a \c dimension tag has just been read, add that information to
 the array on the top of the BaseType stack.
 @param attrs The XML attributes included in the \c dimension tag */
void D4ParserSax2::process_dimension(const xmlChar **attrs, int nb_attributes)
{
    transfer_xml_attrs(attrs, nb_attributes);
    if (check_required_attribute(string("size"))) {
        set_state(inside_dimension);
        Array *ap = dynamic_cast<Array *>(bt_stack.top());
        if (!ap) {
            ddx_fatal_error(this, "Parse error: Expected an array variable.");
            return;
        }

        ap->append_dim(atoi(xml_attrs["size"].value.c_str()), xml_attrs["name"].value);
    }
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
        process_enum_def_helper(attrs, nb_attributes);
        // next state: inside_enum_def
        return true;
    }

    return false;
}

inline bool D4ParserSax2::process_enum_const(const char *name, const xmlChar **attrs, int nb_attributes)
{
    if (strcmp(name, "EnumConst") == 0) {
        process_enum_const_helper(attrs, nb_attributes);
        // next state: inside_enum_const
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
    else if (strcmp(name, "Sequence") == 0) {
        process_variable_helper(dods_sequence_c, inside_sequence, attrs, nb_attributes);
        return true;
    }

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

    set_state(s);

    // TODO Arrays in DAP2 were not required to have names. DAP4 is going to
    // need a different parsing logic, but we'll come to that...
    if (check_required_attribute("name")) { // throws on error/false
        BaseType *btp = factory(t, xml_attrs["name"].value);
        if (!btp)
            ddx_fatal_error(this, "Internal parser error; could not instantiate the variable '%s'.",
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
        D4ParserSax2::ddx_fatal_error(this, "Expected an end tag for a %s; found '%s' instead.", expected, tag);
        return;
    }

    pop_state();

    BaseType *btp = bt_stack.top();

    bt_stack.pop();
    at_stack.pop();

    if (btp->type() != t) {
        D4ParserSax2::ddx_fatal_error(this, "Internal error: Expected a %s variable.", expected);
        return;
    }
    // Once libxml2 validates, this can go away. 05/30/03 jhrg
    if (t == dods_array_c && dynamic_cast<Array *>(btp)->dimensions() == 0) {
        D4ParserSax2::ddx_fatal_error(this, "No dimension element included in the Array '%s'.", btp->name().c_str());
        return;
    }

    BaseType *parent = bt_stack.top();

    if (!(parent->is_vector_type() || parent->is_constructor_type())) {
        D4ParserSax2::ddx_fatal_error(this, "Tried to add the array variable '%s' to a non-constructor type (%s %s).",
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
void D4ParserSax2::ddx_start_document(void * p)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    parser->error_msg = "";
    parser->char_data = "";

    parser->set_state(parser_start);

    DBG2(cerr << "Parser state: " << states[parser->get_state()] << endl);
}

/** Clean up after finishing a parse.
 @param p The SAX parser  */
void D4ParserSax2::ddx_end_document(void * p)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    DBG2(cerr << "Ending state == " << states[parser->get_state()] << endl);

    if (parser->get_state() != parser_start)
        D4ParserSax2::ddx_fatal_error(parser, "The document contained unbalanced tags.");

    // If we've found any sort of error, don't make the DMR; intern() will
    // take care of the error.
    if (parser->get_state() == parser_error)
        return;

    // TODO Decide to remove the outer Group or leave it in place and
    // modify print_xml_writer() so that the DMR is correct.

    parser->dds->add_var_nocopy(parser->bt_stack.top());
    parser->bt_stack.pop();
}

void D4ParserSax2::ddx_start_element(void *p, const xmlChar *l, const xmlChar *prefix, const xmlChar *URI,
        int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int /*nb_defaulted*/,
        const xmlChar **attributes)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    const char *localname = (const char *) l;

    DBG2(cerr << "start element: " << localname << ", state: " << states[parser->get_state()] << endl);

    switch (parser->get_state()) {
        case parser_start:
            if (strcmp(localname, "Group") == 0) {

                parser->set_state(inside_group);

                parser->root_ns = URI ? (const char *) URI : "";
                parser->transfer_xml_attrs(attributes, nb_attributes);

                // Set values in/for the DDS
                if (parser->check_required_attribute(string("name")))
                    parser->dds->set_dataset_name(parser->xml_attrs["name"].value);

                if (parser->check_attribute("dapVersion"))
                    parser->dds->set_dap_version(parser->xml_attrs["dapVersion"].value);

                // FIXME no way to record DMR version information
#if 0
                if (parser->check_attribute("dmrVersion"))
                    parser->dds->set_dap_version(parser->xml_attrs["dmrVersion"].value);
#endif
                if (parser->check_attribute("base"))
                    parser->dds->set_request_xml_base(parser->xml_attrs["base"].value);

                if (!parser->root_ns.empty())
                    parser->dds->set_namespace(parser->root_ns);

                // Set name of the Group; push on stack
                BaseType *btp = parser->factory(dods_group_c, "root");
                parser->bt_stack.push(btp);
                parser->at_stack.push(&btp->get_attr_table());
            }
            else
                D4ParserSax2::ddx_fatal_error(parser,
                        "Expected DMR to start with a Group element; found '%s' instead.", localname);
            break;

            // The state 'inside_group' means we have just parsed the start
            // of a Group element, but none of the child elements
        case inside_group:
            // TODO Add Dimension and Group
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (parser->process_enum_def(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::ddx_fatal_error(parser,
                        "Expected an Attribute,  or variable element; found '%s' instead.", localname);
            break;

        case inside_attribute_container:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an Attribute or Alias element; found '%s' instead.",
                        localname);
            break;

        case inside_attribute:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "value") == 0)
                parser->set_state(inside_attribute_value);
            else
                ddx_fatal_error(parser, "Expected an 'Attribute', 'Alias' or 'value' element; found '%s' instead.",
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
                break;
            else
                ddx_fatal_error(parser, "Expected an 'EnumConst' element; found '%s' instead.", localname);
            break;

        case inside_enum_const:
            // Nothing to do; this element has no content
            break;

        case inside_simple_type:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else
                ddx_fatal_error(parser, "Expected an 'Attribute' or 'Alias' element; found '%s' instead.", localname);
            break;

        case inside_array:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (is_not(localname, "Array") && parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "dimension") == 0) {
                parser->process_dimension(attributes, nb_attributes);
                // next state: inside_dimension
            }
            else
                ddx_fatal_error(parser, "Expected an 'Attribute' or 'Alias' element; found '%s' instead.", localname);
            break;

        case inside_dimension:
            ddx_fatal_error(parser,
                    "Internal parser error; unexpected state, inside dimension while processing element '%s'.",
                    localname);
            break;

        case inside_structure:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (parser->process_variable(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::ddx_fatal_error(parser,
                        "Expected an Attribute, Alias or variable element; found '%s' instead.", localname);
            break;

        case inside_sequence:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (parser->process_variable(localname, attributes, nb_attributes))
                break;
            else
                D4ParserSax2::ddx_fatal_error(parser,
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
                D4ParserSax2::ddx_fatal_error(parser,
                        "Expected an Attribute, Alias or variable element; found '%s' instead.", localname);
            break;

        case inside_map:
            if (parser->process_attribute(localname, attributes, nb_attributes))
                break;
            else if (is_not(localname, "Array") && is_not(localname, "Sequence") && is_not(localname, "Grid")
                    && parser->process_variable(localname, attributes, nb_attributes))
                break;
            else if (strcmp(localname, "dimension") == 0) {
                parser->process_dimension(attributes, nb_attributes);
                // next state: inside_dimension
            }
            else
                ddx_fatal_error(parser,
                        "Expected an 'Attribute', 'Alias', variable or 'dimension' element; found '%s' instead.",
                        localname);
            break;

        case parser_unknown:
            // *** Never used? If so remove/error
            parser->set_state(parser_unknown);
            break;

        case parser_error:
            break;
    }

    DBGN(cerr << " ... " << states[parser->get_state()] << endl);
}

void D4ParserSax2::ddx_end_element(void *p, const xmlChar *l, const xmlChar *prefix, const xmlChar *URI)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);
    const char *localname = (const char *) l;

    DBG2(cerr << "End element " << localname << " (state "
            << states[parser->get_state()] << ")" << endl);

    switch (parser->get_state()) {
        case parser_start:
            ddx_fatal_error(parser,
                    "Internal parser error; unexpected state, inside start state while processing element '%s'.",
                    localname);
            break;

        case inside_group:
            if (strcmp(localname, "Group") == 0)
                parser->pop_state();
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end Group tag; found '%s' instead.", localname);
            break;

        case inside_attribute_container:
            if (strcmp(localname, "Attribute") == 0) {
                parser->pop_state();
                parser->at_stack.pop(); // pop when leaving a container.
            }
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end Attribute tag; found '%s' instead.", localname);
            break;

        case inside_attribute:
            if (strcmp(localname, "Attribute") == 0)
                parser->pop_state();
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end Attribute tag; found '%s' instead.", localname);
            break;

        case inside_attribute_value:
            if (strcmp(localname, "value") == 0) {
                parser->pop_state();
                AttrTable *atp = parser->at_stack.top();
                atp->append_attr(parser->dods_attr_name, parser->dods_attr_type, parser->char_data);
                parser->char_data = ""; // Null this after use.
            }
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end value tag; found '%s' instead.", localname);

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
                    D4ParserSax2::ddx_fatal_error(parser,
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
                    D4ParserSax2::ddx_fatal_error(parser, "Expected a Group to be the current item, while finishing up an %s.", localname);
                else {
                    D4Group *g = static_cast<D4Group*>(btp);
                    g->add_enumeration_nocopy(parser->d_enum_def);
                    parser->pop_state();
                }
            }
            else {
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end Enumeration tag; found '%s' instead.", localname);
            }
            break;

        case inside_enum_const:
            if (strcmp(localname, "EnumConst") == 0)
                parser->pop_state();
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end EnumConst tag; found '%s' instead.", localname);
            break;

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
                    D4ParserSax2::ddx_fatal_error(parser,
                            "Tried to add the simple-type variable '%s' to a non-constructor type (%s %s).", localname,
                            parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
            }
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end tag for a simple type; found '%s' instead.",
                        localname);
            break;

        case inside_array:
            parser->finish_variable(localname, dods_array_c, "Array");
            break;

        case inside_dimension:
            if (strcmp(localname, "dimension") == 0)
                parser->pop_state();
            else
                D4ParserSax2::ddx_fatal_error(parser, "Expected an end dimension tag; found '%s' instead.", localname);
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
void D4ParserSax2::ddx_get_characters(void * p, const xmlChar * ch, int len)
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
void D4ParserSax2::ddx_ignoreable_whitespace(void *p, const xmlChar *ch, int len)
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
void D4ParserSax2::ddx_get_cdata(void *p, const xmlChar *value, int len)
{
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    switch (parser->get_state()) {
        case inside_other_xml_attribute:
            parser->other_xml.append((const char *) (value), len);
            break;

        case parser_unknown:
            break;

        default:
            D4ParserSax2::ddx_fatal_error(parser, "Found a CData block but none are allowed by DAP.");

            break;
    }
}

/** Handle the standard XML entities.

 @param parser The SAX parser
 @param name The XML entity. */
xmlEntityPtr D4ParserSax2::ddx_get_entity(void *, const xmlChar * name)
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
void D4ParserSax2::ddx_fatal_error(void * p, const char *msg, ...)
{
    va_list args;
    D4ParserSax2 *parser = static_cast<D4ParserSax2*>(p);

    parser->set_state(parser_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = xmlSAX2GetLineNumber(parser->ctxt);

    parser->error_msg += "At line " + long_to_string(line) + ": ";
    parser->error_msg += string(str) + string("\n");
}

//@}

void D4ParserSax2::cleanup_parse(xmlParserCtxtPtr & context) const
{
    if (!context->wellFormed) {
        context->sax = NULL;
        xmlFreeParserCtxt(context);
        throw D4ParseError(string("\nThe DDX is not a well formed XML document.\n") + error_msg);
    }

    if (!context->valid) {
        context->sax = NULL;
        xmlFreeParserCtxt(context);
        throw D4ParseError(string("\nThe DDX is not a valid document.\n") + error_msg);
    }

    if (get_state() == parser_error) {
        context->sax = NULL;
        xmlFreeParserCtxt(context);
        throw D4ParseError(string("\nError parsing DMR response.\n") + error_msg);
    }

    context->sax = NULL;
    xmlFreeParserCtxt(context);
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
 * @param dest_dds Value-result parameter. Pass a DDS in and the inforamtion
 * in the DDX will be added to it.
 * @param cid Value-result parameter. When/if read, the value of the <blob>
 * element's @cid attribute will be returned here.
 * @param boundary Value of the M-MIME boundary separator; default is ""
 * @see DDXParserDAP4::intern(). */
void D4ParserSax2::intern(istream &f, DDS *dest_dds)
{
    // Code example from libxml2 docs re: read from a stream.

    if (!f.good())
        throw InternalErr(__FILE__, __LINE__, "Input stream not open or read error");

    const int size = 1024;
    char chars[size];

    f.getline(chars, size);
    int res = f.gcount();
    if (res > 0) {

        DBG(cerr << "line: (" << res << "): " << chars << endl);
        xmlParserCtxtPtr context = xmlCreatePushParserCtxt(NULL, NULL, chars, res - 1, "stream");

        ctxt = context; // need ctxt for error messages
        dds = dest_dds; // dump values here
        d_factory = dynamic_cast<D4BaseTypeFactory*>(dds->get_factory());
        if (!d_factory)
            throw InternalErr(__FILE__, __LINE__, "Invalid factory class");

        xmlSAXHandler ddx_sax_parser;
        memset(&ddx_sax_parser, 0, sizeof(xmlSAXHandler));

        ddx_sax_parser.getEntity = &D4ParserSax2::ddx_get_entity;
        ddx_sax_parser.startDocument = &D4ParserSax2::ddx_start_document;
        ddx_sax_parser.endDocument = &D4ParserSax2::ddx_end_document;
        ddx_sax_parser.characters = &D4ParserSax2::ddx_get_characters;
        ddx_sax_parser.ignorableWhitespace = &D4ParserSax2::ddx_ignoreable_whitespace;
        ddx_sax_parser.cdataBlock = &D4ParserSax2::ddx_get_cdata;
        ddx_sax_parser.warning = &D4ParserSax2::ddx_fatal_error;
        ddx_sax_parser.error = &D4ParserSax2::ddx_fatal_error;
        ddx_sax_parser.fatalError = &D4ParserSax2::ddx_fatal_error;
        ddx_sax_parser.initialized = XML_SAX2_MAGIC;
        ddx_sax_parser.startElementNs = &D4ParserSax2::ddx_start_element;
        ddx_sax_parser.endElementNs = &D4ParserSax2::ddx_end_element;

        context->sax = &ddx_sax_parser;
        context->userData = this;
        context->validate = true;

        f.getline(chars, size);
        while ((f.gcount() > 0)) {
            DBG(cerr << "line: (" << f.gcount() << "): " << chars << endl);
            xmlParseChunk(ctxt, chars, f.gcount() - 1, 0);
            f.getline(chars, size);
        }
        // This call ends the parse: The fourth argument of xmlParseChunk is
        // the bool 'terminate.'
        xmlParseChunk(ctxt, chars, 0, 1);

        cleanup_parse(context);
    }
}

/** Parse a DDX document stored in a file. The XML in the document is parsed
 and a binary DDX is built. This implementation stores the result in a DDS
 object where each instance of BaseType can hold an AttrTable object.

 @param document Read the DDX from this file.
 @param dest_dds Value/result parameter; dumps the information to this DDS
 instance.
 @param cid Value/result parameter; puts the href which references the \c
 CID.
 @exception D4ParseError Thrown if the XML document could not be
 read or parsed. */
void D4ParserSax2::intern(const string &document, DDS *dest_dds)
{
    istringstream iss(document);
    intern(iss, dest_dds);
#if 0
    // Create the context pointer explicitly so that we can store a pointer
    // to it in the DDXParserDAP4 instance. This provides a way to generate our
    // own error messages *with* line numbers. The messages are pretty
    // meaningless otherwise. This means that we use an interface from the
    // 'parser internals' header, and not the 'parser' header. However, this
    // interface is also used in one of the documented examples, so it's
    // probably pretty stable. 06/02/03 jhrg
    xmlParserCtxtPtr context = xmlCreateFileParserCtxt(document.c_str());
    if (!context)
        throw D4ParseError(string("Could not initialize the parser with the file: '") + document + string("'."));

    dds = dest_dds; // dump values here
    ctxt = context; // need ctxt for error messages

    xmlSAXHandler ddx_sax_parser;
    memset(&ddx_sax_parser, 0, sizeof(xmlSAXHandler));

    ddx_sax_parser.getEntity = &D4ParserSax2::ddx_get_entity;
    ddx_sax_parser.startDocument = &D4ParserSax2::ddx_start_document;
    ddx_sax_parser.endDocument = &D4ParserSax2::ddx_end_document;
    ddx_sax_parser.characters = &D4ParserSax2::ddx_get_characters;
    ddx_sax_parser.ignorableWhitespace = &D4ParserSax2::ddx_ignoreable_whitespace;
    ddx_sax_parser.cdataBlock = &D4ParserSax2::ddx_get_cdata;
    ddx_sax_parser.warning = &D4ParserSax2::ddx_fatal_error;
    ddx_sax_parser.error = &D4ParserSax2::ddx_fatal_error;
    ddx_sax_parser.fatalError = &D4ParserSax2::ddx_fatal_error;
    ddx_sax_parser.initialized = XML_SAX2_MAGIC;
    ddx_sax_parser.startElementNs = &D4ParserSax2::ddx_start_element;
    ddx_sax_parser.endElementNs = &D4ParserSax2::ddx_end_element;

    context->sax = &ddx_sax_parser;
    context->userData = this;
    context->validate = false;

    xmlParseDocument(context);

    cleanup_parse(context);
#endif
}

} // namespace libdap
