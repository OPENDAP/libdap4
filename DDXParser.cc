
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

#include "config_dap.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define DODS_DEBUG
#define DODS_DEBUG2
#include "DDXParser.h"
#include "util.h"
#include "debug.h"

static const not_used char *states[] = {
    "start",
    "finish",

    "dataset",

    "attribute_container",
    "attribute",
    "attribute_value",

    "alias",

    "byte",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "float32",
    "float64",
    "string",
    "url",

    "array",
    "dimension",

    "grid",
    "map",
    "grid_array",

    "structure",
    "sequence",

    "unknown",
    "error"
};

/** @name Helper methods */
//@{

void
DDXParser::set_state(DDXParserState *state, DDXParser::ParseState s)
{
    state->s.push(s);
}

DDXParser::ParseState
DDXParser::get_state(DDXParserState *state)
{
    return state->s.top();
}

void
DDXParser::pop_state(DDXParserState *state)
{
    state->s.pop();
}

/** Dump the attrs array into a map<string,string> */
bool
DDXParser::transfer_attrs(DDXParserState *state, const char **attrs)
{
    if (!attrs || !attrs[0]) {
	ddx_fatal_error(state, "This element requires attributes, none of which are present.");
	return false;
    }

    for (int i = 0; attrs && attrs[i] != 0; i = i + 2)
	state->attributes[string(attrs[i])] = string(attrs[i+1]);

    return true;
}

bool
DDXParser::check_required_attribute(DDXParserState *state, const string &attr)
{
    bool found = false;
    map<string,string>::iterator i;
    for (i = state->attributes.begin(); i != state->attributes.end(); ++i)
	if (i->first == attr)
	    found = true;
    
    if (!found) 
	ddx_fatal_error(state, "Required attribute '%s' not found.",
			attr.c_str());
	
    return found;
}

void
DDXParser::process_attribute_element(DDXParserState *state, 
				     const char **attrs)
{
    // These methods set the state to parser_error if a problem is found.
    bool error = !(transfer_attrs(state, attrs)
		   && check_required_attribute(state, string("name"))
		   && check_required_attribute(state, string("type")));
    if (error)
	return;

    if (state->attributes["type"] == "Container") {
	set_state(state, inside_attribute_container);

	AttrTable *child;
	AttrTable *parent = state->at_stack.top();

	child = parent->append_container(state->attributes["name"]);
	state->at_stack.push(child); // save.
	DBG2(cerr << "Pushing at" << endl);
    }
    else {
	set_state(state, inside_attribute);

	state->dods_attr_name = state->attributes["name"];
	state->dods_attr_type = state->attributes["type"];
    }
}

void
DDXParser::process_attribute_alias(DDXParserState *state, const char **attrs)
{
    if (transfer_attrs(state, attrs)
	&& check_required_attribute(state, string("name"))
	&& check_required_attribute(state, string("Attribute")))
	state->at_stack.top()->attr_alias(state->attributes["name"],
					  state->attributes["Attribute"]);
}

//@}

/** @name SAX Parser Callbacks

    These methods are declared static in the class header. This gives them C
    linkage which allows them to be used as callbacks by the SAX parser
    engine. */
//@{

/** Initialize the SAX parser state object. This object is passed to each
    callback as a void pointer. The initial state is parser_start and the
    previous state is parser_unknown with the unknown_depth set to zero.
    @param state The SAX parser state. */
void 
DDXParser::ddx_start_document(DDXParserState *state) 
{
    state->unknown_depth = 0;
    state->error_msg = "";
    state->char_data = "";

    // init attr table stack.
    state->at_stack.push(&state->dds->get_attr_table());

    // trick; dds *should* be a child of Structure. To simplify parsing,
    // stuff a Structure on the bt_stack and dump the top level variables
    // there temporarily. Once we're done, transfer the variables to the DDS. 

    set_state(state, parser_start);

    DBG2(cerr << "Parser state: " << states[get_state(state)] << endl);
}

/** Clean up after finishing a parse.
    @param state The SAX parser state. */
void 
DDXParser::ddx_end_document(DDXParserState *state) 
{
    DBG2(cerr << "Ending state == " << states[get_state(state)] << endl);

    if (state->unknown_depth != 0) {
	DDXParser::ddx_fatal_error(state, 
				   "The document contained unbalanced tags.");

        DBG(cerr << "unknown_depth != 0 (" << state->unknown_depth << ")"
	    << endl);
    }
}

