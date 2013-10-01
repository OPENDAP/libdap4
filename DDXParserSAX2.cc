
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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

//#define DODS_DEBUG 1
//#define DODS_DEBUG2 1

#include <cstring>
#include <cstdarg>

#include "BaseType.h"
#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "DDXParserSAX2.h"

#include "util.h"
#include "mime_util.h"
#include "debug.h"

namespace libdap {

#if defined(DODS_DEBUG) || defined(DODS_DEUG2)
static const char *states[] =
    {
        "start",

        "dataset",

        "attribute_container",
        "attribute",
        "attribute_value",
        "other_xml_attribute",

        "alias",

        "simple_type",

        "array",
        "dimension",

        "grid",
        "map",

        "structure",
        "sequence",

        "blob href",

        "unknown",
        "error"
    };
#endif
// Glue the BaseTypeFactory to the enum-based factory defined statically
// here.

BaseType *DDXParser::factory(Type t, const string & name)
{
    switch (t) {
    case dods_byte_c:
        return d_factory->NewByte(name);
        break;

    case dods_int16_c:
        return d_factory->NewInt16(name);
        break;

    case dods_uint16_c:
        return d_factory->NewUInt16(name);
        break;

    case dods_int32_c:
        return d_factory->NewInt32(name);
        break;

    case dods_uint32_c:
        return d_factory->NewUInt32(name);
        break;

    case dods_float32_c:
        return d_factory->NewFloat32(name);
        break;

    case dods_float64_c:
        return d_factory->NewFloat64(name);
        break;

    case dods_str_c:
        return d_factory->NewStr(name);
        break;

    case dods_url_c:
        return d_factory->NewUrl(name);
        break;

    case dods_array_c:
        return d_factory->NewArray(name);
        break;

    case dods_structure_c:
        return d_factory->NewStructure(name);
        break;

    case dods_sequence_c:
        return d_factory->NewSequence(name);
        break;

    case dods_grid_c:
        return d_factory->NewGrid(name);
        break;

    default:
        return 0;
    }
}

#if 0
/** Get the Type enumeration value which matches the given name. */
static Type get_type(const char *name)
{
    if (strcmp(name, "Byte") == 0)
        return dods_byte_c;

    if (strcmp(name, "Int16") == 0)
        return dods_int16_c;

    if (strcmp(name, "UInt16") == 0)
        return dods_uint16_c;

    if (strcmp(name, "Int32") == 0)
        return dods_int32_c;

    if (strcmp(name, "UInt32") == 0)
        return dods_uint32_c;

    if (strcmp(name, "Float32") == 0)
        return dods_float32_c;

    if (strcmp(name, "Float64") == 0)
        return dods_float64_c;

    if (strcmp(name, "String") == 0)
        return dods_str_c;

    if (strcmp(name, "Url") == 0)
        return dods_url_c;

    if (strcmp(name, "Array") == 0)
        return dods_array_c;

    if (strcmp(name, "Structure") == 0)
        return dods_structure_c;

    if (strcmp(name, "Sequence") == 0)
        return dods_sequence_c;

    if (strcmp(name, "Grid") == 0)
        return dods_grid_c;

    return dods_null_c;
}
#endif

#if 0
// Not used. jhrg 1/17/13
static Type is_simple_type(const char *name)
{
    Type t = get_type(name);
    switch (t) {
    case dods_byte_c:
    case dods_int16_c:
    case dods_uint16_c:
    case dods_int32_c:
    case dods_uint32_c:
    case dods_float32_c:
    case dods_float64_c:
    case dods_str_c:
    case dods_url_c:
        return t;
    default:
        return dods_null_c;
    }
}
#endif

static bool is_not(const char *name, const char *tag)
{
    return strcmp(name, tag) != 0;
}

void DDXParser::set_state(DDXParser::ParseState state)
{
    s.push(state);
}

DDXParser::ParseState DDXParser::get_state() const
{
    return s.top();
}

void DDXParser::pop_state()
{
    s.pop();
}

/** Dump XML attributes to local store so they can be easily manipulated.
    Attribute names are always folded to lower case.
    @param attrs The XML attribute array */
void DDXParser::transfer_xml_attrs(const xmlChar **attributes, int nb_attributes)
{
    if (!attribute_table.empty())
        attribute_table.clear(); // erase old attributes

    unsigned int index = 0;
    for (int i = 0; i < nb_attributes; ++i, index += 5) {
        // Make a value using the attribute name and the prefix, namespace URI
        // and the value. The prefix might be null.
        attribute_table.insert(map<string, XMLAttribute>::value_type(
                string((const char *)attributes[index]),
                XMLAttribute(attributes + index + 1)));

        DBG(cerr << "Attribute '" << (const char *)attributes[index] << "': "
                << attribute_table[(const char *)attributes[index]].value << endl);
    }
}

void DDXParser::transfer_xml_ns(const xmlChar **namespaces, int nb_namespaces)
{
    for (int i = 0; i < nb_namespaces; ++i ) {
        // make a value with the prefix and namespace URI. The prefix might be
        // null.
        namespace_table.insert(map<string,string>::value_type(
                namespaces[i*2] != 0 ? (const char *)namespaces[i*2] : "",
                (const char *)namespaces[i*2+1]));
    }
}

/** Is an attribute present? Attribute names are always lower case.
    @note To use this method, first call transfer_xml_attrs.
    @param attr The XML attribute
    @return True if the XML attribute was present in the last tag */
bool DDXParser::check_required_attribute(const string & attr)
{
    map < string, XMLAttribute >::iterator i = attribute_table.find(attr);
    if (i == attribute_table.end())
        ddx_fatal_error(this, "Required attribute '%s' not found.",
                        attr.c_str());
    return true;
}

/** Is an attribute present? Attribute names are always lower case.
    @note To use this method, first call transfer_xml_attrs.
    @param attr The XML attribute
    @return True if the XML attribute was present in the last/current tag,
    false otherwise. */
bool DDXParser::check_attribute(const string & attr)
{
    return (attribute_table.find(attr) != attribute_table.end());
}

/** Given that an \c Attribute tag has just been read, determine whether the
    element is a container or a simple type, set the state and, for a simple
    type record the type and name for use when \c value elements are found.

    @note Modified to discriminate between OtherXML and the older DAP2.0
    attribute types (container, Byte, ...).

    @param attrs The array of XML attribute values */
void DDXParser::process_attribute_element(const xmlChar **attrs, int nb_attributes)
{
    // These methods set the state to parser_error if a problem is found.
    transfer_xml_attrs(attrs, nb_attributes);

    bool error = !(check_required_attribute(string("name"))
                   && check_required_attribute(string("type")));
    if (error)
        return;

    if (attribute_table["type"].value == "Container") {
        set_state(inside_attribute_container);

        AttrTable *child;
        AttrTable *parent = at_stack.top();

        child = parent->append_container(attribute_table["name"].value);
        at_stack.push(child);   // save.
        DBG2(cerr << "Pushing at" << endl);
    }
    else if (attribute_table["type"].value == "OtherXML") {
        set_state(inside_other_xml_attribute);

        dods_attr_name = attribute_table["name"].value;
        dods_attr_type = attribute_table["type"].value;
    }
    else {
        set_state(inside_attribute);
        // *** Modify parser. Add a special state for inside OtherXML since it
        // does not use the <value> element.

        dods_attr_name = attribute_table["name"].value;
        dods_attr_type = attribute_table["type"].value;
    }
}

/** Given that an \c Alias tag has just been read, set the state and process
    the alias.
    @param attrs The XML attribute array */
void DDXParser::process_attribute_alias(const xmlChar **attrs, int nb_attributes)
{
    transfer_xml_attrs(attrs, nb_attributes);
    if (check_required_attribute(string("name"))
        && check_required_attribute(string("attribute"))) {
        set_state(inside_alias);
        at_stack.top()->attr_alias(attribute_table["name"].value,
                                   attribute_table["attribute"].value);
    }
}

/** Given that a tag which opens a variable declaration has just been read,
    create the variable. Once created, push the variable onto the stack of
    variables, push that variables attribute table onto the attribute table
    stack and update the state of the parser.
    @param t The type of variable to create.
    @param s The next state of the parser.
    @param attrs the attributes read with the tag */
void DDXParser::process_variable(Type t, ParseState s, const xmlChar **attrs,
        int nb_attributes)
{
    transfer_xml_attrs(attrs, nb_attributes);

    set_state(s);
    if (bt_stack.top()->type() == dods_array_c
            || check_required_attribute("name")) { // throws on error/false
        BaseType *btp = factory(t, attribute_table["name"].value);
        if (!btp)
            ddx_fatal_error(
                    this,
                    "Internal parser error; could not instantiate the variable '%s'.",
                    attribute_table["name"].value.c_str());

        // Once we make the new variable, we not only load it on to the
        // BaseType stack, we also load its AttrTable on the AttrTable stack.
        // The attribute processing software always operates on the AttrTable
        // at the top of the AttrTable stack (at_stack).
        bt_stack.push(btp);
        at_stack.push(&btp->get_attr_table());
    }
}

/** Given that a \c dimension tag has just been read, add that information to
    the array on the top of the BaseType stack.
    @param attrs The XML attributes included in the \c dimension tag */
void DDXParser::process_dimension(const xmlChar **attrs, int nb_attributes)
{
    transfer_xml_attrs(attrs, nb_attributes);
    if (check_required_attribute(string("size"))) {
        set_state(inside_dimension);
        Array *ap = dynamic_cast < Array * >(bt_stack.top());
		if (!ap) {
			ddx_fatal_error(this, "Parse error: Expected an array variable.");
			return;
		}
		
        ap->append_dim(atoi(attribute_table["size"].value.c_str()),
                       attribute_table["name"].value);
    }
}

/** Given that a \c blob tag has just been read, extract and save the CID
    included in the element. */
void DDXParser::process_blob(const xmlChar **attrs, int nb_attributes)
{
    transfer_xml_attrs(attrs, nb_attributes);
    if (check_required_attribute(string("href"))) {
        set_state(inside_blob_href);
        *blob_href = attribute_table["href"].value;
    }
}

/** Check to see if the current tag is either an \c Attribute or an \c Alias
    start tag. This method is a glorified macro...

    @param name The start tag name
    @param attrs The tag's XML attributes
    @return True if the tag was an \c Attribute or \c Alias tag */
inline bool
DDXParser::is_attribute_or_alias(const char *name, const xmlChar **attrs,
        int nb_attributes)
{
    if (strcmp(name, "Attribute") == 0) {
        process_attribute_element(attrs, nb_attributes);
        // next state: inside_attribtue or inside_attribute_container
        return true;
    }
    else if (strcmp(name, "Alias") == 0) {
        process_attribute_alias(attrs, nb_attributes);
        // next state: inside_alias
        return true;
    }

    return false;
}

/** Check to see if the current tag is the start of a variable declaration.
    If so, process it. A glorified macro...
    @param name The start tag name
    @param attrs The tag's XML attributes
    @return True if the tag was a variable tag */
inline bool DDXParser::is_variable(const char *name, const xmlChar **attrs,
        int nb_attributes)
{
    Type t = get_type(name);
    //if ((t = is_simple_type(name)) != dods_null_c) {
    if (is_simple_type(t)) {
        process_variable(t, inside_simple_type, attrs, nb_attributes);
        return true;
    }
    else if (strcmp(name, "Array") == 0) {
        process_variable(dods_array_c, inside_array, attrs, nb_attributes);
        return true;
    }
    else if (strcmp(name, "Structure") == 0) {
        process_variable(dods_structure_c, inside_structure, attrs, nb_attributes);
        return true;
    }
    else if (strcmp(name, "Sequence") == 0) {
        process_variable(dods_sequence_c, inside_sequence, attrs, nb_attributes);
        return true;
    }
    else if (strcmp(name, "Grid") == 0) {
        process_variable(dods_grid_c, inside_grid, attrs, nb_attributes);
        return true;
    }

    return false;
}

void DDXParser::finish_variable(const char *tag, Type t, const char *expected)
{
    if (strcmp(tag, expected) != 0) {
        DDXParser::ddx_fatal_error(this,
                                   "Expected an end tag for a %s; found '%s' instead.",
                                   expected, tag);
        return;
    }

    pop_state();

    BaseType *btp = bt_stack.top();

    bt_stack.pop();
    at_stack.pop();

    if (btp->type() != t) {
        DDXParser::ddx_fatal_error(this,
                                   "Internal error: Expected a %s variable.",
                                   expected);
        return;
    }
    // Once libxml2 validates, this can go away. 05/30/03 jhrg
    if (t == dods_array_c
        && static_cast<Array*>(btp)->dimensions() == 0) {
        DDXParser::ddx_fatal_error(this,
                                   "No dimension element included in the Array '%s'.",
                                   btp->name().c_str());
        delete btp;
        return;
    }

    BaseType *parent = bt_stack.top();

    if (!(parent->is_vector_type() || parent->is_constructor_type())) {
        DDXParser::ddx_fatal_error(this,
                                   "Tried to add the array variable '%s' to a non-constructor type (%s %s).",
                                   tag,
                                   bt_stack.top()->type_name().c_str(),
                                   bt_stack.top()->name().c_str());
        delete btp;
        return;
    }

    parent->add_var(btp);
    delete btp;
}

/** @name SAX Parser Callbacks

    These methods are declared static in the class header. This gives them C
    linkage which allows them to be used as callbacks by the SAX parser
    engine. */
//@{

/** Initialize the SAX parser state object. This object is passed to each
    callback as a void pointer. The initial state is parser_start.

    @param p The SAX parser  */
void DDXParser::ddx_start_document(void * p)
{
    DDXParser *parser = static_cast<DDXParser*>(p);
    parser->error_msg = "";
    parser->char_data = "";

    // init attr table stack.
    parser->at_stack.push(&parser->dds->get_attr_table());

    // Trick; DDS *should* be a child of Structure. To simplify parsing,
    // stuff a Structure on the bt_stack and dump the top level variables
    // there. Once we're done, transfer the variables to the DDS.
    parser->bt_stack.push(new Structure("dummy_dds"));

    parser->set_state(parser_start);

    DBG2(cerr << "Parser state: " << states[parser->get_state()] << endl);
}

/** Clean up after finishing a parse.
    @param p The SAX parser  */
void DDXParser::ddx_end_document(void * p)
{
    DDXParser *parser = static_cast<DDXParser*>(p);
    DBG2(cerr << "Ending state == " << states[parser->get_state()] <<
         endl);

    if (parser->get_state() != parser_start)
        DDXParser::ddx_fatal_error(parser, "The document contained unbalanced tags.");

    // If we've found any sort of error, don't make the DDX; intern() will
    // take care of the error.
    if (parser->get_state() == parser_error) {
    	delete parser->bt_stack.top();
        return;
    }

    // Pop the temporary Structure off the stack and transfer its variables
    // to the DDS.
    Constructor *cp = dynamic_cast < Constructor * >(parser->bt_stack.top());
    if (!cp) {
    	ddx_fatal_error(parser, "Parse error: Expected a Structure, Sequence or Grid variable.");
    	delete cp;
		return;
    }

    for (Constructor::Vars_iter i = cp->var_begin(); i != cp->var_end(); ++i) {
        (*i)->set_parent(0);        // top-level vars have no parents
        parser->dds->add_var(*i);
    }

    parser->bt_stack.pop();
    delete cp;
}

void DDXParser::ddx_sax2_start_element(void *p,
        const xmlChar *l, const xmlChar *prefix, const xmlChar *URI,
        int nb_namespaces, const xmlChar **namespaces,
        int nb_attributes, int /*nb_defaulted*/, const xmlChar **attributes)
{
    DDXParser *parser = static_cast<DDXParser*>(p);
    const char *localname = (const char *)l;

    DBG2(cerr << "start element: " << localname << ", states: "
         << states[parser->get_state()]);

    switch (parser->get_state()) {
    case parser_start:
        if (strcmp(localname, "Dataset") == 0) {
            parser->set_state(inside_dataset);
            parser->root_ns = URI != 0 ? (const char *)URI: "";
            parser->transfer_xml_attrs(attributes, nb_attributes);

            if (parser->check_required_attribute(string("name")))
                parser->dds->set_dataset_name(parser->attribute_table["name"].value);

            if (parser->check_attribute("dapVersion"))
                parser->dds->set_dap_version(parser->attribute_table["dapVersion"].value);
        }
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected response to start with a Dataset element; found '%s' instead.",
                                       localname);
        break;

    case inside_dataset:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else if (parser->is_variable(localname, attributes, nb_attributes))
            break;
        else if (strcmp(localname, "blob") == 0 || strcmp(localname, "dataBLOB") == 0) {
            parser->process_blob(attributes, nb_attributes);
            // next state: inside_data_blob
        }
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an Attribute, Alias or variable element; found '%s' instead.",
                                       localname);
        break;

