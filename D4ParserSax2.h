
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

#ifndef d4_parser_sax2_h
#define d4_parser_sax2_h

#define ATTR 1

#include <string.h>

#include <iostream>
#include <map>
#include <stack>
#include <string>

#include <libxml/parserInternals.h>

#define CRLF "\r\n"
#define D4_PARSE_BUFF_SIZE 1048576

namespace libdap {

class DMR;
class BaseType;
class D4BaseTypeFactory;
class D4Group;
class D4Attributes;
class D4EnumDef;
class D4Dimension;

/** Parse the XML text which encodes the network/persistent representation of
    the DMR object. In the current implementation, the DMR is held by an
    instance of the class DDS which in turn holds variables which include
    attributes.

    This parser for the DMR document uses the SAX interface of libxml2.
    Static methods are used as callbacks for the SAX parser. These static
    methods are public because making them private complicates compilation.
    They should not be called by anything other than the intern method.
    They do not throw exceptions because exceptions from within callbacks are
    not reliable or portable. To signal errors, the methods record
    information in the D4ParserSax2 object. Once the error handler is called,
    construction of an DMR object ends even though the SAX parser still
    calls the various callback functions. The parser treats warnings,
    errors and fatal_errors the same way; when any are found parsing
    stops. The intern method throws an Error of InternalErr exception if an
    error was found.

    Note that this class uses the C++-supplied default definitions for the
    default and copy constructors as well as the destructor and assignment
    operator.

    @see DMR */
class D4ParserSax2 {
private:
    /** States used by DDXParserDAP4State. These are the states of the SAX parser
    state-machine. */
    enum ParseState {
        parser_start,

        inside_dataset,

        // inside_group is the state just after parsing the start of a Group
        // element.
        inside_group,

        inside_attribute_container,
        inside_attribute,
        inside_attribute_value,
        inside_other_xml_attribute,

        inside_enum_def,
        inside_enum_const,

        inside_dim_def,

        // This covers Byte, ..., Url, Opaque
        inside_simple_type,

        // inside_array,
        inside_dim,
        inside_map,

        inside_constructor,

        // inside_sequence, Removed from merged code jhrg 5/2/14

        not_dap4_element,

        parser_unknown,
        parser_error,
        parser_fatal_error,

        parser_end
    };
    char d_parse_buffer[D4_PARSE_BUFF_SIZE + 1]; // Buff size plus one byte for NULL termination.

    xmlSAXHandler d_dmr_sax_parser;

    // The results of the parse operation are stored in these fields.
    // This is passed into the parser using the intern() methods.
    DMR *d_dmr; // dump DMR here
    DMR *dmr() const { return d_dmr; }

    // These stacks hold the state of the parse as it progresses.
    stack<ParseState> s; // Current parse state
    void push_state(D4ParserSax2::ParseState state) { s.push(state); }
    D4ParserSax2::ParseState get_state() const { return s.top(); }
    void pop_state() { s.pop(); }
    bool empty_state() const { return s.empty(); }

    stack<BaseType *> btp_stack; // current variable(s)
    void push_basetype(BaseType *btp) { btp_stack.push(btp); }
    BaseType *top_basetype() const { return btp_stack.top(); }
    void pop_basetype() { btp_stack.pop(); }
    bool empty_basetype() const { return btp_stack.empty(); }

    stack<D4Group *> grp_stack; // current groups(s)
    void push_group(D4Group *grp) { grp_stack.push(grp); }
    D4Group *top_group() const { return grp_stack.top(); }
    void pop_group() { grp_stack.pop(); }
    bool empty_group() const { return grp_stack.empty(); }

    stack<D4Attributes *> d_attrs_stack; // DAP4 Attributes
    void push_attributes(D4Attributes *attr) { d_attrs_stack.push(attr); }
    D4Attributes *top_attributes() const { return d_attrs_stack.top(); }
    void pop_attributes() { d_attrs_stack.pop(); }
    bool empty_attributes() const { return d_attrs_stack.empty(); }

    D4EnumDef *d_enum_def;
    D4EnumDef *enum_def();
    void clear_enum_def() { d_enum_def = 0; }

    D4Dimension *d_dim_def;
    D4Dimension *dim_def();
    void clear_dim_def() { d_dim_def = 0; }

    // Accumulate stuff inside an 'OtherXML' DAP attribute here
    string other_xml;

    // When we're parsing unknown XML, how deeply is it nested? This is used
    // for the OtherXML DAP attributes.
    unsigned int other_xml_depth;
    unsigned int unknown_depth;

    // These are used for processing errors.
    string d_error_msg;         // Error message(s), if any.
    xmlParserCtxtPtr d_context; // used for error message line numbers

    // These hold temporary values read during the parse.
    string dods_attr_name; // DAP4 attributes, not XML attributes
    string dods_attr_type; // ... not XML ...
    string char_data;      // char data in value elements; null after use
    string root_ns;        // What is the namespace of the root node (Group)

    bool d_debug;
    bool debug() const { return d_debug; }

    bool d_strict;

    class XMLAttribute {
    public:
        string prefix;
        string nsURI;
        string value;

        void clone(const XMLAttribute &src) {
            prefix = src.prefix;
            nsURI = src.nsURI;
            value = src.value;
        }

