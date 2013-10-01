
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

#include "config.h"

#include <cstdio>
#include <fstream>

#include "AISMerge.h"
#include "AISExceptions.h"
#include "Response.h"

namespace libdap {

/** Access an AIS resource. The resource may be a local file or a URL. Assume
    all resource URIs have no leading spaces. This method is public so that a
    client of libdap++ can specialize its behavior. This implementation
    dereferences a URL or opens a file and returns a FILE pointer which can
    be used to read the contents. In the case of a URL, 'the contents' means
    the body of the HTTP response. For a file, it means the entire file.

    @param res The AIS resource.
    @return Either an open FILE * which points to the resource or null if the
    resource could not be opened. This method does not throw an exception for
    resources that cannot be opened because that can happen for a number of
    reasons which are hardly 'exceptional.' */
Response *AISMerge::get_ais_resource(const string & res)
{
    if (res.find("http:") == 0
        || res.find("file:") == 0 || res.find("https:") == 0) {
        return d_http.fetch_url(res);
    }
    else {
#if 0
        ifstream s(res);
#endif
        FILE *s = fopen(res.c_str(), "r");
        if (!s)
            throw Error("I could not open local AIS resource '"
                        + res + "'.");
        return new Response(s, 0);
    }
}

/** Access the AIS ancillary resources matched to <code>primary</code> and
    merge those with the DAS object \e das.

    This method uses AISResources::has_resource(...) to determine if \c primary
    has an entry in the AIS database. It uses AISResources::get_resource(...)
    to get a ResourceVector of AIS resources for \c primary. It then uses its
    own get_ais_resources(...) to access the actual resources via a FILE
    pointer. Clients may use this information to specialize
    AISMerge::merge(...).

    @param primary The URL of the primary resource.
    @param das The target of the merge operation. This must already contain
    the DAS for \e primary. */
void AISMerge::merge(const string & primary, DAS & das)
{
    if (!d_ais_db.has_resource(primary))
        return;

    try {
        ResourceVector rv = d_ais_db.get_resource(primary);

        for (ResourceVectorIter i = rv.begin(); i != rv.end(); ++i) {
            Response *ais_resource = get_ais_resource(i->get_url());
            switch (i->get_rule()) {
            case Resource::overwrite:
                das.parse(ais_resource->get_stream());
                break;
            case Resource::replace:
                das.erase();
                das.parse(ais_resource->get_stream());
                break;
            case Resource::fallback:
                if (das.get_size() == 0)
                    das.parse(ais_resource->get_stream());
                break;
            }
            delete ais_resource;
            ais_resource = 0;
        }
    }
    catch (NoSuchPrimaryResource & e) {
        throw
        InternalErr(string
                    ("I caught a 'NoSuchPrimaryResource' exception, it said:\n")
                    + e.get_error_message() + string("\n"));
    }
}

} // namespace libdap
