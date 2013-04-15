
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

// Utility functions used by the api.
//
// jhrg 9/21/94

#include "config.h"

#include <fstream>

#include <cassert>
#include <cstring>
#include <climits>

#include <ctype.h>
#ifndef TM_IN_SYS_TIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#ifndef WIN32
#include <unistd.h>    // for stat
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "BaseType.h"
#include "Byte.h"
#include "Int16.h"
#include "Int32.h"
#include "UInt16.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
//#include "Url.h"
#include "Array.h"
//#include "Sequence.h"
#include "Error.h"
//#include "parser.h"
#include "util.h"
#include "GNURegex.h"
#include "debug.h"

using namespace std;

namespace libdap {

/** Given a BaseType pointer, extract the string value it contains and return
 it.

 @param arg The BaseType pointer
 @return A C++ string
 @exception Error thrown if the referenced BaseType object does not contain
 a DAP String. */
string extract_string_argument(BaseType * arg)
{
    if (arg->type() != dods_str_c)
        throw Error(malformed_expr,
                "The function requires a DAP string argument.");

    if (!arg->read_p())
        throw InternalErr(__FILE__, __LINE__,
                "The CE Evaluator built an argument list where some constants held no values.");

    string s = dynamic_cast<Str&>(*arg).value();

    DBG(cerr << "s: " << s << endl);

    return s;
}

// @todo Replace new with vector<T> (vector<T> values(src_len);)
template<class T> static void set_array_using_double_helper(Array * a, double *src, int src_len)
{
    T *values = new T[src_len];
    for (int i = 0; i < src_len; ++i)
        values[i] = (T) src[i];

#ifdef VAL2BUF
    a->val2buf(values, true);
#else
    a->set_value(values, src_len);
#endif

    delete[]values;
}

/** Given an array that holds some sort of numeric data, load it with values
 using an array of doubles. This function makes several assumptions. First,
 it assumes the caller really wants to put the doubles into whatever types
 the array holds! Caveat emptor. Second, it assumes that if the size of
 source (\e src) array is different than the destination (\e dest) the
 caller has made a mistake. In that case it will throw an Error object.

 After setting the values, this method sets the \c read_p property for
 \e dest. Setting \e read_p tells the serialization methods in libdap
 that this variable already holds data values and, given that, the
 serialization code will not try to read the values.

 @param dest An Array. The values are written to this array, reusing
 its storage. Existing values are lost.
 @param src The source data.
 @param src_len The number of elements in the \e src array.
 @exception Error Thrown if \e dest is not a numeric-type array (Byte, ...,
 Float64) or if the number of elements in \e src does not match the number
 is \e dest. */
void set_array_using_double(Array * dest, double *src, int src_len)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if ((dest->type() == dods_array_c && !dest->var()->is_simple_type())
    || dest->var()->type() == dods_str_c
    || dest->var()->type() == dods_url_c)
        throw InternalErr(__FILE__, __LINE__,
                "The function requires a DAP numeric-type array argument.");

    // Test sizes. Note that Array::length() takes any constraint into account
    // when it returns the length. Even if this was removed, the 'helper'
    // function this uses calls Vector::val2buf() which uses Vector::width()
    // which in turn uses length().
    if (dest->length() != src_len)
        throw InternalErr(__FILE__, __LINE__,
                "The source and destination array sizes don't match ("
                + long_to_string(src_len) + " versus "
                + long_to_string(dest->length()) + ").");

    // The types of arguments that the CE Parser will build for numeric
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than
    // just arguments.
    switch (dest->var()->type()) {
    case dods_byte_c:
        set_array_using_double_helper<dods_byte>(dest, src, src_len);
        break;
    case dods_uint16_c:
        set_array_using_double_helper<dods_uint16>(dest, src, src_len);
        break;
    case dods_int16_c:
        set_array_using_double_helper<dods_int16>(dest, src, src_len);
        break;
    case dods_uint32_c:
        set_array_using_double_helper<dods_uint32>(dest, src, src_len);
        break;
    case dods_int32_c:
        set_array_using_double_helper<dods_int32>(dest, src, src_len);
        break;
    case dods_float32_c:
        set_array_using_double_helper<dods_float32>(dest, src, src_len);
        break;
    case dods_float64_c:
        set_array_using_double_helper<dods_float64>(dest, src, src_len);
        break;
    default:
        throw InternalErr(__FILE__, __LINE__,
                "The argument list built by the CE parser contained an unsupported numeric type.");
    }

