
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// jhrg 7/29/94

#include "config_dap.h"

static char rcsid[] not_used ="$Id: AttrTable.cc,v 1.38 2003/04/22 19:40:27 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include <iostream>

#include "Error.h"
#include "debug.h"
#include "util.h"
#include "AttrTable.h"
#include "escaping.h"
#include "AttrIterAdapter.h"

using std::cerr;
using std::string;
using std::endl;

#ifdef WIN32
using std::vector<string>;
#else
using std::vector;
#endif

string 
AttrTable::AttrType_to_String(const AttrType at)
{
    switch (at) {
      case Attr_container: return "Container";
      case Attr_byte: return "Byte";
      case Attr_int16: return "Int16";
      case Attr_uint16: return "Uint16";
      case Attr_int32: return "Int32";
      case Attr_uint32: return "Uint32";
      case Attr_float32: return "Float32";
      case Attr_float64: return "Float64";
      case Attr_string: return "String";
      case Attr_url: return "Url";
      default: return "";
    }
}

AttrType
AttrTable::String_to_AttrType(const string &s)
{
    string s2 = s;
    downcase(s2);

    if (s2 == "container")
	return Attr_container;
    else if (s2 == "byte")
	return Attr_byte;
    else if (s2 == "int16")
	return Attr_int16;
    else if (s2 == "uint16")
	return Attr_uint16;
    else if (s2 == "int32")
	return Attr_int32;
    else if (s2 == "uint32")
	return Attr_uint32;
    else if (s2 == "float32")
	return Attr_float32;
    else if (s2 == "float64")
	return Attr_float64;
    else if (s2 == "string")
	return Attr_string;
    else if (s2 == "url")
	return Attr_url;
    else 
	return Attr_unknown;
}

/** Clone the given attribute table in <tt>this</tt>. 
    Protected. */
void
AttrTable::clone(const AttrTable &at)
{
    Attr_citer i = at.attr_map.begin() ;
    Attr_citer ie = at.attr_map.end() ;
    for( ; i != ie; i++ )
    {
	entry *e = new entry( *(*i) ) ;
	attr_map.push_back( e ) ;
    }
}

/** @name Instance management functions */

//@{
AttrTable::AttrTable()
{
}

AttrTable::AttrTable(const AttrTable &rhs)
{
    clone(rhs);
}

// Private
void
AttrTable::delete_attr_table() 
{
    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); i++)
    {
	entry *e = *i ;		// Why not 'delete *i;'? 02/25/03 jhrg
	delete e ;
    }
}

AttrTable::~AttrTable()
{
    DBG(cerr << "Entering ~AttrTable (" << this << ")" << endl);
    delete_attr_table();
    DBG(cerr << "Exiting ~AttrTable" << endl);
}

AttrTable &
AttrTable::operator=(const AttrTable &rhs)
{
    if (this != &rhs) {
	delete_attr_table();
	clone(rhs);
    }

    return *this;
}	    
//@}

/** Attributes that are containers count one attribute, as do
    attributes with both scalar and vector values. 
    @return The number of entries. 
    @brief Get the number of entries in this attribute table.
*/
unsigned int
AttrTable::get_size() const
{
    return attr_map.size();
}

/** @brief Get the name of this attribute table. 
    @return A string containing the name. */
string
AttrTable::get_name()
{
    return d_name;
}

/** @brief Set the name of this attribute table.
    @param n The new name of the attribute table. */
void
AttrTable::set_name(const string &n)
{
    d_name = www2id(n);
}

/** If the given name already refers to an attribute, and the attribute has a
    value, the given value is appended to the attribute vector. Calling this
    function repeatedly is the way to append to an attribute vector.

    The function returns an error condition if the attribute is a container,
    or if the type of the input value does not match the existing attribute's
    type. Use <tt>append_container()</tt> to add container attributes.

    This method performs a simple search for <tt>name</tt> in this attribute
    table only; sub-tables are not searched and the dot notation is not
    recognized.

    @brief Add an attribute to the table.
    @return Returns the length of the added attribute value.
    @param name The name of the attribute to add or modify.
    @param type The type of the attribute to add or modify.
    @param attribute The value to add to the attribute table. */
