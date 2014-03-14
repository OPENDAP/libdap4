
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Constants, types and function prototypes for use with the DAP parsers.
//
// jhrg 2/3/96

#ifndef _parser_h
#define _parser_h

#ifndef _error_h
#include "Error.h"
#endif

#define YYDEBUG 1
#undef YYERROR_VERBOSE
#define YY_NO_UNPUT 1

#define ID_MAX 256

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

namespace libdap
{

/** <tt>parser_arg</tt> is used to pass parameters to the bison parsers and get
    error codes and objects in return. If <tt>status()</tt> is true, then the
    <tt>object()</tt> returns a pointer to the object built during the parse
    process. If <tt>status()</tt> is false, then the <tt>error()</tt>
    returns a pointer to an Error object.

    Note that the <tt>object()</tt> mfunc returns a void pointer.
    @brief Pass parameters by reference to a parser.
    @brief Pass parameters by reference to a parser.
    */

struct parser_arg
{
    void *_object;  // nominally a pointer to an object
    Error *_error;  // a pointer to an Error object
    int _status;  // parser status

    parser_arg() : _object(0), _error(0), _status(1)
    {}
    parser_arg(void *obj) : _object(obj), _error(0), _status(1)
    {}
    virtual ~parser_arg()
    {
        if (_error) {
            delete _error; _error = 0;
        }
    }

    void *object()
    {
        return _object;
    }
    void set_object(void *obj)
    {
        _object = obj;
    }
    Error *error()
    {
        return _error;
    }
    void set_error(Error *obj)
    {
        _error = obj;
    }
    int status()
    {
        return _status;
    }
    void set_status(int val = 0)
    {
        _status = val;
    }
};

/** <tt>parser_error()</tt> generates error messages for the various
    parsers used by libdap. There are two versions of the
    function, one which takes a <tt>const char *message</tt> and a
    <tt>const int line_num</tt> and writes the message and line number
    too stderr and a second which takes an additional <tt>parser_arg
    *arg</tt> parameter and writes the error message into an Error
    object which is returned to the caller via the <tt>arg</tt>
    parameter.

    \note{The second version of this function also accepts a third parameter
    (<tt>const char *context</tt>) which can be used to provide an
    additional line of information beyond what is in the string
    <tt>message</tt>.}

    @name parse_error
    @return void
    @brief Generate error messages for the various parsers.
    */
//@{
void parse_error(parser_arg *arg, const char *s, const int line_num = 0,
                 const char *context = 0);
void parse_error(const string &msg, const int line_num,
                 const char *context = 0);
//@}

} // namespace libdap

#include "parser-util.h"

#endif // _parser_h

