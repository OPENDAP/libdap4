
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

    // These are used for processing errors.
    string error_msg;  // Error message(s), if any.
    xmlParserCtxtPtr ctxt; // used for error msg line numbers

    // The results of the parse operation are stored in these fields.
    DDS *dds;   // dump DDX here
    string *blob_href;  // put href to blob here

    // These hold temporary values read during the parse.
    string dods_attr_name; // DAP2 attributes, not XML attributes
    string dods_attr_type; // ... not XML ...
    string char_data;  // char data in value elements; null after use
    map<string, string> attributes; // dump XML attributes here

    // These are kind of silly...
    void set_state(DDXParser::ParseState state);
    DDXParser::ParseState get_state() const;
    void pop_state();

    // Glue for the BaseTypeFactory class.
    BaseType *factory(Type t, const string &name);

    // Common cleanup code for intern() and intern_stream()
    void cleanup_parse(xmlParserCtxtPtr &context) const;

    /** @name Parser Actions

    These methods are the 'actions' carried out by the start_element and
    end_element callbacks. Most of what takes place in those has been
    factored out to this set of functions. */
    //@{
    void transfer_attrs(const char **attrs);
    bool check_required_attribute(const string &attr);
    bool check_attribute(const string & attr);

    void process_attribute_element(const char **attrs);
    void process_attribute_alias(const char **attrs);

    void process_variable(Type t, ParseState s, const char **attrs);

    void process_dimension(const char **attrs);
    void process_blob(const char **attrs);

    bool is_attribute_or_alias(const char *name, const char **attrs);
    bool is_variable(const char *name, const char **attrs);

    void finish_variable(const char *tag, Type t, const char *expected);
    //@}

    /// Define the default ctor here to prevent its use.
    DDXParser()
    {}

public:
    DDXParser(BaseTypeFactory *factory) : d_factory(factory)
    {}

    void intern(const string &document, DDS *dest_dds, string &blob);
    void intern(FILE *in, DDS *dest_dds, string &blob, const string &boundary = "");

    static void ddx_start_document(DDXParser *parser);
    static void ddx_end_document(DDXParser *parser);
    static void ddx_start_element(DDXParser *parser, const char *name,
                                  const char **attrs);
    static void ddx_end_element(DDXParser *parser, const char *name);
    static void characters(DDXParser *parser, const xmlChar *ch, int len);
    static xmlEntityPtr ddx_get_entity(DDXParser *parser,
                                       const xmlChar *name);
    static void ddx_fatal_error(DDXParser *parser, const char *msg, ...);
};

} // namespace libdap

#endif // ddx_parser_h
