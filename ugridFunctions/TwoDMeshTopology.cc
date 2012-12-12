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

#include <vector>

#include <gridfields/gridfield.h>
#include <gridfields/grid.h>

#include "BaseType.h"
#include "Array.h"

#include "TwoDMeshTopology.h"

#include "debug.h"

using namespace std;
using namespace libdap;

namespace libdap {

TwoDMeshTopology::TwoDMeshTopology()
{
	_sharedIntArrays = new vector<int *>();
	_sharedFloatArrays = new vector<float *>();
}

TwoDMeshTopology::~TwoDMeshTopology()
{
    DBG(cerr << "Entering ~TwoDMeshTopology (" << this << ")" << endl);


	DBG(cerr << "~TwoDMeshTopology() - Deleting sharedIntArrays..." << endl);
	for (vector<int *>::iterator it = _sharedIntArrays->begin(); it != _sharedIntArrays->end(); ++it) {
		int *i = *it;
		DBG(cerr << "~TwoDMeshTopology() - Deleting int array '" << i << "'" << endl);
		delete [] i;
	}
	delete _sharedIntArrays;

	DBG(cerr << "~TwoDMeshTopology() - Deleting sharedFloatArrays..." << endl);
	for (vector<float *>::iterator it = _sharedFloatArrays->begin(); it != _sharedFloatArrays->end(); ++it) {
		float *f = *it;
		DBG(cerr << "~TwoDMeshTopology() - Deleting float array '" << f << "'" << endl);
		delete [] f;
	}
	delete _sharedFloatArrays;

    DBG(cerr << "Exiting ~TwoDMeshTopology" << endl);
}




} // namespace libdap
