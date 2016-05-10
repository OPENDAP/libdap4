
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

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// These functions are utility functions used by the various DAP parsers (the
// DAS, DDS and constraint expression parsers).
// jhrg 9/7/95

#include "config.h"

#include <cerrno>
#include <cassert>
#include <cstring>
#include <cmath>
#include <cstdlib>

#include <iostream>
#include <sstream>

//  We wrap VC++ 6.x strtod() to account for a short comming
//  in that function in regards to "NaN".
#ifdef WIN32
#include <limits>
double w32strtod(const char *, char **);
#endif

#include "Error.h"
#include "debug.h"
#include "parser.h"             // defines constants such as ID_MAX
#include "dods-limits.h"
#include "util.h"               // Jose Garcia: for append_long_to_string.

using std::cerr;
using std::endl;

#ifdef WIN32
//  VC++ 6.x strtod() doesn't recognize "NaN".  Account for it
//  by wrapping it around a check for the Nan string.  Use of
//  the product is obsolete as of 1/2007, but it is unknown if
//  the issue is still there in later releases of that product.
//  ROM - 01/2007
double w32strtod(const char *val, char **ptr)
{
    //  Convert the two char arrays to compare to strings.
    string *sval = new string(val);
    string *snan = new string("NaN");

    //  If val doesn't contain "NaN|Nan|nan|etc", use strtod as
    //  provided.
    if (stricmp(sval->c_str(), snan->c_str()) != 0)
        return (strtod(val, ptr));

    //  But if it does, return the bit pattern for Nan and point
    //  the parsing ptr arg at the trailing '\0'.
    *ptr = (char *) val + strlen(val);
    return (std::numeric_limits < double >::quiet_NaN());
}
#endif

namespace libdap {

// Deprecated, but still used by the HDF4 EOS server code.
void
parse_error(parser_arg * arg, const char *msg, const int line_num,
            const char *context)
{
    // Jose Garcia
    // This assert(s) is (are) only for developing purposes
    // For production servers remove it by compiling with NDEBUG
    assert(arg);
    assert(msg);

    arg->set_status(FALSE);

    string oss = "";

    if (line_num != 0) {
        oss += "Error parsing the text on line ";
        append_long_to_string(line_num, 10, oss);
    }
    else {
        oss += "Parse error.";
    }

    if (context)
        oss += (string) " at or near: " + context + (string) "\n" + msg
               + (string) "\n";
    else
        oss += (string) "\n" + msg + (string) "\n";

    arg->set_error(new Error(unknown_error, oss));
}

void
parse_error(const char *msg, const int line_num, const char *context)
{
    // Jose Garcia
    // This assert(s) is (are) only for developing purposes
    // For production servers remove it by compiling with NDEBUG
    assert(msg);

    string oss = "";

    if (line_num != 0) {
        oss += "Error parsing the text on line ";
        append_long_to_string(line_num, 10, oss);
    }
    else {
        oss += "Parse error.";
    }

    if (context)
        oss += (string) " at or near: " + context + (string) "\n" + msg
               + (string) "\n";
    else
        oss += (string) "\n" + msg + (string) "\n";

    throw Error(malformed_expr, oss);
}

// context comes from the parser and will always be a char * unless the
// parsers change dramatically.
void
parse_error(const string & msg, const int line_num, const char *context)
{
    parse_error(msg.c_str(), line_num, context);
}

void save_str(char *dst, const char *src, const int line_num)
{
    if (strlen(src) >= ID_MAX)
        parse_error(string("The word `") + string(src)
                    + string("' is too long (it should be no longer than ")
                    + long_to_string(ID_MAX) + string(")."), line_num);

    strncpy(dst, src, ID_MAX);
    dst[ID_MAX - 1] = '\0';     /* in case ... */
}

void save_str(string & dst, const char *src, const int)
{
    dst = src;
}

bool is_keyword(string id, const string & keyword)
{
    downcase(id);
    id = prune_spaces(id);
    DBG(cerr << "is_keyword: " << keyword << " = " << id << endl);
    return id == keyword;
}

/** Check to see if <tt>val</tt> is a valid byte value. If not,
    generate an error message using <tt>parser_error()</tt>. There are
    two versions of <tt>check_byte()</tt>, one which calls
    <tt>parser_error()</tt> and prints an error message to stderr an
    one which calls <tt>parser_error()</tt> and generates and Error
    object.

    @return Returns: TRUE (1) if <i>val</i> is a byte value, FALSE (0) otherwise.
    @brief Is the value a valid byte?
    */
int check_byte(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0);

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        return FALSE;
    }

    DBG(cerr << "v: " << v << endl);

    // We're very liberal here with values. Anything that can fit into 8 bits
    // is allowed through. Clients will have to deal with the fact that the
    // ASCII representation for the value might need to be tweaked. This is
    // especially the case for Java clients where Byte datatypes are
    // signed. 3/20/2000 jhrg
    if ((v < 0 && v < DODS_SCHAR_MIN)
        || (v > 0 && static_cast < unsigned long >(v) > DODS_UCHAR_MAX))
        return FALSE;

    return TRUE;
}

