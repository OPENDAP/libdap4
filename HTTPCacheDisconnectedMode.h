
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#ifndef _http_cache_disconnected_mode_h
#define _http_cache_disconnected_mode_h

/** Disconnected Operation

    The cache can be set to handle disconnected operation where it does not
    use the network to validate entries and does not attempt to load new
    documents. All requests that can not be fulfilled by the cache will be
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

// $Log: HTTPCacheDisconnectedMode.h,v $
// Revision 1.5  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.4  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.3  2002/10/08 05:29:12  jimg
// Minor change: removed unnecessary enum name.
//
// Revision 1.1.2.2  2002/09/17 22:10:02  jimg
// Preprocessor guard has leading underscore like all the others in our code.
//
// Revision 1.1.2.1  2002/09/17 22:05:22  jimg
// Created using code from W3C's libwww distribution, (c) COPYRIGHT MIT 1995.
// See the file COPYRIGHT.libwww
//

#endif // _http_cache_disconnected_mode_h
