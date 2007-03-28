
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the Error class.


#include "config.h"

static char rcsid[] not_used =
    {"$Id$"
    };

#include <stdio.h>
#include <assert.h>

#include "Error.h"
#include "parser.h"
#include "InternalErr.h"
#include "debug.h"

using namespace std;

// Glue routines declared in Error.lex
extern void Error_switch_to_buffer(void *new_buffer);
extern void Error_delete_buffer(void * buffer);
extern void *Error_buffer(FILE *fp);

extern void Errorrestart(FILE *yyin); // defined in Error.tab.c
extern int Errorparse(void *arg);

static const char *err_messages[] =
    {"Unknown error", "No such file",
     "No such variable", "Malformed expression",
     "No authorization", "Cannot read file"
    };

/** Specializations of Error should use this to set the error code and
    message. */
Error::Error()
        : _error_code(undefined_error), _error_message(""),
        _program_type(undefined_prog_type), _program(0)
{}

/** Create an instance with a specific code and message string. This ctor
    provides a way to to use any code and string you'd like. The code can be
    one of the standard codes or it may be specific to your server. Thus a
    client which can tell it's dealing with a specific type of server can use
    the code accordingly. In general, clients simply show the error message
    to users or write it to a log file.

    @param ec The error code
    @param msg The error message string. */
Error::Error(ErrorCode ec, string msg)
        : _error_code(ec), _error_message(msg),
        _program_type(undefined_prog_type), _program(0)
{}

/** Create an instance with a specific message. The error code is set to \c
    unknown_error.

    @param msg The error message.
    @see ErrorCode */
Error::Error(string msg)
        : _error_code(unknown_error), _error_message(msg),
        _program_type(undefined_prog_type), _program(0)
{}
#if 0
/** @deprecated The error-correction program feature is deprecated. */
Error::Error(ErrorCode ec, string msg, ProgramType pt, char *pgm)
        : _error_code(ec), _error_message(msg),
        _program_type(pt), _program(0)
{
    _program = new char[strlen(pgm) + 1];
    strcpy(_program, pgm);
}
#endif
Error::Error(const Error &copy_from)
        : _error_code(copy_from._error_code),
        _error_message(copy_from._error_message),
        _program_type(copy_from._program_type), _program(0)
{
    if (copy_from._program) {
        _program = new char[strlen(copy_from._program) + 1];
        strcpy(_program, copy_from._program);
    }
}

Error::~Error()
{
    delete _program; _program = 0;
}

Error &
Error::operator=(const Error &rhs)
{
    assert(OK());

    if (&rhs == this)  // are they identical?
        return *this;
    else {
        _error_code = rhs._error_code;
        _error_message = rhs._error_message;
        _program_type = rhs._program_type;

        delete[] _program; _program = 0;
        if (rhs._program) {
            _program = new char[strlen(rhs._program) + 1];
            strcpy(_program, rhs._program);
        }

        assert(this->OK());

        return *this;
    }
}

/** Use this function to determine whether an Error object is
    valid.  To be a valid, an Error object must either be: 1)
    empty or contain a message and a code.

    @brief Is the Error object valid?
    @return TRUE if the object is valid, FALSE otherwise. */
bool
Error::OK() const
{
    // The object is empty - users cannot make these, but this class can!
    bool empty = ((_error_code == undefined_error)
                  && (_error_message.empty()));

    // Just a message - the program part is null.
    bool message = ((_error_code != undefined_error)
                    && (!_error_message.empty()));

    DBG(cerr << "empty: " << empty << ", message: " << message << endl);
    return empty || message;
}

/** Given an input stream (FILE *) <tt>fp</tt>, parse an Error object from
    stream. Values for fields of the Error object are parsed and \c this is
    set accordingly. This is how a client program receives an error object
    from a server.

    @brief Parse an Error object.
    @param fp A valid file pointer to an input stream.
    @return TRUE if no error was detected, FALSE otherwise.  */
bool
Error::parse(FILE *fp)
{
    if (!fp)
        throw InternalErr(__FILE__, __LINE__, "Null input stream");

    void *buffer = Error_buffer(fp);
    Error_switch_to_buffer(buffer);

    parser_arg arg(this);

    bool status;
    try {
        status = Errorparse((void *) & arg) == 0;
        Error_delete_buffer(buffer);
    }
    catch (Error &e) {
        throw InternalErr(__FILE__, __LINE__, e.get_error_message());
        Error_delete_buffer(buffer);
    }

    // STATUS is the result of the parser function; if a recoverable error
    // was found it will be true but arg.status() will be false.
    // I'm throwing an InternalErr here since Error objects are generated by
    // the core; they should always parse! 9/21/2000 jhrg
    if (!status || !arg.status())
        throw InternalErr(__FILE__, __LINE__, "Error parsing error object!");
    else
        return OK();  // Check object consistency
}


