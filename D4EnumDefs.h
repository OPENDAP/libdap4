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

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "BaseType.h"

using namespace std;

namespace libdap {

class D4EnumDefs;
class D4Group;

/** @brief Defines one DAP4 Enumeration type and its label/value pairs. */
class D4EnumDef {
    string d_name;
    Type d_type;
    D4EnumDefs *d_parent; // a weak pointer, do not delete

    struct tuple {
        string label;
        long long value;

        tuple(const string &l, long long v) : label(l), value(v) {}
    };

    vector<tuple> d_tuples;

    void print_value(XMLWriter &xml, const D4EnumDef::tuple &tuple) const;

public:
    /** @brief Iterator over enumeration label/value tuples. */
    typedef vector<tuple>::iterator D4EnumValueIter;

    D4EnumDef() : d_name(""), d_type(dods_null_c), d_parent(0) {}

    /**
     * @brief Builds an enumeration definition.
     * @param n Enumeration name.
     * @param t Backing integer type.
     * @param e Parent enumeration-definition collection.
     */
    D4EnumDef(const string &n, const Type &t, D4EnumDefs *e = 0) : d_name(n), d_type(t), d_parent(e) {}
    /** @brief Copy-constructs an enumeration definition. @param rhs Source definition. */
    D4EnumDef(const D4EnumDef &rhs) = default;

    virtual ~D4EnumDef() {}

    /** @brief Assigns this definition from another definition. @param rhs Source definition. @return This definition. */
    D4EnumDef &operator=(const D4EnumDef &rhs) = default;

    /** @brief Returns the enumeration name. */
    string name() const { return d_name; }
    /** @brief Sets the enumeration name. @param n Enumeration name. */
    void set_name(const string &n) { d_name = n; }

    /** @brief Returns the backing integer type. */
    Type type() const { return d_type; }
    /** @brief Sets the backing integer type. @param t Backing integer type. */
    void set_type(Type t) { d_type = t; }

    /** @brief Returns the parent definition collection. */
    D4EnumDefs *parent() const { return d_parent; }
    /** @brief Sets the parent definition collection. @param e Parent definition collection. */
    void set_parent(D4EnumDefs *e) { d_parent = e; }

    /** @brief Returns true when this definition has no label/value pairs. */
    bool empty() const { return d_tuples.empty(); }

    /**
     * @brief Adds one label/value pair to the enumeration definition.
     * @param label Label text.
     * @param value Integer value for the label.
     */
    void add_value(const string &label, long long value) { d_tuples.push_back(tuple(label, value)); }

    /** @brief Returns an iterator to the first label/value tuple. */
    D4EnumValueIter value_begin() { return d_tuples.begin(); }
    /** @brief Returns an iterator one past the last label/value tuple. */
    D4EnumValueIter value_end() { return d_tuples.end(); }
    /** @brief Returns a mutable reference to tuple label text. @param i Tuple iterator. */
    string &label(D4EnumValueIter i) { return (*i).label; }
    /** @brief Returns tuple numeric value. @param i Tuple iterator. */
    long long value(D4EnumValueIter i) { return (*i).value; }

    bool is_valid_enum_value(long long value);
    /**
     * @brief Prints this enumeration definition in DAP4 DMR form.
     * @param xml Destination XML writer.
     */
    void print_dap4(XMLWriter &xml) const;
};

/** The Enumerations defined for a Group. */
class D4EnumDefs {
    vector<D4EnumDef *> d_enums;

    D4Group *d_parent; // the group that holds this set of D4EnumDefs; weak pointer, don't delete

    void m_print_enum(XMLWriter &xml, D4EnumDef *e) const;

    void m_duplicate(const D4EnumDefs &rhs) {
        D4EnumDefCIter i = rhs.d_enums.begin();
        while (i != rhs.d_enums.end()) {
            d_enums.push_back(new D4EnumDef(**i++)); // deep copy
            d_enums.back()->set_parent(this);        // Set the EnumDef's parent
        }

        d_parent = rhs.d_parent;
    }

public:
    /** @brief Mutable iterator over enumeration definitions. */
    typedef vector<D4EnumDef *>::iterator D4EnumDefIter;
    /** @brief Read-only iterator over enumeration definitions. */
    typedef vector<D4EnumDef *>::const_iterator D4EnumDefCIter;

    D4EnumDefs() : d_parent(0) {}
    /** @brief Copy-constructs an enumeration-definition collection. @param rhs Source collection. */
    D4EnumDefs(const D4EnumDefs &rhs) { m_duplicate(rhs); }

    virtual ~D4EnumDefs() {
        D4EnumDefIter i = d_enums.begin();
        while (i != d_enums.end()) {
            delete *i++;
        }
    }

    /**
     * @brief Assigns this collection from another collection.
     * @param rhs Source collection.
     * @return This collection after assignment.
     */
    D4EnumDefs &operator=(const D4EnumDefs &rhs) {
        if (this == &rhs)
            return *this;
        m_duplicate(rhs);
        return *this;
    }

    /** @brief Returns true when there are no enumeration definitions. */
    bool empty() const { return d_enums.empty(); }

    /** @brief Returns the owning group. */
    D4Group *parent() const { return d_parent; }
    /** @brief Sets the owning group. @param g Owning group. */
    void set_parent(D4Group *g) { d_parent = g; }

    /** Append a new D4EnumDef.
     *
     * @param enum_def The enumeration.
     */
    void add_enum(D4EnumDef *enum_def) { add_enum_nocopy(new D4EnumDef(*enum_def)); }
    /**
     * @brief Appends an enumeration definition pointer without deep-copying.
     * @param enum_def Enumeration definition to store.
     */
    void add_enum_nocopy(D4EnumDef *enum_def) {
        enum_def->set_parent(this);
        d_enums.push_back(enum_def);
    }

    /// Get an iterator to the start of the enumerations
    D4EnumDefIter enum_begin() { return d_enums.begin(); }

    /// Get an iterator to the end of the enumerations
    D4EnumDefIter enum_end() { return d_enums.end(); }

    /**
     * @brief Finds an enumeration definition by name.
     * @param name Enumeration definition name.
     * @return Matching definition or null.
     */
    D4EnumDef *find_enum_def(const string &name);

    /**
     * @brief Insert a D4EnumDef.
     * Insert a D4EnumDef before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * D4EnumDef object.
     * @param enum_def Make a deep copy and insert the enumeration definition
     * @param i iterator
     */
    void insert_enum(D4EnumDef *enum_def, D4EnumDefIter i) {
        D4EnumDef *enum_def_copy = new D4EnumDef(*enum_def);
        enum_def_copy->set_parent(this);
        d_enums.insert(i, enum_def_copy);
    }

    /**
     * @brief Prints all enumeration definitions in DAP4 DMR form.
     * @param xml Destination XML writer.
     * @param constrained Present for interface parity; ignored here.
     */
    void print_dap4(XMLWriter &xml, bool constrained = false) const;
};

} /* namespace libdap */
#endif /* D4ENUMDEF_H_ */
