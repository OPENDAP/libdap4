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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// jhrg 7/29/94

#include "config.h"

#include <cassert>
#include <sstream>

#include "AttrTable.h"

#include "util.h"
#include "escaping.h"

#include "debug.h"

// Should the www2id and id2www functions be used to encode attribute names?
// Probably not... jhrg 11/16/11
#define WWW_ENCODING 0
// See the note for del_attr_table(). That method now deletes the contained
// AttrTable.
#define NEW_DEL_ATTR_TABLE_BEHAVIOR 0

using std::cerr;
using std::string;
using std::endl;
using std::vector;

namespace libdap {

/** Remove %20 space encoding */
string remove_space_encoding(const string &s)
{
    string::size_type pos = s.find("%20");
    if (pos != string::npos) {
        string n = s;
        do {
            n.replace(pos, 3, " ");
            pos = n.find("%20");
        } while (pos != string::npos);
        return n;
    }
    else {
        return s;
    }
}

/** Add %20 space encoding. */
string add_space_encoding(const string &s)
{
    string::size_type pos = s.find(" ");
    if (pos != string::npos) {
        string n = s;
        do {
            n.replace(pos, 1, "%20");
            pos = n.find(" ");
        } while (pos != string::npos);
        return n;
    }
    else {
        return s;
    }
}

/** Convert an AttrType to it's string representation.
 @param at The Attribute Type.
 @return The type's string representation */
string AttrType_to_String(const AttrType at)
{
    switch (at) {
    case Attr_container:
        return "Container";
    case Attr_byte:
        return "Byte";
    case Attr_int16:
        return "Int16";
    case Attr_uint16:
        return "UInt16";
    case Attr_int32:
        return "Int32";
    case Attr_uint32:
        return "UInt32";
    case Attr_float32:
        return "Float32";
    case Attr_float64:
        return "Float64";
    case Attr_string:
        return "String";
    case Attr_url:
        return "Url";
    case Attr_other_xml:
        return "OtherXML";
    default:
        return "";
    }
}

AttrType String_to_AttrType(const string &s)
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
    else if (s2 == "otherxml")
        return Attr_other_xml;
    else
        return Attr_unknown;
}

/** Clone the given attribute table in <tt>this</tt>.
 Protected. */
void AttrTable::clone(const AttrTable &at)
{
    d_name = at.d_name;
    d_is_global_attribute = at.d_is_global_attribute;

    // Set the parent to null (no parent, not in container)
    // since using at.d_parent is semantically incorrect
    // and potentially dangerous.
    d_parent = 0;

    Attr_citer i = at.attr_map.begin();
    Attr_citer ie = at.attr_map.end();
    for (; i != ie; ++i) {
        // this deep-copies containers recursively
        entry *e = new entry(*(*i));
        attr_map.push_back(e);

        // If the entry being added was a container,
        // set its parent to this to maintain invariant.
        if (e->type == Attr_container) {
            assert(e->attributes);
            e->attributes->d_parent = this;
        }
    }
}

/** @name Instance management functions */

//@{
AttrTable::AttrTable() :
    DapObj(), d_name(""), d_parent(0), attr_map(), d_is_global_attribute(true)
{
}

AttrTable::AttrTable(const AttrTable &rhs) :
    DapObj()
{
    clone(rhs);
}

// Private
void AttrTable::delete_attr_table()
{
    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); ++i) {
        delete *i;
    }
    attr_map.clear();
}

