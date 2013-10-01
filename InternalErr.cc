
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

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the InternalErr class.


#include "config.h"

static char rcsid[] not_used =
    {"$Id$"
    };

#include <string>
#include <sstream>

#include "InternalErr.h"
#include "util.h"

using std::endl;
using std::ostringstream;

namespace libdap {

InternalErr::InternalErr() : Error()
{
    _error_code = internal_error;
}

InternalErr::InternalErr(const string &msg) : Error()
{
    _error_code = internal_error;
    _error_message = "";
    _error_message += "An internal error was encountered:\n";
    _error_message += msg + "\n";
    _error_message += "Please report this to support@opendap.org\n";
}


//InternalErr::InternalErr(string msg, string file, int line)
//    : Error(unknown_error, msg)
InternalErr::InternalErr(const string &file, const int &line, const string &msg) : Error()
{
    _error_code = internal_error;
    _error_message = "";
    _error_message += "An internal error was encountered in " + file + " at line ";
    // Jose Garcia. Next we append line to the string _error_code.
    // This function is defined in util.h
    append_long_to_string(line, 10, _error_message);
    _error_message += ":\n";
    _error_message += msg + "\n";
    _error_message += "Please report this to support@opendap.org\n";
}

InternalErr::InternalErr(const InternalErr &copy_from)
        : Error(copy_from)
{}

InternalErr::~InternalErr()
{}

/**
    @brief Is the InternalErr object valid?
    @return TRUE if the object is valid, FALSE otherwise. */
bool
InternalErr::OK()
{
    return Error::OK();
}

} // namespace libdap