    // Set the read_p property.
    dest->set_read_p(true);
}

template<class T> static double *extract_double_array_helper(Array * a)
{
    int length = a->length();
    // Could improve this using vector<T>. jhrg
    T *b = new T[length];
    a->value(b);

    double *dest = new double[length];
    for (int i = 0; i < length; ++i)
        dest[i] = (double) b[i];
    delete[]b;

    return dest;
}

/** Given a pointer to an Array which holds a numeric type, extract the
 values and return in an array of doubles. This function allocates the
 array using 'new double[n]' so delete[] MUST be used when you are done
 the data. */
double *extract_double_array(Array * a)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if ((a->type() == dods_array_c && !a->var()->is_simple_type())
    || a->var()->type() == dods_str_c || a->var()->type() == dods_url_c)
        throw Error(malformed_expr,
                "The function requires a DAP numeric-type array argument.");

    if (!a->read_p())
        throw InternalErr(__FILE__, __LINE__,
                string("The Array '") + a->name() +
                "'does not contain values.");

    // The types of arguments that the CE Parser will build for numeric
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than
    // just arguments.
    switch (a->var()->type()) {
    case dods_byte_c:
        return extract_double_array_helper<dods_byte>(a);
    case dods_uint16_c:
        return extract_double_array_helper<dods_uint16>(a);
    case dods_int16_c:
        return extract_double_array_helper<dods_int16>(a);
    case dods_uint32_c:
        return extract_double_array_helper<dods_uint32>(a);
    case dods_int32_c:
        return extract_double_array_helper<dods_int32>(a);
    case dods_float32_c:
        return extract_double_array_helper<dods_float32>(a);
    case dods_float64_c:
        return extract_double_array_helper<dods_float64>(a);
    default:
        throw InternalErr(__FILE__, __LINE__,
                "The argument list built by the CE parser contained an unsupported numeric type.");
    }
}

/** Given a BaseType pointer, extract the numeric value it contains and return
 it in a C++ double.

 @param arg The BaseType pointer
 @return A C++ double
 @exception Error thrown if the referenced BaseType object does not contain
 a DAP numeric value. */
double extract_double_value(BaseType * arg)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if (!arg->is_simple_type() || arg->type() == dods_str_c || arg->type()
            == dods_url_c)
        throw Error(malformed_expr,
                "The function requires a DAP numeric-type argument.");

    if (!arg->read_p())
        throw InternalErr(__FILE__, __LINE__,
                "The CE Evaluator built an argument list where some constants held no values.");

    // The types of arguments that the CE Parser will build for numeric
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than
    // just arguments.
    switch (arg->type()) {
    case dods_byte_c:
        return (double)(dynamic_cast<Byte&>(*arg).value());
    case dods_uint16_c:
        return (double)(dynamic_cast<UInt16&>(*arg).value());
    case dods_int16_c:
        return (double)(dynamic_cast<Int16&>(*arg).value());
    case dods_uint32_c:
        return (double)(dynamic_cast<UInt32&>(*arg).value());
    case dods_int32_c:
        return (double)(dynamic_cast<Int32&>(*arg).value());
    case dods_float32_c:
        return (double)(dynamic_cast<Float32&>(*arg).value());
    case dods_float64_c:
        return dynamic_cast<Float64&>(*arg).value();
    default:
        throw InternalErr(__FILE__, __LINE__,
                "The argument list built by the CE parser contained an unsupported numeric type.");
    }
}

// Remove spaces from the start of a URL and from the start of any constraint
// expression it contains. 4/7/98 jhrg