AttrTable::~AttrTable()
{
    delete_attr_table();
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
unsigned int AttrTable::get_size() const
{
    return attr_map.size();
}

/** @brief Get the name of this attribute table.
 @return A string containing the name. */
string AttrTable::get_name() const
{
    return d_name;
}

/** @brief Set the name of this attribute table.
 @param n The new name of the attribute table. */
void AttrTable::set_name(const string &n)
{
#if WWW_ENCODING
    d_name = www2id(n);
#else
    d_name = remove_space_encoding(n);
#endif
}

#if 0
// This was taken from das.y and could be used here to make the 'dods_errors'
// attribute container like the parser used to. Then again, maybe this feature
// was just BS. jhrg (ticket 1469)
static void add_bad_attribute(AttrTable *attr, const string &type, const string &name, const string &value,
        const string &msg) {
    // First, if this bad value is already in a *_dods_errors container,
    // then just add it. This can happen when the server side processes a DAS
    // and then hands it off to a client which does the same.
    // Make a new container. Call it <attr's name>_errors. If that container
    // already exists, use it.
    // Add the attribute.
    // Add the error string to an attribute in the container called
    // `<name_explanation.'.

    if (attr->get_name().find("_dods_errors") != string::npos) {
        attr->append_attr(name, type, value);
    }
    else {
        // I think _dods_errors should be _dap_error. jhrg 11/16/11
        string error_cont_name = attr->get_name() + "_dods_errors";
        AttrTable *error_cont = attr->get_attr_table(error_cont_name);
        if (!error_cont)
        error_cont = attr->append_container(error_cont_name);

        error_cont->append_attr(name, type, value);

#ifndef ATTR_STRING_QUOTE_FIX
        error_cont->append_attr(name + "_dap_explanation", "String", "\"" + msg + "\"");
#else
        error_cont->append_attr(name + "_dap_explanation", "String", msg);
#endif
    }
}
#endif

/** If the given name already refers to an attribute, and the attribute has a
 value, the given value is appended to the attribute vector. Calling this
 function repeatedly is the way to append to an attribute vector.

 The function throws an Error if the attribute is a container,
 or if the type of the input value does not match the existing attribute's
 type. Use <tt>append_container()</tt> to add container attributes.

 This method performs a simple search for <tt>name</tt> in this attribute
 table only; sub-tables are not searched and the dot notation is not
 recognized.

 @brief Add an attribute to the table.
 @return Returns the length of the added attribute value.
 @param name The name of the attribute to add or modify.
 @param type The type of the attribute to add or modify.
 @param value The value to add to the attribute table. */
unsigned int AttrTable::append_attr(const string &name, const string &type, const string &value)
{
    DBG(cerr << "Entering AttrTable::append_attr" << endl);
#if WWW_ENCODING
    string lname = www2id(name);
#else
    string lname = remove_space_encoding(name);
#endif

    Attr_iter iter = simple_find(lname);

    // If the types don't match OR this attribute is a container, calling
    // this mfunc is an error!
    if (iter != attr_map.end() && ((*iter)->type != String_to_AttrType(type)))
        throw Error(string("An attribute called `") + name + string("' already exists but is of a different type"));
    if (iter != attr_map.end() && (get_type(iter) == "Container"))
        throw Error(string("An attribute called `") + name + string("' already exists but is a container."));

    if (iter != attr_map.end()) { // Must be a new attribute value; add it.
        (*iter)->attr->push_back(value);
        return (*iter)->attr->size();
    }
    else { // Must be a completely new attribute; add it
        entry *e = new entry;

        e->name = lname;
        e->is_alias = false;
        e->type = String_to_AttrType(type); // Record type using standard names.
        e->attr = new vector<string> ;
        e->attr->push_back(value);

        attr_map.push_back(e);

        return e->attr->size(); // return the length of the attr vector
    }
}

/** This version of append_attr() takes a vector<string> of values.
 If the given name already refers to an attribute, and the attribute has
 values, append the new values to the existing ones.

 The function throws an Error if the attribute is a container,
 or if the type of the input value does not match the existing attribute's
 type. Use <tt>append_container()</tt> to add container attributes.

 This method performs a simple search for <tt>name</tt> in this attribute
 table only; sub-tables are not searched and the dot notation is not
 recognized.

 @brief Add an attribute to the table.
 @return Returns the length of the added attribute value.
 @param name The name of the attribute to add or modify.
 @param type The type of the attribute to add or modify.
 @param values A vector of values. Note: The vector is COPIED, not stored. */

unsigned int AttrTable::append_attr(const string &name, const string &type, vector<string> *values)
{
    DBG(cerr << "Entering AttrTable::append_attr(..., vector)" << endl);
#if WWW_ENCODING
    string lname = www2id(name);
#else
    string lname = remove_space_encoding(name);
#endif
    Attr_iter iter = simple_find(lname);

    // If the types don't match OR this attribute is a container, calling
    // this mfunc is an error!
    if (iter != attr_map.end() && ((*iter)->type != String_to_AttrType(type)))
        throw Error(string("An attribute called `") + name + string("' already exists but is of a different type"));
    if (iter != attr_map.end() && (get_type(iter) == "Container"))
        throw Error(string("An attribute called `") + name + string("' already exists but is a container."));

    if (iter != attr_map.end()) { // Must be new attribute values; add.
        vector<string>::iterator i = values->begin();
        while (i != values->end())
            (*iter)->attr->push_back(*i++);

        return (*iter)->attr->size();
    }
    else { // Must be a completely new attribute; add it
        entry *e = new entry;

        e->name = lname;
        e->is_alias = false;
        e->type = String_to_AttrType(type); // Record type using standard names.
        e->attr = new vector<string> (*values);

        attr_map.push_back(e);

        return e->attr->size(); // return the length of the attr vector
    }
}

/** Create and append an attribute container to this AttrTable. If this
 attribute table already contains an attribute container called
 <tt>name</tt> an exception is thrown. Return a pointer to the new container.

 @brief Add a container to the attribute table.
 @param name The name of the container to create.
 @return A pointer to the new AttrTable object.
 */

AttrTable *
AttrTable::append_container(const string &name)
{
    AttrTable *new_at = new AttrTable;
    AttrTable *ret = NULL;
    try {
        ret = append_container(new_at, name);
    } catch (Error &e) {
        // an error occurred, attribute with that name already exists
        delete new_at;
        new_at = 0;
        throw;
    }
    return ret;
}

/** Append a new attribute container to this attribute table. The new
 container is <tt>at</tt> and its name is set to
 <tt>name</tt>. If this attribute
 table already contains an attribute container called
 <tt>name</tt> an exception is thrown.

 @note The value of \e name will override the name of \e at set using the
 set_name() method.

 @brief Add a container to the attribute table.
 @param at A pointer to the new attribute table to append.
 @param name The name of the new attribute table.
 @return A pointer to the new AttrTable object.
 */
AttrTable *
AttrTable::append_container(AttrTable *at, const string &name)
{
#if WWW_ENCODING
    string lname = www2id(name);
#else
    string lname = remove_space_encoding(name);
#endif

    if (simple_find(name) != attr_end())
        throw Error("There already exists a container called '" + name + "' in this attribute table (" + at->get_name() + "). (1)");

    DBG(cerr << "Setting appended attribute container name to: " << lname << endl);
    at->set_name(lname);

    entry *e = new entry;
    e->name = lname;
    e->is_alias = false;
    e->type = Attr_container;
    e->attributes = at;

    attr_map.push_back(e);

    at->d_parent = this;

    return e->attributes;
}

/** Look for an attribute or an attribute container. If used to search
 for an attribute container, this method returns the container's \e
 parent using the value-result parameter \c at and a reference to the
 container using the iterator value-result parameter \c iter. If used
 to search for an attribute, the attribute's container is returned using
 \c at; the attribute itself can be accessed using the iterator \c iter.

 @param target The name (using dot notation) of the attribute or
 container to find.
 @param at A value-result used to return the attribute container in
 which \c target was found. Null if \c target was not found.
 @param iter The iterator which will reference the attribute found.
 Can be used to access \c target from within \c at. References
 dim_end() within \c at if the attribute or container does not exist. */
void AttrTable::find(const string &target, AttrTable **at, Attr_iter *iter)
{
    string::size_type dotpos = target.rfind('.');
    if (dotpos != string::npos) {
        string container = target.substr(0, dotpos);
        string field = target.substr(dotpos + 1);

        *at = find_container(container);
        if (*at) {
            *iter = (*at)->simple_find(field);
        }
        else {
            *iter = attr_map.end();
        }
    }
    else {
        *at = recurrsive_find(target, iter);
    }
}

/** This method scans for attributes using recursion to look inside containers
 even when the name of the attribute is not fully qualified. It starts
 looking in itself and descends into its children depth first. It will find
 attributes and attribute containers.

 @param target Look for the attribute with this name.
 @param location A value-result parameter. This returns an iterator to the
 attribute within the returned AttrTable object
 @return Returns a pointer to the AttrTable which holds \e target, or null
 if \e target is not found. In the latter case, the value of \e location is
 attr_end() for this AttrTable. */
AttrTable *
AttrTable::recurrsive_find(const string &target, Attr_iter *location)
{
    Attr_iter i = attr_begin();
    while (i != attr_end()) {
        if (target == (*i)->name) {
            *location = i;
            return this;
        }
        else if ((*i)->type == Attr_container) {
            AttrTable *at = (*i)->attributes->recurrsive_find(target, location);
            if (at)
                return at;
        }

        ++i;
    }

    *location = i;
    return 0;
}

// Made public for callers that want non-recursive find.  [mjohnson 6 oct 09]
/** Look in this AttrTable for the attribute called \c name. If found return
 an Attr_iter which references it, otherwise return the end iterator for
 this AttrTable.

 @param target The name of the attribute.
 @return An Attr_iter which references \c target. */
AttrTable::Attr_iter AttrTable::simple_find(const string &target)
{
    Attr_iter i;
    for (i = attr_map.begin(); i != attr_map.end(); ++i) {
        if (target == (*i)->name) {
            break;
        }
    }
    return i;
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
        string field = target.substr(dotpos + 1);

        AttrTable *at = simple_find_container(container);
        return (at) ? at->find_container(field) : 0;
    }
    else {
        return simple_find_container(target);
    }
}

