
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// jhrg 7/29/94

// $Log: AttrTable.cc,v $
// Revision 1.18  1997/07/15 21:58:04  jimg
// Formatting.
//
// Revision 1.17  1997/06/06 03:12:44  jimg
// Added mfuncs with char * parameters for some of the calls. See AttrTable.h
//
// Revision 1.16  1997/05/13 23:32:11  jimg
// Added changes to handle the new Alias and lexical scoping rules.
//
// Revision 1.15  1997/01/13 16:56:03  jimg
// Changed the name of the private member `map' to `attr_map' to avoid a name
// collision with the STL'd map class.
//
// Revision 1.14  1996/08/13 20:49:30  jimg
// Added __unused__ to definition of char rcsid[].
//
// Revision 1.13  1996/05/31 23:29:21  jimg
// Updated copyright notice.
//
// Revision 1.12  1996/04/05 00:21:18  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.11  1995/07/09  21:28:50  jimg
// Added copyright notice.
//
// Revision 1.10  1995/02/10  02:27:53  jimg
// Fixed an error where two attributes could be declared with the same name
// (for the same variable) if they hade different types.
//
// Revision 1.9  1994/12/21  03:03:37  reza
// Added overloading functions for get_attr_num().
//
// Revision 1.8  1994/12/07  21:09:24  jimg
// Added support for vectors of attributes (using XPlex from libg++).
//
// Revision 1.7  1994/10/13  15:43:29  jimg
// Added a new version of append_attr that takes (const char *)s and modified
// the version that takes strings to take (const String &).
//
// Revision 1.6  1994/10/05  16:38:17  jimg
// Changed internal representation of the attribute table from a Map
// to a DLList<>.
//
// Revision 1.5  1994/09/27  22:42:44  jimg
// Changed definition of the class AttrTable; it no longer inherits from
// AttrVHMap, instead it uses that class (contains a member that is an instance
// of AttrVHMap).
// Added mfuncs to AttrTable so that the new member could be set/accessed.
//
// Revision 1.4  1994/09/09  15:26:39  jimg
// Removed operator<< and added print() since I have no good way to define
// operator>>. It seems best to define all operators from a set (like <<, >>)
// or none at all. Since parse() is the input mfunc, it seems that output
// should be a mfunc too.
//
// Revision 1.3  1994/08/02  20:11:27  jimg
// Changes operator<< so that it writes a parsable version of the
// attribute table.
//' comments and rcsid[] variables (syntax errors due to //
// comments caused compilation failures.
// das.tab.c and .h are commited now as well.
//
// Revision 1.1  1994/08/02  18:32:04  jimg
// The implementation of AttrTable. This file defined ostream &operator<< and
// a static class variable String empty (it is initialized to "").
//

#include "config_dap.h"

static char rcsid[] __unused__ ="$Id: AttrTable.cc,v 1.18 1997/07/15 21:58:04 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include <ostream.h>

#include "AttrTable.h"

// Private member functions

// When CON_ONLY is true, stop recurring when at the last container.
// This ensures that the Pix returned will reference the AttrTable which
// containes the attribute - not the attribute itself.
Pix 
AttrTable::find(const String &target, bool cont_only = false)
{
    if (target.contains(".")) {
	String name = (String)target; // cast away const
	String container = name.before(".");
	String field = name.after(".");
	
	Pix p = simple_find(container);
	if ((p) && attr_map(p).type == Attr_container) {
	    if (cont_only && !field.contains("."))
		return p;
	    else
		return attr_map(p).value.attributes->find(field, cont_only);
	}
	else
	    return 0;
    }
    else
	return simple_find(target);
}

Pix
AttrTable::simple_find(const String &target)
{
    for (Pix p = attr_map.first(); p; attr_map.next(p))
	if (target == attr_map(p).name)
	    return p;
    return 0;
}