/** Removed spaces from the front of a URL and also from the front of the CE.
    This function assumes that there are no holes in both the URL and the CE.
    It will remove \e leading space, but not other spaces.

    @param name The URL to process
    @return Returns a new string object that contains the pruned URL. */
string
prune_spaces(const string &name)
{
    // If the URL does not even have white space return.
    if (name.find_first_of(' ') == name.npos)
        return name;
    else {
        // Strip leading spaces from http://...
        unsigned int i = name.find_first_not_of(' ');
        string tmp_name = name.substr(i);

        // Strip leading spaces from constraint part (following `?').
        unsigned int j = tmp_name.find('?') + 1;
        i = tmp_name.find_first_not_of(' ', j);
        tmp_name.erase(j, i - j);

        return tmp_name;
    }
}

// Compare elements in a list of (BaseType *)s and return true if there are
// no duplicate elements, otherwise return false.

bool
unique_names(vector<BaseType *> l, const string &var_name,
             const string &type_name, string &msg)
{
    // copy the identifier names to a vector
    vector<string> names(l.size());

    int nelem = 0;
    typedef std::vector<BaseType *>::const_iterator citer ;
    for (citer i = l.begin(); i != l.end(); i++) {
        assert(*i);
        names[nelem++] = (*i)->name();
        DBG(cerr << "NAMES[" << nelem - 1 << "]=" << names[nelem-1] << endl);
    }

    // sort the array of names
    sort(names.begin(), names.end());

    // sort the array of names
    sort(names.begin(), names.end());

    // look for any instance of consecutive names that are ==
    for (int j = 1; j < nelem; ++j) {
        if (names[j-1] == names[j]) {
            ostringstream oss;
            oss << "The variable `" << names[j]
            << "' is used more than once in " << type_name << " `"
            << var_name << "'";
            msg = oss.str();

            return false;
        }
    }

    return true;
}

const char *
libdap_root()
{
    return LIBDAP_ROOT;
}

/** Return the version string for this package.
    @note This function has C linkage so that it can be found using autoconf
    tests.
    @return The version string. */
extern "C" const char *
libdap_version()
{
    return PACKAGE_VERSION;
}

extern "C"
    const char *
    libdap_name()
{
    return PACKAGE_NAME;
}

/**
 * Use the system time() function to get the current time. Return a string,
 * removing the trailing newline that time() includes in its response.
 * @return A C++ string with the current system time as formatted by time()
 */
string
systime()
{
    time_t TimBin;

    if (time(&TimBin) == (time_t) - 1)
        return string("time() error");
    else {
        string TimStr = ctime(&TimBin);
        return TimStr.substr(0, TimStr.size() - 2); // remove the \n
    }
}

/**
 * Downcase the source string. This function modifies its argument.
 * @param The string to modify
 */
void
downcase(string &s)
{
    for (unsigned int i = 0; i < s.length(); i++)
        s[i] = tolower(s[i]);
}

/**
 * Is the string surrounded by double quotes?
 * @param s The source string
 * @reurn True if the string is quoted, false otherwise.
 */
bool
is_quoted(const string &s)
{
    return (!s.empty() && s[0] == '\"' && s[s.length()-1] == '\"');
}

/**
 * Return a new string that is not quoted. This will return a new string
 * regardless of whether the source string is actualy quoted.
 * @param s The source string
 * @return A new string without quotes
 */
string
remove_quotes(const string &s)
{
    if (is_quoted(s))
        return s.substr(1, s.length() - 2);
    else
        return s;
}