// Made public for callers that want non-recursive find.  [mjohnson 6 oct 09]
AttrTable *
AttrTable::simple_find_container(const string &target)
{
    if (get_name() == target)
        return this;

    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); ++i) {
        if (is_container(i) && target == (*i)->name) {
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

/** @brief Get the type name of an attribute within this attribute table. */
string AttrTable::get_type(const string &name)
{
    Attr_iter p = simple_find(name);
    return (p != attr_map.end()) ? get_type(p) : (string) "";
}

/** @brief Get the type of an attribute.
 @return The <tt>AttrType</tt> value describing the attribute. */
AttrType AttrTable::get_attr_type(const string &name)
{
    Attr_iter p = simple_find(name);
    return (p != attr_map.end()) ? get_attr_type(p) : Attr_unknown;
}

/** If the indicated attribute is a container attribute, this function
 returns the number of attributes in <i>its</i> attribute table. If the
 indicated attribute is not a container, the method returns the number
 of values for the attribute (1 for a scalar attribute, N for a vector
 attribute value).
 @brief Get the number of attributes in this container.
 */
unsigned int AttrTable::get_attr_num(const string &name)
{
    Attr_iter iter = simple_find(name);
    return (iter != attr_map.end()) ? get_attr_num(iter) : 0;
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
    Attr_iter p = simple_find(name);
    return (p != attr_map.end()) ? get_attr_vector(p) : 0;
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
void AttrTable::del_attr(const string &name, int i)
{
#if WWW_ENCODING
    string lname = www2id(name);
#else
    string lname = remove_space_encoding(name);
#endif

    Attr_iter iter = simple_find(lname);
    if (iter != attr_map.end()) {
        if (i == -1) { // Delete the whole attribute
            entry *e = *iter;
            attr_map.erase(iter);
            delete e;
            e = 0;
        }
        else { // Delete one element from attribute array
            // Don't try to delete elements from the vector of values if the
            // map is a container!
            if ((*iter)->type == Attr_container)
                return;

            vector<string> *sxp = (*iter)->attr;

            assert(i >= 0 && i < (int) sxp->size());
            sxp->erase(sxp->begin() + i); // rm the element
        }
    }
}

//@} Accessors using an attribute name

/** @name get information using an iterator */
//@{
/** Get an iterator to the first entry in this attribute table.
 @return Attr_iter; references the end of the array if empty list. */
AttrTable::Attr_iter AttrTable::attr_begin()
{
    return attr_map.begin();
}

/** Get an iterator to the end attribute table. Does not point to
 the last attribute in the table
 @return Attr_iter */
AttrTable::Attr_iter AttrTable::attr_end()
{
    return attr_map.end();
}

/** Given an index \c i, return the \c Attr_iter to the corresponding
 element. This method provides a way to use all the methods that take an
 \c Attr_iter using a simple integer index. Use the get_attr_num() or
 get_size() methods to determine how many items the AttrTable contains.

 @param i The index
 @return The corresponding Attr_iter
 @see get_attr_num, get_size */
AttrTable::Attr_iter AttrTable::get_attr_iter(int i)
{
    return attr_map.begin() + i;
}

/** Returns the name of the attribute referenced by \e iter. */
string AttrTable::get_name(Attr_iter iter)
{
    assert(iter != attr_map.end());

    return (*iter)->name;
}

/** Returns true if the attribute referenced by \e i is a container. */
bool AttrTable::is_container(Attr_iter i)
{
    return (*i)->type == Attr_container;
}

/** Get the attribute container referenced by \e iter. If no
 such container exists, then return a reference to the end of the
 table.
 @param iter Reference to a table contained by this object.
 @return The child attribute table. */
AttrTable *
AttrTable::get_attr_table(Attr_iter iter)
{
    assert(iter != attr_map.end());
    return (*iter)->type == Attr_container ? (*iter)->attributes : 0;
}

/** Delete the iterator.  Since AttrTable stores pointers to AttrTable
 objects, the caller should be sure to delete the AttrTable itself.
 The caller will gain control of the AttrTable* located at
 get_attr_table(iter) prior to this call.

 @note The original semantics of this methods were odd. The caller was
 responsible for deleting the AttrTable, but if they did that before calling
 this, then memory corruption would happen (because this code accesses a
 field of the table). If the caller did not delete the table, memory leaked.
 The only correct way to call the method was to grab the pointer, call this
 and then delete the pointer. I added a call to delete the contained
 AttrTable pointer, which changes the behavior of this, but probably in a
 way that will fix leaks in existing code. This change can be reverted by
 setting NEW_DEL_ATTR_TABLE_BEHAVIOR to false. jhrg 4/26/13

 @note calling this method <b>invalidates</b> the iterator \e iter.
 @param iter points to the entry to be deleted.
 @return The Attr_iter for the element following \e iter */
AttrTable::Attr_iter AttrTable::del_attr_table(Attr_iter iter)
{
    if ((*iter)->type != Attr_container)
        return ++iter;

    // the caller intends to delete/reuse the contained AttrTable,
    // so zero it out so it doesn't get deleted before we delete the entry
    // [mjohnson]
    struct entry *e = *iter;
    // container no longer has a parent.
    if (e->attributes) {
        e->attributes->d_parent = 0;

#if NEW_DEL_ATTR_TABLE_BEHAVIOR
        delete e->attributes;
#endif
        e->attributes = 0;
    }

    delete e;

    return attr_map.erase(iter);
}

/** Get the type name of an attribute referenced by \e iter.
 @param iter Reference to the Attribute.
 @return A string with the name of this attribute datatype. */
string AttrTable::get_type(Attr_iter iter)
{
    assert(iter != attr_map.end());
    return AttrType_to_String((*iter)->type);
}

/** Get the type of the attribute referenced by \e iter.
 @param iter
 @return The datatype of this attribute in an instance of AttrType. */
AttrType AttrTable::get_attr_type(Attr_iter iter)
{
    return (*iter)->type;
}

/** If the attribute referenced by \e iter is a container attribute, this
 method returns the number of attributes in its attribute table.
 If the indicated attribute is not a container, the method returns the
 number of values for the attribute (1 for a scalar attribute, N for a
 vector attribute value).
 @param iter Reference to an attribute
 @return The number of elements in the attribute. */
unsigned int AttrTable::get_attr_num(Attr_iter iter)
{
    assert(iter != attr_map.end());
    return ((*iter)->type == Attr_container) ? (*iter)->attributes->get_size() : (*iter)->attr->size();
}

/** Returns the value of an attribute. If the attribute has a vector
 value, you can indicate which is the desired value with the index
 argument, \e i. If the argument is omitted, the first value is
 returned. If the attribute has only a single value, the index
 argument is ignored. If \e i is greater than the number of
 elements in the attribute, an error is produced.

 All values in an attribute table are stored as string data. They may
 be converted to a more appropriate internal format by the calling
 program.

 @param iter Reference to an attribute
 @param i The attribute value index, zero-based. Default value: 0
 @return If the indicated attribute is a container, this function
 returns the string ``None''. If using a name to refer to the attribute
 and the named attribute does not exist, return the empty string. */
string AttrTable::get_attr(Attr_iter iter, unsigned int i)
{
    assert(iter != attr_map.end());

    return (*iter)->type == Attr_container ? (string) "None" : (*(*iter)->attr)[i];
}

string AttrTable::get_attr(const string &name, unsigned int i)
{
    Attr_iter p = simple_find(name);
    return (p != attr_map.end()) ? get_attr(p, i) : (string) "";
}

/** Returns a pointer to the vector of values associated with the
 attribute referenced by iterator \e iter.

 Note that all values in an attribute table are stored as string data.
 They may be converted to a more appropriate internal format by the
 calling program.

 @param iter Reference to the Attribute.
 @return If the indicated attribute is a container, this function
 returns the null pointer.  Otherwise returns a pointer to the
 the attribute vector value. */
vector<string> *
AttrTable::get_attr_vector(Attr_iter iter)
{
    assert(iter != attr_map.end());
    return (*iter)->type != Attr_container ? (*iter)->attr : 0;
}

bool AttrTable::is_global_attribute(Attr_iter iter)
{
    assert(iter != attr_map.end());
    if ((*iter)->type == Attr_container)
        return (*iter)->attributes->is_global_attribute();
    else
        return (*iter)->is_global;
}

void AttrTable::set_is_global_attribute(Attr_iter iter, bool ga)
{
    assert(iter != attr_map.end());
    if ((*iter)->type == Attr_container)
        (*iter)->attributes->set_is_global_attribute(ga);
    else
        (*iter)->is_global = ga;
}

//@} Accessors that use an iterator

// Alias an attribute table. The alias should be added to this object.
/** @brief Add an alias to a container held by this attribute table.
 @param name The name of the alias. May <i>not</i> use dot notation.
 @param src The existing attribute container to alias.
 @exception Error if an attribute, container or alias called
 <tt>name</tt> already exists in this attribute table. */
void AttrTable::add_container_alias(const string &name, AttrTable *src)
{
#if WWW_ENCODING
    string lname = www2id(name);
#else
    string lname = remove_space_encoding(name);
#endif

    if (simple_find(lname) != attr_end())
        throw Error(string("There already exists a container called `") + name + string("in this attribute table. (2)"));

    entry *e = new entry;
    e->name = lname;
    e->is_alias = true;
    e->aliased_to = src->get_name();
    e->type = Attr_container;

    e->attributes = src;

    attr_map.push_back(e);
}

/** Assume \e source names an attribute value in some container. Add an alias
 \e name for that value in this object.

 @brief Add an alias for an attribute.

 @param das
 @param name The name of the alias. May <i>not</i> use dot notation.
 @param source The name of the attribute to alias. May use dot
 notation.
 @exception Error if the attribute table already contains an
 attribute, container or alias called <tt>name</tt> or if an
 attribute called <tt>source</tt> does not exist. */
void AttrTable::add_value_alias(AttrTable *das, const string &name, const string &source)
{
#if WWW_ENCODING
    string lname = www2id(name);
#else
    string lname = remove_space_encoding(name);
#endif

#if WWW_ENCODING
    string lsource = www2id(source);
#else
    string lsource = remove_space_encoding(source);
#endif

    // find the container that holds source and its (sources's) iterator
    // within that container. Search at the uppermost level of the attribute
    // object to find values defined `above' the current container.
    AttrTable *at;
    Attr_iter iter;
    das->find(lsource, &at, &iter);

    // If source is not found by looking at the topmost level, look in the
    // current table (i.e., alias z x where x is in the current container
    // won't be found by looking for `x' at the top level). See test case 26
    // in das-testsuite.
    if (!at || (iter == at->attr_end()) || !*iter) {
        find(lsource, &at, &iter);
        if (!at || (iter == at->attr_end()) || !*iter)
            throw Error(string("Could not find the attribute `") + source + string("' in the attribute object."));
    }

    // If we've got a value to alias and it's being added at the top level of
    // the DAS, that's an error.
    if (at && !at->is_container(iter) && this == das)
        throw Error(
                string(
                        "A value cannot be aliased to the top level of the DAS;\nOnly containers may be present at that level of the DAS."));

    if (simple_find(lname) != attr_end())
        throw Error(string("There already exists a container called `") + name + string("in this attribute table. (3)"));

    entry *e = new entry;
    e->name = lname;
    e->is_alias = true;
    e->aliased_to = lsource;
    e->type = get_attr_type(iter);
    if (at && e->type == Attr_container)
        e->attributes = at->get_attr_table(iter);
    else
        e->attr = (*iter)->attr;

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
 @see get_attr_table
 @deprecated The current alias design is flawed. It is impossible to map
 this onto the XML implementation where the DAS and DDS information are
 combined in one object.
 @param alias The alias to insert into the attribute table.
 @param name The name of the already-existing attribute to which
 the alias will refer.
 @param at An attribute table in which to insert the alias. */
bool AttrTable::attr_alias(const string &alias, AttrTable *at, const string &name)
{
    add_value_alias(at, alias, name);
    return true;
}

/** @deprecated The current alias design is flawed. It is impossible to map
 this onto the XML implementation where the DAS and DDS information are
 combined in one object.

 @param alias The alias to insert into the attribute table.
 @param name The name of the already-existing attribute to which
 the alias will refer. */
bool AttrTable::attr_alias(const string &alias, const string &name)
{
    return attr_alias(alias, this, name);
}

/** Erase the entire attribute table. This returns an AttrTable to the empty
 state that's the same as the object generated by the null constructor.
 @brief Erase the attribute table. */
void AttrTable::erase()
{
    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); ++i) {
        delete *i;
        *i = 0;
    }

    attr_map.erase(attr_map.begin(), attr_map.end());

    d_name = "";
}

const string double_quote = "\"";

// This is here as a result of the problem described in ticket #1163 where
// the data handlers are adding quotes to string attributes so the DAS will
// be printed correctly. But that has the affect of adding the quotes to the
// attribute's _value_ not just it's print representation. As part of the fix
// I made the code here add the quotes if the handlers are fixed (but not if
// handlers are still adding them). The other part of 1163 is to fix all of
// the handlers... What this fix means is that attributes whose values really
// do contain bracketing quotes might be misunderstood, since we're assuming
// those quotes were added by the handlers as a hack to get the output
// formatting correct for the DAS. jhrg 7/30/08

static void write_string_attribute_for_das(ostream &out, const string &value, const string &term)
{
    if (is_quoted(value))
        out << value << term;
    else
        out << double_quote << value << double_quote << term;
}

#if 0
static void
write_string_attribute_for_das(FILE *out, const string &value, const string &term)
{
    if (is_quoted(value))
    fprintf(out, "%s%s", value.c_str(), term.c_str());
    else
    fprintf(out, "\"%s\"%s", value.c_str(), term.c_str());
}
#endif

// Special treatment for XML: Make sure to escape double quotes when XML is
// printed in a DAS.
static void write_xml_attribute_for_das(ostream &out, const string &value, const string &term)
{
    if (is_quoted(value))
        out << escape_double_quotes(value) << term;
    else
        out << double_quote << escape_double_quotes(value) << double_quote << term;
}

#if 0
static void
write_xml_attribute_for_das(FILE *out, const string &value, const string &term)
{
    if (is_quoted(value))
    fprintf(out, "%s%s", escape_double_quotes(value).c_str(), term.c_str());
    else
    fprintf(out, "\"%s\"%s", escape_double_quotes(value).c_str(), term.c_str());
}
#endif

/** A simple printer that does nothing fancy with aliases.
 Protected. */
void AttrTable::simple_print(FILE *out, string pad, Attr_iter i, bool dereference)
{
    ostringstream oss;
    simple_print(oss, pad, i, dereference);
    fwrite(oss.str().data(), 1, oss.str().length(), out);

#if 0
    switch ((*i)->type) {
        case Attr_container:
#if WWW_ENCODING
        fprintf(out, "%s%s {\n", pad.c_str(), id2www(get_name(i)).c_str());
#else
        fprintf(out, "%s%s {\n", pad.c_str(), get_name(i).c_str());
#endif
        (*i)->attributes->print(out, pad + "    ", dereference);

        fprintf(out, "%s}\n", pad.c_str());
        break;

        case Attr_string: {
#if WWW_ENCODING
            fprintf(out, "%s%s %s ", pad.c_str(), get_type(i).c_str(), id2www(get_name(i)).c_str());
#else
            fprintf(out, "%s%s %s ", pad.c_str(), get_type(i).c_str(), get_name(i).c_str());
#endif
            vector<string> *sxp = (*i)->attr;
            vector<string>::iterator last = sxp->end() - 1;
            for (vector<string>::iterator i = sxp->begin(); i != last; ++i) {
                write_string_attribute_for_das(out, *i, ", ");
            }
            write_string_attribute_for_das(out, *last, ";\n");
        }
        break;

        case Attr_other_xml: {
#if WWW_ENCODING
            fprintf(out, "%s%s %s ", pad.c_str(), get_type(i).c_str(), id2www(get_name(i)).c_str());
#else
            fprintf(out, "%s%s %s ", pad.c_str(), get_type(i).c_str(), get_name(i).c_str());
#endif
            vector<string> *sxp = (*i)->attr;
            vector<string>::iterator last = sxp->end() - 1;
            for (vector<string>::iterator i = sxp->begin(); i != last; ++i) {
                write_xml_attribute_for_das(out, *i, ", ");
            }
            write_xml_attribute_for_das(out, *last, ";\n");
        }
        break;

        default: {
#if WWW_ENCODING
            fprintf(out, "%s%s %s ", pad.c_str(), get_type(i).c_str(), id2www(get_name(i)).c_str());
#else
            fprintf(out, "%s%s %s ", pad.c_str(), get_type(i).c_str(), get_name(i).c_str());
#endif

            vector<string> *sxp = (*i)->attr;
            vector<string>::iterator last = sxp->end() - 1;
            for (vector<string>::iterator i = sxp->begin(); i != last; ++i) {
                fprintf(out, "%s%s", (*i).c_str(), ", ");
            }
            fprintf(out, "%s%s", (*last).c_str(), ";\n");
        }
        break;
    }
#endif
}

/** A simple printer that does nothing fancy with aliases.
 Protected. */
void AttrTable::simple_print(ostream &out, string pad, Attr_iter i, bool dereference)
{
    switch ((*i)->type) {
    case Attr_container:
#if WWW_ENCODING
        out << pad << id2www(get_name(i)) << " {\n";
#else
        out << pad << add_space_encoding(get_name(i)) << " {\n";
#endif
        (*i)->attributes->print(out, pad + "    ", dereference);
        out << pad << "}\n";
        break;

    case Attr_string: {
#if WWW_ENCODING
        out << pad << get_type(i) << " " << id2www(get_name(i)) << " ";
#else
        out << pad << get_type(i) << " " << add_space_encoding(get_name(i)) << " ";
#endif
        vector<string> *sxp = (*i)->attr;
        vector<string>::iterator last = sxp->end() - 1;
        for (vector<string>::iterator i = sxp->begin(); i != last; ++i) {
            write_string_attribute_for_das(out, *i, ", ");
        }
        write_string_attribute_for_das(out, *last, ";\n");
    }
        break;

    case Attr_other_xml: {
#if WWW_ENCODING
        out << pad << get_type(i) << " " << id2www(get_name(i)) << " ";
#else
        out << pad << get_type(i) << " " << add_space_encoding(get_name(i)) << " ";
#endif
        vector<string> *sxp = (*i)->attr;
        vector<string>::iterator last = sxp->end() - 1;
        for (vector<string>::iterator i = sxp->begin(); i != last; ++i) {
            write_xml_attribute_for_das(out, *i, ", ");
        }
        write_xml_attribute_for_das(out, *last, ";\n");
    }
        break;

    default: {
#if WWW_ENCODING
        out << pad << get_type(i) << " " << id2www(get_name(i)) << " ";
#else
        out << pad << get_type(i) << " " << add_space_encoding(get_name(i)) << " ";
#endif
        vector<string> *sxp = (*i)->attr;
        vector<string>::iterator last = sxp->end() - 1;
        for (vector<string>::iterator i = sxp->begin(); i != last; ++i) {
            out << *i << ", ";
        }
        out << *last << ";\n";
    }
        break;
    }
}

/** Prints an ASCII representation of the attribute table to the
 indicated FILE pointer. The \c pad argument is prefixed to each
 line of the output to provide control of indentation.

 @brief Prints the attribute table.
 @param out Print to the given output FILE.
 @param pad Indent elements of a table using this string of spaces. By
 default this is a string of four spaces
 @param dereference If true, follow aliases. Default is false. */

void AttrTable::print(FILE *out, string pad, bool dereference)
{
    ostringstream oss;
    print(oss, pad, dereference);
    fwrite(oss.str().data(), 1, oss.str().length(), out);

#if 0
    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); ++i) {
        if ((*i)->is_alias) {
            if (dereference) {
                simple_print(out, pad, i, dereference);
            }
            else {
#if WWW_ENCODING
                fprintf(out, "%sAlias %s %s;\n",
                        pad.c_str(),
                        id2www(get_name(i)).c_str(),
                        id2www((*i)->aliased_to).c_str());
#else
                fprintf(out, "%sAlias %s %s;\n",
                        pad.c_str(), add_space_encoding(get_name(i)).c_str(), add_space_encoding((*i)->aliased_to).c_str());

#endif
            }
        }
        else {
            simple_print(out, pad, i, dereference);
        }
    }
#endif
}

