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

using namespace std;

namespace libdap {

class Array;
class XMLWriter;

/**
 * A 'Map' in DAP4 is an Array in the dataset that is used to provide the
 * domain values for a Coverage (aka a Grid). These Maps are a more
 * general case of the DAP2 'Map vectors' because a DAP4 Map can have N
 * dimensions.
 *
 * Because the Maps can be shared by any or all of the Arrays in a dataset,
 * they also correspond to the NetCDF/CF notion of a Shared Dimension.
 *
 * In this implementation of the D4Map, each Map has a name and two weak
 * pointers, one to the Array that holds the domain values and one to the
 * Array that uses the Map. Note that while Maps can be shared by Arrays,
 * each Array has it's own collection of these D4Map objects. This makes
 * processing constraints possible (because it is possible to write
 * different constraints for two arrays that share Maps).
 */
class D4Map {
    std::string d_name;
    Array *d_array;		// the actual map data; weak pointer
    Array *d_parent;	// what array holds this map; weak pointer

public:
    D4Map() : d_name(""), d_array(0), d_parent(0) { }
    D4Map(const string &name, Array *array, Array *parent = 0) : d_name(name), d_array(array), d_parent(parent) { }

	virtual ~D4Map() { }

	const string& name() const { return d_name; }
	void set_name(const string& name) { d_name = name; }

	const Array* array() const { return d_array; }
	void set_array(Array* array) { d_array = array; }

	/**
	 * @brief The Array that holds this Map
	 */
	const Array* parent() const { return d_parent; }
	void set_parent(Array* parent) { d_parent = parent; }

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

	void m_duplicate(const D4Maps &maps) {
		d_parent = maps.d_parent;
		for (D4MapsCIter ci = maps.d_maps.begin(), ce = maps.d_maps.end(); ci != ce; ++ci) {
			d_maps.push_back(new D4Map(**ci));
		}
	}

public:
    D4Maps() {}
    D4Maps(Array* parent) : d_parent(parent) { }
    D4Maps(const D4Maps &maps) { m_duplicate(maps); }
    virtual ~D4Maps() {
    	for (D4MapsIter i = d_maps.begin(), e = d_maps.end(); i != e; ++i)
    		delete *i;
    }

    D4Maps &operator=(const D4Maps &rhs);

    /**
     * Add a map. This does not test for duplicate names or Array pointers;
     * It assumes that the caller has done that!
     */
    void add_map(D4Map *map) {
    	d_maps.push_back(map);
    	// if the Map parent is not set, do so now
    	if (!d_maps.back()->parent())
    		d_maps.back()->set_parent(d_parent);
    }

    void remove_map(D4Map *map) {
        for (D4MapsIter i = d_maps.begin(), e = d_maps.end(); i != e; ++i) {
            /* && (*i)->parent() == map->parent() */
            // Don't test if the map->parent() matches - we only care about the name and array.
            // This method is intended for processing CE array slices that are edge cases and
            // is only called from code where we know map->parent() matches *i->parent().
            // jhrg 4/12/16
            if ((*i)->name() == map->name() && (*i)->array() == map->array()) {
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
