
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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

#ifndef ais_exceptions_h
#define ais_exceptions_h

#ifndef _error_h
#include "Error.h"
#endif

namespace libdap
{

/** Thrown when a client asks for the AIS resources for an unknown primary
    resource. */
class NoSuchPrimaryResource : public Error
{
public:
    NoSuchPrimaryResource(): Error("The primary resource given has no matching entry in the AIS database.")
    {}
};

/** Thrown when the AIS database/document cannot be read. */
class AISDatabaseReadFailed : public Error
{
public:
    AISDatabaseReadFailed() : Error("The AIS database read failed.")
    {}
    AISDatabaseReadFailed(const string &msg) :
            Error(string("The AIS database read failed: ") + msg)
    {}
};

/** Thrown when the AIS database/document cannot be written. */
class AISDatabaseWriteFailed : public Error
{
public:
    AISDatabaseWriteFailed() : Error("The AIS database write failed.")
    {}
    AISDatabaseWriteFailed(const string &msg) :
            Error(string("The AIS database write failed: ") + msg)
    {}
};

} // namespace libdap

#endif // ais_exceptions_h
