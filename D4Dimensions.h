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

#ifndef D4DIMENSIONS_H_
#define D4DIMENSIONS_H_

#include <string>
#include <vector>

using namespace std;

namespace libdap {

class D4Group;
class D4Dimensions;
class XMLWriter;

/** @brief Describes one named DAP4 shared dimension. */
class D4Dimension {
    string d_name;
    unsigned long long d_size;

    D4Dimensions *d_parent; // This is used to get the Dimensions and then the Group object

    bool d_constrained;
    int64_t d_c_start, d_c_stride, d_c_stop;

    bool d_used_by_projected_var;

public:
    D4Dimension()
        : d_name(""), d_size(0), d_parent(0), d_constrained(false), d_c_start(0), d_c_stride(0), d_c_stop(0),
          d_used_by_projected_var(false) {}

    /**
     * @brief Builds a named shared dimension.
     * @param name Dimension name.
     * @param size Unconstrained size.
     * @param d Parent dimensions collection.
     */
    D4Dimension(const string &name, unsigned long long size, D4Dimensions *d = 0)
        : d_name(name), d_size(size), d_parent(d), d_constrained(false), d_c_start(0), d_c_stride(0), d_c_stop(0),
          d_used_by_projected_var(false) {}

    /** @brief Returns the dimension name. */
    string name() const { return d_name; }
    /** @brief Sets the dimension name. @param name Dimension name. */
    void set_name(const string &name) { d_name = name; }
    string fully_qualified_name() const;

    /** @brief Returns the unconstrained dimension size. */
    unsigned long long size() const { return d_size; }
    /** @brief Sets the unconstrained dimension size. @param size Dimension size. */
    void set_size(unsigned long long size) { d_size = size; }
    // Because we build these in the XML parser and it's all text...
    /** @brief Sets the unconstrained dimension size from text. @param size Dimension size text. */
    void set_size(const string &size);

    /** @brief Returns the parent dimensions collection. */
    D4Dimensions *parent() const { return d_parent; }
    /** @brief Sets the parent dimensions collection. @param d Parent dimensions collection. */
    void set_parent(D4Dimensions *d) { d_parent = d; }

    /** @brief Returns true when this dimension is constrained by a slice. */
    bool constrained() const { return d_constrained; }
    /** @brief Returns the constrained start index. */
    int64_t c_start() const { return d_c_start; }
    /** @brief Returns the constrained stride. */
    int64_t c_stride() const { return d_c_stride; }
    /** @brief Returns the constrained stop index. */
    int64_t c_stop() const { return d_c_stop; }

    /** @brief Returns whether any projected variable uses this shared dimension. */
    bool used_by_projected_var() const { return d_used_by_projected_var; }
    /** @brief Sets whether any projected variable uses this shared dimension. @param state Usage flag. */
    void set_used_by_projected_var(bool state) { d_used_by_projected_var = state; }

    /**
     * Set this Shared Dimension's constraint. While an Array Dimension object uses a
     * stop value of -1 to indicate the end of the dimension, this method does not support
     * that; the caller will have to sort out the correct end value for 'stop'.
     * @param start Starting index (zero-based)
     * @param stride The stride for the slice
     * @param stop The stopping index (never greater than size -1)
     */
    void set_constraint(int64_t start, int64_t stride, int64_t stop) {
        d_c_start = start;
        d_c_stride = stride;
        d_c_stop = stop;
        d_constrained = true;
    }

    /**
     * @brief Prints this dimension in DAP4 DMR form.
     * @param xml Destination XML writer.
     */
    void print_dap4(XMLWriter &xml) const;
};

/**
 * This class holds information about dimensions. This can be used to store
 * actual dimension information in an instance of BaseType and it can be
 * used to store the definition of a dimension in an instance of Group.
 */
class D4Dimensions {
    vector<D4Dimension *> d_dims;