unsigned int
AttrTable::append_attr(const string &name, const string &type, 
		       const string &attribute) throw (Error)
{
    string lname = www2id(name);

    Attr_iter iter = simple_find( lname, true ) ;

    // If the types don't match OR this attribute is a container, calling
    // this mfunc is an error!
    if (iter != attr_map.end() && ((*iter)->type != String_to_AttrType(type)))
	throw Error(string("An attribute called `") + name 
		    + string("' already exists but is of a different type"));
    if (iter != attr_map.end() && (get_type(iter) == "Container"))
	throw Error(string("An attribute called `") + name 
		    + string("' already exists but is a container."));

    if (iter != attr_map.end()) {    // Must be a new attribute value; add it.
        (*iter)->attr->push_back(attribute);
	return (*iter)->attr->size();
    } else {			// Must be a completely new attribute; add it
	entry *e = new entry;

	e->name = lname;
	e->is_alias = false;
	e->type = String_to_AttrType(type); // Record type using standard names.
	e->attr = new vector<string>;
	e->attr->push_back(attribute);

	attr_map.push_back(e);
    
	return e->attr->size();	// return the length of the attr vector
    }
}

unsigned int
AttrTable::append_attr(const char *name, const char *type, const char *attr)
    throw (Error)
{
    return append_attr((string)name, (string)type, (string)attr);
}

/** Create and append an attribute container to this AttrTable. If this
    attribute table already contains an attribute container called
    <tt>name</tt> an exception is thrown.

    @brief Add a container to the attribute table.
    @return A pointer to the new AttrTable object. 
*/

AttrTable *
AttrTable::append_container(const string &name) throw (Error)
{
    AttrTable *new_at = new AttrTable ;
    AttrTable *ret = NULL ;
    try
    {
	ret = append_container( new_at, name ) ;
    }
    catch( Error &e )
    {
	// an error occurred, attribute with that name already exists
	delete new_at ;
	throw e ;
    }
    return ret ;
}

/** Append a new attribute container to this attribute table. The new
    container is <tt>at</tt> and its name is set to
    <tt>name</tt>. If this attribute 
    table already contains an attribute container called
    <tt>name</tt> an exception is thrown.

    @brief Add a container to the attribute table.
    @return A pointer to the new AttrTable object. 
*/
AttrTable *
AttrTable::append_container(AttrTable *at, const string &name) throw (Error)
{
    string lname = www2id(name);

    if (simple_find(name))
	throw Error(string("There already exists a container called `")
		    + name + string("in this attribute table."));

    at->set_name(lname);

    entry *e = new entry;
    e->name = lname;
    e->is_alias = false;
    e->type = Attr_container;
    e->attributes = at;

    attr_map.push_back(e);

    return e->attributes;
}

/** Look for an attribute or an attribute container. If used to search
    for an attribute container, this method returns the container's {\it
    parent} using the value-result parameter #at# and a reference to the
    container using the Pix return value. If used to search for an
    attribute, the attribute's container is returned using #at#; the
    attribute itself can be accessed using the Pix return value.

    @param target The name (using dot notation) of the attribute or
    container to find.
    @param at A value-result used to return the attribute container in
    which #target# was found. Null if #target# was not found.
    @param iter The itereator which will reference the attribute found
    which can be used to access #target# from within #at#. References
    dim_end() within #at# if the attribute or container does not exist. */

void
AttrTable::find( const string &target, AttrTable **at, Attr_iter &iter )
{
    string::size_type dotpos = target.rfind('.');
    if (dotpos != string::npos)
    {
	string container = target.substr(0, dotpos);
	string field = target.substr(dotpos+1);

	*at = find_container( container ) ;
	if(*at)
	{
	    iter = (*at)->simple_find(field, true) ;
	} else {
	    iter = attr_map.end() ;
	}
    }
    else {
	*at = this;
	iter = simple_find(target, true);
    }
}

// Private
AttrTable::Attr_iter
AttrTable::simple_find( const string &target, bool )
{
    Attr_iter i ;
    for( i = attr_map.begin(); i != attr_map.end(); i++ )
    {
	if( target == (*i)->name )
	{
	    break ;
	}
    }
    return i ;
}

/** Look in this attribute table for an attribute container named
    <tt>target</tt>. The search starts at this attribute table;
    <tt>target</tt> should
    use the dot notation to name containers held within children of this
    attribute table. 

    To search the entire DAS object, make sure to invoke this method from
    that object.

    @brief Find an attribute with a given name.
    @param target The attribute container to find.
    @return A pointer to the attribute table or null if the container
    cannot be found. */
AttrTable *
AttrTable::find_container(const string &target)
{
    string::size_type dotpos = target.find('.');
    if (dotpos != string::npos) {
	string container = target.substr(0, dotpos);
	string field = target.substr(dotpos+1);
	
	AttrTable *at= simple_find_container(container);
	return (at) ? at->find_container(field) : 0;
    }
    else {
	return simple_find_container(target);
    }
}

