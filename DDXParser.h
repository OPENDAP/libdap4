
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

/** Parse the XML text which encodes the network/persistent representation of
    the DDX object. In the current implementation, the DDX is held by an
    instance of the class DDS which in turn holds variables which include
    attributes. 

    Static methods are used as callbacks for the SAX parser. They do not
    throw exceptions because exceptions from within callbacks are not
    reliable or portable (although exceptions themselves are not 100%
    portable...). To signal errors, the methods record information in the
    DDXParserState object. Once any of the error handlers (aisWarning,
    aisError or aisFatalError) is called, construction of an DDX/DDS object
    ends even though the SAX parser still calls the various callback
    functions. The intern method throws an DDXParseFailed exception if an
    error was found.

    Note that this class uses the C++-supplied default definitions for the
    default and copy constructors as well as the destructor and assignment
    operator.

    @see DDS */
class DDXParser {
private:
    /** States used by DDXParserState. These are the states of the SAX parser
	state-machine. */ 
    enum ParseState {
	parser_start,
	parser_finish,

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
	inside_grid_array,

	inside_structure,
	inside_sequence,

	inside_blob_url,

	parser_unknown,
	parser_error
    };

    /** This holds the state information for the SAX parser that is used to
	intern the XML DDX response. The parser is designed to ignore unknown
	tags and attributes, so long as the input is well-formed. Note that a
	pointer to a DDS object is part of the SAX parser state. As the XML
	input document is parsed, information is added to that object. Also
	note that an DDXParserState object holds a pointer to the
	xmlParserCtxt which, in turn, holds a pointer to DDXParserState (via
	its \c userData field). This cirular referencing is done because
	libxml2's SAX parser invokes the callbacks using just the
	DDXParserState instance but we need the whole xmlParserCtxt for some
	of the callbacks.

	NB: Since the callbacks all use static methods, stuff that would
	nomally be fields of the class wind up here in the 'state' sub
	object. */
    struct DDXParserState {
	stack<ParseState> s;

	string error_msg;	// Error message(s), if any.
	xmlParserCtxtPtr ctxt;	// used for error msg line numbers

	DDS *dds;		// dump DDX here
	string blob_url;	// put URL to blob here

	stack<BaseType*> bt_stack; // current variable(s)
	stack<AttrTable*> at_stack; // current attribute table

	string dods_attr_name;	// DODS attributes, not XML attributes
	string dods_attr_type;	// ... not XML ...
	string char_data;	// null after use
	map<string,string> attributes; // dump XML attributes here
    };

    DDXParserState state;

    static void set_state(DDXParserState *state, DDXParser::ParseState s);
    static DDXParser::ParseState get_state(DDXParserState *state);
    static void pop_state(DDXParserState *state);

    static bool transfer_attrs(DDXParserState *state, const char **attrs);
    static bool check_required_attribute(DDXParserState *state, 
					 const string &attr);
    static void process_attribute_element(DDXParserState *state,
					  const char **attrs);
    static void process_attribute_alias(DDXParserState *state,
					const char **attrs);
    static void process_simple_type(DDXParserState *state, const char *name,
				    const char **attrs);
    static BaseType *factory(DDXParserState *state, Type t);
    static void process_blob(DDXParserState *state, const char **attrs);

public:
    void intern(const string &document, DDS *dds)
	throw(DDXParseFailed);

    static void ddx_start_document(DDXParserState *state);
    static void ddx_end_document(DDXParserState *state);
    static void ddx_start_element(DDXParserState *state, const char *name, 
				const char **attrs);
    static void ddx_end_element(DDXParserState *state, const char *name);
    static void characters(DDXParserState *state, const xmlChar *ch, int len);
    static xmlEntityPtr ddx_get_entity(DDXParserState *state,
				     const xmlChar *name);
    static void ddx_warning(DDXParserState *state, const char *msg, ...);
    static void ddx_error(DDXParserState *state, const char *msg, ...);
    static void ddx_fatal_error(DDXParserState *state, const char *msg, ...);
};

// $Log: DDXParser.h,v $
// Revision 1.2  2003/05/30 02:01:03  jimg
// Parses top level attributes and simple variables.
//
// Revision 1.1  2003/05/29 19:07:15  jimg
// Added.
//

#endif // ddx_parser_h
