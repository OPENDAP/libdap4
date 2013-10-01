
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//         Dan Holloway <dan@hollywood.gso.uri.edu>
//         Reza Nekovei <reza@intcomm.net>
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

#ifndef ais_merge_h
#define ais_merge_h

#include <string>

#ifndef _das_h
#include "DAS.h"
#endif

#ifndef _httpconnect_h
#include "HTTPConnect.h"
#endif

#ifndef ais_resources_h
#include "AISResources.h"
#endif

#ifndef ais_exceptions_h
#include "AISExceptions.h"
#endif

#ifndef response_h
#include "Response.h"
#endif

namespace libdap
{

/** Manage a single AIS database. Open an AIS database and handle merging DAP
    objects with the ancillary resources it references. This class uses
    AISResources to actually open the AIS database and locate AIS resources
    for a particular primary data source. This class  dereferences AIS
    resources returned by AISResources and merges that with an existing DAP
    object (currently only the DAS object is supported. 02/25/03 jhrg).

    @brief Merge AIS resources. */
class AISMerge
{
private:
    AISResources d_ais_db;
    HTTPConnect d_http;  // used to access remote resources

    friend class AISMergeTest;
public:
    /** Initialize an instance so that it reads from \c database.
    @param database Name of a database of AIS resources.
    @exception AISDatabaseReadFailed Thrown if the named database cannot
    be opened. */
    AISMerge(const string &database) throw(AISDatabaseReadFailed) :
            d_ais_db(database), d_http(RCReader::instance())
    { }

    /** Destroy an instance. This is explicitly declared virtual to support
    subclassing. */
    virtual ~AISMerge()
    {}

    // Change this when HTTPConnect/HTTPCache are changed.
    virtual Response *get_ais_resource(const string &res);

    virtual void merge(const string &primary, DAS &das);
};

} // namespace libdap

#endif // ais_merge_h
