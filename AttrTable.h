
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

// An AttrTable is a table of attributes (type-name-value tuples).

#ifndef _attrtable_h
#define _attrtable_h 1


#include <string>
#include <vector>

#ifndef _error_h
#include "Error.h"
#endif

using std::vector;
using std::string;
using std::vector;

/** <b>AttrType</b> identifies the data types which may appear in an
    attribute table object. 

    \code
    enum AttrType {
	Attr_unknown,
	Attr_container,
	Attr_byte,
	Attr_int16,
	Attr_uint16,
	Attr_int32,
	Attr_uint32,
	Attr_float32,
	Attr_float64,
	Attr_string,
	Attr_url
    };
    \endcode

    @see AttrTable */
enum AttrType {
    Attr_unknown,
    Attr_container,
    Attr_byte,
    Attr_int16,
    Attr_uint16,
    Attr_int32,
    Attr_uint32,
    Attr_float32,
    Attr_float64,
    Attr_string,
    Attr_url
};

string AttrType_to_String(const AttrType at);
AttrType String_to_AttrType(const string &s);

/** An AttrTable (``Attribute Table'') stores a set of names and, for
    each name, either a type and a value, or another attribute table.
    The attribute value can be a vector containing many values of the
    same type.  The attributes can have any of the types listed in the
    <tt>AttrType</tt> list.  However, all attribute types are stored as
    string data, except for the container type, which is stored as a
    pointer to another attribute table.

    Each element in the attribute table can itself be an attribute
    table.  The table can also contain ``alias'' attributes whose
    value is given by the value of another attribute to which it is
    linked.

    The attribute tables have a standard printed representation.
    There is a member function <tt>print()</tt> for writing this form.  Use
    the <tt>DAS::parse()</tt> function to read the printed form.

    An attribute table might look something like this:

    \verbatim
    string long_name "Weekly Means of Sea Surface Temperature";
    actual_range {
        Float64 min -1.8;
        Float64 max 35.09;
    }
    string units "degC";
    conversion_data {
        Float64 add_offset 0.;
        Float64 scale_factor 0.0099999998;
    }
    Int32 missing_value 32767;
    \endverbatim

    Here, <tt>long_name</tt>, <tt>units</tt>, and
    <tt>missing_value</tt> are simple 
    attributes, and <tt>actual_range</tt> and <tt>conversion_data</tt>
    are container attributes containing other attribute tables.

    @todo Look at refactoring this by splitting it into three classes. Move
    the struct entry into its own calls (maybe called Attribute?), make
    AttrTable a child of that class and then make alises a separate class,
    also a child of Attribute. Look at the design of the Java code.

    @todo A problem with this class is that Attr_iter objects cannot be
    dereferenced to return attributes. Instead they must be passed to methods
    which require that you have access to the AttrTable object into which
    they point.03/09/04 jhrg

    @brief Contains the attributes for a dataset.
    @see DAS
    @see AttrType */
class AttrTable {
    // entry needs to be made public to make up for issues with this class'
    // design. It should probably be moved to it's own class. 05/22/03 jhrg
public:
    /** Each AttrTable has zero or more entries. Instead of accessing this
	struct's members directly, use AttrTable methods.

	This struct is public because its type is used in public typedefs. */
    struct entry {
	string name;
	AttrType type;
	
	bool is_alias;
        string aliased_to;

	// If type == Attr_container, use attributes to read the contained
	// table, otherwise use attr to read the vector of values.
	AttrTable *attributes;
	std::vector<string> *attr;	// a vector of values. jhrg 12/5/94

	entry(): name(""), type(Attr_unknown), is_alias(false),
		 aliased_to("") {
	    attributes = 0;
	    attr = 0;
	}

	entry(const entry &rhs) {
	    clone(rhs);
	}

	void delete_entry() {
	    if (is_alias)	// alias copies the pointers.
		return;
	    if (type == Attr_container) {
		delete attributes; attributes = 0;
	    }
	    else {
		delete attr; attr = 0;
	    }
	}