/** Prints an ASCII representation of the attribute table to the
 indicated output stream. The \c pad argument is prefixed to each
 line of the output to provide control of indentation.

 @brief Prints the attribute table.
 @param out Print to the given output stream.
 @param pad Indent elements of a table using this string of spaces. By
 default this is a string of four spaces
 @param dereference If true, follow aliases. Default is false. */

void AttrTable::print(ostream &out, string pad, bool dereference)
{
    for (Attr_iter i = attr_map.begin(); i != attr_map.end(); ++i) {
        if ((*i)->is_alias) {
            if (dereference) {
                simple_print(out, pad, i, dereference);
            }
            else {
#if WWW_ENCODING
                out << pad << "Alias " << id2www(get_name(i))
                << " " << id2www((*i)->aliased_to) << ";\n";
#else
                out << pad << "Alias " << add_space_encoding(get_name(i)) << " "
                        << add_space_encoding((*i)->aliased_to) << ";\n";
#endif
            }
        }
        else {
            simple_print(out, pad, i, dereference);
        }
    }
}

/** Print the attribute table in XML.
 @param out Destination
 @param pad Indent lines of text/xml this much. Default is four spaces.
 @param constrained Not used
 @deprecated */
void AttrTable::print_xml(FILE *out, string pad, bool /*constrained*/)
{
    XMLWriter xml(pad);
    print_xml_writer(xml);
    fwrite(xml.get_doc(), sizeof(char), xml.get_doc_size(), out);

#if OLD_XML_MOETHODS
    ostringstream oss;
    print_xml(oss, pad);
    fwrite(oss.str().data(), 1, oss.str().length(), out);
#endif

#if 0
    // Why this works: AttrTable is really a hacked class that used to
    // implement a single-level set of attributes. Containers
    // were added several years later by dropping in the 'entry' structure.
    // It's not a class in its own right; instead accessors from AttrTable
    // are used to access information from entry. So... the loop below
    // actually iterates over the entries of *this* (which is an instance of
    // AttrTable). A container is an entry whose sole value is an AttrTable
    // instance. 05/19/03 jhrg
    for (Attr_iter i = attr_begin(); i != attr_end(); ++i) {
        if ((*i)->is_alias) {
            fprintf(out, "%s<Alias name=\"%s\" Attribute=\"%s\"/>\n",
                    pad.c_str(), id2xml(get_name(i)).c_str(),
                    (*i)->aliased_to.c_str());

        }
        else if (is_container(i)) {
            fprintf(out, "%s<Attribute name=\"%s\" type=\"%s\">\n",
                    pad.c_str(), id2xml(get_name(i)).c_str(),
                    get_type(i).c_str());

            get_attr_table(i)->print_xml(out, pad + "    "/*, constrained*/);

            fprintf(out, "%s</Attribute>\n", pad.c_str());
        }
        else {
            fprintf(out, "%s<Attribute name=\"%s\" type=\"%s\">\n",
                    pad.c_str(), id2xml(get_name(i)).c_str(), get_type(i).c_str());

            string value_pad = pad + "    ";
            // Special handling for the OtherXML attribute type - don't escape
            // the XML and don't include the <value> element. Note that there
            // cannot be an vector of XML things as can be with the other types.
            if (get_attr_type(i) == Attr_other_xml) {
                if (get_attr_num(i) != 1)
                throw Error("OtherXML attributes cannot be vector-valued.");
                fprintf(out, "%s%s\n", value_pad.c_str(), get_attr(i, 0).c_str());
            }
            else {
                for (unsigned j = 0; j < get_attr_num(i); ++j) {
                    fprintf(out, "%s<value>%s</value>\n", value_pad.c_str(),
                            id2xml(get_attr(i, j)).c_str());
                }
            }
            fprintf(out, "%s</Attribute>\n", pad.c_str());
        }
    }
#endif
}