/** Get the Type enumeration value which matches the given name. */
Type get_type(const char *name)
{
    if (strcmp(name, "Byte") == 0)
        return dods_byte_c;

    if (strcmp(name, "Int8") == 0)
        return dods_int8_c;

    if (strcmp(name, "UInt8") == 0)
        return dods_uint8_c;

    if (strcmp(name, "Int16") == 0)
        return dods_int16_c;

    if (strcmp(name, "UInt16") == 0)
        return dods_uint16_c;

    if (strcmp(name, "Int32") == 0)
        return dods_int32_c;

    if (strcmp(name, "UInt32") == 0)
        return dods_uint32_c;

    if (strcmp(name, "Int64") == 0)
        return dods_int64_c;

    if (strcmp(name, "UInt64") == 0)
        return dods_uint64_c;

    if (strcmp(name, "Float32") == 0)
        return dods_float32_c;

    if (strcmp(name, "Float64") == 0)
        return dods_float64_c;

    if (strcmp(name, "String") == 0)
        return dods_str_c;

    if (strcmp(name, "URL") == 0)
        return dods_url4_c;

    if (strcmp(name, "Url") == 0)
        return dods_url_c;

    if (strcmp(name, "Enum") == 0)
        return dods_enum_c;

    if (strcmp(name, "Opaque") == 0)
        return dods_opaque_c;

   if (strcmp(name, "Array") == 0)
        return dods_array_c;

    if (strcmp(name, "Structure") == 0)
        return dods_structure_c;

    if (strcmp(name, "Sequence") == 0)
        return dods_sequence_c;

    if (strcmp(name, "Grid") == 0)
        return dods_grid_c;

    return dods_null_c;
}

/** @brief Returns the type of the class instance as a string. */
string
type_name(Type t)
{
    switch (t) {
    case dods_null_c:
        return string("Null");
    case dods_byte_c:
        return string("Byte");
    case dods_int16_c:
        return string("Int16");
    case dods_uint16_c:
        return string("UInt16");
    case dods_int32_c:
        return string("Int32");
    case dods_uint32_c:
        return string("UInt32");
    case dods_float32_c:
        return string("Float32");
    case dods_float64_c:
        return string("Float64");
    case dods_str_c:
        return string("String");
    case dods_url_c:
        return string("Url");
    case dods_array_c:
        return string("Array");
    case dods_structure_c:
        return string("Structure");
    case dods_sequence_c:
        return string("Sequence");
    case dods_grid_c:
        return string("Grid");

    case dods_int8_c:
        return string("Int8");
    case dods_uint8_c:
        return string("UInt8");
    case dods_int64_c:
        return string("Int64");
    case dods_uint64_c:
        return string("UInt64");
    case dods_url4_c:
        return string("URL");
    case dods_group_c:
        return string("Group");
    case dods_enum_c:
        return string("Enum");

    default:
        throw InternalErr(__FILE__, __LINE__, "Unknown type.");
    }
}

/** @brief Returns true if the instance is a numeric, string or URL
    type variable.
    @return True if the instance is a scalar numeric, String or URL variable,
    False otherwise. Arrays (even of simple types) return False.
    @see is_vector_type() */
bool
is_simple_type(Type t)
{
    switch (t) {

    case dods_byte_c:

    case dods_int8_c:
    case dods_uint8_c:

    case dods_int16_c:
    case dods_uint16_c:
    case dods_int32_c:
    case dods_uint32_c:

    case dods_int64_c:
    case dods_uint64_c:

    case dods_float32_c:
    case dods_float64_c:
    case dods_str_c:
    case dods_url_c:

    case dods_url4_c:
    case dods_enum_c:
        return true;

    case dods_null_c:
    case dods_array_c:
    case dods_structure_c:
    case dods_sequence_c:
    case dods_grid_c:
    case dods_group_c:
        return false;
    }

    return false;
}

/** @brief Returns true if the instance is a vector (i.e., array) type
    variable.
    @return True if the instance is an Array, False otherwise. */
bool
is_vector_type(Type t)
{
    switch (t) {
    case dods_null_c:
    case dods_byte_c:

    case dods_int8_c:
    case dods_uint8_c:

    case dods_int16_c:
    case dods_uint16_c:

    case dods_int32_c:
    case dods_uint32_c:

    case dods_int64_c:
    case dods_uint64_c:

    case dods_float32_c:
    case dods_float64_c:

    case dods_str_c:
    case dods_url_c:

    case dods_url4_c:
    case dods_enum_c:
        return false;

    case dods_array_c:
        return true;

    case dods_structure_c:
    case dods_sequence_c:
    case dods_grid_c:
    case dods_group_c:
        return false;
    }

    return false;
}

