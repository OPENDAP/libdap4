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

#include "config.h"

#include <gridfields/array.h>

#include "Array.h"
#include "debug.h"

#include "ugrid_utils.h"
#include "LocationType.h"
#include "MeshDataVariable.h"
#include "TwoDMeshTopology.h"

using namespace std;
using namespace libdap;

namespace libdap {

MeshDataVariable::MeshDataVariable(){}

void MeshDataVariable::init(Array *rangeVar)
{
	meshDataVar = rangeVar;
	DBG(cerr << "init() - The user submitted the range data array: " << rangeVar->name() << endl);

	/**
	 * TODO: STOP doing this check and deal with face nodes!
	 * Confirm that submitted variable has a 'location' attribute whose value is "node".
	 */
	if (!checkAttributeValue(rangeVar, UGRID_LOCATION, UGRID_NODE)) {
		// Missing the 'location' attribute? Check for a 'grid_location' attribute whose value is "node".
		if (!checkAttributeValue(rangeVar, UGRID_GRID_LOCATION, UGRID_NODE)) {
			throw Error(
					"The requested range variable '" + rangeVar->name()
							+ "' has neither a '" + UGRID_LOCATION + "' attribute "
							+ "or a " + UGRID_GRID_LOCATION
							+ " attribute whose value is equal to '" + UGRID_NODE
							+ "'.");
		}
	}

	setLocation(node);

	meshName = getAttributeValue(rangeVar, UGRID_MESH);

	DBG(cerr << "init() - Range data array refers to 'mesh' variable:: " << meshVarName << endl);

}


} // namespace libdap
