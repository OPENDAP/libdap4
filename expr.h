
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

// Types for the expr parser.
//
// 11/4/95 jhrg

#ifndef _expr_h
#define _expr_h

#include <string>
#include <vector>

#include "Type.h"
#include "parser-util.h"
#include "Error.h"

namespace libdap
{

class BaseType;
class DDS;
class ConstraintEvaluator;

// VALUE is used to return constant values from the scanner to the parser.
// Constants are packaged in BaseType *s for evaluation by the parser.
//
// I changed this to simplify support of natural axes subsetting. Since 'value'
// was only being used for the string* return, I added a string* filed to ce_exprlval
// and moved 'value' out of it. This meant that I can use non-trivial ctors for
// value without breaking the rules for c++ unions. Those ctors make it easier to
// build value instances without errors. jhrg 2/10/20

typedef struct value {
    bool is_range_value;    // true if this is part of a natural axes projection
    Type type;   // Type is an enum defined in Type.h
    union {
        unsigned int ui;
        int i;
        double f;
        std::string *s;
    } v;

    /**
     * @brief build an instance by divining the type.
     * @param token
     */
    void build_instance(const std::string &token) {
        if (check_int32(token.c_str(), v.i)) {
            type = dods_int32_c;
#if 0
            v.i = atoi(token.c_str());
#endif
        }
        else if (check_uint32(token.c_str(), v.ui)) {
            type = dods_uint32_c;
#if 0
            v.ui = atoi(token.c_str());
#endif
        }
        else if (check_float64(token.c_str(), v.f)) {
            type = dods_float64_c;
#if 0
            v.f = atof(token.c_str());
#endif
        }
        else {
            type = dods_str_c;
            v.s = new std::string(token);
        }
    }

    /**
     * @brief Build an instance given the type
     * @param token
     */
    void build_typed_instance(const std::string &token) {
        switch (type) {
            case dods_uint32_c:
                v.ui = get_uint32(token.c_str());
                break;
            case dods_int32_c:
                v.i = get_int32(token.c_str());
                break;
            case dods_float64_c:
                v.ui = get_float64(token.c_str());
                break;
            case dods_str_c:
                v.s = new std::string(token);
                break;
            default:
                throw Error("Expected an int32, unsigned int32, float64 or string token.");
        }
    }

    // By default, value instances are int32s with values of 0 and the range_value
    // property is set to false.
    value() : is_range_value(false), type(dods_int32_c) { v.i = 0; }

    // set a value
    value(const std::string &token, bool rv = false, Type t = dods_null_c) : is_range_value(rv),  type(t) {
        if (type == dods_null_c)
            build_instance(token);
        else
            build_typed_instance(token);
    }

    value(int val, bool rv = false, Type t = dods_null_c) : is_range_value(rv),  type(t) {
        switch (type) {
            case dods_uint32_c:
                v.ui = val;
                break;
            case dods_int32_c:
                v.i = val;
                break;
            default:
                throw Error("Expected an int32 or unsigned int32 token.");
        }
    }

    value(unsigned int val, bool rv = false, Type t = dods_null_c) : is_range_value(rv),  type(t) {
        switch (type) {
            case dods_uint32_c:
                v.ui = val;
                break;
            default:
                throw Error("Expected an unsigned int32 token.");
        }
    }

} value;

// Syntactic sugar for `pointer to function returning boolean' (bool_func)
// and `pointer to function returning BaseType *' (btp_func). Both function
// types take four arguments, an integer (argc), a vector of BaseType *s
// (argv), the DDS for the dataset for which these function is being
// evaluated (analogous to the ENVP in UNIX) and a pointer for the function's
// return value. ARGC is the length of ARGV.

typedef void (*bool_func)(int argc, BaseType *argv[], DDS &dds, bool *result);
typedef void (*btp_func)(int argc, BaseType *argv[], DDS &dds, BaseType **btpp);

// Projection function: A function that appears in the projection part of the
// CE and is executed for its side-effect. Usually adds a new variable to
// the DDS. These are run _during the parse_ so their side-effects can be used
// by subsequent parts of the CE.

typedef void (*proj_func)(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce);

// INT_LIST and INT_LIST_LIST are used by the parser to store the array
// indexes.

// To add the new feature of 'to the end' in an array projection (denoted using
// star), I used the value -1 for an index. This makes do difference here. jhrg
// 12/20/12

// By using 'value' and not integers, the slices can use floats which is a better fit
// for lat and lon values. jhrg 4/18/19
// I used a vector of pointers to dim_slice because pointers have trivial copy ctors and
// these types are using by the CE parsers in a union. Unions in C++ require trivial copy
// ctors. jhrg 2/5/20
typedef std::vector<value> dim_slice;
typedef std::vector<dim_slice *> slices;

} // namespace libdap

#endif /* _expr_h */