/** @brief Returns true if the instance is a constructor (i.e., Structure,
    Sequence or Grid) type variable.
    @return True if the instance is a Structure, Sequence or Grid, False
    otherwise. */
bool
is_constructor_type(Type t)
{
    switch (t) {
    case dods_null_c:
    case dods_byte_c:

    case dods_int8_c:
    case dods_uint8_c:

    case dods_int16_c:
    case dods_uint16_c:
    case dods_int32_c:
    case dods_uint32_c:

    case dods_int64_c:
    case dods_uint64_c:

    case dods_float32_c:
    case dods_float64_c:
    case dods_str_c:
    case dods_url_c:

    case dods_url4_c:
    case dods_enum_c:

    case dods_array_c:
        return false;

    case dods_structure_c:
    case dods_sequence_c:
    case dods_grid_c:
    case dods_group_c:
        return true;
    }

    return false;
}

/**
 * Is this an integer type?
 * @return True if the type holds an interger value, false otherwise.
 */
bool is_integer_type(Type t)
{
    switch (t) {
        case dods_byte_c:
        case dods_int8_c:
        case dods_uint8_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_int64_c:
        case dods_uint64_c:
            return true;
        default:
            return false;
    }
}

/**
 * Does the directory exist?
 *
 * @param dir The pathname to test.
 * @return True if the directory exists, false otherwise
 */
bool
dir_exists(const string &dir)
{
    struct stat buf;

    return (stat(dir.c_str(), &buf) == 0) && (buf.st_mode & S_IFDIR);
}

#if 0

// UNTESTED 11/7/12

/**
 * Is the directory writable?
 *
 * @param dir The pathname to test
 * @return True if the pathname is a directory and can be written by the
 * caller, false otherwise.
 */
bool
dir_writable(const string &dir)
{
    try {
        string test = dir + "/test.txt";
        ofstream ofs(dir.c_str());
        ofs.write("test", 5);
        ofs.close();
        unlink(test.c_str());
        return true;
    }
    catch (...) {
        return false;
    }
}
#endif

#ifdef WIN32
//  Sometimes need to buffer within an iostream under win32 when
//  we want the output to go to a FILE *.  This is because
//  it's not possible to associate an ofstream with a FILE *
//  under the Standard ANSI C++ Library spec.  Unix systems
//  don't follow the spec in this regard.
void flush_stream(iostream ios, FILE *out)
{
    int nbytes;
    char buffer[512];

    ios.get(buffer, 512, NULL);
    while ((nbytes = ios.gcount()) > 0) {
        fwrite(buffer, 1, nbytes, out);
        ios.get(buffer, 512, NULL);
    }

    return;
}
#endif

// Jose Garcia
void
append_long_to_string(long val, int base, string &str_val)
{
    // The array digits contains 36 elements which are the
    // posible valid digits for out bases in the range
    // [2,36]
    char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // result of val / base
    ldiv_t r;

    if (base > 36 || base < 2) {
        // no conversion if wrong base
        std::invalid_argument ex("The parameter base has an invalid value.");
        throw ex;
    }
    if (val < 0)
        str_val += '-';
    r = ldiv(labs(val), base);

    // output digits of val/base first
    if (r.quot > 0)
        append_long_to_string(r.quot, base, str_val);

    // output last digit

    str_val += digits[(int)r.rem];
}

// base defaults to 10
string
long_to_string(long val, int base)
{
    string s;
    append_long_to_string(val, base, s);
    return s;
}

// Jose Garcia
void append_double_to_string(const double &num, string &str)
{
    // s having 100 characters should be enough for sprintf to do its job.
    // I want to banish all instances of sprintf. 10/5/2001 jhrg
    ostringstream oss;
    oss.precision(9);
    oss << num;
    str += oss.str();
}