	virtual ~entry() {
	    delete_entry();
	}
	
	void clone(const entry &rhs) {
	    name = rhs.name;
	    type = rhs.type;
	    is_alias = rhs.is_alias;
	    aliased_to = rhs.aliased_to;
	    switch (rhs.type) {
	      case Attr_unknown:
		break;
	      case Attr_container: {
		  AttrTable *src_atp = rhs.attributes;
		  AttrTable *dest_atp = new AttrTable(*src_atp);
		  attributes = dest_atp;
		  break;
	      }
	      default: {
		  std::vector<string> *src_attr = rhs.attr;
		  std::vector<string> *dest_attr = new std::vector<string>(*src_attr);
		  attr = dest_attr;
		  break;
	      }
	    }
	}

	entry &operator=(const entry &rhs) {
	    if (this != &rhs) {
		delete_entry();
		clone(rhs);
	    }
	    return *this;
	}
    };

    typedef std::vector<entry *>::const_iterator Attr_citer ;
    typedef std::vector<entry *>::iterator Attr_iter ;

private:
    string d_name;
    AttrTable *d_parent;
    std::vector<entry *> attr_map;
    
    Attr_iter simple_find(const string &target);
    AttrTable *simple_find_container(const string &target);

    void delete_attr_table();

    friend class AttrTableTest;

protected:
    void clone(const AttrTable &at);

    void simple_print(FILE *out, string pad, Attr_iter i,
		      bool dereference);

public:
    AttrTable();
    AttrTable(const AttrTable &rhs);
    virtual ~AttrTable();
    AttrTable & operator=(const AttrTable &rhs);

    void erase();

    unsigned int get_size() const;
    string get_name() const;
    void set_name(const string &n);
    /** Return a pointer to the AttrTable which holds this table (aka, its
        parent. If this AttrTable has no parent, this returns null.
        @return A pointer to the parent AttrTable. */
    AttrTable *get_parent() const {
        return d_parent;
    }

    unsigned int append_attr(const string &name, const string &type, 
                             const string &value);
    unsigned int append_attr(const string &name, const string &type, 
                             vector<string> *values);
    
    AttrTable *append_container(const string &name);
    AttrTable *append_container(AttrTable *at, const string &name);

    void find(const string &target, AttrTable **at, Attr_iter *iter);
    AttrTable *find_container(const string &target);
    AttrTable *recurrsive_find(const string &target, Attr_iter *location);
    
    AttrTable *get_attr_table(const string &name);
    string get_type(const string &name);
    AttrType get_attr_type(const string &name);
    unsigned int get_attr_num(const string &name);
    string get_attr(const string &name, unsigned int i = 0);
    vector<string> *get_attr_vector(const string &name);
    void del_attr(const string &name, int i = -1);

    Attr_iter attr_begin();
    Attr_iter attr_end();
    Attr_iter get_attr_iter(int i);
    string get_name(Attr_iter iter);
    bool is_container(Attr_iter iter);
    AttrTable *get_attr_table(Attr_iter iter);
    Attr_iter del_attr_table(Attr_iter iter);
    string get_type(Attr_iter iter);
    AttrType get_attr_type(Attr_iter iter);
    unsigned int get_attr_num(Attr_iter iter);
    string get_attr(Attr_iter iter, unsigned int i = 0);
    std::vector<string> *get_attr_vector(Attr_iter iter);

    void add_container_alias(const string &name, AttrTable *src);
    void add_value_alias(AttrTable *das, const string &name, 
			 const string &source);
    bool attr_alias(const string &alias, AttrTable *at, const string &name);
    bool attr_alias(const string &alias, const string &name);

    void print(FILE *out, string pad = "    ", bool dereference = false);

    void print_xml(FILE *out, string pad = "    ", bool constrained = false);
};

#endif // _attrtable_h
