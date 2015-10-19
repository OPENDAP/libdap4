
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

#ifndef ddx_parser_h
#define ddx_parser_h

#include <string>
#include <map>
#include <stack>

#include <libxml/parserInternals.h>

#ifndef ddx_exceptions_h
#include "DDXExceptions.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef base_type_factory_h
#include "BaseTypeFactory.h"
#endif

namespace libdap
{

/** Parse the XML text which encodes the network/persistent representation of
    the DDX object. In the current implementation, the DDX is held by an
    instance of the class DDS which in turn holds variables which include
    attributes. That is, the binary \e implementation of a DDX uses the old
    DDS, BaseType and AttrTable classes, albeit arranged in a slightly new
    way.

    This parser for the DDX \e document uses the SAX interface of \c libxml2.
    Static methods are used as callbacks for the SAX parser. These static
    methods are public because making them private complicates compilation.
    They should not be called by anything other than the \e intern method.
    They do not throw exceptions because exceptions from within callbacks are
    not reliable or portable. To signal errors, the methods record
    information in the DDXParser object. Once the error handler is called,
    construction of an DDX/DDS object ends even though the SAX parser still
    calls the various callback functions. The parser treats \e warnings, \e
    errors and \e fatal_errors the same way; when any are found parsing
    stops. The \e intern method throws an DDXParseFailed exception if an
    error was found.

    Note that this class uses the C++-supplied default definitions for the
    default and copy constructors as well as the destructor and assignment
    operator.

    @see DDS */
class DDXParser
{
private:
    /** States used by DDXParserState. These are the states of the SAX parser
    state-machine. */
    enum ParseState {
        parser_start,

        inside_dataset,

        inside_attribute_container,
        inside_attribute,
        inside_attribute_value,
        inside_other_xml_attribute,

        inside_alias,

        // This covers Byte, ..., Url.
        inside_simple_type,

        inside_array,
        inside_dimension,

        inside_grid,
        inside_map,

        inside_structure,
        inside_sequence,

        inside_blob_href,

        parser_unknown,
        parser_error
    };

    BaseTypeFactory *d_factory;

    // These stacks hold the state of the parse as it progresses.
    stack<ParseState> s; // Current parse state
    stack<BaseType*> bt_stack; // current variable(s)
    stack<AttrTable*> at_stack; // current attribute table

    // Accumulate stuff inside an 'OtherXML' DAP attribute here
    string other_xml;

    // When we're parsing unknown XML, how deeply is it nested? This is used
    // for the OtherXML DAP attributes.
    unsigned int other_xml_depth;
    unsigned int unknown_depth;

    // These are used for processing errors.
    string error_msg;  // Error message(s), if any.
    xmlParserCtxtPtr ctxt; // used for error message line numbers

    // The results of the parse operation are stored in these fields.
    DDS *dds;   // dump DDX here
    string *blob_href;  // put href to blob here

    // These hold temporary values read during the parse.
    string dods_attr_name; // DAP2 attributes, not XML attributes
    string dods_attr_type; // ... not XML ...
    string char_data;  // char data in value elements; null after use
    string root_ns;     // What is the namespace of the root node (Dataset)

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
        XMLAttribute(const string &p, const string &ns, const string &v)
            : prefix(p), nsURI(ns), value(v) {}
        // 'attributes' as passed from libxml2 is a five element array but this
        // ctor gets the back four elements.
        XMLAttribute(const xmlChar **attributes/*[4]*/) {
            prefix = attributes[0] != 0 ? (const char *)attributes[0]: "";
            nsURI = attributes[1] != 0 ? (const char *)attributes[1]: "";
            value = string((const char *)attributes[2], (const char *)attributes[3]);
        }
        XMLAttribute(const XMLAttribute &rhs) {
            clone(rhs);
        }
        XMLAttribute &operator=(const XMLAttribute &rhs) {
            if (this == &rhs)
                return *this;
            clone(rhs);
            return *this;
        }
    };

    typedef map<string, XMLAttribute> XMLAttrMap;
    XMLAttrMap attribute_table; // dump XML attributes here

    XMLAttrMap::iterator attr_table_begin() {
        return attribute_table.begin();
    }

    XMLAttrMap::iterator attr_table_end() {
        return attribute_table.end();
    }

    map<string, string> namespace_table;

    // These are kind of silly...
    void set_state(DDXParser::ParseState state);
    DDXParser::ParseState get_state() const;
    void pop_state();

    // Glue for the BaseTypeFactory class.
    BaseType *factory(Type t, const string &name);

    // Common cleanup code for intern() and intern_stream()
    void cleanup_parse(xmlParserCtxtPtr &context);

    /** @name Parser Actions

    These methods are the 'actions' carried out by the start_element and
    end_element callbacks. Most of what takes place in those has been
    factored out to this set of functions. */
    //@{
    void transfer_xml_attrs(const xmlChar **attrs, int nb_attributes);
    void transfer_xml_ns(const xmlChar **namespaces, int nb_namespaces);
    bool check_required_attribute(const string &attr);
    bool check_attribute(const string & attr);

    void process_attribute_element(const xmlChar **attrs, int nb_attrs);
    void process_attribute_alias(const xmlChar **attrs, int nb_attrs);

    void process_variable(Type t, ParseState s, const xmlChar **attrs,
            int nb_attributes);

    void process_dimension(const xmlChar **attrs, int nb_attrs);
    void process_blob(const xmlChar **attrs, int nb_attrs);

    bool is_attribute_or_alias(const char *name, const xmlChar **attrs,
            int nb_attributes);
    bool is_variable(const char *name, const xmlChar **attrs, int nb_attributes);

    void finish_variable(const char *tag, Type t, const char *expected);
    //@}

    /// Declare the default ctor here to prevent its use.
    DDXParser();

    friend class DDXParserTest;

public:
    DDXParser(BaseTypeFactory *factory)
        : d_factory(factory),
        other_xml(""), other_xml_depth(0), unknown_depth(0),
        error_msg(""), ctxt(0), dds(0), blob_href(0),
        dods_attr_name(""), dods_attr_type(""),
        char_data(""), root_ns("")
    {}

    void intern(const string &document, DDS *dest_dds, string &cid);
    void intern_stream(FILE *in, DDS *dds, string &cid, const string &boundary = "");
    void intern_stream(istream &in, DDS *dds, string &cid, const string &boundary = "");

    static void ddx_start_document(void *parser);
    static void ddx_end_document(void *parser);

    static void ddx_sax2_start_element(void *parser,
            const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI,
            int nb_namespaces, const xmlChar **namespaces, int nb_attributes,
            int nb_defaulted, const xmlChar **attributes);
    static void ddx_sax2_end_element(void *parser, const xmlChar *localname,
            const xmlChar *prefix, const xmlChar *URI);

    static void ddx_get_characters(void *parser, const xmlChar *ch, int len);
    static void ddx_ignoreable_whitespace(void *parser,
            const xmlChar * ch, int len);
    static void ddx_get_cdata(void *parser, const xmlChar *value, int len);

    static xmlEntityPtr ddx_get_entity(void *parser, const xmlChar *name);
    static void ddx_fatal_error(void *parser, const char *msg, ...);
};

} // namespace libdap

#endif // ddx_parser_h
