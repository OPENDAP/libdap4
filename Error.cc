
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
//      jhrg,jimg James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the Error class.

#include "config.h"

#include <cassert>
#include <cstdio>
#include <utility>

#include "Error.h"
#include "InternalErr.h"
#include "debug.h"
#include "parser.h"

using namespace std;

// Glue routines declared in Error.lex
extern void Error_switch_to_buffer(void *new_buffer);
extern void Error_delete_buffer(void *buffer);
extern void *Error_buffer(FILE *fp);

extern int Errorparse(libdap::parser_arg *arg);

constexpr auto msg = "Assertion failed in: ";
#define ASSERT(x)                                                                                                      \
    do {                                                                                                               \
        if (!(x))                                                                                                      \
            throw libdap::InternalErr(__FILE__, __LINE__, string(msg) + __func__);                                     \
    } while (false)

namespace libdap {

// There are two entries for 'cannot read file' because of an error made
// when the message was first added to this class.
static const char *err_messages[] = {
    "Undefined error",      "Unknown error",    "Internal error",   "No such file",    "No such variable",
    "Malformed expression", "No authorization", "Cannot read file", "Not Implemented", ""};

Error &Error::operator=(const Error &rhs) {
    ASSERT(OK());

    if (&rhs == this) // are they identical?
        return *this;
    else {
        _error_code = rhs._error_code;
        _error_message = rhs._error_message;

        d_file = rhs.d_file;
        d_line = rhs.d_line;

        ASSERT(this->OK());

        return *this;
    }
}

/** Use this function to determine whether an Error object is
    valid.  To be a valid, an Error object must either be: 1)
    empty or contain a message and a code.

    @brief Is the Error object valid?
    @return TRUE if the object is valid, FALSE otherwise. */
bool Error::OK() const {
    // The object is empty - users cannot make these, but this class can!
    bool empty = ((_error_code == undefined_error) && (_error_message.empty()));

    // Just a message - the program part is null.
    bool message = ((_error_code != undefined_error) && (!_error_message.empty()));

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
bool Error::parse(FILE *fp) {
    if (!fp)
        throw InternalErr(__FILE__, __LINE__, "Null input stream");

    void *buffer = Error_buffer(fp);
    Error_switch_to_buffer(buffer);

    parser_arg arg(this);

    bool status;
    try {
        status = Errorparse(&arg) == 0;
        Error_delete_buffer(buffer);
    } catch (Error &e) {
        Error_delete_buffer(buffer);
        throw InternalErr(__FILE__, __LINE__, e.get_error_message());
    }

    // STATUS is the result of the parser function; if a recoverable error
    // was found, it will be true but arg.status() will be false.
    // I'm throwing an InternalErr here since the core generates Error objects;
    // they should always parse! 9/21/2000 jhrg
    if (!status || !arg.status())
        throw InternalErr(__FILE__, __LINE__, "Error parsing error object!");
    else
        return OK(); // Check object consistency
}

/** Creates a printable representation of the Error object. It is suitable
    for framing, and also for printing and sending over a network.

    The printed representation produced by this function can be parsed by the
    parse() member function. Thus parse and print form a symmetrical pair
    that can be used to send and receive an Error object over the network in
    a MIME document.

    @param out A pointer to the output stream on which the Error object is to
    be rendered. */
void Error::print(FILE *out) const {
    ASSERT(OK());

    fprintf(out, "Error {\n");

    fprintf(out, "    code = %d;\n", static_cast<int>(_error_code));

    // If the error message is wrapped in double quotes, print it, else, add
    // wrapping double quotes.
    if (*_error_message.begin() == '"' && *(_error_message.end() - 1) == '"')
        fprintf(out, "    message = %s;\n", _error_message.c_str());
    else
        fprintf(out, "    message = \"%s\";\n", _error_message.c_str());

    fprintf(out, "};\n");
}

/** Creates a printable representation of the Error object. It is suitable
    for framing, and also for printing and sending over a network.

    The printed representation produced by this function can be parsed by the
    parse() member function. Thus parse and print form a symmetrical pair
    that can be used to send and receive an Error object over the network in
    a MIME document.

    @param strm A reference to the output stream on which the Error object is to
    be rendered. */
void Error::print(ostream &strm) const {
    ASSERT(OK());

    strm << "Error {\n";

    strm << "    code = " << static_cast<int>(_error_code) << ";\n";

    // If the error message is wrapped in double quotes, print it, else, add
    // wrapping double quotes.
    if (*_error_message.begin() == '"' && *(_error_message.end() - 1) == '"')
        strm << "    message = " << _error_message.c_str() << ";\n";
    else
        strm << "    message = \"" << _error_message.c_str() << "\";\n";

    strm << "};\n";
}

/** Get the ErrorCode for this instance. */
ErrorCode Error::get_error_code() const {
    ASSERT(OK());
    return _error_code;
}

/** Set the ErrorCode. If the current error message has not been set, use \e
    ec to set the error message. The resulting error message string is the
    same as the ErrorCode name. If \e ec is not within the range of values
    for an OPeNDAP ErrorCode, the error message is left unchanged.

    @param ec The new ErrorCode value. */
void Error::set_error_code(ErrorCode ec) {
    _error_code = ec;
    // Added check to make sure that err_messages is not accessed beyond its
    // bounds. 02/02/04 jhrg
    if (_error_message.empty() && ec > undefined_error && ec <= cannot_read_file) {
        _error_message = err_messages[ec - undefined_error];
    } else {
        _error_message = err_messages[0];
    }
}

/** Return the current error message. */
string Error::get_error_message() const {
    ASSERT(OK());

    return {_error_message};
}

/** Set the error message. */
void Error::set_error_message(const string &msg) { _error_message = msg; }

} // namespace libdap