// Private
AttrTable *
AttrTable::simple_find_container(const string &target)
{
    if (get_name() == target)
	return this;

    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); i++)
    {
	if (is_container(i) && target == (*i)->name)
	{
	    return (*i)->attributes;
	}
    }

    return 0;
}

/** Each of the following accessors get information using the name of an
    attribute. They perform a simple search for the name in this
    attribute table only; sub-tables are not searched and the dot
    notation is not recognized.

    @name Accessors using an attribute name */
//@{

/** @brief Get an attribute container. */
AttrTable *
AttrTable::get_attr_table(const string &name)
{
    return find_container(name);
}

AttrTable *
AttrTable::get_attr_table(const char *name)
{
    return get_attr_table((string)name);
}

/** @brief Get the type name of an attribute within this attribute table. */
string
AttrTable::get_type(const string &name)
{
    Pix p = simple_find(name);
    return (p) ?  get_type(p) : (string)"";
}

string
AttrTable::get_type(const char *name)
{
    return get_type((string)name);
}

/** @brief Get the type of an attribute.
    @return The <tt>AttrType</tt> value describing the attribute. */
AttrType
AttrTable::get_attr_type(const string &name)
{
    Pix p = simple_find(name);
    return (p) ?  get_attr_type(p) : Attr_unknown;
}

AttrType
AttrTable::get_attr_type(const char *name)
{
    return get_attr_type((string)name);
}

/** If the indicated attribute is a container attribute, this function
    returns the number of attributes in <i>its</i> attribute table. If the
    indicated attribute is not a container, the method returns the number
    of values for the attribute (1 for a scalar attribute, N for a vector
    attribute value). 
    @brief Get the number of attributes in this container.
*/
unsigned int 
AttrTable::get_attr_num(const string &name)
{
    Attr_iter iter = simple_find(name, true);
    return (iter != attr_map.end()) ?  get_attr_num(iter) : 0;
}

unsigned int 
AttrTable::get_attr_num(const char *name)
{
    return get_attr_num((string)name);
}

/** Get a pointer to the vector of values associated with the attribute
    referenced by Pix <tt>p</tt> or named <tt>name</tt>.

    Note that all values in an attribute table are stored as string data.
    They may be converted to a more appropriate internal format by the
    calling program.

    @return If the indicated attribute is a container, this function
    returns the null pointer.  Otherwise returns a pointer to the
    the attribute vector value. 
    @brief Get a vector-valued attribute.
*/
vector<string> *
AttrTable::get_attr_vector(const string &name)
{
    Pix p = simple_find(name);
    return (p) ?  get_attr_vector(p) : 0;
}

vector<string> *
AttrTable::get_attr_vector(const char *name)
{
    return get_attr_vector((string)name);
}

/** Delete the attribute named <tt>name</tt>. If <tt>i</tt> is given, and
    the attribute has a vector value, delete the <tt>i</tt>$^th$
    element of the vector.

    You can use this function to delete container attributes, although
    the <tt>i</tt> parameter has no meaning for that operation.

    @brief Deletes an attribute.
    @param name The name of the attribute to delete.  This can be an
    attribute of any type, including containers. However, this method
    looks only in this attribute table and does not recognize the dot
    notation. 
    @param i If the named attribute is a vector, and <tt>i</tt> is
    non-negative, the i-th entry in the vector is deleted, and the
    array is repacked.  If <tt>i</tt> equals -1 (the default), the
    entire attribute is deleted. */
void
AttrTable::del_attr(const string &name, int i)
{
    string lname = www2id(name);

    Attr_iter iter = simple_find( lname, true ) ;
    if ( iter != attr_map.end() ) {
	if (i == -1) {		// Delete the whole attribute
	    entry *e = *iter ;
	    attr_map.erase( iter ) ;
	    delete e ;
	}
	else {			// Delete one element from attribute array
	    // Don't try to delete elements from the vector of values if the
	    // map is a container!
	    if ((*iter)->type == Attr_container) 
		return;

	    vector<string> *sxp = (*iter)->attr;
		
	    assert(i >= 0 && i < (int)sxp->size());
	    sxp->erase(sxp->begin() + i); // rm the element
	}
    }
}

//@} Accessors using an attribute name

/** @name Deprecated Pix accessors */
//@{