string
double_to_string(const double &num)
{
    string s;
    append_double_to_string(num, s);
    return s;
}

// Given a pathname, return the file at the end of the path. This is used
// when reporting errors (maybe other times, too) to keep the server from
// revealing too much about its organization when sending error responses
// back to clients. 10/11/2000 jhrg
// MT-safe. 08/05/02 jhrg

#ifdef WIN32
static const char path_sep[] =
    {"\\"
    };
#else
static const char path_sep[] =
    {"/"
    };
#endif

/** Get the filename part from a path. This function can be used to return a
    string that has the directory components stripped from a path. This is
    useful when building error message strings.

    If WIN32 is defined, use '\' as the path separator, otherwise use '/' as
    the path separator.

    @return A string containing only the filename given a path. */
string
path_to_filename(string path)
{
    string::size_type pos = path.rfind(path_sep);

    return (pos == string::npos) ? path : path.substr(++pos);
}

#define CHECK_BIT( tab, bit ) ( tab[ (bit)/8 ] & (1<<( (bit)%8 )) )
#define BITLISTSIZE 16 /* bytes used for [chars] in compiled expr */

/*
 * globchars() - build a bitlist to check for character group match
 */

static void globchars(const char *s, const char *e, char *b) {
    int neg = 0;

    memset(b, '\0', BITLISTSIZE);

    if (*s == '^')
        neg++, s++;

    while (s < e) {
        int c;

        if (s + 2 < e && s[1] == '-') {
            for (c = s[0]; c <= s[2]; c++)
                b[c / 8] |= (1 << (c % 8));
            s += 3;
        }
        else {
            c = *s++;
            b[c / 8] |= (1 << (c % 8));
        }
    }

    if (neg) {
        int i;
        for (i = 0; i < BITLISTSIZE; i++)
            b[i] ^= 0377;
    }

    /* Don't include \0 in either $[chars] or $[^chars] */

    b[0] &= 0376;
}

/**
 * glob:  match a string against a simple pattern
 *
 * Understands the following patterns:
 *
 *  *   any number of characters
 *  ?   any single character
 *  [a-z]   any single character in the range a-z
 *  [^a-z]  any single character not in the range a-z
 *  \x  match x
 *
 * @param c The pattern
 * @param s The string
 * @return 0 on success, -1 if the pattern is exhausted but there are
 * characters remaining in the string and 1 if the pattern does not match
 */
int
glob(const char *c, const char *s)
{
    if (!c || !s)
        return 1;

    char bitlist[BITLISTSIZE];
    int i = 0;
    for (;;) {
        ++i;
        switch (*c++) {
        case '\0':
            return *s ? -1 : 0;

        case '?':
            if (!*s++)
                return i/*1*/;
            break;

        case '[': {
            /* scan for matching ] */

            const char *here = c;
            do {
                if (!*c++)
                    return i/*1*/;
            } while (here == c || *c != ']');
            c++;

            /* build character class bitlist */

            globchars(here, c, bitlist);

            if (!CHECK_BIT( bitlist, *(unsigned char *)s ))
                return i/*1*/;
            s++;
            break;
        }

        case '*': {
            const char *here = s;

            while (*s)
                s++;

            /* Try to match the rest of the pattern in a recursive */
            /* call.  If the match fails we'll back up chars, retrying. */

            while (s != here) {
                int r;

                /* A fast path for the last token in a pattern */

                r = *c ? glob(c, s) : *s ? -1 : 0;

                if (!r)
                    return 0;
                else if (r < 0)
                    return i/*1*/;

                --s;
            }
            break;
        }

        case '\\':
            /* Force literal match of next char. */

            if (!*c || *s++ != *c++)
                return i/*1*/;
            break;

        default:
            if (*s++ != c[-1])
                return i/*1*/;
            break;
        }
    }

    return 1;   // Should never get here; this quiets gcc's warning
}

/** @name Security functions */
//@{