// This version of check_int will pass base 8, 10 and 16 numbers when they
// use the ANSI standard for string representation of those number bases.

int check_int16(const char *val)
{
    char *ptr;
    long v = strtol(val, &ptr, 0);      // `0' --> use val to determine base

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        return FALSE;
    }
    // Don't use the constant from limits.h, use the ones in dods-limits.h
    if (v > DODS_SHRT_MAX || v < DODS_SHRT_MIN) {
        return FALSE;
    }

    return TRUE;
}

int check_uint16(const char *val)
{
    char *ptr;
    unsigned long v = strtol(val, &ptr, 0);

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        return FALSE;
    }

    if (v > DODS_USHRT_MAX) {
        return FALSE;
    }

    return TRUE;
}

int check_int32(const char *val)
{
    char *ptr;
    errno = 0;
    long v = strtol(val, &ptr, 0);      // `0' --> use val to determine base

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        return FALSE;
    }

    // We need to check errno since strtol return clamps on overflow so the
    // check against the DODS values below will always pass, even for out of
    // bounds values in the string. mjohnson 7/20/09
    if (errno == ERANGE) {
        return FALSE;
    }
    // This could be combined with the above, or course, but I'm making it
    // separate to highlight the test. On 64-bit linux boxes 'long' may be
    // 64-bits and so 'v' can hold more than a DODS_INT32. jhrg 3/23/10
    else if (v > DODS_INT_MAX || v < DODS_INT_MIN) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int check_uint32(const char *val)
{
  // Eat whitespace and check for an initial '-' sign...
  // strtoul allows an initial minus. mjohnson
    const char* c = val;
    while (c && isspace(*c)) {
         c++;
    }
    if (c && (*c == '-')) {
         return FALSE;
    }

    char *ptr;
    errno = 0;
    unsigned long v = strtoul(val, &ptr, 0);

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        return FALSE;
    }

	// check overflow first, or the below check is invalid due to
	// clamping to the maximum value by strtoul
	// maybe consider using long long for these checks? mjohnson
	if (errno == ERANGE) {
		return FALSE;
	}
	// See above.
	else if (v > DODS_UINT_MAX) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

int check_int64(const char *val)
{
    char *ptr;
    errno = 0;
    long long v = strtoll(val, &ptr, 0);      // `0' --> use val to determine base

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        return FALSE;
    }

    // We need to check errno since strtol return clamps on overflow so the
    // check against the DODS values below will always pass, even for out of
    // bounds values in the string. mjohnson 7/20/09
    if (errno == ERANGE) {
        return FALSE;
    }
#if 0
    // This could be combined with the above, or course, but I'm making it
    // separate to highlight the test. On 64-bit linux boxes 'long' may be
    // 64-bits and so 'v' can hold more than a DODS_INT32. jhrg 3/23/10
    //
    // Removed - Coverity says it can never be false. Makes sense. jhrg 5/10/16
    else if (v <= DODS_LLONG_MAX && v >= DODS_LLONG_MIN) {
        return FALSE;
    }
#endif
    else {
        return TRUE;
    }
}

int check_uint64(const char *val)
{
  // Eat whitespace and check for an initial '-' sign...
  // strtoul allows an initial minus. mjohnson
    const char* c = val;
    while (c && isspace(*c)) {
         c++;
    }
    if (c && (*c == '-')) {
        return FALSE;
    }

    char *ptr;
    errno = 0;
    unsigned long long v = strtoull(val, &ptr, 0);

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        return FALSE;
    }

    if (errno == ERANGE) {
        return FALSE;
    }
    else if (v > DODS_ULLONG_MAX) { // 2^61
        return FALSE;
    }
    else {
        return v;
    }
}

// Check first for system errors (like numbers so small they convert
// (erroneously) to zero. Then make sure that the value is within
// limits.

