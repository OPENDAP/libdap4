
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2004 OPeNDAP, Inc.
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

#ifndef _signal_handler_registered_err_h
#define _signal_handler_registered_err_h 1

#include <string>

#ifndef _error_h
#include "Error.h"
#endif

namespace libdap
{

/** SignalHandlerRegisteredErr is thrown by HTTPCache::instance() if a signal
    handler is already registered.

    @author jhrg */

class SignalHandlerRegisteredErr: public Error
{

public:
    SignalHandlerRegisteredErr(const string &msg) : Error()
    {
        _error_code = unknown_error;
        _error_message = "";
        _error_message += "A caching error was encountered:\n";
        _error_message += msg + "\n";
    }

    SignalHandlerRegisteredErr() : Error()
    {
        _error_code = unknown_error;
    }
};

} // namespace libdap

#endif // _signal_handler_registered_err_h
