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

#ifndef D4MAPS_H_
#define D4MAPS_H_

#include <string>
#include <vector>

#include "Array.h"

using namespace std;

namespace libdap {

class Array;
class XMLWriter;

/**
 * A 'Map' in DAP4 is an Array in the dataset that is used to provide the
 * domain values for a Coverage (aka a DAP2 Grid). These Maps are a more
 * general case of the DAP2 'Map vectors' because a DAP4 Map can have N
 * dimensions.
 *
 * Because the Maps can be shared by many of the Arrays in a dataset,
 * they also correspond to the NetCDF/CF notion of a Shared Dimension.
 *
 * In this implementation of the D4Map, each Map has a name and two weak
 * pointers, one to the Array that holds the domain values and one to the
 * Array that uses the Map. Note that while Maps can be shared by Arrays,
 * each Array has it's own collection of these D4Map objects. This makes
 * processing constraints possible (because it is possible to write
 * different constraints for two arrays that share Maps).
 *
 * Second try. Including these 'weak' pointers lead to a paradox because
 * the pointers don't exist when making deep copies of the Array. In that
 * case the parent and source Array (the array that holds the Map's data)
 * cannot be found because the Array that holds the Maps _does not yet exist_.
 * It's being copied and the source object holds pointers to objects that
 * are not reliable - the source Array is being copied because it's maybe
 * going to be deleted!
 *
 * I removed the parent pointer from the D4Map object and added a pathname
 * to the source Array. So, now we can set _either_ the weak pointer to
 * the source array _or_ the pathname to the source array. The former is
 * used when building the Array (as would be done in a handler) and the
 * latter is used during a deep copy (where even though the pointer to the
 * source Array will soon be invalid, the pathname won't be).
 */
class D4Map {
    std::string d_name;
    std::string d_array_path; ///< The data source for the Map's values
    Array *d_array = nullptr;   // the actual map data; cached weak pointer

public:
    D4Map() = default;
    ///@{
    /// Special constructors for object creation and deep copy
    D4Map(std::string name, Array *array)
            : d_name(std::move(name)), d_array(array) { }
    /// This ctor mimics older behavior - the parent arg is ignored now.
    /// @deprecated
    D4Map(std::string name, Array *array, BaseType * /*parent*/)
            : d_name(std::move(name)), d_array(array) { }
    D4Map(std::string name, std::string array)
            : d_name(std::move(name)), d_array_path(std::move(array)) { }
    ///@}
	virtual ~D4Map() = default;

	const string& name() const { return d_name; }
	void set_name(const string& name) { d_name = name; }

    const std::string &get_array_path() const { return d_array_path; }
    ///@note We can set the path even if the referenced Array does not yet exist!
    void set_array_path(const std::string &array) { d_array_path = array; }

    ///@{
    /// Ways to get the Array that holds a Map's values.

    ///@brief This will always return the correct pointer for a valid data set.
    Array* array(D4Group *root);
    ///@brief Only use this accessor in code that can deal with a nullptr return!
    Array* array() const { return d_array; }
    ///@}

    void set_array(Array *array) {
        d_array = array;
        d_array_path = array->FQN();
    }

	virtual void print_dap4(XMLWriter &xml);
};

/**
 * The D4Maps object holds pointers to all of the Maps used by
 * a given Array.
 */
class D4Maps {
public:
    typedef vector<D4Map*>::iterator D4MapsIter;
    typedef vector<D4Map*>::const_iterator D4MapsCIter;

private:
	vector<D4Map*> d_maps;
	Array *d_parent;	// Array these Maps belong to; weak pointer

    ///@brief Used by the const copy ctor; we only know the source Array path is valid.
	void m_duplicate(const D4Maps &maps) {
		d_parent = maps.d_parent;
        d_maps.reserve(maps.size());
        for (auto const map: maps.d_maps) {
            d_maps.emplace_back(new D4Map(map->name(), map->get_array_path()));
        }
	}

public:
    D4Maps() {}
    D4Maps(Array* parent) : d_parent(parent) { }
    D4Maps(const D4Maps &maps) { m_duplicate(maps); }
    // Build valid Maps for a true deep copy of a whole dataset
    //D4Maps(const D4Maps *maps, Array *parent, D4Group *root);
    virtual ~D4Maps() {
    	for (D4MapsIter i = d_maps.begin(), e = d_maps.end(); i != e; ++i)
    		delete *i;
    }

    D4Maps &operator=(const D4Maps &rhs);

    /**
     * Add a map. This does not test for duplicate names or Array pointers.
     * It assumes that the caller has done that!
     */
    void add_map(D4Map *map) {
    	d_maps.push_back(map);
    }

    void remove_map(D4Map *map) {
        // TODO Refactor this to use erase() and find_if(). There is no reason
        //  to code an explicit loop like this in C++11. jhrg 9/16/22
        for (D4MapsIter i = d_maps.begin(), e = d_maps.end(); i != e; ++i) {
            /* && (*i)->parent() == map->parent() */
            // Don't test if the map->parent() matches - we only care about the name and array.
            // This method is intended for processing CE array slices that are edge cases and
            // is only called from code where we know map->parent() matches *i->parent().
            // jhrg 4/12/16
            if ((*i)->name() == map->name()) {
                d_maps.erase(i);
                break;
            }
        }
    }

    D4Map* get_map(int i) { return d_maps.at(i); }

    D4MapsIter map_begin() { return d_maps.begin(); }
    D4MapsIter map_end() { return d_maps.end(); }

    int size() const { return d_maps.size(); }
    bool empty() const { return d_maps.empty(); }

    virtual void print_dap4(XMLWriter &xml) {
    	for (D4MapsIter i = d_maps.begin(), e = d_maps.end(); i != e; ++i)
    		(*i)->print_dap4(xml);
    }
};

} /* namespace libdap */
#endif /* D4MAPS_H_ */
