
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

#if 0
#define DODS_DEBUG
#define DODS_DEBUG2
#endif
#include "BaseType.h"
#include "Constructor.h"
#include "DDXParser.h"

#include "util.h"
#include "debug.h"

static const not_used char *states[] = {
    "start",

    "dataset",

    "attribute_container",
    "attribute",
    "attribute_value",

    "alias",

    "simple_type",

    "array",
    "dimension",

    "grid",
    "map",

    "structure",
    "sequence",

    "blob url",

    "unknown",
    "error"
};

/** factory for simple types */
static BaseType *
factory(Type t, const string &name)
{
    switch (t) {
      case dods_byte_c:
	return NewByte(name);
	break;

      case dods_int16_c:
	return NewInt16(name);
	break;

      case dods_uint16_c:
	return NewUInt16(name);
	break;

      case dods_int32_c:
	return NewInt32(name);
	break;

      case dods_uint32_c:
	return NewUInt32(name);
	break;

      case dods_float32_c:
	return NewFloat32(name);
	break;

      case dods_float64_c:
	return NewFloat64(name);
	break;

      case dods_str_c:
	return NewStr(name);
	break;

      case dods_url_c:
	return NewUrl(name);
	break;
	    
      case dods_array_c:
	return NewArray(name);
	break;
	    
      case dods_structure_c:
	return NewStructure(name);
	break;
	    
      case dods_sequence_c:
	return NewSequence(name);
	break;
	    
      case dods_grid_c:
	return NewGrid(name);
	break;
	    
      default: 
	return 0;
    }
}

/** Get the Type enumeration value which matches the given name. */
static Type
get_type(const char *name)
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

static Type
is_simple_type(const char *name)
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

/** @name Helper methods */
//@{

void
DDXParser::set_state(DDXParser::ParseState state)
{
    s.push(state);
}

DDXParser::ParseState
DDXParser::get_state()
{
    return s.top();
}

void
DDXParser::pop_state()
{
    s.pop();
}

/** Dump the attrs array into a map<string,string>. Attribute names are
    always lower case.  */
void
DDXParser::transfer_attrs(const char **attrs)
{
    attributes.clear();		// erase old attributes

    if (!attrs || !attrs[0])
	return;

    for (int i = 0; attrs && attrs[i] != 0; i = i + 2) {
	string attr_i = attrs[i];
	downcase(attr_i);
	attributes[attr_i] = string(attrs[i+1]);
    }
}

/** Is an attribute present. Attribute names are always lowercase. */
bool
DDXParser::check_required_attribute(const string &attr)
{
    bool found = false;
    map<string,string>::iterator i;
    for (i = attributes.begin(); i != attributes.end(); ++i)
	if (i->first == attr)
	    found = true;
    
    if (!found) 
	ddx_fatal_error(this, "Required attribute '%s' not found.",
			attr.c_str());
	
    return found;
}

void
DDXParser::process_attribute_element(const char **attrs)
{
    // These methods set the state to parser_error if a problem is found.
    transfer_attrs(attrs);
    bool error = !(check_required_attribute(string("name"))
		   && check_required_attribute(string("type")));
    if (error)
	return;

    if (attributes["type"] == "Container") {
	set_state(inside_attribute_container);

	AttrTable *child;
	AttrTable *parent = at_stack.top();

	child = parent->append_container(attributes["name"]);
	at_stack.push(child); // save.
	DBG2(cerr << "Pushing at" << endl);
    }
    else {
	set_state(inside_attribute);

	dods_attr_name = attributes["name"];
	dods_attr_type = attributes["type"];
    }
}

void
DDXParser::process_attribute_alias(const char **attrs)
{
    transfer_attrs(attrs);
    if(check_required_attribute(string("name"))
       && check_required_attribute(string("attribute"))) {
	set_state(inside_alias);
	at_stack.top()->attr_alias(attributes["name"],
					 attributes["attribute"]);
    }
}