/** @brief sanitize the size of an array.
    Test for integer overflow when dynamically allocating an array.
    @param nelem Number of elements.
    @param sz size of each element.
    @return True if the \c nelem elements of \c sz size will overflow an array. */
bool
size_ok(unsigned int sz, unsigned int nelem)
{
    return (sz > 0 && nelem < UINT_MAX / sz);
}

/** @brief Does the string name a potentially valid pathname?
    Test the given pathname to verify that it is a valid name. We define this
    as: Contains only printable characters; and Is less then 256 characters.
    If \e strict is true, test that the pathname consists of only letters,
    digits, and underscore, dash and dot characters instead of the more general
    case where a pathname can be composed of any printable characters.

    @note Using this function does not guarantee that the path is valid, only
    that the path \e could be valid. The intent is foil attacks where an
    exploit is encoded in a string then passed to a library function. This code
    does not address whether the pathname references a valid resource.

    @param path The pathname to test
    @param strict Apply more restrictive tests (true by default)
    @return true if the pathname consists of legal characters and is of legal
    size, false otherwise. */
bool
pathname_ok(const string &path, bool strict)
{
    if (path.length() > 255)
        return false;

    Regex name("[-0-9A-z_./]+");
    if (!strict)
        name = "[:print:]+";

    string::size_type len = path.length();
    int result = name.match(path.c_str(), len);
    // Protect against casting too big an uint to int
    // if LEN is bigger than the max int32, the second test can't work
    if (len > INT_MAX || result != static_cast<int>(len))
        return false;

    return true;
}

//@}

/**
 * Get the version of the DAP library.
 * @deprecated
 */
string
dap_version()
{
    return (string)"OPeNDAP DAP/" + libdap_version() + ": compiled on " + __DATE__ + ":" + __TIME__ ;
}

// Since Server4 can get compressed responses using Tomcat, bail on this
// software (which complicates building under Win32). It can be turned on
// for use with Server3 in configure.ac.

#if COMPRESSION_FOR_SERVER3

// Return true if the program deflate exists and is executable by user, group
// and world. If this returns false the caller should assume that server
// filter programs won't be able to find the deflate program and thus won't
// be able to compress the return document.
// NB: this works because this function uses the same rules as compressor()
// (which follows) to look for deflate. 2/11/98 jhrg

bool
deflate_exists()
{
    DBG(cerr << "Entering deflate_exists...");

    int status = false;
    struct stat buf;

#ifdef WIN32
    string deflate = (string)libdap_root() + "\\bin\\deflate";
#else
    string deflate = (string)libdap_root() + "/sbin/deflate";
#endif

    // Check that the file exists...
    // First look for deflate using DODS_ROOT (compile-time constant subsumed
    // by an environment variable) and if that fails in the CWD which finds
    // the program when it is in the same directory as the dispatch script
    // and other server components. 2/11/98 jhrg
    status = (stat(deflate.c_str(), &buf) == 0)
#ifdef WIN32
             || (stat(".\\deflate", &buf) == 0);
#else
             || (stat("./deflate", &buf) == 0);
#endif

    // and that it can be executed.
#ifdef WIN32
    status &= (buf.st_mode & _S_IEXEC);
#else
    status &= buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);
#endif
    DBG(cerr << " returning " << (status ? "true." : "false.") << endl);
    return (status != 0);
}