String 
AttrTable::AttrTypeToString(const AttrType at)
{
    switch (at) {
      case Attr_container: return "Container";
      case Attr_byte: return "Byte";
      case Attr_int32: return "Int32";
      case Attr_uint32: return "Uint32";
      case Attr_float64: return "Float64";
      case Attr_string: return "String";
      case Attr_url: return "Url";
      default: return "";
    }
}

AttrType
AttrTable::StringToAttrType(const String &s)
{
    String s2 = capitalize(s);
    if (s2 == "Container")
	return Attr_container;
    else if (s2 == "Byte")
	return Attr_byte;
    else if (s2 == "Int32")
	return Attr_int32;
    else if (s2 == "Uint32")
	return Attr_uint32;
    else if (s2 == "Float64")
	return Attr_float64;
    else if (s2 == "String")
	return Attr_string;
    else if (s2 == "Url")
	return Attr_url;
    else 
	return Attr_unknown;
}

// Public member functions

AttrTable::AttrTable()
{
}

AttrTable::~AttrTable()
{
    for (Pix p = attr_map.first(); p; attr_map.next(p))
	// Don't delete the referenced objects in an alias!
	if (!attr_map(p).is_alias)
	    if (attr_map(p).type == Attr_container)
		delete attr_map(p).value.attributes;
	    else
		delete attr_map(p).value.attr;
}

Pix 
AttrTable::first_attr()
{
    return attr_map.first();
}

void
AttrTable::next_attr(Pix &p)
{
    attr_map.next(p);
}

String
AttrTable::get_name(Pix p)
{
    assert(p);
    return attr_map(p).name;
}

bool
AttrTable::is_container(Pix p)
{
    assert(p);
    return attr_map(p).type == Attr_container;
}

AttrTable *
AttrTable::get_attr_table(Pix p)
{
    assert(p);
    return attr_map(p).type == Attr_container ? attr_map(p).value.attributes : 0;
}

AttrTable *
AttrTable::get_attr_table(const String &name)
{
    Pix p = find(name, true);	// Return only Pixes to container attributes
    return (p) ?  get_attr_table(p) : 0;
}

AttrTable *
AttrTable::get_attr_table(const char *name)
{
    return get_attr_table((String)name);
}

String
AttrTable::get_type(Pix p)
{
    assert(p);
    return AttrTypeToString(attr_map(p).type);
}

String
AttrTable::get_type(const String &name)
{
    Pix p = find(name);
    return (p) ?  get_type(p) : (String)"";
}

String
AttrTable::get_type(const char *name)
{
    return get_type((String)name);
}

AttrType
AttrTable::get_attr_type(Pix p)
{
    assert(p);
    return attr_map(p).type;
}

AttrType
AttrTable::get_attr_type(const String &name)
{
    Pix p = find(name);
    return (p) ?  get_attr_type(p) : Attr_unknown;
}

AttrType
AttrTable::get_attr_type(const char *name)
{
    return get_attr_type((String)name);
}

unsigned int 
AttrTable::get_attr_num(Pix p)
{
    assert(p);
    return attr_map(p).type == Attr_container ? 0 : attr_map(p).value.attr->length();
}

unsigned int 
AttrTable::get_attr_num(const String &name)
{
    Pix p = find(name);
    return (p) ?  get_attr_num(p) : 0;
}

unsigned int 
AttrTable::get_attr_num(const char *name)
{
    return get_attr_num((String)name);
}

String
AttrTable::get_attr(Pix p, unsigned int i)
{
    assert(p);
    return attr_map(p).type == Attr_container ? (String)"None" : (*attr_map(p).value.attr)[i];
}

String
AttrTable::get_attr(const String &name, unsigned int i)
{
    Pix p = find(name);
    return (p) ? get_attr(p, i) : (String)"";
}

String
AttrTable::get_attr(const char *name, unsigned int i)
{
    return get_attr((String)name, i);
}

StringXPlex *
AttrTable::get_attr_vector(Pix p)
{
    assert(p);
    return attr_map(p).type != Attr_container ? attr_map(p).value.attr : 0;
}

StringXPlex *
AttrTable::get_attr_vector(const String &name)
{
    Pix p = find(name);
    return (p) ?  get_attr_vector(p) : 0;
}

