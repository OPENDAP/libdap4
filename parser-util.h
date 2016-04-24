// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
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


#ifndef PARSER_UTIL_H_
#define PARSER_UTIL_H_

namespace libdap {

/** Given a string (<tt>const char *src</tt>), save it to the
    temporary variable pointed to by <tt>dst</tt>. If the string is
    longer than <tt>ID_MAX</tt>, generate and error indicating that
    <tt>src</tt> was truncated to <tt>ID_MAX</tt> characters during
    the copy operation. There are two versions of this function; one
    calls the version of <tt>parser_error()</tt> which writes to
    stderr. The version which accepts the <tt>parser_arg *arg</tt>
    argument calls the version of <tt>parser_error()</tt> which
    generates and Error object.

    @return void
    @brief Save a string to a temporary variable during the parse.
    */

void save_str(char *dst, const char *src, const int line_num);
void save_str(string &dst, const char *src, const int);

bool is_keyword(string id, const string &keyword);

/** Check to see if <tt>val</tt> is a valid byte value. If not,
    generate an error message using <tt>parser_error()</tt>. There are
    two versions of <tt>check_byte()</tt>, one which calls
    <tt>parser_error()</tt> and prints an error message to stderr an
    one which calls <tt>parser_error()</tt> and generates and Error
    object.

    @return Returns: True if <i>val</i> is a byte value, False otherwise.
    @brief Is the value a valid byte?
    */

int check_byte(const char *val);

/** Like <tt>check_byte()</tt> but for 32-bit integers
    (<tt>check_uint()</tt> is for unsigned integers).


    @brief Is the value a valid integer?
    */

int check_int16(const char *val);
int check_uint16(const char *val);
int check_int32(const char *val);
int check_uint32(const char *val);
long long check_int64(const char *val);
unsigned long long check_uint64(const char *val);

unsigned long long get_ull(const char *val);

/** Like <tt>check_byte()</tt> but for 64-bit float values.

    @brief Is the value a valid float? */

int check_float32(const char *val);
int check_float64(const char *val);

/** Currently this function always returns true.

    @brief Is the value a valid URL? */

int check_url(const char *val);

} // namespace libdap

#endif /* PARSER_UTIL_H_ */