// Private, deprecated
Pix
AttrTable::simple_find(const string &target)
{
    for (Pix p = first_attr(); p; next_attr(p))
    {
	if (target == attr(p)->name)
	{
	    return p ;
	}
    }
    return (IteratorAdapter *)0;
}

/** @brief Get a reference to the first entry in this attribute table. 
    @return Pix; returns null if there's nothing in the list. 
    @deprecated */
Pix 
AttrTable::first_attr()
{
    AttrIterAdapter *i = new AttrIterAdapter( attr_map ) ;
    i->first() ;
    return i ;
}

/** Advance to the next element of this attribute table. Set to null when
    there are no more elements in the list.
    @brief Increment a Pix pointer into the attribute table.
    @param p Pix to advance. 
    @deprecated */
void
AttrTable::next_attr(Pix p)
{
    p.next() ;
}

AttrTable::entry *
AttrTable::attr(Pix p)
{
    AttrIterAdapter *i = (AttrIterAdapter *)p.getIterator() ;

    if( i ) {
	return i->entry() ;
    }
    return 0 ;
}

/** @brief Returns the name of the attribute table. 
    @deprecated */
string
AttrTable::get_name(Pix p)
{
    AttrIterAdapter *i = (AttrIterAdapter *)p.getIterator() ;
    assert(i);
    return i->entry()->name ;
}

/** @brief Returns true if the attribute is a container. 
    @deprecated */
bool
AttrTable::is_container(Pix p)
{
    AttrIterAdapter *i = (AttrIterAdapter *)p.getIterator() ;
    assert(i);
    return i->entry()->type == Attr_container;
}

/** Get the attribute container referenced by <tt>p</tt>. If no
    such container exists, then return null. 
    @brief Return an attribute container.
    @param p Reference to a table contained by this object.
    @return The child attribute table. 
    @deprecated */
AttrTable *
AttrTable::get_attr_table(Pix p)
{
    AttrIterAdapter *i = (AttrIterAdapter *)p.getIterator() ;
    assert(i);
    return i->entry()->type == Attr_container ? i->entry()->attributes : 0;
}

/** @brief Get the type name of an attribute.
    @param p
    @return A string with the name of this attribute datatype. 
    @deprecated */
string
AttrTable::get_type(Pix p)
{
    AttrIterAdapter *i = (AttrIterAdapter *)p.getIterator() ;
    assert(i);
    return AttrType_to_String(i->entry()->type);
}

/** @brief Get the type of an attribute.
    @param p
    @return The datatype of this attribute in an instance of AttrType. 
    @deprecated */
AttrType
AttrTable::get_attr_type(Pix p)
{
    return attr(p)->type;
}

/** If the indicated attribute is a container attribute, this function
    returns the number of attributes in <i>its</i> attribute table. If the
    indicated attribute is not a container, the method returns the number
    of values for the attribute (1 for a scalar attribute, N for a vector
    attribute value).
    @param p
    @return The number of elements in the attribute.
    @brief Get the number of attributes in this container.

    @deprecated */
unsigned int 
AttrTable::get_attr_num(Pix p)
{
    assert(p);
    return (attr(p)->type == Attr_container)
	? attr(p)->attributes->get_size() 
	: attr(p)->attr->size();
}

/** Returns the value of an attribute. If the attribute has a vector
    value, you can indicate which is the desired value with the index
    argument, <tt>i</tt>. If the argument is omitted, the first value is
    returned. If the attribute has only a single value, the index
    argument is ignored. If <tt>i</tt> is greater than the number of
    elements in the attribute, an error is produced.

    All values in an attribute table are stored as string data. They may
    be converted to a more appropriate internal format by the calling
    program.

    @brief Return the value of an attribute.
    @param p
    @param i The attribute value index, zero-based.
    @return If the indicated attribute is a container, this function
    returns the string ``None''. If using a name to refer to the attribute
    and the named attribute does not exist, return the empty string. 
    @deprecated */
string
AttrTable::get_attr(Pix p, unsigned int i)
{
    assert(p);
    return attr(p)->type == Attr_container ? (string)"None" : (*attr(p)->attr)[i];
}

/** Get the value of an attribute. If the attribute has a vector value,
    you can indicate which is the desired value with the index argument,
    <tt>i</tt>. If the argument is omitted, the first value is
    returned. If the 
    attribute has only a single value, the index argument is ignored. If
    <tt>i</tt> is greater than the number of elements in the
    attribute, an error is produced.

    All values in an attribute table are stored as string data. They may
    be converted to a more appropriate internal format by the calling
    program. 
    @brief Get the value of an attribute.

    @deprecated */