StringXPlex *
AttrTable::get_attr_vector(const char *name)
{
    return get_attr_vector((String)name);
}

unsigned int
AttrTable::append_attr(const String &name, const String &type, 
		       const String &attr)
{
    Pix p = find(name);
    // If the types don't match OR this attribute is a container, calling
    // this mfunc is an error!
    if (p && (get_type(p) != type || get_type(p) == "Container"))
	return 0;
    else if (p)			// Must be a new attribute value; add it.
	return attr_map(p).value.attr->add_high(attr) + 1;
    else {			// Must be a completely new attribute; add it
	entry e;

	e.name = name;
	e.is_alias = false;
	e.type = StringToAttrType(type); // Record type using standard names.
	e.value.attr = new StringXPlex;
	unsigned int len = e.value.attr->add_high(attr) + 1;

	attr_map.append(e);
    
	return len;		// return the length of the attr XPlex
    }
}

unsigned int
AttrTable::append_attr(const char *name, const char *type, const char *attr)
{
    return append_attr((String)name, (String)type, (String)attr);
}

AttrTable *
AttrTable::append_container(const String &name)
{
    Pix p = find(name);
    
    // Return an error if NAME already exists.
    if (p)
	return 0;
	
    entry e;
    e.name = name;
    e.is_alias = false;
    e.type = Attr_container;
    e.value.attributes = new AttrTable();

    attr_map.append(e);

    return e.value.attributes;
}

bool
AttrTable::attr_alias(const String &alias, AttrTable *at, const String &name)
{
    // It is an error for alias to exist already.
    if (find(alias))
	return false;

    // Check for null Attrtable for source (information to be aliased).
    if (!at)
	return false;

    // Make sure that `name' really exists.
    Pix p = at->find(name);
    if (!p)
	return false;

    entry e;
    e.name = alias;
    e.is_alias = true;
    e.type = at->attr_map(p).type;
    if (e.type == Attr_container)
	e.value.attributes = at->get_attr_table(p);
    else {
	e.value.attr = at->attr_map(p).value.attr;
    }

    attr_map.append(e);
    
    return true;
}

bool
AttrTable::attr_alias(const String &alias, const String &name)
{
    return attr_alias(alias, this, name);
}

// Delete the attribute NAME. If NAME is an array of attributes, delete the
// I(th) element if I is >= 0. If I is -1 (the default), remove the entire
// attribute even f it is an array of values.
//
// Returns: void

void
AttrTable::del_attr(const String &name, int i)
{
    Pix p = find(name);
    if (p) {
	if (i == -1) {		// Delete the whole attribute
	    attr_map.prev(p);	// p now points to the previous element
	    attr_map.del_after(p);	// ... delete the following element
	}
	else {			// Delete one element from attribute array
	    // Don't try to delete elements from the vector of values if the
	    // map is a container!
	    if (attr_map(p).type == Attr_container) 
		return;

	    StringXPlex *sxp = attr_map(p).value.attr;
		
	    assert(i >= 0 && i < sxp->fence());
	    for (int j = i+1; j < sxp->fence(); ++j)
		sxp[j-1] = sxp[j]; // mv array elements to empty space
	    (void) sxp->del_high(); // rm extra element
	}
    }
}
	
	    
void
AttrTable::print(ostream &os, String pad)
{
    for(Pix p = attr_map.first(); p; attr_map.next(p)) {
	switch (attr_map(p).type) {
	  case Attr_container:
	    os << pad << get_name(p) << " {" << endl;

	    attr_map(p).value.attributes->print(os, pad + "    ");

	    os << pad << "}" << endl;
	    break;

	  default: {
		os << pad << get_type(p) << " " << get_name(p) << " " ;

		StringXPlex *sxp = attr_map(p).value.attr;

		for (int i = 0; i < sxp->high(); ++i)
		    os << (*sxp)[i] << ", ";

		os << (*sxp)[sxp->high()] << ";" << endl;
	    }
	    break;
	}
    }
}