/** Creates a printable representation of the Error object. It is suitable
    for framing, and also for printing and sending over a network.

    The printed representation produced by this function can be parsed by the
    parse() member function. Thus parse and print form a symmetrical pair
    that can be used to send and receive an Error object over the network in
    a MIME document.

    @param out A pointer to the output stream on which the Error object is to
    be rendered. */
void
Error::print(FILE *out) const
{
    assert(OK());

    fprintf(out, "Error {\n") ;

    fprintf(out, "    code = %d;\n", static_cast<int>(_error_code)) ;

    // If the error message is wrapped in double quotes, print it, else, add
    // wrapping double quotes.
    if (*_error_message.begin() == '"' && *(_error_message.end() - 1) == '"')
        fprintf(out, "    message = %s;\n", _error_message.c_str()) ;
    else
        fprintf(out, "    message = \"%s\";\n", _error_message.c_str()) ;

    if (_program_type != undefined_prog_type) {
        fprintf(out, "    program_type = %d;\n",
                static_cast<int>(_program_type)) ;
        fprintf(out, "    program = %s;\n", _program) ;
    }

    fprintf(out, "};\n") ;
}
#if 0
/** @deprecated Use the set/get methods instead. */
ErrorCode
Error::error_code(ErrorCode ec)
{
    assert(OK());
    if (ec == undefined_error)
        return _error_code;
    else {
        _error_code = ec;
        // Added check to make sure that messages is not accessed beyond its
        // bounds. 02/02/04 jhrg
        if (_error_message == ""
            && ec > undefined_error && ec <= cannot_read_file)
            _error_message = err_messages[ec - undefined_error - 1];

        return _error_code;
    }
}
#endif
/** Get the ErrorCode for this instance. */
ErrorCode
Error::get_error_code() const
{
    assert(OK());
    return _error_code;
}

/** Set the ErrorCode. If the current error message has not been set, use \e
    ec to set the error message. The resulting error message string is the
    same as the ErrorCode name. If \e ec is not within the range of values
    for an OPeNDAP ErrorCode, the error message is left unchanged.

    @param ec The new ErrorCode value. */
void
Error::set_error_code(ErrorCode ec)
{
    _error_code = ec;
    // Added check to make sure that messages is not accessed beyond its
    // bounds. 02/02/04 jhrg
    if (_error_message.empty()
        && ec > undefined_error && ec <= cannot_read_file)
        _error_message = err_messages[ec - undefined_error - 1];
}
#if 0
/** @deprecated Use the set/get methods instead. */
string
Error::error_message(string msg)
{
    if (msg == "")
        return string(_error_message);
    else {
        _error_message = msg;
        return string(_error_message);
    }
}
#endif
/** Return the current error message. */
string
Error::get_error_message() const
{
    assert(OK());

    return string(_error_message);
}

/** Set the error message. */
void
Error::set_error_message(string msg)
{
    _error_message = msg;
}
#if 0
/** @deprecated Use get_error_message() instead. */
void
Error::display_message(void *) const
{
    assert(OK());
    cerr << _error_message << endl;
}

/** @deprecated The error-correction program feature is deprecated. */
ProgramType
Error::program_type(ProgramType pt)
{
    assert(OK());
    if (pt == undefined_prog_type)
        return _program_type;
    else {
        _program_type = pt;
        return _program_type;
    }
}

/** @deprecated The error-correction program feature is deprecated. */
ProgramType
Error::get_program_type() const
{
    assert(OK());

    return _program_type;
}

/** @deprecated The error-correction program feature is deprecated. */
void
Error::set_program_type(ProgramType pt)
{
    _program_type = pt;
}

/** @deprecated The error-correction program feature is deprecated. */
char *
Error::program(char *pgm)
{
    if (pgm == 0)
        return _program;
    else {
        _program = new char[strlen(pgm) + 1];
        strcpy(_program, pgm);
        return _program;
    }
}

/** @deprecated The error-correction program feature is deprecated. */
const char *
Error::get_program() const
{
    return _program;
}

/** @deprecated The error-correction program feature is deprecated. */
void
Error::set_program(char *pgm)
{
    _program = new char[strlen(pgm) + 1];
    strcpy(_program, pgm);
}

/** @deprecated The error-correction program feature is deprecated. */
string
Error::correct_error(void *) const
{
    assert(OK());
    if (!OK())
        return string("");

    display_message(NULL);
    return string("");
}
#endif