string
AttrTable::get_attr(const string &name, unsigned int i)
{
    Pix p = simple_find(name);
    return (p) ? get_attr(p, i) : (string)"";
}

/** Returns a pointer to the vector of values associated with the
    attribute referenced by Pix <tt>p</tt> or named <tt>name</tt>. 

    Note that all values in an attribute table are stored as string data.
    They may be converted to a more appropriate internal format by the
    calling program.

    @brief Return a vector-valued attribute.
    @param p
    @return If the indicated attribute is a container, this function
    returns the null pointer.  Otherwise returns a pointer to the
    the attribute vector value. 
    @deprecated */
vector<string> *
AttrTable::get_attr_vector(Pix p)
{
    assert(p);
    return attr(p)->type != Attr_container ? attr(p)->attr : 0;
}

/** Look for an attribute or an attribute container. If used to search for an
    attribute container, this method returns the container's <i> parent</i>
    using the value-result parameter <tt>at</tt> and a reference to the
    container using the Pix return value. If used to search for an attribute,
    the attribute's container is returned using <tt>at</tt>; the attribute
    itself can be accessed using the Pix return value.

    @brief Find an attribute or container with a given name.
    @param target The name (using dot notation) of the attribute or
    container to find.
    @param at A value-result used to return the attribute container in
    which <tt>target</tt> was found. Null if <tt>target</tt> was not found.
    @return A Pix which can be used to access <tt>target</tt> from
    within <tt>at</tt>. 
    Null if the attribute or container does not exist. 
    @deprecated */
Pix 
AttrTable::find(const string &target, AttrTable **at)
{
    string::size_type dotpos = target.rfind('.');
    if (dotpos != string::npos) {
	string container = target.substr(0, dotpos);
	string field = target.substr(dotpos+1);
	
	*at = find_container(container);
	return (*at) ? (*at)->simple_find(field) : (IteratorAdapter *)0;
    }
    else {
	*at = this;
	return simple_find(target);
    }
}

/** @name get information using an iterator */
//@{
/** Get an iterator to the first entry in this attribute table. 
    @return Attr_iter; references the end of the array if empty list. */
AttrTable::Attr_iter
AttrTable::attr_begin()
{
    return attr_map.begin() ;
}

/** Get an iterator to the end attribute table. Does not point to 
    the last attribute in the table
    @return Attr_iter */
AttrTable::Attr_iter
AttrTable::attr_end()
{
    return attr_map.end() ;
}

/** Given an index \c i, return the \c Attr_iter to the corresponding
    element. This method provides a way to use all the methods that take an
    \c Attr_iter using a simple integer index. Use the get_attr_num() or
    get_size() methods to determine how many items the AttrTable contains.

    @param i The index
    @return The corresponding Attr_iter
    @see get_attr_num, get_size */
AttrTable::Attr_iter
AttrTable::get_attr_iter(int i)
{
    return attr_map.begin() + i;
}

/** Returns the name of the attribute referenced by #iter#. */
string
AttrTable::get_name( Attr_iter &iter )
{
    assert( iter != attr_map.end() ) ;

    return (*iter)->name ;
}

/** Returns true if the attribute referenced by #iter# is a container. */
bool
AttrTable::is_container( Attr_iter &i )
{
    return (*i)->type == Attr_container ;
}

/** Get the attribute container referenced by #iter#. If no
    such container exists, then return a reference to the end of the
    table.
    @param iter Reference to a table contained by this object.
    @return The child attribute table. */
AttrTable *
AttrTable::get_attr_table( Attr_iter &iter )
{
    assert( iter != attr_map.end() ) ;
    return (*iter)->type == Attr_container ? (*iter)->attributes : 0 ;
}

/** Get the type name of an attribute referenced by #iter#.
    @param iter
    @return A string with the name of this attribute datatype. */
string
AttrTable::get_type( Attr_iter &iter )
{
    assert( iter != attr_map.end() ) ;
    return AttrType_to_String( (*iter)->type ) ;
}

/** Get the type of the attribute referenced by #iter#.
    @param iter
    @return The datatype of this attribute in an instance of AttrType. */
AttrType
AttrTable::get_attr_type( Attr_iter &iter )
{
    return (*iter)->type ;
}

/** If the attribute referenced by #iter# is a container attribute, this
    method returns the number of attributes in {\it its} attribute table.
    If the indicated attribute is not a container, the method returns the 
    number of values for the attribute (1 for a scalar attribute, N for a 
    vector attribute value).
    @param iter Reference to an attribute
    @return The number of elements in the attribute. */
