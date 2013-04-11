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
#if 0
/** Enumeration values. This class is only used when defining an D4EnumDef
 * (which is effectively a d_type definition).
 */
class D4EnumDefValues {
    struct tuple {
        string label;
        long long value;

        tuple(const string &i, long long n) : label(i), value(n) {}
    };

    vector<tuple> d_tuples;
    void print_value(XMLWriter &xml, const D4EnumDefValues::tuple &ev) const;

public:
    typedef vector<tuple>::iterator valuesIter;
    typedef vector<tuple>::const_iterator valuesCIter;

    D4EnumDefValues() {}
    virtual ~D4EnumDefValues() {}

    /** Append a new tuple.
     *
     * @param label Name bound to the tuple
     * @param tuple Numeric tuple
     */
    void add_value(const string &item, const long long num) {
        d_tuples.push_back(tuple(item, num));
    }

    /// Get an iterator to the start of the values
    valuesIter val_begin() { return d_tuples.begin(); }

    /// Get an iterator to the end of the values
    valuesIter val_end() { return d_tuples.end(); }

    /// Given an iterator to a tuple, return its label tuple.
    string get_label(valuesCIter i) const { return (*i).label; }

    /// Given an iterator to a tuple, return its numeric tuple.
    long long get_value(valuesCIter i) const { return (*i).value; }

    /** Insert a tuple.
     * Insert a tuple before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * D4EnumDefValues object.
     * @param label
     * @param tuple
     * @param i iterator
     */
    void insert_enum(const string &label, long long value, valuesIter i) {
        d_tuples.insert(i, tuple(label, value));
    }

    void print_dap4(XMLWriter &xml) const;
};
#endif
#if 0
class D4EnumDef {
    string d_name;
    Type d_type;

    D4EnumDefValues *d_values;

    void m_duplicate(const D4EnumDef &rhs) {
        d_name = rhs.name();
        d_type = rhs.type();
        d_values = values(); // getter allocates
        D4EnumDefValues::valuesCIter i = rhs.d_values->val_begin();
        while (i != rhs.d_values->val_end()) {
            d_values->add_value(rhs.d_values->get_label(i), rhs.d_values->get_value(i));
            ++i;
        }
    }

public:
    D4EnumDef() : d_name(""), d_type(dods_null_c), d_values(0) {}
    D4EnumDef(const D4EnumDef &rhs) : d_values(0) {
        m_duplicate(rhs);
    }
    D4EnumDef(const string &n, const Type &t, D4EnumDefValues *v)
        : d_name(n), d_type(t), d_values(v) {}
    D4EnumDef &operator=(const D4EnumDef &rhs) {
        if (this == &rhs)  return *this;

        m_duplicate(rhs);

        return *this;
    }

    string name() const { return d_name; }
    void set_name(const string &n) { d_name = n; }

    Type type() const { return d_type; }
    void set_type(Type &t) { d_type = t; }

    add_value(const string &name, long long value);

    // TODO lazy instantiation in C++. Is it a benefit?
    D4EnumDefValues *values() {
        if (!d_values) d_values = new D4EnumDefValues;
        return d_values;
    }
    void set_values(D4EnumDefValues *v) { d_values = v; }

    bool is_valid_enum_value(long long value);
    void print(XMLWriter &xml);
};
#endif

class D4EnumDef {
    string d_name;
    Type d_type;

    struct tuple {
        string label;
        long long value;

        tuple(const string &l, long long v) : label(l), value(v) {}
    };

    vector<tuple> d_tuples;

    void print_value(XMLWriter &xml, const D4EnumDef::tuple &tuple) const;

public:
    typedef vector<tuple>::iterator D4EnumValueIter;

    D4EnumDef() : d_name(""), d_type(dods_null_c) {}
    D4EnumDef(const string &n, const Type &t) : d_name(n), d_type(t) {}

    string name() const { return d_name; }
    void set_name(const string &n) { d_name = n; }

    Type type() const { return d_type; }
    void set_type(Type t) { d_type = t; }

    bool empty() const { return d_tuples.empty(); }

    void add_value(const string &label, long long value) {
        d_tuples.push_back(tuple(label, value));
    }

    D4EnumValueIter value_begin() { return d_tuples.begin(); }
    D4EnumValueIter value_end() { return d_tuples.end(); }
    string &label(D4EnumValueIter i) { return (*i).label; }
    long long value(D4EnumValueIter i) { return (*i).value; }

    bool is_valid_enum_value(long long value);
    void print_dap4(XMLWriter &xml) const;
};

/** The Enumerations defined for a Group. */
class D4EnumDefs {
    vector<D4EnumDef*> d_enums;

    void m_print_enum(XMLWriter &xml, D4EnumDef *e) const;

    void m_duplicate(const D4EnumDefs &rhs) {
        D4EnumDefCIter i = rhs.d_enums.begin();
        while (i != rhs.d_enums.end()) {
            d_enums.push_back(new D4EnumDef(**i++));    // deep copy
        }
    }

public:
    typedef vector<D4EnumDef*>::iterator D4EnumDefIter;
    typedef vector<D4EnumDef*>::const_iterator D4EnumDefCIter;

    D4EnumDefs() {}
    D4EnumDefs(const D4EnumDefs &rhs) {
        m_duplicate(rhs);
    }

    virtual ~D4EnumDefs() {
        D4EnumDefIter i = d_enums.begin();
        while(i != d_enums.end()) {
            delete *i++;
        }
    }

    D4EnumDefs &operator=(const D4EnumDefs &rhs) {
        if (this == &rhs) return *this;
        m_duplicate(rhs);
        return *this;
    }

    bool empty() const { return d_enums.empty(); }

    /** Append a new D4EnumDef.
     *
     * @param enum_def The enumeration.
     */
    void add_enum(D4EnumDef *enum_def) {
        d_enums.push_back(new D4EnumDef(*enum_def));
    }
    void add_enum_nocopy(D4EnumDef *enum_def) {
        d_enums.push_back(enum_def);
    }

    /// Get an iterator to the start of the enumerations
    D4EnumDefIter enum_begin() { return d_enums.begin(); }

    /// Get an iterator to the end of the enumerations
    D4EnumDefIter enum_end() { return d_enums.end(); }

    //TODO Needed? Aggregation maybe?
#if 1
    /** Insert a D4EnumDef.
     * Insert a D4EnumDef before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * D4EnumDef object.
     * @param enum_def Make a deep copy and insert the enumeration definition
     * @param i iterator
     */
    void insert_enum(D4EnumDef *enum_def, D4EnumDefIter i) {
        d_enums.insert(i, new D4EnumDef(*enum_def));
    }
#endif
    void print_dap4(XMLWriter &xml) const;
};

} /* namespace libdap */
#endif /* D4ENUMDEF_H_ */
