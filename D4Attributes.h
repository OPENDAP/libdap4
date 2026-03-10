
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

#include "D4AttributeType.h"
#include "DapObj.h"
#include "XMLWriter.h"

using namespace std;

namespace libdap {

class AttrTable;
class D4Attributes;

/** @brief Represents one DAP4 attribute value or container node. */
class D4Attribute : public DapObj {
    string d_name;
    D4AttributeType d_type; // Attributes are limited to the simple types
    bool is_utf8_str = false;

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
    /** @brief Mutable iterator over scalar attribute values. */
    typedef vector<string>::iterator D4AttributeIter;
    /** @brief Read-only iterator over scalar attribute values. */
    typedef vector<string>::const_iterator D4AttributeCIter;

    D4Attribute() : d_name(""), d_type(attr_null_c), d_attributes(0) {}

    /**
     * @brief Builds an attribute with a name and declared type.
     * @param name Attribute name.
     * @param type Attribute type.
     */
    D4Attribute(const string &name, D4AttributeType type) : d_name(name), d_type(type), d_attributes(0) {}

    /**
     * @brief Copy-constructs an attribute.
     * @param src Source attribute.
     */
    D4Attribute(const D4Attribute &src);
    ~D4Attribute() override;

    /**
     * @brief Assigns this attribute from another attribute.
     * @param rhs Source attribute.
     * @return This attribute after assignment.
     */
    D4Attribute &operator=(const D4Attribute &rhs);

    /** @brief Returns the attribute name. */
    string name() const { return d_name; }

    /**
     * @brief Sets the attribute name.
     * @param name Attribute name.
     */
    void set_name(const string &name) { d_name = name; }

    /** @brief Returns the attribute type. */
    D4AttributeType type() const { return d_type; }

    /**
     * @brief Sets the attribute type.
     * @param type Attribute type.
     */
    void set_type(D4AttributeType type) { d_type = type; }

    /** @brief Returns whether string values should be interpreted as UTF-8. */
    bool get_utf8_str_flag() const { return is_utf8_str; }

    /**
     * @brief Sets whether string values should be interpreted as UTF-8.
     * @param utf8_str_flag True when string values are UTF-8 encoded text.
     */
    void set_utf8_str_flag(bool utf8_str_flag) { is_utf8_str = utf8_str_flag; }

    /**
     * @brief Appends one scalar value.
     * @param value Value to append.
     */
    void add_value(const string &value) { d_values.push_back(value); }

    /**
     * @brief Replaces scalar values with a vector of values.
     * @param values New scalar values.
     */
    void add_value_vector(const vector<string> &values) { d_values = values; }

    /** @brief Returns an iterator to the first scalar value. */
    D4AttributeIter value_begin() { return d_values.begin(); }
    /** @brief Returns an iterator one past the last scalar value. */
    D4AttributeIter value_end() { return d_values.end(); }

    /** @brief Returns the number of scalar values. */
    unsigned int num_values() const { return d_values.size(); }

    /**
     * @brief Returns one scalar value by index.
     * @param i Zero-based value index.
     * @return The requested value.
     */
    string value(unsigned int i) const { return d_values[i]; }

    /**
     * @brief Returns the nested attribute container for `attr_container_c` attributes.
     * @return Nested attribute container.
     */
    D4Attributes *attributes();

    bool is_dap4_type(const std::string &path, std::vector<std::string> &inventory);

    /**
     * @brief Prints this attribute in DAP4 DMR form.
     * @param xml Destination XML writer.
     */
    void print_dap4(XMLWriter &xml) const;

    void dump(ostream &strm) const override;
};

/** @brief Container for a variable's DAP4 attributes. */
class D4Attributes : public DapObj {
public:
    /** @brief Mutable iterator over `D4Attribute` pointers. */
    typedef vector<D4Attribute *>::iterator D4AttributesIter;
    /** @brief Read-only iterator over `D4Attribute` pointers. */
    typedef vector<D4Attribute *>::const_iterator D4AttributesCIter;

private:
    vector<D4Attribute *> d_attrs;

    void m_duplicate(const D4Attributes &src) {
        D4AttributesCIter i = src.d_attrs.begin();
        while (i != src.d_attrs.end()) {
            d_attrs.push_back(new D4Attribute(**i++)); // deep copy
        }
    }

    D4Attribute *find_depth_first(const string &name, D4AttributesIter i);

public:
    D4Attributes() {}

    /**
     * @brief Copy-constructs an attribute collection.
     * @param rhs Source collection.
     */
    D4Attributes(const D4Attributes &rhs) { m_duplicate(rhs); }

    ~D4Attributes() override {
        D4AttributesIter i = d_attrs.begin();
        while (i != d_attrs.end()) {
            delete *i++;
        }
    }

    /**
     * @brief Assigns this collection from another collection.
     * @param rhs Source collection.
     * @return This collection after assignment.
     */
    D4Attributes &operator=(const D4Attributes &rhs) {
        if (this == &rhs)
            return *this;
        m_duplicate(rhs);
        return *this;
    }

    void transform_to_dap4(AttrTable &at);
    void transform_attrs_to_dap2(AttrTable *d2_attr_table);

    /** @brief Returns true when this collection has no attributes. */
    bool empty() const { return d_attrs.empty(); }

    /**
     * @brief Appends a deep copy of an attribute.
     * @param attr Source attribute.
     */
    void add_attribute(D4Attribute *attr) { d_attrs.push_back(new D4Attribute(*attr)); }

    /**
     * @brief Appends an attribute pointer without copying.
     * @param attr Attribute pointer to store.
     */
    void add_attribute_nocopy(D4Attribute *attr) { d_attrs.push_back(attr); }

    /// Get an iterator to the start of the enumerations
    D4AttributesIter attribute_begin() { return d_attrs.begin(); }

    /// Get an iterator to the end of the enumerations
    D4AttributesIter attribute_end() { return d_attrs.end(); }

    /**
     * @brief Finds an attribute by name.
     * @param name Attribute name.
     * @return Matching attribute or null.
     */
    D4Attribute *find(const string &name);
    D4Attribute *get(const string &fqn);
    void erase(const string &fqn);
    void erase_named_attribute(const string &name);

    /**
     * Get a const reference to the vector of D$attribute pointers.
     * @note Use this in range-based for loops to iterate over the variables.
     * @return A const reference to the vector of D4Attribute pointers.
     */
    const vector<D4Attribute *> &attributes() const { return d_attrs; }

    bool has_dap4_types(const std::string &path, std::vector<std::string> &inventory) const;

    /**
     * @brief Prints all attributes in DAP4 DMR form.
     * @param xml Destination XML writer.
     */
    void print_dap4(XMLWriter &xml) const;

    void dump(ostream &strm) const override;
};

string D4AttributeTypeToString(D4AttributeType at);
D4AttributeType StringToD4AttributeType(string s);

} // namespace libdap

#endif // _d4attributes_h