FILE *
compressor(FILE *output, int &childpid)
{
#ifdef WIN32
    //  There is no such thing as a "fork" under win32. This makes it so that
    //  we have to juggle handles more aggressively. This code hasn't been
    //  tested and shown to work as of 07/2000.
    int pid, data[2];
    int hStdIn, hStdOut;

    if (_pipe(data, 512, O_BINARY | O_NOINHERIT) < 0) {
        cerr << "Could not create IPC channel for compressor process"
        << endl;
        return NULL;
    }


    // This sets up for the child process, but it has to be reversed for the
    // parent after the spawn takes place.

    // Store stdin, stdout so we have something to restore to
    hStdIn  = _dup(_fileno(stdin));
    hStdOut = _dup(_fileno(stdout));

    // Child is to read from read end of pipe
    if (_dup2(data[0], _fileno(stdin)) != 0) {
        cerr << "dup of child stdin failed" << endl;
        return NULL;
    }
    // Child is to write its's stdout to file
    if (_dup2(_fileno(output), _fileno(stdout)) != 0) {
        cerr << "dup of child stdout failed" << endl;
        return NULL;
    }

    // Spawn child process
    string deflate = "deflate.exe";
    if ((pid = _spawnlp(_P_NOWAIT, deflate.c_str(), deflate.c_str(),
                        "-c", "5", "-s", NULL)) < 0) {
        cerr << "Could not spawn to create compressor process" << endl;
        return NULL;
    }

    // Restore stdin, stdout for parent and close duplicate copies
    if (_dup2(hStdIn, _fileno(stdin)) != 0) {
        cerr << "dup of stdin failed" << endl;
        return NULL;
    }
    if (_dup2(hStdOut, _fileno(stdout)) != 0) {
        cerr << "dup of stdout failed" << endl;
        return NULL;
    }
    close(hStdIn);
    close(hStdOut);

    // Tell the parent that it reads from the opposite end of the
    // place where the child writes.
    close(data[0]);
    FILE *input = fdopen(data[1], "w");
    setbuf(input, 0);
    childpid = pid;
    return input;

#else
    FILE *ret_file = NULL ;

    int pid, data[2];

    if (pipe(data) < 0) {
        cerr << "Could not create IPC channel for compressor process"
        << endl;
        return NULL;
    }

    if ((pid = fork()) < 0) {
        cerr << "Could not fork to create compressor process" << endl;
        return NULL;
    }

    // The parent process closes the write end of the Pipe, and creates a
    // FILE * using fdopen(). The FILE * is used by the calling program to
    // access the read end of the Pipe.

    if (pid > 0) {   // Parent, pid is that of the child
        close(data[0]);
        ret_file = fdopen(data[1], "w");
        setbuf(ret_file, 0);
        childpid = pid;
    }
    else {   // Child
        close(data[1]);
        dup2(data[0], 0); // Read from the pipe...
        dup2(fileno(output), 1); // Write to the FILE *output.

        DBG(cerr << "Opening compression stream." << endl);

        // First try to run deflate using DODS_ROOT (the value read from the
        // DODS_ROOT environment variable takes precedence over the value set
        // at build time. If that fails, try the CWD.
        string deflate = (string)libdap_root() + "/sbin/deflate";
        (void) execl(deflate.c_str(), "deflate", "-c",  "5", "-s", NULL);
        (void) execl("./deflate", "deflate", "-c",  "5", "-s", NULL);
        cerr << "Warning: Could not start compressor!" << endl;
        cerr << "defalte should be in DODS_ROOT/etc or in the CWD!"
        << endl;
        _exit(127);  // Only here if an error occurred.
    }

    return ret_file ;
#endif
}

#endif // COMPRESSION_FOR_SERVER3

#if 0
/** Read stuff from a file and dump it into a string. This assumes the file
    holds character data only. Intended for testing...
    @param fp Read from this file
    @return Returns a string which holds the character data. */
string
file_to_string(FILE *fp)
{
    rewind(fp);
    ostringstream oss;
    char c;
    while (fread(&c, 1, 1, fp))
        oss << c;
    return oss.str();
}

int
wildcmp(const char *wild, const char *string)
{
  // Written by Jack Handy - jakkhandy@hotmail.com

  if (!wild || !string)
      return 0;

  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}
#endif
#if 0
int wmatch(const char *pat, const char *s)
{
    if (!pat || !s)
        return 0;

  switch (*pat) {
    case '\0': return (*s == '\0');
    case '?': return (*s != '\0') && wmatch(pat+1, s+1);
    case '*': return wmatch(pat+1, s) || (*s != '\0' && wmatch(pat, s+1));
    default: return (*s == *pat) && wmatch(pat+1, s+1);
  }
}
#endif

} // namespace libdap