        XMLAttribute() : prefix(""), nsURI(""), value("") {}
        XMLAttribute(const string &p, const string &ns, const string &v) : prefix(p), nsURI(ns), value(v) {}
        // 'attributes' as passed from libxml2 is a five element array but this
        // ctor gets the back four elements.
        XMLAttribute(const xmlChar **attributes /*[4]*/) {
            prefix = attributes[0] != 0 ? (const char *)attributes[0] : "";
            nsURI = attributes[1] != 0 ? (const char *)attributes[1] : "";
            value = string((const char *)attributes[2], (const char *)attributes[3]);
        }
        XMLAttribute(const XMLAttribute &rhs) { clone(rhs); }
        ~XMLAttribute() {}
        XMLAttribute &operator=(const XMLAttribute &rhs) {
            if (this == &rhs)
                return *this;
            clone(rhs);
            return *this;
        }
    };

    typedef map<string, XMLAttribute> XMLAttrMap;
    XMLAttrMap xml_attrs; // dump XML attributes here

    XMLAttrMap::iterator xml_attr_begin() { return xml_attrs.begin(); }

    XMLAttrMap::iterator xml_attr_end() { return xml_attrs.end(); }

    map<string, string> namespace_table;

    void cleanup_parse();

    /** @name Parser Actions

    These methods are the 'actions' carried out by the start_element and
    end_element callbacks. Most of what takes place in those has been
    factored out to this set of functions. */
    //@{
    void transfer_xml_attrs(const xmlChar **attrs, int nb_attributes);
    void transfer_xml_ns(const xmlChar **namespaces, int nb_namespaces);
    bool check_required_attribute(const string &attr);
    bool check_attribute(const string &attr);
    void process_variable_helper(Type t, ParseState s, const xmlChar **attrs, int nb_attributes);

    void process_enum_const_helper(const xmlChar **attrs, int nb_attributes);
    void process_enum_def_helper(const xmlChar **attrs, int nb_attributes);

    bool process_dimension(const char *name, const xmlChar **attrs, int nb_attrs);
    bool process_dimension_def(const char *name, const xmlChar **attrs, int nb_attrs);
    bool process_map(const char *name, const xmlChar **attrs, int nb_attributes);
    bool process_attribute(const char *name, const xmlChar **attrs, int nb_attributes);
    bool process_variable(const char *name, const xmlChar **attrs, int nb_attributes);
    bool process_group(const char *name, const xmlChar **attrs, int nb_attributes);
    bool process_enum_def(const char *name, const xmlChar **attrs, int nb_attributes);
    bool process_enum_const(const char *name, const xmlChar **attrs, int nb_attributes);

    void finish_variable(const char *tag, Type t, const char *expected);
    //@}

    friend class D4ParserSax2Test;

public:
    D4ParserSax2()
        : d_dmr(0), d_enum_def(0), d_dim_def(0), other_xml(""), other_xml_depth(0), unknown_depth(0), d_error_msg(""),
          d_context(0), dods_attr_name(""), dods_attr_type(""), char_data(""), root_ns(""), d_debug(false),
          d_strict(true) {
        // xmlSAXHandler ddx_sax_parser;
        memset(&d_dmr_sax_parser, 0, sizeof(xmlSAXHandler));

        d_dmr_sax_parser.getEntity = &D4ParserSax2::dmr_get_entity;
        d_dmr_sax_parser.startDocument = &D4ParserSax2::dmr_start_document;
        d_dmr_sax_parser.endDocument = &D4ParserSax2::dmr_end_document;
        d_dmr_sax_parser.characters = &D4ParserSax2::dmr_get_characters;
        d_dmr_sax_parser.ignorableWhitespace = &D4ParserSax2::dmr_ignoreable_whitespace;
        d_dmr_sax_parser.cdataBlock = &D4ParserSax2::dmr_get_cdata;
        d_dmr_sax_parser.warning = &D4ParserSax2::dmr_error;
        d_dmr_sax_parser.error = &D4ParserSax2::dmr_error;
        d_dmr_sax_parser.fatalError = &D4ParserSax2::dmr_fatal_error;
        d_dmr_sax_parser.initialized = XML_SAX2_MAGIC;
        d_dmr_sax_parser.startElementNs = &D4ParserSax2::dmr_start_element;
        d_dmr_sax_parser.endElementNs = &D4ParserSax2::dmr_end_element;
    }

    void intern(istream &f, DMR *dest_dmr, bool debug = false);
    // Deprecated - this does not read from a file, it parses text in the string 'document'
    void intern(const string &document, DMR *dest_dmr, bool debug = false);
    void intern(const char *buffer, int size, DMR *dest_dmr, bool debug = false);

    /**
     * @defgroup strict The 'strict' mode
     * @{
     * The strict mode of the parser is the default. In this mode any error
     * will result in an exception and parsing will stop. When strict mode
     * is set to false (forgiving mode?), some errors will be silently ignored.
     * The list of allowed errors is:
     *     The Array named by a Map element is not required to be in the DMR.
     *     There are no other allowed errors at this time (4/13/16)
     */

    /** @brief Set the 'strict' mode to true or false. */
    void set_strict(bool s) { d_strict = s; }
    /** @brief Get the setting of the 'strict' mode.
     * @return True or False.
     */
    bool get_strict() const { return d_strict; }
    /** @} */

    static void dmr_start_document(void *parser);
    static void dmr_end_document(void *parser);

    static void dmr_start_element(void *parser, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI,
                                  int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted,
                                  const xmlChar **attributes);
    static void dmr_end_element(void *parser, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);

    static void dmr_get_characters(void *parser, const xmlChar *ch, int len);
    static void dmr_ignoreable_whitespace(void *parser, const xmlChar *ch, int len);
    static void dmr_get_cdata(void *parser, const xmlChar *value, int len);

    static xmlEntityPtr dmr_get_entity(void *parser, const xmlChar *name);
    static void dmr_fatal_error(void *parser, const char *msg, ...);
    static void dmr_error(void *parser, const char *msg, ...);
};

} // namespace libdap

#endif // d4_parser_sax2_h