unsigned int
AttrTable::get_attr_num( Attr_iter &iter )
{
    assert( iter != attr_map.end() ) ;
    return ( (*iter)->type == Attr_container )
	? (*iter)->attributes->get_size()
	: (*iter)->attr->size() ;
}

/** Returns the value of an attribute. If the attribute has a vector
    value, you can indicate which is the desired value with the index
    argument, #i#. If the argument is omitted, the first value is
    returned. If the attribute has only a single value, the index
    argument is ignored. If #i# is greater than the number of
    elements in the attribute, an error is produced.

    All values in an attribute table are stored as string data. They may
    be converted to a more appropriate internal format by the calling
    program.

    @param iter Reference to an attribute
    @param i The attribute value index, zero-based.
    @return If the indicated attribute is a container, this function
    returns the string ``None''. If using a name to refer to the attribute
    and the named attribute does not exist, return the empty string. */
string
AttrTable::get_attr(Attr_iter &iter, unsigned int i)
{
    assert(iter != attr_map.end());
    return (*iter)->type == Attr_container ? (string)"None" : (*(*iter)->attr)[i];
}

string
AttrTable::get_attr(const char *name, unsigned int i)
{
    return get_attr((string)name, i);
}

/** Returns a pointer to the vector of values associated with the
    attribute referenced by iterator #iter#. 

    Note that all values in an attribute table are stored as string data.
    They may be converted to a more appropriate internal format by the
    calling program.

    @param iter
    @return If the indicated attribute is a container, this function
    returns the null pointer.  Otherwise returns a pointer to the
    the attribute vector value. */
vector<string> *
AttrTable::get_attr_vector(Attr_iter &iter)
{
    assert(iter != attr_map.end());
    return (*iter)->type != Attr_container ? (*iter)->attr : 0;
}

//@} Accessors that use an iterator

// Alias an attribute table. The alias should be added to this object.
/** @brief Add an alias to a container held by this attribute table. 
    @param name The name of the alias. May <i>not</i> use dot notation.
    @param src The existing attribute container to alias.
    @exception Error if an attribute, container or alias called
    <tt>name</tt> already exists in this attribute table. */
void
AttrTable::add_container_alias(const string &name, AttrTable *src) 
    throw (Error)
{
    string lname = www2id(name);

    if (simple_find(lname))
	throw Error(string("There already exists a container called `")
		    + name + string("in this attribute table."));

    entry *e = new entry;
    e->name = lname;
    e->is_alias = true;
    e->aliased_to = src->get_name();
    e->type = Attr_container;

    e->attributes = src;

    attr_map.push_back(e);
}

// Assume #source# names an attribute value in some container. Add an alias
// #name# for that value in this object.
/** @brief Add an alias to an attribute held by this attribute table.
    @param das 
    @param name The name of the alias. May <i>not</i> use dot notation.
    @param source The name of the attribute to alias. May use dot
    notation. 
    @exception Error if the attribute table already contains an
    attribute, container or alias called <tt>name</tt> or if an
    attribute called <tt>source</tt> does not exist. */
void
AttrTable::add_value_alias(AttrTable *das, const string &name, 
			   const string &source) throw (Error)
{
    string lname = www2id(name);
    string lsource = www2id(source);

    // find the container that holds #source# and then find #source#'s Pix
    // within that container. Search at the uppermost level of the attribtue
    // object to find values defined `above' the current container.
    AttrTable *at;
    Pix p = das->find(lsource, &at);

    // If #source# is not found by looking at the topmost level, look in the
    // current table (i.e., alias z x where x is in the current container
    // won't be found by looking for `x' at the top level). See test case 26
    // in das-testsuite.
    if (!(at && p)) {
	p = find(lsource, &at);
	if (!(at && p))
	    throw Error(string("Could not find the attribute `")
			+ source + string("' in the attribute object."));
    }

    // If we've got a value to alias and it's being added at the top level of
    // the DAS, that's an error.
    if (!at->is_container(p) && this == das)
	throw Error(string("A value cannot be aliased to the top level of the\
 DAS;\nOnly containers may be present at that level of the DAS."));

    if (simple_find(lname))
	throw Error(string("There already exists a container called `")
		    + name + string("in this attribute table."));

    entry *e = new entry;
    e->name = lname;
    e->is_alias = true;
    e->aliased_to = lsource;
    e->type = at->attr(p)->type;
    if (e->type == Attr_container)
	e->attributes = at->get_attr_table(p);
    else
	e->attr = at->attr(p)->attr;

    attr_map.push_back(e);
}

