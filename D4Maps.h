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

class D4Map {
    std::string d_name;
    Array *d_array;		// the actual map data; weak pointer
    Array *d_parent;	// what array holds this map; weak pointer

public:
    D4Map() : d_name(""), d_array(0) { }
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
 * Maps in DAP4 are simply the names of Dimensions. When a dimensioned
 * variable (i.e., an array) also has one or more 'maps, ' then that
 * array is a 'grid' (the 'maps' define the domain of a sampled function
 * or a 'coverage').
 */
class D4Maps {
public:
    typedef vector<D4Map*>::iterator D4MapsIter;

private:
	vector<D4Map*> d_maps;
	Array *d_parent;	// Array these Maps belong to; weak pointer

	void m_duplicate(const D4Maps &maps) {
		d_parent = maps.d_parent;
		for (D4MapsIter i = d_maps.begin(), e = d_maps.end(); i != e; ++i) {
			d_maps.push_back(new D4Map(**i));
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

    void add_map(D4Map *map) {
    	d_maps.push_back(map);
    	// if the Map parent is not set, do so now
    	if (!d_maps.back()->parent())
    		d_maps.back()->set_parent(d_parent);
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