int check_float32(const char *val)
{
    char *ptr;
    errno = 0;                  // Clear previous value. Fix for the 64bit
				// IRIX from Rob Morris. 5/21/2001 jhrg

#ifdef WIN32
    double v = w32strtod(val, &ptr);
#else
    double v = strtod(val, &ptr);
#endif

    DBG(cerr << "v: " << v << ", ptr: " << ptr
        << ", errno: " << errno << ", val==ptr: " << (val == ptr) << endl);

    if (errno == ERANGE || (v == 0.0 && val == ptr) || *ptr != '\0')
        return FALSE;

#if 0
    if ((v == 0.0 && (val == ptr || errno == HUGE_VAL || errno == ERANGE))
        || *ptr != '\0') {
        return FALSE;
    }
#endif

    DBG(cerr << "fabs(" << val << ") = " << fabs(v) << endl);
    double abs_val = fabs(v);
    if (abs_val > DODS_FLT_MAX
        || (abs_val != 0.0 && abs_val < DODS_FLT_MIN))
        return FALSE;

    return TRUE;
}

int check_float64(const char *val)
{
    DBG(cerr << "val: " << val << endl);
    char *ptr;
    errno = 0;                  // Clear previous value. 5/21/2001 jhrg

#ifdef WIN32
    double v = w32strtod(val, &ptr);
#else
    double v = strtod(val, &ptr);
#endif

    DBG(cerr << "v: " << v << ", ptr: " << ptr
        << ", errno: " << errno << ", val==ptr: " << (val == ptr) << endl);


    if (errno == ERANGE || (v == 0.0 && val == ptr) || *ptr != '\0')
	return FALSE;
#if 0
    if ((v == 0.0 && (val == ptr || errno == HUGE_VAL || errno == ERANGE))
        || *ptr != '\0') {
        return FALSE;
    }
#endif
    DBG(cerr << "fabs(" << val << ") = " << fabs(v) << endl);
    double abs_val = fabs(v);
    if (abs_val > DODS_DBL_MAX
        || (abs_val != 0.0 && abs_val < DODS_DBL_MIN))
        return FALSE;

    return TRUE;
}

long long get_int64(const char *val)
{
    char *ptr;
    errno = 0;
    long long v = strtoll(val, &ptr, 0);      // `0' --> use val to determine base

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        throw Error("The value '" + string(val) + "' contains extra characters.");
    }

    // We need to check errno since strtol return clamps on overflow so the
    // check against the DODS values below will always pass, even for out of
    // bounds values in the string. mjohnson 7/20/09
    if (errno == ERANGE) {
        throw Error("The value '" + string(val) + "' is out of range.");
    }

#if 0
    // This could be combined with the above, or course, but I'm making it
    // separate to highlight the test. On 64-bit linux boxes 'long' may be
    // 64-bits and so 'v' can hold more than a DODS_INT32. jhrg 3/23/10
    //
    // Removed because coverity flags it as useless, which it is until we
    // have 128-bit ints... jhrg 5/9/16
    else if (v > DODS_LLONG_MAX || v < DODS_LLONG_MIN) {
        throw Error("The value '" + string(val) + "' is out of range.");
    }
#endif

    else {
        return v;
    }
}

unsigned long long get_uint64(const char *val)
{
    // Eat whitespace and check for an initial '-' sign...
    // strtoul allows an initial minus. mjohnson
    const char* c = val;
    while (c && isspace(*c)) {
         c++;
    }
    if (c && (*c == '-')) {
        throw Error("The value '" + string(val) + "' is not a valid array index.");
    }

    char *ptr;
    errno = 0;
    unsigned long long v = strtoull(val, &ptr, 0);

    if ((v == 0 && val == ptr) || *ptr != '\0') {
        throw Error("The value '" + string(val) + "' contains extra characters.");
    }

    if (errno == ERANGE) {
        throw Error("The value '" + string(val) + "' is out of range.");
    }
#if 0
    // Coverity; see above. jhrg 5/9/16
    else if (v > DODS_MAX_ARRAY_INDEX) { // 2^61
        throw Error("The value '" + string(val) + "' is out of range.");
    }
#endif
    else {
        return v;
    }
}

double get_float64(const char *val)
{
    DBG(cerr << "val: " << val << endl);
    char *ptr;
    errno = 0;                  // Clear previous value. 5/21/2001 jhrg

#ifdef WIN32
    double v = w32strtod(val, &ptr);
#else
    double v = strtod(val, &ptr);
#endif

    if (errno == ERANGE || (v == 0.0 && val == ptr) || *ptr != '\0')
        throw Error("The value '" + string(val) + "' is out of range.");;

    DBG(cerr << "fabs(" << val << ") = " << fabs(v) << endl);
    double abs_val = fabs(v);
    if (abs_val > DODS_DBL_MAX || (abs_val != 0.0 && abs_val < DODS_DBL_MIN))
        throw Error("The value '" + string(val) + "' is out of range.");;

    return v;
}

/*
  Maybe someday we will really check the Urls to see if they are valid...
*/

int check_url(const char *)
{
    return TRUE;
}

} // namespace libdap