// Deprecated
/** Once an alias is
    inserted into an attribute table, reading the attributes for
    <i>alias</i> will return those stored for <i>name</i>. 

    Two forms for this function exist: one searches for <i>name</i>
    in the AttrTable referenced by <i>at</i> while the other uses
    <tt>this</tt>. You can use <tt>DAS::get_attr_table()</tt> to
    get the attribute table for an arbitrary name.

    @brief Adds an alias to the set of attributes.  
    @name attr_alias()
    @see get_attr_table
    @deprecated 
    @param alias The alias to insert into the attribute table.
    @param name The name of the already-existing attribute to which
    the alias will refer.
    @param at An attribute table in which to insert the alias. */
bool
AttrTable::attr_alias(const string &alias, AttrTable *at, const string &name)
{
    add_value_alias(at, alias, name);
    return true;
}

/** @deprecated
    @param alias The alias to insert into the attribute table.
    @param name The name of the already-existing attribute to which
    the alias will refer. */
bool
AttrTable::attr_alias(const string &alias, const string &name)
{
    return attr_alias(alias, this, name);
}

/** Erase the entire attribute table. This returns an AttrTable to the empty
    state that's the same as the object generated by the null constructor.
    @brief Erase the attribute table. */
void
AttrTable::erase()
{
    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); i++) {
	delete *i;
	*i = 0;
    }

    attr_map.erase(attr_map.begin(), attr_map.end());

    d_name = "";
}

/** A simple printer that does nothing fancy with aliases.
    Protected.
    @deprecated
    @see simple_print(FILE *out, string pad, Attr_iter &i, bool dereference);
*/
void
AttrTable::simple_print(ostream &os, string pad, Attr_iter &i,
			bool dereference)
{
    switch ((*i)->type) {
      case Attr_container:
	os << pad << id2www(get_name(i)) << " {" << endl;

	(*i)->attributes->print(os, pad + "    ", dereference);

	os << pad << "}" << endl;
	break;

      default: {
	    os << pad << get_type(i) << " " << id2www(get_name(i)) << " " ;

	    vector<string> *sxp = (*i)->attr;
	    
	    vector<string>::iterator last = sxp->end()-1;
	    for (vector<string>::iterator i = sxp->begin(); i != last; ++i)
		os << *i << ", ";
  
	    os << *(sxp->end()-1) << ";" << endl;
	}
	break;
    }
}

/** A simple printer that does nothing fancy with aliases. 
    Protected. */
void
AttrTable::simple_print(FILE *out, string pad, Attr_iter &i,
			bool dereference)
{
    switch ((*i)->type) {
      case Attr_container:
	fprintf( out, "%s%s {\n", pad.c_str(), id2www( get_name(i) ).c_str()) ;

	(*i)->attributes->print(out, pad + "    ", dereference);

	fprintf( out, "%s}\n", pad.c_str() ) ;
	break;

      default: {
	    fprintf( out, "%s%s %s ", pad.c_str(), get_type(i).c_str(),
				      id2www(get_name(i)).c_str() ) ;

	    vector<string> *sxp = (*i)->attr;
	    
	    vector<string>::iterator last = sxp->end()-1;
	    for (vector<string>::iterator i = sxp->begin(); i != last; ++i)
		fprintf( out, "%s, ", (*i).c_str() ) ;
  
	    fprintf( out, "%s;\n", (*(sxp->end()-1)).c_str() ) ;
	}
	break;
    }
}

/** Prints an ASCII representation of the attribute table to the
    indicated output stream. The <tt>pad</tt> argument is prefixed to each
    line of the output to provide control of indentation.

    @brief Prints an attribute table.
    @param os Print to the given output stream.
    @param pad Indent elements of a table using this string of spaces. By
    default this is a string of four spaces
    @param dereference If true, follow aliases. Default is false. */
void
AttrTable::print(ostream &os, string pad, bool dereference)
{
    for(Attr_iter i = attr_map.begin(); i != attr_map.end(); i++)
    {
	if ((*i)->is_alias) {
	    if (dereference) {
		simple_print(os, pad, i, dereference);
	    }
	    else {
		os << pad << "Alias " << id2www(get_name(i)) << " " 
		   << id2www((*i)->aliased_to) << ";" << endl;
	    }
	} 
	else {
	    simple_print(os, pad, i, dereference);
	}
    }
}