    case inside_attribute_container:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an Attribute or Alias element; found '%s' instead.",
                                       localname);
        break;

    case inside_attribute:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else if (strcmp(localname, "value") == 0)
            parser->set_state(inside_attribute_value);
        else
            ddx_fatal_error(parser,
                            "Expected an 'Attribute', 'Alias' or 'value' element; found '%s' instead.",
                            localname);
        break;

    case inside_attribute_value:
        ddx_fatal_error(parser,
                        "Internal parser error; unexpected state, inside value while processing element '%s'.",
                        localname);
        break;

    case inside_other_xml_attribute:
        DBGN(cerr << endl << "\t inside_other_xml_attribute: " << localname << endl);

        parser->other_xml_depth++;

        // Accumulate the elements here

        parser->other_xml.append("<");
        if (prefix) {
            parser->other_xml.append((const char *)prefix);
            parser->other_xml.append(":");
        }
        parser->other_xml.append(localname);

        if (nb_namespaces != 0) {
            parser->transfer_xml_ns(namespaces, nb_namespaces);

            for (map<string,string>::iterator i = parser->namespace_table.begin();
                i != parser->namespace_table.end();
                ++i) {
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
            for (XMLAttrMap::iterator i = parser->attr_table_begin();
                i != parser->attr_table_end();
                ++i) {
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

    case inside_alias:
        ddx_fatal_error(parser,
                        "Internal parser error; unexpected state, inside alias while processing element '%s'.",
                        localname);
        break;

    case inside_simple_type:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else
            ddx_fatal_error(parser,
                            "Expected an 'Attribute' or 'Alias' element; found '%s' instead.",
                            localname);
        break;

    case inside_array:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else if (is_not(localname, "Array")
                && parser->is_variable(localname, attributes, nb_attributes))
            break;
        else if (strcmp(localname, "dimension") == 0) {
            parser->process_dimension(attributes, nb_attributes);
            // next state: inside_dimension
        }
        else
            ddx_fatal_error(parser,
                            "Expected an 'Attribute' or 'Alias' element; found '%s' instead.",
                            localname);
        break;

    case inside_dimension:
        ddx_fatal_error(parser,
                        "Internal parser error; unexpected state, inside dimension while processing element '%s'.",
                        localname);
        break;

    case inside_structure:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else if (parser->is_variable(localname, attributes, nb_attributes))
            break;
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an Attribute, Alias or variable element; found '%s' instead.",
                                       localname);
        break;

    case inside_sequence:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else if (parser->is_variable(localname, attributes, nb_attributes))
            break;
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an Attribute, Alias or variable element; found '%s' instead.",
                                       localname);
        break;

    case inside_grid:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else if (strcmp(localname, "Array") == 0)
            parser->process_variable(dods_array_c, inside_array, attributes, nb_attributes);
        else if (strcmp(localname, "Map") == 0)
            parser->process_variable(dods_array_c, inside_map, attributes, nb_attributes);
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an Attribute, Alias or variable element; found '%s' instead.",
                                       localname);
        break;

    case inside_map:
        if (parser->is_attribute_or_alias(localname, attributes, nb_attributes))
            break;
        else if (is_not(localname, "Array") && is_not(localname, "Sequence")
                 && is_not(localname, "Grid")
                 && parser->is_variable(localname, attributes, nb_attributes))
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

    case inside_blob_href:
        ddx_fatal_error(parser,
                        "Internal parser error; unexpected state, inside blob href while processing element '%s'.",
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

void DDXParser::ddx_sax2_end_element(void *p, const xmlChar *l,
        const xmlChar *prefix, const xmlChar *URI)
{
    DDXParser *parser = static_cast<DDXParser*>(p);
    const char *localname = (const char *)l;

    DBG2(cerr << "End element " << localname << " (state "
         << states[parser->get_state()] << ")" << endl);

    switch (parser->get_state()) {
    case parser_start:
        ddx_fatal_error(parser,
                        "Internal parser error; unexpected state, inside start state while processing element '%s'.",
                        localname);
        break;

    case inside_dataset:
        if (strcmp(localname, "Dataset") == 0)
            parser->pop_state();
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an end Dataset tag; found '%s' instead.",
                                       localname);
        break;

    case inside_attribute_container:
        if (strcmp(localname, "Attribute") == 0) {
            parser->pop_state();
            parser->at_stack.pop();     // pop when leaving a container.
        }
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an end Attribute tag; found '%s' instead.",
                                       localname);
        break;

    case inside_attribute:
        if (strcmp(localname, "Attribute") == 0)
            parser->pop_state();
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an end Attribute tag; found '%s' instead.",
                                       localname);
        break;

    case inside_attribute_value:
        if (strcmp(localname, "value") == 0) {
            parser->pop_state();
            AttrTable *atp = parser->at_stack.top();
            atp->append_attr(parser->dods_attr_name,
                             parser->dods_attr_type, parser->char_data);
            parser->char_data = "";     // Null this after use.
        }
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an end value tag; found '%s' instead.",
                                       localname);

        break;

    case inside_other_xml_attribute: {
            if (strcmp(localname, "Attribute") == 0
                    && parser->root_ns == (const char *)URI) {

                DBGN(cerr << endl << "\t Popping the 'inside_other_xml_attribute' state"
                        << endl);

                parser->pop_state();

                AttrTable *atp = parser->at_stack.top();
                atp->append_attr(parser->dods_attr_name,
                        parser->dods_attr_type, parser->other_xml);

                parser->other_xml = ""; // Null this after use.
            }
            else {
                DBGN(cerr << endl << "\t inside_other_xml_attribute: " << localname
                        << ", depth: " << parser->other_xml_depth << endl);
                if (parser->other_xml_depth == 0)
                    DDXParser::ddx_fatal_error(parser,
                                               "Expected an OtherXML attribute to end! Instead I found '%s'",
                                               localname);
                parser->other_xml_depth--;

                parser->other_xml.append("</");
                if (prefix) {
                    parser->other_xml.append((const char *)prefix);
                    parser->other_xml.append(":");
                }
                parser->other_xml.append(localname);
                parser->other_xml.append(">");
            }
            break;
        }
        // Alias is busted in libdap++ 05/29/03 jhrg
    case inside_alias:
        parser->pop_state();
        break;

    case inside_simple_type: {
        Type t = get_type(localname);
        if (is_simple_type(t)) {
            parser->pop_state();
            BaseType *btp = parser->bt_stack.top();
            parser->bt_stack.pop();
            parser->at_stack.pop();

            BaseType *parent = parser->bt_stack.top();

            if (parent->is_vector_type() || parent->is_constructor_type()) {
                parent->add_var(btp);
                delete btp;
            }
            else
                DDXParser::ddx_fatal_error(parser,
                                           "Tried to add the simple-type variable '%s' to a non-constructor type (%s %s).",
                                           localname,
                                           parser->bt_stack.top()->
                                           type_name().c_str(),
                                           parser->bt_stack.top()->name().
                                           c_str());
        }
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an end tag for a simple type; found '%s' instead.",
                                       localname);
        break;
    }

    case inside_array:
        parser->finish_variable(localname, dods_array_c, "Array");
        break;

    case inside_dimension:
        if (strcmp(localname, "dimension") == 0)
            parser->pop_state();
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an end dimension tag; found '%s' instead.",
                                       localname);
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

    case inside_blob_href:
        if (strcmp(localname, "blob") == 0 || strcmp(localname, "dataBLOB") == 0)
            parser->pop_state();
        else
            DDXParser::ddx_fatal_error(parser,
                                       "Expected an end dataBLOB/blob tag; found '%s' instead.",
                                       localname);
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
void DDXParser::ddx_get_characters(void * p, const xmlChar * ch, int len)
{
    DDXParser *parser = static_cast<DDXParser*>(p);

    switch (parser->get_state()) {
        case inside_attribute_value:
            parser->char_data.append((const char *)(ch), len);
            DBG2(cerr << "Characters: '" << parser->char_data << "'" << endl);
            break;

        case inside_other_xml_attribute:
            parser->other_xml.append((const char *)(ch), len);
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
void DDXParser::ddx_ignoreable_whitespace(void *p, const xmlChar *ch,
        int len)
{
    DDXParser *parser = static_cast<DDXParser*>(p);

    switch (parser->get_state()) {
         case inside_other_xml_attribute:
             parser->other_xml.append((const char *)(ch), len);
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
void DDXParser::ddx_get_cdata(void *p, const xmlChar *value, int len)
{
    DDXParser *parser = static_cast<DDXParser*>(p);

    switch (parser->get_state()) {
         case inside_other_xml_attribute:
             parser->other_xml.append((const char *)(value), len);
             break;

         case parser_unknown:
             break;

         default:
             DDXParser::ddx_fatal_error(parser,
                                        "Found a CData block but none are allowed by DAP.");

             break;
    }
}

/** Handle the standard XML entities.

    @param parser The SAX parser
    @param name The XML entity. */
xmlEntityPtr DDXParser::ddx_get_entity(void *, const xmlChar * name)
{
    return xmlGetPredefinedEntity(name);
}

/** Process an XML fatal error. Note that SAX provides for warnings, errors
    and fatal errors. This code treats them all as fatal errors since there's
    typically no way to tell a user about the error since there's often no
    user interface for this software.

    @param p The SAX parser
    @param msg A printf-style format string. */
void DDXParser::ddx_fatal_error(void * p, const char *msg, ...)
{
    va_list args;
    DDXParser *parser = static_cast<DDXParser*>(p);

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

void DDXParser::cleanup_parse(xmlParserCtxtPtr & context) const
{
    if (!context->wellFormed) {
        context->sax = NULL;
        xmlFreeParserCtxt(context);
        throw
        DDXParseFailed(string
                       ("\nThe DDX is not a well formed XML document.\n")
                       + error_msg);
    }

    if (!context->valid) {
        context->sax = NULL;
        xmlFreeParserCtxt(context);
        throw DDXParseFailed(string("\nThe DDX is not a valid document.\n")
                             + error_msg);
    }

    if (get_state() == parser_error) {
        context->sax = NULL;
        xmlFreeParserCtxt(context);
        throw DDXParseFailed(string("\nError parsing DDX response.\n") +
                             error_msg);
    }

    context->sax = NULL;
    xmlFreeParserCtxt(context);
}

/** Read a DDX from a C++ input stream and populate a DDS object.
 *
 * @param in
 * @param dds
 * @param cid
 * @param boundary
 */
void DDXParser::intern_stream(istream &in, DDS *dest_dds, string &cid, const string &boundary)
{
    // Code example from libxml2 docs re: read from a stream.
    if (!in || in.eof())
        throw InternalErr(__FILE__, __LINE__, "Input stream not open or read error");

    const int size = 1024;
    char chars[size + 1];

    // int res = fread(chars, 1, 4, in);
    in.readsome(chars, 4);
    int res = in.gcount();
    if (res > 0) {
        chars[4]='\0';
        xmlParserCtxtPtr context = xmlCreatePushParserCtxt(NULL, NULL, chars, res, "stream");

        ctxt = context;         // need ctxt for error messages
        dds = dest_dds;         // dump values here
        blob_href = &cid; 	// cid goes here

        xmlSAXHandler ddx_sax_parser;
        memset( &ddx_sax_parser, 0, sizeof(xmlSAXHandler) );

        ddx_sax_parser.getEntity = &DDXParser::ddx_get_entity;
        ddx_sax_parser.startDocument = &DDXParser::ddx_start_document;
        ddx_sax_parser.endDocument = &DDXParser::ddx_end_document;
        ddx_sax_parser.characters = &DDXParser::ddx_get_characters;
        ddx_sax_parser.ignorableWhitespace = &DDXParser::ddx_ignoreable_whitespace;
        ddx_sax_parser.cdataBlock = &DDXParser::ddx_get_cdata;
        ddx_sax_parser.warning = &DDXParser::ddx_fatal_error;
        ddx_sax_parser.error = &DDXParser::ddx_fatal_error;
        ddx_sax_parser.fatalError = &DDXParser::ddx_fatal_error;
        ddx_sax_parser.initialized = XML_SAX2_MAGIC;
        ddx_sax_parser.startElementNs = &DDXParser::ddx_sax2_start_element;
        ddx_sax_parser.endElementNs = &DDXParser::ddx_sax2_end_element;

        context->sax = &ddx_sax_parser;
        context->userData = this;
        context->validate = true;

        in.getline(chars, size);	// chars has size+1 elements
        res = in.gcount();
        chars[res-1] = '\n';		// libxml needs the newline; w/o it the parse will fail
        chars[res] = '\0';
        while (res > 0 && !is_boundary(chars, boundary)) {
        	DBG(cerr << "line (" << res << "): " << chars << endl);
        	xmlParseChunk(ctxt, chars, res, 0);

        	in.getline(chars, size);	// chars has size+1 elements
        	res = in.gcount();
        	if (res > 0) {
        		chars[res-1] = '\n';
        		chars[res] = '\0';
        	}
        }

        // This call ends the parse: The fourth argument of xmlParseChunk is
        // the bool 'terminate.'
        xmlParseChunk(ctxt, chars, 0, 1);

        cleanup_parse(context);
    }
}

/** @brief Read the DDX from a stream instead of a file.
    @see DDXParser::intern(). */
void DDXParser::intern_stream(FILE *in, DDS *dest_dds, string &cid, const string &boundary)
{
    // Code example from libxml2 docs re: read from a stream.
    if (!in || feof(in) || ferror(in))
        throw InternalErr(__FILE__, __LINE__,
                          "Input stream not open or read error");

    const int size = 1024;
    char chars[size];

    int res = fread(chars, 1, 4, in);
    if (res > 0) {
        chars[4]='\0';
        xmlParserCtxtPtr context =
            xmlCreatePushParserCtxt(NULL, NULL, chars, res, "stream");

        ctxt = context;         // need ctxt for error messages
        dds = dest_dds;         // dump values here
        blob_href = &cid; 	// cid goes here

        xmlSAXHandler ddx_sax_parser;
        memset( &ddx_sax_parser, 0, sizeof(xmlSAXHandler) );

        ddx_sax_parser.getEntity = &DDXParser::ddx_get_entity;
        ddx_sax_parser.startDocument = &DDXParser::ddx_start_document;
        ddx_sax_parser.endDocument = &DDXParser::ddx_end_document;
        ddx_sax_parser.characters = &DDXParser::ddx_get_characters;
        ddx_sax_parser.ignorableWhitespace = &DDXParser::ddx_ignoreable_whitespace;
        ddx_sax_parser.cdataBlock = &DDXParser::ddx_get_cdata;
        ddx_sax_parser.warning = &DDXParser::ddx_fatal_error;
        ddx_sax_parser.error = &DDXParser::ddx_fatal_error;
        ddx_sax_parser.fatalError = &DDXParser::ddx_fatal_error;
        ddx_sax_parser.initialized = XML_SAX2_MAGIC;
        ddx_sax_parser.startElementNs = &DDXParser::ddx_sax2_start_element;
        ddx_sax_parser.endElementNs = &DDXParser::ddx_sax2_end_element;

        context->sax = &ddx_sax_parser;
        context->userData = this;
        context->validate = true;


        while ((fgets(chars, size, in) > 0) && !is_boundary(chars, boundary)) {
            //chars[size-1] = '\0';
            DBG(cerr << "line (" << strlen(chars) << "): " << chars << endl);

            xmlParseChunk(ctxt, chars, strlen(chars), 0);
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
    @exception DDXParseFailed Thrown if the XML document could not be
    read or parsed. */
void DDXParser::intern(const string & document, DDS * dest_dds, string &cid)
{
    // Create the context pointer explicitly so that we can store a pointer
    // to it in the DDXParser instance. This provides a way to generate our
    // own error messages *with* line numbers. The messages are pretty
    // meaningless otherwise. This means that we use an interface from the
    // 'parser internals' header, and not the 'parser' header. However, this
    // interface is also used in one of the documented examples, so it's
    // probably pretty stable. 06/02/03 jhrg
    xmlParserCtxtPtr context = xmlCreateFileParserCtxt(document.c_str());
    if (!context)
        throw
        DDXParseFailed(string
                       ("Could not initialize the parser with the file: '")
                       + document + string("'."));

    dds = dest_dds;             // dump values here
    blob_href = &cid;
    ctxt = context;             // need ctxt for error messages

    xmlSAXHandler ddx_sax_parser;
    memset( &ddx_sax_parser, 0, sizeof(xmlSAXHandler) );

    ddx_sax_parser.getEntity = &DDXParser::ddx_get_entity;
    ddx_sax_parser.startDocument = &DDXParser::ddx_start_document;
    ddx_sax_parser.endDocument = &DDXParser::ddx_end_document;
    ddx_sax_parser.characters = &DDXParser::ddx_get_characters;
    ddx_sax_parser.ignorableWhitespace = &DDXParser::ddx_ignoreable_whitespace;
    ddx_sax_parser.cdataBlock = &DDXParser::ddx_get_cdata;
    ddx_sax_parser.warning = &DDXParser::ddx_fatal_error;
    ddx_sax_parser.error = &DDXParser::ddx_fatal_error;
    ddx_sax_parser.fatalError = &DDXParser::ddx_fatal_error;
    ddx_sax_parser.initialized = XML_SAX2_MAGIC;
    ddx_sax_parser.startElementNs = &DDXParser::ddx_sax2_start_element;
    ddx_sax_parser.endElementNs = &DDXParser::ddx_sax2_end_element;

    context->sax = &ddx_sax_parser;
    context->userData = this;
    context->validate = false;

    xmlParseDocument(context);

    cleanup_parse(context);
}

} // namespace libdap