// create a byte, load its name, push onto stack
void
DDXParser::process_simple_type(Type t, const char **attrs)
{
    transfer_attrs(attrs);

    set_state(inside_simple_type);
    BaseType *btp = factory(t, attributes["name"]);
    if (!btp)
	ddx_fatal_error(this, "Internal parser error; could not instantiate the variable '%s'.", attributes["name"].c_str());

    // Once we make the new variable, we not only load it on to the
    // BaseType stack, we also load its AttrTable on the AttrTable stack.
    // The attribute processing software always operates on the AttrTable
    // at the top of the AttrTable stack (at_stack).
    bt_stack.push(btp);
    at_stack.push(&btp->get_attr_table());
}

void
DDXParser::process_array_type(Type t, const char **attrs)
{
    transfer_attrs(attrs);

    set_state(inside_array);
    BaseType *btp = factory(t, attributes["name"]);
    if (!btp)
	ddx_fatal_error(this, "Internal parser error; could not instantiate the array '%s'.", attributes["name"].c_str());

    // Once we make the new variable, we not only load it on to the
    // BaseType stack, we also load its AttrTable on the AttrTable stack.
    // The attribute processing software always operates on the AttrTable
    // at the top of the AttrTable stack (at_stack).
    bt_stack.push(btp);
    at_stack.push(&btp->get_attr_table());
}

void
DDXParser::process_structure_type(Type t, const char **attrs)
{
    transfer_attrs(attrs);

    set_state(inside_structure);
    BaseType *btp = factory(t, attributes["name"]);
    if (!btp)
	ddx_fatal_error(this, "Internal parser error; could not instantiate the structure '%s'.", attributes["name"].c_str());

    // Once we make the new variable, we not only load it on to the
    // BaseType stack, we also load its AttrTable on the AttrTable stack.
    // The attribute processing software always operates on the AttrTable
    // at the top of the AttrTable stack (at_stack).
    bt_stack.push(btp);
    at_stack.push(&btp->get_attr_table());
}

void
DDXParser::process_sequence_type(Type t, const char **attrs)
{
    transfer_attrs(attrs);

    set_state(inside_sequence);
    BaseType *btp = factory(t, attributes["name"]);
    if (!btp)
	ddx_fatal_error(this, "Internal parser error; could not instantiate the sequence '%s'.", attributes["name"].c_str());

    // Once we make the new variable, we not only load it on to the
    // BaseType stack, we also load its AttrTable on the AttrTable stack.
    // The attribute processing software always operates on the AttrTable
    // at the top of the AttrTable stack (at_stack).
    bt_stack.push(btp);
    at_stack.push(&btp->get_attr_table());
}

void
DDXParser::process_grid_type(Type t, const char **attrs)
{
    transfer_attrs(attrs);

    set_state(inside_grid);
    BaseType *btp = factory(t, attributes["name"]);
    if (!btp)
	ddx_fatal_error(this, "Internal parser error; could not instantiate the grid '%s'.", attributes["name"].c_str());

    // Once we make the new variable, we not only load it on to the
    // BaseType stack, we also load its AttrTable on the AttrTable stack.
    // The attribute processing software always operates on the AttrTable
    // at the top of the AttrTable stack (at_stack).
    bt_stack.push(btp);
    at_stack.push(&btp->get_attr_table());
}

void
DDXParser::process_map_type(Type t, const char **attrs)
{
    transfer_attrs(attrs);

    set_state(inside_map);
    BaseType *btp = factory(t, attributes["name"]);
    if (!btp)
	ddx_fatal_error(this, "Internal parser error; could not instantiate the grid map '%s'.", attributes["name"].c_str());

    // Once we make the new variable, we not only load it on to the
    // BaseType stack, we also load its AttrTable on the AttrTable stack.
    // The attribute processing software always operates on the AttrTable
    // at the top of the AttrTable stack (at_stack).
    bt_stack.push(btp);
    at_stack.push(&btp->get_attr_table());
}

void
DDXParser::process_dimension(const char **attrs)
{
    transfer_attrs(attrs);
    if (check_required_attribute(string("size"))) {
	set_state(inside_dimension);
	Array *ap = dynamic_cast<Array*>(bt_stack.top());

	ap->append_dim(atoi(attributes["size"].c_str()), attributes["name"]);
    }
}

