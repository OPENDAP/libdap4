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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef D4ENUMDEF_H_
#define D4ENUMDEF_H_

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "BaseType.h"

using namespace std;

namespace libdap {

//class XMLWriter;

/** Enumeration values. This class is only used when defining an enumeration
 * (which is effectively a type definition).
 */
class enumValues {
    struct value {
        string item;
        long long num;

        value(const string &i, long long n) : item(i), num(n) {}
    };

    vector<value> d_values;
    void print_value(XMLWriter &xml, const enumValues::value &ev) const;

public:
    typedef vector<value>::iterator valuesIter;

    enumValues() {}
    virtual ~enumValues() {}

    /** Append a new value.
     *
     * @param item Name bound to the value
     * @param value Numeric value
     */
    void add_value(const string &item, const long long num) {
        d_values.push_back(value(item, num));
    }

    /// Get an iterator to the start of the values
    valuesIter val_begin() { return d_values.begin(); }

    /// Get an iterator to the end of the values
    valuesIter val_end() { return d_values.end(); }

    /// Given an iterator to a value, return its item value.
    string get_val_name(valuesIter i) const { return (*i).item; }

    /// Given an iterator to a value, return its numeric value.
    long long get_val_type(valuesIter i) const { return (*i).num; }

    /** Insert a value.
     * Insert a value before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * enumValues object.
     * @param item
     * @param value
     * @param i iterator
     */
    void insert_enum(const string &item, long long num, valuesIter i) {
        d_values.insert(i, value(item, num));
    }

    void print(XMLWriter &xml) const;
};

/** The Enumerations defined for a Group. */
class D4EnumDefs {
    struct enumeration {
        string name;
        Type type;

        enumValues values;

        enumeration(const string &n, const Type &t, const enumValues &v)
            : name(n), type(t), values(v) {}

        void print(XMLWriter &xml);
    };

    vector<enumeration> d_enums;

    void print_enum(XMLWriter &xml, const enumeration &e) const;

public:
    D4EnumDefs() {}
    virtual ~D4EnumDefs() {}

    typedef vector<enumeration>::iterator D4EnumIter;

    /** Append a new enumeration.
     *
     * @param name Name of the enumeration
     * @param type Type of the enumeration
     * @param values The enumeration values
     */
    void add_enum(const string &name, const Type type, const enumValues &values) {
        d_enums.push_back(enumeration(name, type, values));
    }

    /// Get an iterator to the start of the enumerations
    D4EnumIter enum_begin() { return d_enums.begin(); }

    /// Get an iterator to the end of the enumerations
    D4EnumIter enum_end() { return d_enums.end(); }

    /// Given an iterator  to a enumeration, return its name.
    string get_enum_name(D4EnumIter i) const { return (*i).name; }

    /// Given an iterator to a enumeration, return its type.
    Type get_enum_type(D4EnumIter i) const { return (*i).type; }

    // Given an iterator to a enumeration, return its vector of items and values?
    enumValues get_enum_values(D4EnumIter i) const { return (*i).values; }

    /** Insert an enumeration.
     * Insert an enumeration before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * D4EnumDef object.
     * @param name
     * @param type
     * @param values
     * @param i iterator
     */
    void insert_enum(const string &name, const Type type, const enumValues &values, D4EnumIter i) {
        d_enums.insert(i, enumeration(name, type, values));
    }

    void print(XMLWriter &xml) const;
};

} /* namespace libdap */
#endif /* D4ENUMDEF_H_ */