/**
 * @deprecated
 */
void AttrTable::print_xml(ostream &out, string pad, bool /*constrained*/)
{
    XMLWriter xml(pad);
    print_xml_writer(xml);
    out << xml.get_doc();

#if 0
    for (Attr_iter i = attr_begin(); i != attr_end(); ++i) {
        if ((*i)->is_alias) {
            out << pad << "<Alias name=\"" << id2xml(get_name(i))
            << "\" Attribute=\"" << (*i)->aliased_to << "\"/>\n";

        }
        else if (is_container(i)) {
            out << pad << "<Attribute name=\"" << id2xml(get_name(i))
            << "\" type=\"" << get_type(i) << "\">\n";

            get_attr_table(i)->print_xml(out, pad + "    "/*, constrained*/);

            out << pad << "</Attribute>\n";
        }
        else {
            out << pad << "<Attribute name=\"" << id2xml(get_name(i))
            << "\" type=\"" << get_type(i) << "\">\n";

            string value_pad = pad + "    ";
            if (get_attr_type(i) == Attr_other_xml) {
                if (get_attr_num(i) != 1)
                throw Error("OtherXML attributes cannot be vector-valued.");
                out << value_pad << get_attr(i, 0) << "\n";
            }
            else {
                string value_pad = pad + "    ";
                for (unsigned j = 0; j < get_attr_num(i); ++j) {
                    out << value_pad << "<value>" << id2xml(get_attr(i, j)) << "</value>\n";
                }
            }
            out << pad << "</Attribute>\n";
        }
    }
#endif
}

