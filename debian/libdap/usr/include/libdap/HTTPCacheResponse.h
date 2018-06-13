
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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

#ifndef cache_http_response_h
#define cache_http_response_h

#include <cstdio>

#ifndef response_h
#include "Response.h"
#endif

#ifndef _debug_h
#include "debug.h"
#endif

namespace libdap
{

/** Encapsulate a response. Instead of directly returning the FILE pointer
    from which a response is read, return an instance of this object. */
class HTTPCacheResponse : public HTTPResponse
{
private:
    HTTPCache *d_cache;  // pointer to singleton instance

protected:
    /** @name Suppressed default methods */
    //@{
    HTTPCacheResponse();
    HTTPCacheResponse(const HTTPCacheResponse &rs);
    HTTPCacheResponse &operator=(const HTTPCacheResponse &);
    //@}

public:
    /** Build a Response object. Instances of this class are used to
    represent responses from a local HTTP/1.1 cache. The stream and
    headers pointer are passed to the parent (HTTPResponse); there's no
    temporary file for the parent to manage since the body is read from a
    file managed by the cache subsystem. This class releases the lock on
    the cache entry when the destructor is called. */
    HTTPCacheResponse(FILE *s, int status_code, vector<string> *headers, HTTPCache *c)
            : HTTPResponse(s, status_code, headers, ""), d_cache(c)
    {}

    /** Build a Response object. Instances of this class are used to
    represent responses from a local HTTP/1.1 cache. The stream and
    headers pointer are passed to the parent (HTTPResponse); there's no
    temporary file for the parent to manage since the body is read from a
    file managed by the cache subsystem. This class releases the lock on
    the cache entry when the destructor is called. */
    HTTPCacheResponse(FILE *s, int status_code, vector<string> *headers,
	    const string &file_name, HTTPCache *c)
            : HTTPResponse(s, status_code, headers, file_name), d_cache(c)
    {}

    /** Free the cache entry lock. Call the parent's destructor. */
    virtual ~HTTPCacheResponse()
    {
        DBG(cerr << "Freeing HTTPCache resources... ");
        set_file(""); // This keeps ~HTTPResponse() from removing the cache entry.
        d_cache->release_cached_response(get_stream());
        DBGN(cerr << endl);
    }
};

} // namespace libdap

#endif // cache_http_response_h