void
DDXParser::process_blob(const char **attrs)
{
    transfer_attrs(attrs);
   if (check_required_attribute(string("url"))) {
       set_state(inside_blob_url);
       blob_url = attributes["url"];
   }
}

//@}

/** @name SAX Parser Callbacks

    These methods are declared static in the class header. This gives them C
    linkage which allows them to be used as callbacks by the SAX parser
    engine. */
//@{

/** Initialize the SAX parser state object. This object is passed to each
    callback as a void pointer. The initial state is parser_start.

    @param state The SAX parser  */
void 
DDXParser::ddx_start_document(DDXParser *parser) 
{
    parser->error_msg = "";
    parser->char_data = "";

    // init attr table stack.
    parser->at_stack.push(&parser->dds->get_attr_table());

    // trick; dds *should* be a child of Structure. To simplify parsing,
    // stuff a Structure on the bt_stack and dump the top level variables
    // there temporarily. Once we're done, transfer the variables to the DDS. 
    parser->bt_stack.push(new Structure("dummy_dds"));

    parser->set_state(parser_start);

    DBG2(cerr << "Parser state: " << states[parser->get_state()] << endl);
}

/** Clean up after finishing a parse.
    @param state The SAX parser  */
void 
DDXParser::ddx_end_document(DDXParser *parser) 
{
    DBG2(cerr << "Ending state == " << states[parser->get_state()] << endl);

    if (parser->get_state() != parser_start)
	DDXParser::ddx_fatal_error(parser, "The document contained unbalanced tags.");

    // If we've found any sort of error, don't make the DDX; intern() will
    // take care of the error.
    if (parser->get_state() == parser_error)
	return;

    // Pop the temporary Structure off the stack and transfer its variables
    // to the DDS. 
    Constructor *cp = dynamic_cast<Constructor*>(parser->bt_stack.top());
    for (Constructor::Vars_iter i = cp->var_begin(); i != cp->var_end(); ++i)
	parser->dds->add_var(*i);

    parser->bt_stack.pop();
    delete cp;
}

/** Process a start element tag. Because the DDX schema uses attributes and
    because libxml2 does not validate those, we do attribute validation here.

    Values held in attributes are recorded 
    @param state The SAX parser  */
