
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

#ifndef _d4attributes_h
#define _d4attributes_h 1

#include <string>
#include <vector>

#include "DapObj.h"
#include "D4AttributeType.h"
#include "XMLWriter.h"

using namespace std;

namespace libdap
{

class AttrTable;
class D4Attributes;

class D4Attribute : public DapObj {
    string d_name;
    D4AttributeType d_type;    // Attributes are limited to the simple types

    // If d_type is attr_container_c is true, use d_attributes to read
    // the contained attributes, otherwise use d_values to read the vector
    // of values.
    D4Attributes *d_attributes;

    // IF d_type is attr_otherxml_c, the first string in d_values holds the
    // XML, otherwise, the strings hold attributes of type d_type.
    vector<string> d_values;

    // perform a deep copy
    void m_duplicate(const D4Attribute &src);

public:
    typedef vector<string>::iterator D4AttributeIter;
    typedef vector<string>::const_iterator D4AttributeCIter;

    D4Attribute() : d_name(""), d_type(attr_null_c), d_attributes(0) {}
    D4Attribute(const string &name, D4AttributeType type)
        : d_name(name), d_type(type), d_attributes(0) {}

    D4Attribute(const D4Attribute &src);
    ~D4Attribute();
    D4Attribute &operator=(const D4Attribute &rhs);

    string name() const { return d_name; }
    void set_name(const string &name) { d_name = name; }

    D4AttributeType type() const { return d_type; }
    void set_type(D4AttributeType type) { d_type = type; }

    void add_value(const string &value) { d_values.push_back(value); }
    void add_value_vector(const vector<string> &values) { d_values = values; }

    D4AttributeIter value_begin() { return d_values.begin(); }
    D4AttributeIter value_end() { return d_values.end(); }

    unsigned int num_values() const { return d_values.size(); }
    string value(unsigned int i) const { return d_values[i]; }

    D4Attributes *attributes();

    void print_dap4(XMLWriter &xml) const;

    virtual void dump(ostream &strm) const;
};

class D4Attributes : public DapObj {
public:
    typedef vector<D4Attribute*>::iterator D4AttributesIter;
    typedef vector<D4Attribute*>::const_iterator D4AttributesCIter;

private:
    vector<D4Attribute*> d_attrs;

    void m_duplicate(const D4Attributes &src) {
        D4AttributesCIter i = src.d_attrs.begin();
        while (i != src.d_attrs.end()) {
            d_attrs.push_back(new D4Attribute(**i++));    // deep copy
        }
    }

    D4Attribute *find_depth_first(const string &name, D4AttributesIter i);

public:

    D4Attributes() {}
    D4Attributes(const D4Attributes &rhs) {
        m_duplicate(rhs);
    }

    virtual ~D4Attributes() {
        D4AttributesIter i = d_attrs.begin();
        while(i != d_attrs.end()) {
            delete *i++;
        }
    }

    D4Attributes &operator=(const D4Attributes &rhs) {
        if (this == &rhs) return *this;
        m_duplicate(rhs);
        return *this;
    }

    void transform_to_dap4(AttrTable &at);
    void transform_attrs_to_dap2(AttrTable *d2_attr_table);

#if 0
    // Can these be replaced with transform_to_dap2() above? jhrg 6/17/19
    AttrTable *get_AttrTable(const std::string name);
    static void load_AttrTable(AttrTable *d2_attr_table, D4Attributes *d4_attrs);
#endif

    bool empty() const { return d_attrs.empty(); }

    void add_attribute(D4Attribute *attr) {
        d_attrs.push_back(new D4Attribute(*attr));
    }

    void add_attribute_nocopy(D4Attribute *attr) {
        d_attrs.push_back(attr);
    }

    /// Get an iterator to the start of the enumerations
    D4AttributesIter attribute_begin() { return d_attrs.begin(); }

    /// Get an iterator to the end of the enumerations
    D4AttributesIter attribute_end() { return d_attrs.end(); }

    D4Attribute *find(const string &name);
    D4Attribute *get(const string &fqn);

    // D4Attribute *find_container(const string &name);
    // D4Attribute *get_container(const string &fqn);

    // Might add erase()

    void print_dap4(XMLWriter &xml) const;

    virtual void dump(ostream &strm) const;
};

string D4AttributeTypeToString(D4AttributeType at);
D4AttributeType StringToD4AttributeType(string s);

} // namespace libdap

#endif // _d4attributes_h
