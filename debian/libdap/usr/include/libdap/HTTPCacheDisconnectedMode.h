
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

#ifndef _http_cache_disconnected_mode_h
#define _http_cache_disconnected_mode_h

namespace libdap
{

/** Disconnected Operation

    The cache can be set to handle disconnected operation where it does not
    use the network to validate entries and does not attempt to load new
    documents. All requests that cannot be fulfilled by the cache will be
    returned with a "504 Gateway Timeout" response. There are two operational
    disconnected modes:

    No network activity at all: Here it uses its own persistent cache to
    answer the request, if possible.

    Forward all disconnected requests to a proxy cache: Here it uses the
    HTTP/1.1 cache-control header to indicate that the proxy should operate in
    disconnected mode. */

typedef enum {
    DISCONNECT_NONE     = 0,
    DISCONNECT_NORMAL   = 1,
    DISCONNECT_EXTERNAL = 2
} CacheDisconnectedMode;

} // namespace libdap

#endif // _http_cache_disconnected_mode_h