void 
DDXParser::ddx_start_element(DDXParser *parser, const char *name, 
				   const char **attrs)
{
    DBG2(cerr << "start element: " << name << ", states: "
	 << states[parser->get_state()]);

    switch (parser->get_state()) {
      case parser_start:
	if (strcmp(name, "Dataset") == 0) {
	    parser->set_state(inside_dataset);

	    parser->transfer_attrs(attrs);
	    if (parser->check_required_attribute(string("name")))
		parser->dds->set_dataset_name(parser->attributes["name"]);
	}
	else
	    DDXParser::ddx_fatal_error(parser, "Expected response to start with a Dataset element; found '%s' instead.", name);
	break;

      case inside_dataset: {
	  Type t;
	if (strcmp(name, "Attribute") == 0) {
	    parser->process_attribute_element(attrs);
	    // next state: inside_attribtue or inside_attribute_container
	}
	else if (strcmp(name, "Alias") == 0) {
	    parser->process_attribute_alias(attrs);
	    // next state: inside_alias
	}
	else if ((t = is_simple_type(name)) != dods_null_c) {
	    parser->process_simple_type(t, attrs);
	    // next state: inside_ismple_type
	}
	else if (strcmp(name, "Array") == 0) {
	    parser->process_array_type(dods_array_c, attrs);
	    // next state: inside_array
	}
	else if (strcmp(name, "Structure") == 0) {
	    parser->process_structure_type(dods_structure_c, attrs);
	    // next state: inside_structure
	}
	else if (strcmp(name, "Sequence") == 0) {
	    parser->process_sequence_type(dods_sequence_c, attrs);
	    // next state: inside_sequence
	}
	else if (strcmp(name, "Grid") == 0) {
	    parser->process_grid_type(dods_grid_c, attrs);
	    // next state: inside_grid
	}
	else if (strcmp(name, "dodsBLOB") == 0) {
	    parser->process_blob(attrs);
	    // next state: inside_dods_blob
	}
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an Attribute, Alias or variable element; found '%s' instead.", name);
	break;
      }

      case inside_attribute_container:
	if (strcmp(name, "Attribute") == 0) {
	    parser->process_attribute_element(attrs);
	    // next state: inside_attribtue or inside_attribute_container
	}
	else if (strcmp(name, "Alias") == 0) {
	    parser->process_attribute_alias(attrs);
	    // next state: inside_alias
	}
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an Attribute or Alias element; found '%s' instead.", name);
	break;

      case inside_attribute:
	if (strcmp(name, "Attribute") == 0) {
	    parser->process_attribute_element(attrs);
	    // next state: inside_attribtue or inside_attribute_container
	}
	else if (strcmp(name, "Alias") == 0) {
	    parser->process_attribute_alias(attrs);
	    // next state: inside_alias
	}
	else if (strcmp(name, "value") == 0)
	    parser->set_state(inside_attribute_value);
	else
	    ddx_fatal_error(parser, "Expected an 'Attribute', 'Alias' or 'value' element; found '%s' instead.", name);
	break;

      case inside_attribute_value:
	ddx_fatal_error(parser, "Internal parser error; unexpected state, inside value while processing element '%s'.", name);
	break;

      case inside_alias:
	ddx_fatal_error(parser, "Internal parser error; unexpected state, inside alias while processing element '%s'.", name);
	break;

      case inside_simple_type:
	if (strcmp(name, "Attribute") == 0) {
	    parser->process_attribute_element(attrs);
	    // next state: inside_attribtue or inside_attribute_container
	}
	else if (strcmp(name, "Alias") == 0) {
	    parser->process_attribute_alias(attrs);
	    // next state: inside_alias
	}
	else
	    ddx_fatal_error(parser, "Expected an 'Attribute' or 'Alias' element; found '%s' instead.", name);
	break;
	
      case inside_array: {
	  Type t;
	  if (strcmp(name, "Attribute") == 0) {
	      parser->process_attribute_element(attrs);
	      // next state: inside_attribtue or inside_attribute_container
	  }
	  else if (strcmp(name, "Alias") == 0) {
	      parser->process_attribute_alias(attrs);
	      // next state: inside_alias
	  }
	  else if ((t = is_simple_type(name)) != dods_null_c) {
	      parser->process_simple_type(t, attrs);
	      // next state: inside_ismple_type
	  }
	  else if (strcmp(name, "Structure") == 0) {
	      parser->process_structure_type(dods_structure_c, attrs);
	      // next state: inside_structure
	  }
	else if (strcmp(name, "Sequence") == 0) {
	    parser->process_sequence_type(dods_sequence_c, attrs);
	    // next state: inside_sequence
	}
	else if (strcmp(name, "Grid") == 0) {
	    parser->process_grid_type(dods_grid_c, attrs);
	    // next state: inside_grid
	}
	  else if (strcmp(name, "dimension") == 0) {
	      parser->process_dimension(attrs);
	      // next state: inside_dimension
	  }
	  else
	      ddx_fatal_error(parser, "Expected an 'Attribute' or 'Alias' element; found '%s' instead.", name);
	  break;
      }

      case inside_dimension:
	ddx_fatal_error(parser, "Internal parser error; unexpected state, inside dimension while processing element '%s'.", name);
	break;

      case inside_structure:{
	  Type t;
	  if (strcmp(name, "Attribute") == 0) {
	      parser->process_attribute_element(attrs);
	      // next state: inside_attribtue or inside_attribute_container
	  }
	  else if (strcmp(name, "Alias") == 0) {
	      parser->process_attribute_alias(attrs);
	      // next state: inside_alias
	  }
	  else if ((t = is_simple_type(name)) != dods_null_c) {
	      parser->process_simple_type(t, attrs);
	      // next state: inside_ismple_type
	  }
	  else if (strcmp(name, "Array") == 0) {
	      parser->process_array_type(dods_array_c, attrs);
	      // next state: inside_array
	  }
	  else if (strcmp(name, "Structure") == 0) {
	      parser->process_structure_type(dods_structure_c, attrs);
	      // next state: inside_structure
	  }
	else if (strcmp(name, "Sequence") == 0) {
	    parser->process_sequence_type(dods_sequence_c, attrs);
	    // next state: inside_sequence
	}
	else if (strcmp(name, "Grid") == 0) {
	    parser->process_grid_type(dods_grid_c, attrs);
	    // next state: inside_grid
	}
	  else
	      DDXParser::ddx_fatal_error(parser, "Expected an Attribute, Alias or variable element; found '%s' instead.", name);
	  break;
      }

      case inside_sequence:{
	  Type t;
	  if (strcmp(name, "Attribute") == 0) {
	      parser->process_attribute_element(attrs);
	      // next state: inside_attribtue or inside_attribute_container
	  }
	  else if (strcmp(name, "Alias") == 0) {
	      parser->process_attribute_alias(attrs);
	      // next state: inside_alias
	  }
	  else if ((t = is_simple_type(name)) != dods_null_c) {
	      parser->process_simple_type(t, attrs);
	      // next state: inside_ismple_type
	  }
	  else if (strcmp(name, "Array") == 0) {
	      parser->process_array_type(dods_array_c, attrs);
	      // next state: inside_array
	  }
	  else if (strcmp(name, "Structure") == 0) {
	      parser->process_structure_type(dods_structure_c, attrs);
	      // next state: inside_structure
	  }
	else if (strcmp(name, "Sequence") == 0) {
	    parser->process_sequence_type(dods_sequence_c, attrs);
	    // next state: inside_sequence
	}
	else if (strcmp(name, "Grid") == 0) {
	    parser->process_grid_type(dods_grid_c, attrs);
	    // next state: inside_grid
	}
	  else
	      DDXParser::ddx_fatal_error(parser, "Expected an Attribute, Alias or variable element; found '%s' instead.", name);
	  break;
      }

      case inside_grid:
	  if (strcmp(name, "Attribute") == 0) {
	      parser->process_attribute_element(attrs);
	      // next state: inside_attribtue or inside_attribute_container
	  }
	  else if (strcmp(name, "Alias") == 0) {
	      parser->process_attribute_alias(attrs);
	      // next state: inside_alias
	  }
	  else if (strcmp(name, "Array") == 0) {
	      parser->process_array_type(dods_array_c, attrs);
	      // next state: inside_array
	  }
	  else if (strcmp(name, "Map") == 0) {
	      parser->process_map_type(dods_array_c, attrs);
	      // next state: inside_map
	  }
	  else
	      DDXParser::ddx_fatal_error(parser, "Expected an Attribute, Alias or variable element; found '%s' instead.", name);
	  break;

      case inside_map: {
	  Type t;
	  if (strcmp(name, "Attribute") == 0) {
	      parser->process_attribute_element(attrs);
	      // next state: inside_attribtue or inside_attribute_container
	  }
	  else if (strcmp(name, "Alias") == 0) {
	      parser->process_attribute_alias(attrs);
	      // next state: inside_alias
	  }
	  else if ((t = is_simple_type(name)) != dods_null_c) {
	      parser->process_simple_type(t, attrs);
	      // next state: inside_ismple_type
	  }
	  else if (strcmp(name, "Structure") == 0) {
	      parser->process_structure_type(dods_structure_c, attrs);
	      // next state: inside_structure
	  }
	  else if (strcmp(name, "dimension") == 0) {
	      parser->process_dimension(attrs);
	      // next state: inside_dimension
	  }
	  else
	      ddx_fatal_error(parser, "Expected an 'Attribute', 'Alias', variable or 'dimension' element; found '%s' instead.", name);
	  break;
      }

      case inside_blob_url:
	ddx_fatal_error(parser, "Internal parser error; unexpected state, inside blob url while processing element '%s'.", name);
	break;

      case parser_unknown:
	parser->set_state(parser_unknown);
	break;

      case parser_error:
	break;
    }

    DBGN(cerr << " ... " << states[parser->get_state()] << endl);
}

