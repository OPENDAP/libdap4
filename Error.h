
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

// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for the Error class
//
// jhrg 4/23/96

#ifndef _error_h
#define _error_h

#include <iostream>
#include <string>
#include <exception>

#include <cstdio>  // For FILE *

using std::cout;
using std::string;
using std::ostream;

namespace libdap
{

/** The most common errors within DAP2 have special codes so that they
    can be spotted easily by the client software. Any error
    without a matching code gets the <tt>unknown_error</tt> code.

    @brief An enumerated type for common errors.  */
typedef int ErrorCode; //using standard errno+netCDF error codes from server

/** @name Internal DAP errors */
//@{
#define    undefined_error   1000 ///< Undefined error code, an empty Error object was built
#define    unknown_error     1001 ///< Unknown error (the default code) (HTTP 400)
#define    internal_error    1002 ///< Internal server error (500)
#define    no_such_file      1003 ///< (400)
#define    no_such_variable  1004 ///< (400)
#define    malformed_expr    1005 ///< (400)
#define    no_authorization  1006 ///< (401)
#define    cannot_read_file  1007 ///< (400)
#define    not_implemented   1008 ///< Implies that it will/might be impl. (501)
#define    dummy_message     1009 ///< @see Error.cc; end the array with ""
//@}

/** The Error class is used to transport error information from the server to
    the client within libdap. This class is also the base class for all the
    errors thrown by methods in the DAP, so catching Error will catch all DAP
    throws. Errors consist of an error code and a string. The code can be
    used to quickly distinguish between certain common errors while the
    string is used to convey information about the error to the user. The
    error code should never be displayed to the user.

    This class is used on both clients and servers. The \e print() and \e
    parse() methods are used to send the object back and forth.

    @note A past version of this class supported the notion of an error
    correcting program (Tcl, Java, ...) that could be sent from the server to
    the client to help users correct the error and resubmit the request. This
    never worked well in practice and that feature of the class is deprecated.

    @brief A class for error processing.
    @author jhrg */

class Error : public std::exception
{
protected:
    ErrorCode _error_code;
    std::string _error_message;

public:
    Error(ErrorCode ec, std::string msg);
    Error(std::string msg);
    Error();

    Error(const Error &copy_from);

    virtual ~Error() throw();

    Error &operator=(const Error &rhs);

    bool OK() const;
    bool parse(FILE *fp);
    void print(FILE *out) const;
    void print(std::ostream &out) const;
    ErrorCode get_error_code() const;
    std::string get_error_message() const;
    void set_error_code(ErrorCode ec = undefined_error);
    void set_error_message(std::string msg = "");

    virtual const char* what() const throw() {
        return get_error_message().c_str();
    }
};

} // namespace libdap

#endif // _error_h
