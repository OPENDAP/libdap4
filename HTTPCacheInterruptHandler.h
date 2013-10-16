
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

#ifndef http_cache_interrupt_handler_h
#define http_cache_interrupt_handler_h

#include <signal.h>

#include <cassert>
#include <iostream>
#include <algorithm>

#include "HTTPCache.h"
#include "EventHandler.h"
#include "debug.h"

namespace libdap
{

static void
unlink_file(const string &f)
{
    unlink(f.c_str());
}

/** Handle SIGINT for HTTPCache. When the cache is in use and the process is
    sent SIGINT, we must make sure that the cache is left in a consistent
    state. This includes removing the lock file, updating the index file and
    making sure no partially written data or meta data files exist.

    @see EventHandler
    @see SignalHandler
    @see HTTPCache
    @author James Gallagher <jgallagher@opendap.org> */
class HTTPCacheInterruptHandler : public EventHandler
{
private:

public:
    ///
    HTTPCacheInterruptHandler()
    {}

    ///
    virtual ~HTTPCacheInterruptHandler()
    {}

    /** Handle SIGINT. This handler first deletes any files opened but not
    added to the cache index files and then calls
    HTTPCache::delete_instance().

    @param signum We know it is SIGINT; included here as a check and only
    when NDEBUG is not defined.
    @return Never returns. */
    virtual void handle_signal(int signum)
    {
        assert(signum == SIGINT);
        DBG(cerr << "Inside the HTTPCacheInterruptHandler." << endl);

        vector<string> *of = &HTTPCache::_instance->d_open_files;

        DBG(copy(of->begin(), of->end(),
                 ostream_iterator<string>(cerr, "\n")));

        for_each(of->begin(), of->end(), unlink_file);

        HTTPCache::delete_instance();
    }
};

} // namespace libdap

#endif // http_cache_interrupt_handler_h
