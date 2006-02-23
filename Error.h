
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


#include <stdio.h>		// For FILE *

#include <iostream>
#include <string>

using std::cout;
using std::string;
//using std::ostream;

/** The most common errors within DAP2 have special codes so that they
    can be spotted easily by the client software. Any error
    without a matching code gets the <tt>unknown_error</tt> code.

    @brief An enumerated type for common errors.  */
typedef int ErrorCode; //using standard errno+netCDF error codes from server

/** @name Internal DAP errors */
//@{
#define    undefined_error   1000 ///< Undefined error code
#define    unknown_error     1001 ///< Unknown error
#define    internal_error    1002 ///< Internal server error
#define    no_such_file      1003
#define    no_such_variable  1004
#define    malformed_expr    1005
#define    no_authorization  1006
#define    can_not_read_file 1007
#define    cannot_read_file  1008
//@}

/** @deprecated Use of the error-correction program feature is deprecated. */
enum ProgramType {
    undefined_prog_type = -1,
    no_program,			// if there is no program
    java,
    tcl
};

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

class Error {
protected:
    ErrorCode _error_code;
    string _error_message;
    ProgramType _program_type;	///< deprecated; don't use
    char *_program;		///< deprecated; don't use

public:
    /// Create an instance with an error code and a message
    Error(ErrorCode ec, string msg);
    /// Create an instance with the unknown_error code.
    Error(string msg);
    /// Deprecated.
    Error(ErrorCode ec, string msg, ProgramType pt, char *pgm);
    /// Create a default Error object.
    Error();

    /// Copy constructor
    Error(const Error &copy_from);

    virtual ~Error();

    Error &operator=(const Error &rhs);

    /// Class invariant
    bool OK() const;
    /// Deserailze object from the wire.
    bool parse(FILE *fp);
    /// Print the Error object on the given output stream.
    void print(FILE *out) const;
    /// Get the error code.
    ErrorCode get_error_code() const;
    /// Get the error message.
    string get_error_message() const;
    /// Deprecated
    ProgramType get_program_type() const;
    /// Deprecated
    const char *get_program() const;
    /// Set the error code.
    void set_error_code(ErrorCode ec = undefined_error);
    /// Set the error message.
    void set_error_message(string msg = "");
    /// Deprecated
    void set_program_type(ProgramType pt = undefined_prog_type);
    /// Deprecated
    void set_program(char *program);
    /// Deprecated
    void display_message(void *gui = 0) const;
    /// Deprecated
    string correct_error(void *gui) const;
    /// Deprecated
    string error_message(string msg = "");
    /// Deprecated
    ProgramType program_type(ProgramType pt = undefined_prog_type);
    /// Deprecated
    char *program(char *program = 0);
    /// Deprecated
    ErrorCode error_code(ErrorCode ec = undefined_error);
};

#endif // _error_h