/** Process a start element tag. Because the DDX schema uses attributes and
    because libxml2 does not validate those, we do attribute validation here.

    Values held in attributes are recorded 
    @param state The SAX parser state. */
void 
DDXParser::ddx_start_element(DDXParserState *state, const char *name, 
				   const char **attrs)
{
    DBG2(cerr << "start element: " << name << ", states: "
	 << states[get_state(state)]);

    switch (get_state(state)) {
      case parser_start:
	if (strcmp(name, "Dataset") == 0) {
	    set_state(state, inside_dataset);

	    if (transfer_attrs(state, attrs)
		&& check_required_attribute(state, string("name")))
		state->dds->set_dataset_name(state->attributes["name"]);
	}
	else
	    DDXParser::ddx_error(state, "Expected response to start with 'Dataset'\n");
	break;

      case parser_finish:
	break;

      case inside_dataset:
	// Process attributes
	if (strcmp(name, "Attribute") == 0) {
	    process_attribute_element(state, attrs);
	}
	// Process aliases
	else if (strcmp(name, "Alias") == 0) {
	    set_state(state, inside_alias);
	    process_attribute_alias(state, attrs);
	}
	// Process variables
	else if (strcmp(name, "Byte") == 0) {
	}
	// According to the schema, it's OK to have an empty Dataset.
	break;

      case inside_attribute_container:
	if (strcmp(name, "Attribute") == 0) {
	    process_attribute_element(state, attrs);
	}
	// Process aliases
	else if (strcmp(name, "Alias") == 0) {
	    set_state(state, inside_alias);
	    process_attribute_alias(state, attrs);
	}
	// It's OK to have an empty Attribute table.
	break;

      case inside_attribute:
	if (strcmp(name, "Attribute") == 0) {
	    process_attribute_element(state, attrs);
	}
	// Process aliases
	else if (strcmp(name, "Alias") == 0) {
	    set_state(state, inside_alias);
	    process_attribute_alias(state, attrs);
	}
	else if (strcmp(name, "value") == 0)
	    set_state(state, inside_attribute_value);
	else
	    ddx_fatal_error(state, "Expected a 'value' element inside an Attribtue.");
	break;

      case inside_attribute_value:
	// value has no child elements and no attributes, nothing to do here
	break;

      case parser_unknown:
	set_state(state, parser_unknown);
	break;

      case parser_error:
	break;

      default:
	// *** Remove me when done
	break;
    }

    DBGN(cerr << " ... " << states[get_state(state)] << endl);
}

/** Process an end element tag. This is where values are added to the
    DDS/DDX or their parent BaseType.
    @param state The SAX parser state. */
