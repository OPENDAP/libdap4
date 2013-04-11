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

// FIXME
//class XMLWriter;

/**
 * This class holds information about dimensions. This can be used to store
 * actual dimension information in an instance of BaseType and it can be
 * used to store the definition of a dimension in an instance of Group.
 *
 * @note In DAP4 dimensions are either of a known size or are 'varying.'
 * Users of this class will need to know this and test the 'size' and 'varying'
 * fields of the 'dimension' struct to figure out which of the two casees are
 * true for any given dimension.
 *
 * @todo Add methods to record constraints? Or subclass this and add that
 * behavior to the specialization?
 */
class D4Dimensions {
    struct dimension {
        string name;
        unsigned long size;
        bool varying;

        dimension(const string &n, const unsigned long s, const bool v) :
            name(n), size(s), varying(v) {}
    };

    vector<dimension> d_dims;

    void print_dim(XMLWriter &xml, const dimension &d) const;

public:
    D4Dimensions() {}
    virtual ~D4Dimensions() {}

    /// Iterator used for D4Dimensions
    typedef vector<dimension>::iterator D4DimensionsIter;

    /** Append a new dimension.
     * In DAP4 dimensions are either of a known size or are varying. For
     * fixed-size dimensions, the value of varying should be false. For varying
     * dimensions the value of 'size' will be ignored - any value can be used
     * when called this method.
     *
     * @param name Name of the dimension
     * @param size Size of the dimension
     * @param varying True if this is a varying dimension. False by default.
     */
    void add_dim(const string &name, const unsigned long size, const bool varying = false) {
        d_dims.push_back(dimension(name, size, varying));
    }

    /// Get an iterator to the start of the dimensions
    D4DimensionsIter dim_begin() { return d_dims.begin(); }

    /// Get an iterator to the end of the dimensions
    D4DimensionsIter dim_end() { return d_dims.end(); }

    /// Given an iterator  to a dimension, return its name.
    string get_dim_name(D4DimensionsIter i) const { return (*i).name; }

    /// Given an iterator to a dimension, return its size.
    unsigned long get_dim_size(D4DimensionsIter i) const { return (*i).size; }

    // Given an iterator to a dimension, is it varying in size?
    bool get_dim_varying(D4DimensionsIter i) const { return (*i).varying; }

    /** Insert a dimension.
     * Insert a dimension before the position specified by the iterator.
     * @note Calling this method invalidates all iterators that reference this
     * D4Dimension object.
     * @param name
     * @param size
     * @param varying
     * @param i iterator
     */
    void insert_dim(const string &name, const unsigned long size, const bool varying, D4DimensionsIter i) {
        d_dims.insert(i, dimension(name, size, varying));
    }

    void print_dap4(XMLWriter &xml) const;
};

} /* namespace libdap */
#endif /* D4DIMENSIONS_H_ */
