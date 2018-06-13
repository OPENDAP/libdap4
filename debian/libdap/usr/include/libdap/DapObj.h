// DapObj.h

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.org>
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

/** @brief top level DAP object to house generic methods
 */

#ifndef A_DapObj_h
#define A_DapObj_h 1

#include <iostream>

namespace libdap
{

/** @brief libdap base object for common functionality of libdap objects
 *
 * A base object for any libdap objects to use. Provides simple
 * methods for dumping the contents of the object.
 */

class DapObj
{
public:
    virtual  ~DapObj()
    {}

    /** @brief dump the contents of this object to the specified ostream
     *
     * This method is implemented by all derived classes to dump their
     * contents, in other words, any state they might have, private variables,
     * etc...
     *
     * @param strm C++ i/o stream to dump the object to
     */
    virtual void dump(std::ostream &strm) const = 0 ;
} ;

} // namespace libdap

/** @brief dump the contents of the specified object to the specified ostream
 *
 * This inline method uses the dump method of the DapObj instance passed
 * to it. This allows a user to dump the contents of an object instead of just
 * getting the pointer value of the object.
 *
 * @param strm C++ i/o stream to dump the object to
 * @param obj The DapObj to dump
 */
inline std::ostream &
operator<<(std::ostream &strm, const libdap::DapObj &obj)
{
    obj.dump(strm) ;
    return strm ;
}

#endif // A_DapObj_h