/** Prints an ASCII representation of the attribute table to the
    indicated FILE pointer. The \c pad argument is prefixed to each
    line of the output to provide control of indentation.

    @memo Prints the attribute table.
    @param out Print to the given output FILE.
    @param pad Indent elements of a table using this string of spaces. By
    default this is a string of four spaces
    @param dereference If true, follow aliases. Default is false. */

void
AttrTable::print(FILE *out, string pad, bool dereference)
{
    for(Attr_iter i = attr_map.begin(); i != attr_map.end(); i++)
    {
	if ((*i)->is_alias) {
	    if (dereference) {
		simple_print(out, pad, i, dereference);
	    }
	    else {
		fprintf( out, "%sAlias %s %s;\n",
			      pad.c_str(),
			      id2www(get_name(i)).c_str(),
			      id2www((*i)->aliased_to).c_str() ) ;
	    }
	} 
	else {
	    simple_print(out, pad, i, dereference);
	}
    }
}

// $Log: AttrTable.cc,v $
// Revision 1.38  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.35.2.2  2003/04/15 00:47:43  jimg
// Added get_attr_iter method.
// Added documentation from the header.
//
// Revision 1.37  2003/02/25 23:27:27  jimg
// Added erase() method.
//
// Revision 1.36  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.35.2.1  2003/02/21 00:10:06  jimg
// Repaired copyright.
//
// Revision 1.35  2003/01/23 00:22:23  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.34  2003/01/15 19:24:39  pwest
// Removing IteratorAdapterT and replacing with non-templated versions.
//
// Revision 1.33  2003/01/10 19:46:39  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.28.4.13  2002/12/31 16:43:20  rmorris
// Patches to handle some of the fancier template code under VC++ 6.0.
//
// Revision 1.28.4.12  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.28.4.11  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.28.4.10  2002/11/06 22:56:52  pwest
// Memory delete errors and uninitialized memory read errors corrected
//
// Revision 1.28.4.9  2002/10/28 21:17:43  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.28.4.8  2002/09/12 22:49:57  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.28.4.7  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.28.4.6  2002/08/08 06:54:56  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.32  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.31  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.28.4.5  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.30  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.28.4.4  2001/08/18 01:48:53  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.28.4.3  2001/07/28 01:10:41  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.29  2001/01/26 19:48:09  jimg
// Merged with release-3-2-3.
//
// Revision 1.28.4.2  2000/11/30 05:24:46  jimg
// Significant changes and improvements to the AttrTable and DAS classes. DAS
// now is a child of AttrTable, which makes attributes behave uniformly at
// all levels of the DAS object. Alias now work. I've added unit tests for
// several methods in AttrTable and some of the functions in parser-util.cc.
// In addition, all of the DAS tests now work.
//
// Revision 1.28.4.1  2000/11/22 21:47:42  jimg
// Changed the implementation of DAS; it now inherits from AttrTable
//
// Revision 1.28  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.27  2000/06/16 18:14:59  jimg
// Merged with 3.1.7
//
// Revision 1.23.6.4  2000/06/14 16:59:00  jimg
// Added instrumentation for the dtor.
//
// Revision 1.26  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.23.6.3  2000/05/18 17:47:21  jimg
// Fixed a bug in the AttrTable. Container attributes below the top level were
// broken in the latest changes to the DAS code.
//
// Revision 1.23.6.2  2000/05/12 18:55:54  jimg
// See comments in AttrTable.h.
//
// Revision 1.23.6.1  2000/01/26 23:55:02  jimg
// Fixed the return type of string::find.
//
// Revision 1.23  1999/04/29 02:29:26  jimg
// Merge of no-gnu branch
//
// Revision 1.22  1999/03/24 23:37:13  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.21  1998/11/24 06:50:07  jimg
// Added instrumentation. Used while I was removing DASVHMap.
//
// Revision 1.20.2.2  1999/02/05 09:32:10  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.20.2.1  1999/02/02 19:43:13  jimg
// StringSPlex Removed
//
// Revision 1.20  1998/08/06 16:06:42  jimg
// Now prints aliases as such rather than replicating the aliased entry.
//
// Revision 1.19  1997/08/09 21:18:41  jimg
// Changed/fixed the type comparison in attr_append so that code which calls
// unsigned int `UInt32' won't break (the type is called Uint32, but the das
// parser and scanner accept UInt32 as well).
//
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
// Added not_used to definition of char rcsid[].
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