/** Print the attribute table in XML.
 @param out Destination stream
 @param pad Indent lines of text/xml this much. Default is four spaces.
 @param constrained Not used */
void AttrTable::print_xml_writer(XMLWriter &xml)
{
    for (Attr_iter i = attr_begin(); i != attr_end(); ++i) {
        if ((*i)->is_alias) {
            if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Alias") < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write Alias element");
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name",
                    (const xmlChar*) get_name(i).c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "Attribute",
                    (const xmlChar*) (*i)->aliased_to.c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
            if (xmlTextWriterEndElement(xml.get_writer()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not end Alias element");
        }
        else if (is_container(i)) {
            if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Attribute") < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write Attribute element");
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name",
                    (const xmlChar*) get_name(i).c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "type",
                    (const xmlChar*) get_type(i).c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

            get_attr_table(i)->print_xml_writer(xml);

            if (xmlTextWriterEndElement(xml.get_writer()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not end Attribute element");
        }
        else {
            if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Attribute") < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write Attribute element");
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name",
                    (const xmlChar*) get_name(i).c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
            if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "type",
                    (const xmlChar*) get_type(i).c_str()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

            if (get_attr_type(i) == Attr_other_xml) {
                if (get_attr_num(i) != 1)
                    throw Error("OtherXML attributes cannot be vector-valued.");
                // Replaced xmltextWriterWriteString with xmlTextWriterWriteRaw to keep the
                // libxml2 code from escaping the xml (which was breaking all of the inferencing
                // code. jhrg
                if (xmlTextWriterWriteRaw(xml.get_writer(), (const xmlChar*) get_attr(i, 0).c_str()) < 0)
                    throw InternalErr(__FILE__, __LINE__, "Could not write OtherXML value");
            }
            else {
                for (unsigned j = 0; j < get_attr_num(i); ++j) {
                    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "value") < 0)
                        throw InternalErr(__FILE__, __LINE__, "Could not write value element");

                    if (xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) get_attr(i, j).c_str()) < 0)
                        throw InternalErr(__FILE__, __LINE__, "Could not write attribute value");

                    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
                        throw InternalErr(__FILE__, __LINE__, "Could not end value element");
                }
            }
            if (xmlTextWriterEndElement(xml.get_writer()) < 0)
                throw InternalErr(__FILE__, __LINE__, "Could not end Attribute element");
        }
    }
}

