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

// NOTE: This file is built only when the gridfields library is linked with
// the netcdf_handler (i.e., the handler's build is configured using the
// --with-gridfields=... option to the 'configure' script).

#ifndef _UgridUtilities_h
#define _UgridUtilities_h 1


#include "Array.h"


#include <gridfields/array.h>

using namespace std;
using namespace libdap;

namespace libdap {

/**
 *  UGrid attribute vocabulary
 */
const string _cfRole = "cf_role";
const string _standardName = "standard_name";
const string _meshTopology = "mesh_topology";
const string _nodeCoordinates = "node_coordinates";
const string _faceNodeConnectivity = "face_node_connectivity";
const string _dimension = "dimension";
const string _location = "location";
const string _gridLocation = "grid_location";
const string _node = "node";
const string _mesh = "mesh";
const string _start_index = "start_index";


enum locationType {
	node, edge, face
};

static vector<string> &split(const string &s, char delim, vector<string> &elems);
static vector<string> split(const string &s, char delim);

static GF::Array *extractGridFieldArray(Array *a, vector<int*> *sharedIntArrays, vector<float*> *sharedFloatArrays);

template<typename T>static T *extract_array(Array * a) ;

template<typename DODS, typename T> static T *extract_array_helper(Array *a);


static string getAttributeValue(BaseType *bt, string aName) ;
static bool matchesCfRoleOrStandardName(BaseType *bt, string aValue);
static bool same_dimensions(Array *arr1, Array *arr2);

static bool checkAttributeValue(BaseType *bt, string aName, string aValue);

}








#endif // _UgridUtilities_h