    D4Group *d_parent; // the group that holds this set of D4Dimensions; weak pointer, don't delete

protected:
    // Note Code in Array depends on the order of these 'new' dimensions
    // matching the 'old' dimensions they are derived from. See
    // Array::update_dimension_pointers. jhrg 8/25/14
    /**
     * @brief Deep-copies dimensions and parent linkage from another collection.
     * @param rhs Source dimensions collection.
     */
    void m_duplicate(const D4Dimensions &rhs) {
        D4DimensionsCIter i = rhs.d_dims.begin();
        while (i != rhs.d_dims.end()) {
            d_dims.push_back(new D4Dimension(**i++)); // deep copy
            d_dims.back()->set_parent(this);          // Set the Dimension's parent
        }

        d_parent = rhs.d_parent;
    }

public:
    /// Iterator used for D4Dimensions
    typedef vector<D4Dimension *>::iterator D4DimensionsIter;
    /// Read-only iterator used for D4Dimensions
    typedef vector<D4Dimension *>::const_iterator D4DimensionsCIter;

    D4Dimensions() : d_parent(0) {}

    /**
     * @brief Builds a dimensions collection owned by a group.
     * @param g Owning group.
     */
    D4Dimensions(D4Group *g) : d_parent(g) {}

    /**
     * @brief Copy-constructs a dimensions collection.
     * @param rhs Source collection.
     */
    D4Dimensions(const D4Dimensions &rhs) : d_parent(0) { m_duplicate(rhs); }

    virtual ~D4Dimensions() {
        D4DimensionsIter i = d_dims.begin();
        while (i != d_dims.end())
            delete *i++;
    }

    /**
     * @brief Assigns this dimensions collection from another collection.
     * @param rhs Source collection.
     * @return This collection after assignment.
     */
    D4Dimensions &operator=(const D4Dimensions &rhs) {
        if (this == &rhs)
            return *this;
        m_duplicate(rhs);
        return *this;
    }

    /// Does this D4Dimensions object actually have dimensions?
    bool empty() const { return d_dims.empty(); }

    /** @brief Returns the owning group. */
    D4Group *parent() const { return d_parent; }
    /** @brief Sets the owning group. @param g Owning group. */
    void set_parent(D4Group *g) { d_parent = g; }

    /** Append a new dimension.
     * In DAP4 dimensions are either of a known size or are varying. For
     * fixed-size dimensions, the value of varying should be false. For varying
     * dimensions the value of 'size' will be ignored - any value can be used
     * when called this method.
     *
     * @param dim Pointer to the D4Dimension object to add; deep copy
     */
    void add_dim(D4Dimension *dim) { add_dim_nocopy(new D4Dimension(*dim)); }

    /** Append a new dimension.
     * @param dim Pointer to the D4Dimension object to add; copies the pointer
     */
    void add_dim_nocopy(D4Dimension *dim) {
        dim->set_parent(this);
        d_dims.push_back(dim);
    }

    /// Get an iterator to the start of the dimensions
    D4DimensionsIter dim_begin() { return d_dims.begin(); }

    /// Get an iterator to the end of the dimensions
    D4DimensionsIter dim_end() { return d_dims.end(); }

    /**
     * @brief Finds a dimension by name.
     * @param name Dimension name.
     * @return Matching dimension or null.
     */
    D4Dimension *find_dim(const string &name);

    /** Insert a dimension.
     * Insert a dimension before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * D4Dimension object.
     * @param dim Inserted before i; deep copy
     * @param i iterator
     */
    void insert_dim(D4Dimension *dim, D4DimensionsIter i) { insert_dim_nocopy(new D4Dimension(*dim), i); }

    /** Insert a dimension.
     * @param dim Inserted before i; pointer copy
     * @param i iterator
     */
    void insert_dim_nocopy(D4Dimension *dim, D4DimensionsIter i) {
        dim->set_parent(this);
        d_dims.insert(i, dim);
    }

    /**
     * @brief Prints all dimensions in DAP4 DMR form.
     * @param xml Destination XML writer.
     * @param constrained True to print constrained dimensions when available.
     */
    void print_dap4(XMLWriter &xml, bool constrained = false) const;
};

} /* namespace libdap */
#endif /* D4DIMENSIONS_H_ */
