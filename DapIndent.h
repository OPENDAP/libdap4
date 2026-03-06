// DapIndent.h

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

/** @brief indentation to help with dump methods and debugging
 */

#ifndef I_DapIndent_h
#define I_DapIndent_h 1

#include <iostream>
#include <string>

using std::ostream;
using std::string;

namespace libdap {

/** @brief class with static methods to help with indentation of debug
 * information.
 */
class DapIndent {
private:
    static string _indent;

public:
    /** @brief Increase current indentation by one level. */
    static void Indent();

    /** @brief Decrease current indentation by one level. */
    static void UnIndent();

    /** @brief Reset indentation to an empty string. */
    static void Reset();

    /** @brief Get the current indentation string.
     * @return Current indentation text.
     */
    static const string &GetIndent();

    /** @brief Set the indentation string explicitly.
     * @param indent New indentation text.
     */
    static void SetIndent(const string &indent);

    /** @brief Write the current indentation to a stream.
     * @param strm Stream that receives indentation text.
     * @return `strm`, for chaining.
     */
    static ostream &LMarg(ostream &strm);
};

} // namespace libdap

#endif // I_DapIndent_h