void 
DDXParser::ddx_end_element(DDXParserState *state, const char *name) 
{
    DBG2(cerr << "End element " << name << " (state " 
	 << states[get_state(state)] << ")" << endl);

    switch (get_state(state)) {
      case inside_dataset:
	pop_state(state);
	break;

      case inside_attribute_container:
	pop_state(state);
	state->at_stack.pop();	// pop when leaving a container.
	DBG2(cerr << "Popping at stack" << endl);
	break;
	
      case inside_attribute:
	pop_state(state);
	break;
	
      case inside_attribute_value: {
	pop_state(state);
	AttrTable *atp = state->at_stack.top();
	atp->append_attr(state->dods_attr_name, state->dods_attr_type,
		    state->char_data);
	state->char_data = "";	// Null this after use.
	break;    
      }

      case inside_alias: {
	  pop_state(state);
	  break;

      case parser_unknown:
	pop_state(state);
	break;

      default:
	break;
    }
}

/** Process/accumulate character data. This may be called more than once for
    one logical clump of data. Only save character data when processing
    'value' elements; throw away all other characters. */
void 
DDXParser::characters(DDXParserState *state, const xmlChar *ch, int len)
{
    switch (get_state(state)) {
      case inside_attribute_value:
	state->char_data.append((const char *)(ch), len);
	DBG2(cerr << "Characters: '" << state->char_data << "'" << endl);
	break;

      default:
	break;
    }
}

/** Handle the standard XML entities.
    @param state The SAX parser state. 
    @param name The XML entity. */
xmlEntityPtr
DDXParser::ddx_get_entity(DDXParserState *state, const xmlChar *name) 
{
    return xmlGetPredefinedEntity(name);
}

/** Process an XML warning. This is treated as a fatal error since there's no
    easy way for libdap++ to signal warning to users.
    @param state The SAX parser state. 
    @param msg A printf-style format string. */
void 
DDXParser::ddx_warning(DDXParserState *state, const char *msg, ...) 
{
    va_list args;

    set_state(state, parser_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = getLineNumber(state->ctxt);
    state->error_msg += "At line " + long_to_string(line) + ": ";
    state->error_msg += string(str) + string("\n");
}

/** Process an XML error. This is treated as a fatal error since there's no
    easy way for libdap++ to signal warning to users.
    @param state The SAX parser state. 
    @param msg A printf-style format string. */
void 
DDXParser::ddx_error(DDXParserState *state, const char *msg, ...)
{
    va_list args;

    set_state(state, parser_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = getLineNumber(state->ctxt);
    state->error_msg += "At line " + long_to_string(line) + ": ";
    state->error_msg += string(str) + string("\n");
}

/** Process an XML fatal error. 
    @param state The SAX parser state. 
    @param msg A printf-style format string. */
void 
DDXParser::ddx_fatal_error(DDXParserState *state, const char *msg, ...)
{
    va_list args;

    set_state(state, parser_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = getLineNumber(state->ctxt);
    state->error_msg += "At line " + long_to_string(line) + ": ";
    state->error_msg += string(str) + string("\n");
}

//@}

/** This local variable holds pointers to the callback <i>functions</i> which
    comprise the SAX parser. */
static xmlSAXHandler ddx_sax_parser = {
    0, // internalSubset 
    0, // isStandalone 
    0, // hasInternalSubset 
    0, // hasExternalSubset 
    0, // resolveEntity 
    (getEntitySAXFunc)DDXParser::ddx_get_entity, // getEntity 
    0, // entityDecl 
    0, // notationDecl 
    0, // attributeDecl 
    0, // elementDecl 
    0, // unparsedEntityDecl 
    0, // setDocumentLocator 
    (startDocumentSAXFunc)DDXParser::ddx_start_document, // startDocument
    (endDocumentSAXFunc)DDXParser::ddx_end_document,  // endDocument 
    (startElementSAXFunc)DDXParser::ddx_start_element,  // startElement 
    (endElementSAXFunc)DDXParser::ddx_end_element,  // endElement 
    0, // reference 
    (charactersSAXFunc)DDXParser::characters,
    0, // ignorableWhitespace 
    0, // processingInstruction 
    0, // (commentSAXFunc)gladeComment,  comment 
    (warningSAXFunc)DDXParser::ddx_warning, // warning 
    (errorSAXFunc)DDXParser::ddx_error, // error 
    (fatalErrorSAXFunc)DDXParser::ddx_fatal_error // fatalError 
};

/** Parse an AIS database encoded in XML. The information in the XML document
    is loaded into an instance of AISResources. 
    @param database Read from this XML file.
    @param ais Load information into this instance of AISResources.
    @exception AISDatabaseReadFailed Thrown if the XML document could not be
    read or parsed. */
void
DDXParser::intern(const string &document, DDS *dds)
    throw(DDXParseFailed)
{
    xmlParserCtxtPtr ctxt;
    DDXParserState state;
	
    ctxt = xmlCreateFileParserCtxt(document.c_str());
    if (!ctxt)
	throw DDXParseFailed(string("Could not initialize the parser with the file: '") + document + string("'."));

    state.dds = dds;		// dump values here
    state.ctxt = ctxt;		// need ctxt for error messages

    ctxt->sax = &ddx_sax_parser;
    ctxt->userData = &state;
    ctxt->validate = true;

    xmlParseDocument(ctxt);
	
    // use getLineNumber and getColumnNumber to make the error messages better.
    if (!ctxt->wellFormed) {
	ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	throw DDXParseFailed(string("\nThe database is not a well formed XML document.\n") + state.error_msg);
    }

    if (!ctxt->valid) {
	ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	throw DDXParseFailed(string("\nThe database is not a valid document.\n") + state.error_msg);
    }

    if (get_state(&state) == parser_error) {
	ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
	throw DDXParseFailed(string("\nError parsing DDX response.\n") + state.error_msg);
    }

    ctxt->sax = NULL;
    xmlFreeParserCtxt(ctxt);
}

// $Log: DDXParser.cc,v $
// Revision 1.1  2003/05/29 19:07:15  jimg
// Added.
//
