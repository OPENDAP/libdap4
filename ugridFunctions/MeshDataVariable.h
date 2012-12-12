// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2011,2012 OPeNDAP, Inc.
// Authors: Nathan Potter <ndp@opendap.org>
//          James Gallagher <jgallagher@opendap.org>
//          Scott Moe <smeest1@gmail.com>
//          Bill Howe <billhowe@cs.washington.edu>
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

#ifndef _MeshDataVariable_h
#define _MeshDataVariable_h 1

#include "LocationType.h"

using namespace std;
using namespace libdap;

class Array;

namespace libdap {

class MeshDataVariable {

private:

	/**
	 * The DAP dataset variable that the user requested.
	 */
	Array *_meshDataVar;

	/**
	 * REQUIRED
	 * The attribute mesh points to the mesh_topology variable containing the meta-data attributes
	 * of the mesh on which the variable has been defined.
	 */
	string _meshName;

	/**
	 * REQUIRED
	 * The first DAP dataset variable in the dataset that has a 'cf_role' attribute whose value is equal the value of
	 * the string 'mesh' or an attribute named 'standard_name' whose value is the same as the value of the string 'mesh'.
	 */
	BaseType *_meshTopologyVariable;

	/**
	 * REQUIRED
	 * The attribute location points to the (stagger) location within the mesh at which the
	 * variable is defined. (face or node)
	 */
	locationType _myLocation;

	/**
	 * OPTIONAL
	 * The use of the coordinates attribute is copied from the CF-conventions.
	 * It is used to map the values of variables defined on the unstructured
	 * meshes directly to their location: latitude, longitude and optional elevation.
	 *
	 * The attribute node_coordinates contains a list of the whitespace separated names of
	 * the auxiliary coordinate variables representing the node locations (latitude,
	 * longitude, and optional elevation or other coordinates). These auxiliary coordinate
	 * variables will have length nNodes or nFaces, depending on the value of the location attribute.
	 *
	 * It appears that the coordinates attribute is redundant since the coordinates could also be
	 * obtained by using the appropriate coordinates definition in the mesh topology.
	 *
	 */
	//vector<string> *coordinateNames;
	//vector<Array *> *coordinateArrays;
public:

	MeshDataVariable(Array *dapArray);

	void setLocation(locationType loc);
	locationType getLocation();
};


}
#endif // _MeshDataVariable_h