/** Process an end element tag. This is where values are added to the
    DDS/DDX or their parent BaseType.
    @param state The SAX parser state */
void 
DDXParser::ddx_end_element(DDXParser *parser, const char *name) 
{
    DBG2(cerr << "End element " << name << " (state " 
	 << states[parser->get_state()] << ")" << endl);

    switch (parser->get_state()) {
      case parser_start:
	ddx_fatal_error(parser, "Internal parser error; unexpected state, inside start state while processing element '%s'.", name);
	break;

      case inside_dataset:
	if (strcmp(name, "Dataset") == 0)
	    parser->pop_state();
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an end Dataset tag; found '%s' instead.", name);
	break;

      case inside_attribute_container:
	if (strcmp(name, "Attribute") == 0) {
	parser->pop_state();
	parser->at_stack.pop();	// pop when leaving a container.
	DBG2(cerr << "Popping at stack" << endl);
	}
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an end Attribute tag; found '%s' instead.", name);
	break;
	
      case inside_attribute:
	if (strcmp(name, "Attribute") == 0)
	parser->pop_state();
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an end Attribute tag; found '%s' instead.", name);
	break;
	
      case inside_attribute_value: {
	if (strcmp(name, "value") == 0) {
	parser->pop_state();
	AttrTable *atp = parser->at_stack.top();
	atp->append_attr(parser->dods_attr_name, parser->dods_attr_type,
		    parser->char_data);
	parser->char_data = "";	// Null this after use.
	}
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an end value tag; found '%s' instead.", name);

	break;    
      }

	// Alias is busted in C++ 05/29/03 jhrg
      case inside_alias:
	  parser->pop_state();
	  break;

      case inside_simple_type: {
	  if (is_simple_type(name) != dods_null_c) {
	      parser->pop_state();
	      BaseType *btp = parser->bt_stack.top();
	      parser->bt_stack.pop();
	      parser->at_stack.pop();

	      BaseType *parent = parser->bt_stack.top();

	      if (parent->is_vector_type() || parent->is_constructor_type())
		  parent->add_var(btp);
	      else
		  DDXParser::ddx_fatal_error(parser, "Tried to add the simple-type variable '%s' to a non-constructor type (%s %s).", name, parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
	  }
	  else
	      DDXParser::ddx_fatal_error(parser, "Expected an end tag for a simple type; found '%s' instead.", name);
	  break;
      }

      case inside_array: {
	  if (get_type(name) != dods_array_c) {
	      DDXParser::ddx_fatal_error(parser, "Expected an end tag for an array; found '%s' instead.", name);
	      break;
	  }

	  parser->pop_state();
	  Array *ap = dynamic_cast<Array*>(parser->bt_stack.top());
	  parser->bt_stack.pop();
	  parser->at_stack.pop();

	  if (!ap) {
	      DDXParser::ddx_fatal_error(parser, "Internal error: Expected an Array variable.");
	      break;
	  }

	  // Once libxml2 validates, this can go away. 05/30/03 jhrg
	  if (ap->dimensions() == 0) {
	      DDXParser::ddx_fatal_error(parser, "No dimension element included in the Array '%s'.", ap->name().c_str());
	      break;
	  }

	  BaseType *parent = parser->bt_stack.top();

	  if (!(parent->is_vector_type() || parent->is_constructor_type())) {
	      DDXParser::ddx_fatal_error(parser, "Tried to add the array variable '%s' to a non-constructor type (%s %s).", name, parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
	      break;
	  }

	  parent->add_var(ap);
	  break;
      }

      case inside_dimension:
	if (strcmp(name, "dimension") == 0)
	    parser->pop_state();
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an end dimension tag; found '%s' instead.", name);
	break;

      case inside_structure: {
	  if (get_type(name) != dods_structure_c) {
	      DDXParser::ddx_fatal_error(parser, "Expected an end tag for a structure; found '%s' instead.", name);
	      break;
	  }

	  parser->pop_state();
	  Structure *sp = dynamic_cast<Structure*>(parser->bt_stack.top());
	  parser->bt_stack.pop();
	  parser->at_stack.pop();

	  if (!sp) {
	      DDXParser::ddx_fatal_error(parser, "Internal error: Expected a Structure variable.");
	      break;
	  }

	  BaseType *parent = parser->bt_stack.top();

	  if (!(parent->is_vector_type() || parent->is_constructor_type())) {
	      DDXParser::ddx_fatal_error(parser, "Tried to add the array variable '%s' to a non-constructor type (%s %s).", name, parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
	      break;
	  }

	  parent->add_var(sp);
	  break;
      }

      case inside_sequence: {
	  if (get_type(name) != dods_sequence_c) {
	      DDXParser::ddx_fatal_error(parser, "Expected an end tag for a sequence; found '%s' instead.", name);
	      break;
	  }

	  parser->pop_state();
	  Sequence *sp = dynamic_cast<Sequence*>(parser->bt_stack.top());
	  parser->bt_stack.pop();
	  parser->at_stack.pop();

	  if (!sp) {
	      DDXParser::ddx_fatal_error(parser, "Internal error: Expected a Sequence variable.");
	      break;
	  }

	  BaseType *parent = parser->bt_stack.top();

	  if (!(parent->is_vector_type() || parent->is_constructor_type())) {
	      DDXParser::ddx_fatal_error(parser, "Tried to add the array variable '%s' to a non-constructor type (%s %s).", name, parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
	      break;
	  }

	  parent->add_var(sp);
	  break;
      }

      case inside_grid: {
	  if (get_type(name) != dods_grid_c) {
	      DDXParser::ddx_fatal_error(parser, "Expected an end tag for a grid; found '%s' instead.", name);
	      break;
	  }

	  parser->pop_state();
	  Grid *gp = dynamic_cast<Grid*>(parser->bt_stack.top());
	  parser->bt_stack.pop();
	  parser->at_stack.pop();

	  if (!gp) {
	      DDXParser::ddx_fatal_error(parser, "Internal error: Expected a Grid variable.");
	      break;
	  }

	  BaseType *parent = parser->bt_stack.top();

	  if (!(parent->is_vector_type() || parent->is_constructor_type())) {
	      DDXParser::ddx_fatal_error(parser, "Tried to add the array variable '%s' to a non-constructor type (%s %s).", name, parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
	      break;
	  }

	  parent->add_var(gp);
	  break;
      }

      case inside_map: {
	  if (strcmp(name, "Map") != 0) {
	      DDXParser::ddx_fatal_error(parser, "Expected an end tag for a Map; found '%s' instead.", name);
	      break;
	  }

	  parser->pop_state();
	  Array *ap = dynamic_cast<Array*>(parser->bt_stack.top());
	  parser->bt_stack.pop();
	  parser->at_stack.pop();

	  if (!ap) {
	      DDXParser::ddx_fatal_error(parser, "Internal error: Expected an Array variable.");
	      break;
	  }

	  // Once libxml2 validates, this can go away. 05/30/03 jhrg
	  if (ap->dimensions() == 0) {
	      DDXParser::ddx_fatal_error(parser, "No dimension element included in the Array '%s'.", ap->name().c_str());
	      break;
	  }

	  BaseType *parent = parser->bt_stack.top();

	  if (!(parent->is_vector_type() || parent->is_constructor_type())) {
	      DDXParser::ddx_fatal_error(parser, "Tried to add the array variable '%s' to a non-constructor type (%s %s).", name, parser->bt_stack.top()->type_name().c_str(), parser->bt_stack.top()->name().c_str());
	      break;
	  }

	  parent->add_var(ap);
	  break;
      }

      case inside_blob_url:
	if (strcmp(name, "dodsBLOB") == 0)
	    parser->pop_state();
	else
	    DDXParser::ddx_fatal_error(parser, "Expected an end dodsBLOB tag; found '%s' instead.", name);
	break;

      case parser_unknown:
	parser->pop_state();
	break;

      case parser_error:
	break;
    }
}

/** Process/accumulate character data. This may be called more than once for
    one logical clump of data. Only save character data when processing
    'value' elements; throw away all other characters. */
void 
DDXParser::characters(DDXParser *parser, const xmlChar *ch, int len)
{
    switch (parser->get_state()) {
      case inside_attribute_value:
	parser->char_data.append((const char *)(ch), len);
	DBG2(cerr << "Characters: '" << parser->char_data << "'" << endl);
	break;

      default:
	break;
    }
}

/** Handle the standard XML entities.
    @param state The SAX parser  
    @param name The XML entity. */
xmlEntityPtr
DDXParser::ddx_get_entity(DDXParser *parser, const xmlChar *name) 
{
    return xmlGetPredefinedEntity(name);
}

/** Process an XML fatal error. Note that SAX provides for warnings, errors
    and fatal errors. This code treats them all as fatal errors since there's
    typically no way to tell a user about the error since there's often no
    user interface for this software.

    @param state The SAX parser  
    @param msg A printf-style format string. */
void 
DDXParser::ddx_fatal_error(DDXParser *parser, const char *msg, ...)
{
    va_list args;

    parser->set_state(parser_error);

    va_start(args, msg);
    char str[1024];
    vsnprintf(str, 1024, msg, args);
    va_end(args);

    int line = getLineNumber(parser->ctxt);
    parser->error_msg += "At line " + long_to_string(line) + ": ";
    parser->error_msg += string(str) + string("\n");
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
    (warningSAXFunc)DDXParser::ddx_fatal_error, // warning 
    (errorSAXFunc)DDXParser::ddx_fatal_error, // error 
    (fatalErrorSAXFunc)DDXParser::ddx_fatal_error // fatalError 
};

/** Parse an AIS database encoded in XML. The information in the XML document
    is loaded into an instance of AISResources. 
    @param database Read from this XML file.
    @param ais Load information into this instance of AISResources.
    @exception AISDatabaseReadFailed Thrown if the XML document could not be
    read or parsed. */
void
DDXParser::intern(const string &document, DDS *destination_dds)
    throw(DDXParseFailed)
{
    xmlParserCtxtPtr context = xmlCreateFileParserCtxt(document.c_str());
    if (!context)
	throw DDXParseFailed(string("Could not initialize the parser with the file: '") + document + string("'."));

    dds = destination_dds;	// dump values here
    ctxt = context;		// need ctxt for error messages

    context->sax = &ddx_sax_parser;
    context->userData = this;
    context->validate = true;

    xmlParseDocument(context);
	
    // use getLineNumber and getColumnNumber to make the error messages better.
    if (!context->wellFormed) {
	context->sax = NULL;
	xmlFreeParserCtxt(context);
	throw DDXParseFailed(string("\nThe DDX is not a well formed XML document.\n") + error_msg);
    }

    if (!context->valid) {
	context->sax = NULL;
	xmlFreeParserCtxt(context);
	throw DDXParseFailed(string("\nThe DDX is not a valid document.\n") + error_msg);
    }

    if (get_state() == parser_error) {
	context->sax = NULL;
	xmlFreeParserCtxt(context);
	throw DDXParseFailed(string("\nError parsing DDX response.\n") + error_msg);
    }

    context->sax = NULL;
    xmlFreeParserCtxt(context);
}

// $Log: DDXParser.cc,v $
// Revision 1.3  2003/05/30 21:43:44  jimg
// Parser now parses all data types correctly. Needs to be refactored so common
// code can be combined.
//
// Revision 1.2  2003/05/30 02:00:55  jimg
// Parses top level attributes and simple variables.
//
// Revision 1.1  2003/05/29 19:07:15  jimg
// Added.
//
