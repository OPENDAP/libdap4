
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

#include "config.h"

// TODO: REmoved unneeded includes.

#include <pthread.h>
#include <limits.h>
#include <unistd.h>   // for stat
#include <sys/types.h>  // for stat and mkdir
#include <sys/stat.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <set>

#include "Error.h"
#include "InternalErr.h"
#include "ResponseTooBigErr.h"
#ifndef WIN32
#include "SignalHandler.h"
#endif
#include "HTTPCacheInterruptHandler.h"
#include "HTTPCacheTable.h"

#include "util_mit.h"
#include "debug.h"

using namespace std;

namespace libdap {

HTTPCacheTable::HTTPCacheTable() {
#if 0
	d_cache_table = new CacheTable[CACHE_TABLE_SIZE];
#endif	
    // Initialize the cache table.
    for (int i = 0; i < CACHE_TABLE_SIZE; ++i)
        d_cache_table[i] = 0;
}

HTTPCacheTable::~HTTPCacheTable() {
#if 0
	delete d_cache_table;
#endif
}

} // namespace libdap