/** Write the DAP4 XML representation for this attribute table. This
 * method is used to build the DAP4 DMR response object.
 *
 * @param xml An XMLWriter that will do the serialization
 */
void
AttrTable::print_dap4(XMLWriter &xml)
{
    print_xml_writer(xml);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and all attributes stored
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void AttrTable::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "AttrTable::dump - (" << (void *) this << ")" << endl;
    DapIndent::Indent();
    strm << DapIndent::LMarg << "table name: " << d_name << endl;
    if (attr_map.size()) {
        strm << DapIndent::LMarg << "attributes: " << endl;
        DapIndent::Indent();
        Attr_citer i = attr_map.begin();
        Attr_citer ie = attr_map.end();
        for (; i != ie; ++i) {
            entry *e = (*i);
            string type = AttrType_to_String(e->type);
            if (e->is_alias) {
                strm << DapIndent::LMarg << "alias: " << e->name << " aliased to: " << e->aliased_to << endl;
            }
            else if (e->type == Attr_container) {
                strm << DapIndent::LMarg << "attr: " << e->name << " of type " << type << endl;
                DapIndent::Indent();
                e->attributes->dump(strm);
                DapIndent::UnIndent();
            }
            else {
                strm << DapIndent::LMarg << "attr: " << e->name << " of type " << type << endl;
                DapIndent::Indent();
                strm << DapIndent::LMarg;
                vector<string>::const_iterator iter = e->attr->begin();
                vector<string>::const_iterator last = e->attr->end() - 1;
                for (; iter != last; ++iter) {
                    strm << (*iter) << ", ";
                }
                strm << (*(e->attr->end() - 1)) << endl;
                DapIndent::UnIndent();
            }
        }
        DapIndent::UnIndent();
    }
    else {
        strm << DapIndent::LMarg << "attributes: empty" << endl;
    }
    if (d_parent) {
        strm << DapIndent::LMarg << "parent table:" << d_name << ":" << (void *) d_parent << endl;
    }
    else {
        strm << DapIndent::LMarg << "parent table: none" << d_name << endl;
    }
    DapIndent::UnIndent();
}

} // namespace libdap

