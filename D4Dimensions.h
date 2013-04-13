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

#include "XMLWriter.h"

using namespace std;

namespace libdap {

// FIXME Why does this break other .cc files?
//class XMLWriter;

class D4Dimension {
    string d_name;
    unsigned long d_size;
    bool d_varying;

public:
    D4Dimension(const string &name, unsigned long size): d_name(name), d_size(size), d_varying(false) {}
    D4Dimension(const string &name): d_name(name), d_size(0), d_varying(true) {}
    D4Dimension() : d_name(""), d_size(0), d_varying(0) {}

    string name() {return d_name;}
    void set_name(const string &name) { d_name = name; }

    unsigned long size() { return d_size; }
    void set_size(unsigned long size) { d_size = size; }

    bool varying() { return d_varying; }
    void set_varying(bool varying) { d_varying = varying; }

    void print_dap4(XMLWriter &xml) const;
};

/**
 * This class holds information about dimensions. This can be used to store
 * actual dimension information in an instance of BaseType and it can be
 * used to store the definition of a dimension in an instance of Group.
 *
 * @note In DAP4, dimensions are either of a known size or are 'varying.'
 * Users of this class will need to know this and test the 'size' and 'varying'
 * fields of the 'dimension' struct to figure out which of the two cases are
 * true for any given dimension.
 *
 * @todo Add methods to record constraints? Or subclass this and add that
 * behavior to the specialization?
 */
class D4Dimensions {
    vector<D4Dimension*> d_dims;

    void m_duplicate(const D4Dimensions &rhs) {
        D4DimensionsCIter i = rhs.d_dims.begin();
        while (i != rhs.d_dims.end()) {
            d_dims.push_back(new D4Dimension(**i++));    // deep copy
        }
    }

public:
    /// Iterator used for D4Dimensions
    typedef vector<D4Dimension*>::iterator D4DimensionsIter;
    typedef vector<D4Dimension*>::const_iterator D4DimensionsCIter;

    D4Dimensions() {}
    D4Dimensions(const D4Dimensions &rhs) { m_duplicate(rhs); }

    virtual ~D4Dimensions() {
        D4DimensionsIter i = d_dims.begin();
        while (i != d_dims.end())
            delete *i++;
    }

    D4Dimensions &operator=(const D4Dimensions &rhs) {
        if (this == &rhs) return *this;
        m_duplicate(rhs);
        return *this;
    }

    /// Does this D4Dimensions object actually have dimensions?
    bool empty() const { return d_dims.empty(); }

    /** Append a new dimension.
     * In DAP4 dimensions are either of a known size or are varying. For
     * fixed-size dimensions, the value of varying should be false. For varying
     * dimensions the value of 'size' will be ignored - any value can be used
     * when called this method.
     *
     * @param dim Pointer to the D4Dimension object to add; deep dopy
     */
    void add_dim(D4Dimension *dim) { d_dims.push_back(new D4Dimension(*dim)); }

    /** Append a new dimension.
     * @param dim Pointer to the D4Dimension object to add; copies the pointer
     */
    void add_dim_nocopy(D4Dimension *dim) { d_dims.push_back(dim); }

    /// Get an iterator to the start of the dimensions
    D4DimensionsIter dim_begin() { return d_dims.begin(); }

    /// Get an iterator to the end of the dimensions
    D4DimensionsIter dim_end() { return d_dims.end(); }

    /** Insert a dimension.
     * Insert a dimension before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * D4Dimension object.
     * @param dim Inserted before i; deep copy
     * @param i iterator
     */
    void insert_dim(D4Dimension *dim, D4DimensionsIter i) {
        d_dims.insert(i, new D4Dimension(*dim));
    }

    /** Insert a dimension.
     * @param dim Inserted before i; pointer copy
     * @param i iterator
     */
    void insert_dim_nocopy(D4Dimension *dim, D4DimensionsIter i) {
        d_dims.insert(i, dim);
    }

    void print_dap4(XMLWriter &xml) const;
};

} /* namespace libdap */
#endif /* D4DIMENSIONS_H_ */
