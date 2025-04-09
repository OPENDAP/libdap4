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

#include <cerrno>
#include <cstdlib>

#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <sstream>
#include <string>

#include "D4FunctionEvaluator.h"
#include "D4FunctionScanner.h"
#include "d4_function_parser.tab.hh"

#include "Array.h"
#include "D4Enum.h"
#include "D4Group.h"
#include "DMR.h"

#include "debug.h"
#include "escaping.h"
#include "util.h"

namespace libdap {

/**
 * Parse the DAP4 function expression.
 *
 * Calling this method with a DAP4 function expression builds a
 * D4RvalueList that can then be evaluated. The list of rvalues
 * can be accessed or evaluated (using the result() or eval()
 * methods). Note that the result is a list of rvalues because the
 * input can be zero or more function expressions.
 *
 * @param expr The function expression.
 * @return True if the parse succeeded, false otherwise.
 */
bool D4FunctionEvaluator::parse(const std::string &expr) {
    d_expr = expr; // set for error messages. See the %initial-action section of .yy

    std::istringstream iss(expr);
    D4FunctionScanner scanner(iss);
    D4FunctionParser parser(scanner, *this /* driver */);

    if (trace_parsing()) {
        parser.set_debug_level(1);
        parser.set_debug_stream(std::cerr);
    }

    return parser.parse() == 0;
}

/**
 * Evaluate the recently parsed function expression and put the resulting
 * rvalues (which return values packaged in libdap BaseType objects) into
 * the top-level Group of the DMR passed as a param here.
 *
 * @note The DMR passed to this method can (and usually will) be different
 * from the DMR passed to the D4FunctionEvaluator constructor. That DMR is
 * the 'source dataset' for values of variables used by the functions. It
 * is not generally where the results of evaluating the functions wind up.
 * Usually, you'll want those results in their own DMR; the functions
 * effectively make a new dataset. However, you _can_ pass the source dataset
 * DMR into this method and in that case the new variables will be appended
 * to its root Group.
 *
 * @note This code is really just a place to package results. The actual
 * evaluation happens in the D4RValue object when values are accessed.
 * The parse() method here builds the list of D4RValue objects and this
 * code accesses the values which triggers the function evaluation. Thus,
 * if you'd like to build a system that performs lazy evaluation, you can
 * work with the result() method from this class instead of this one.
 *
 * @note Calling this method will delete the D4RValueList object built
 * by the parse() method.
 *
 * @param dmr Store the results here
 * @exception Throws Error if the evaluation fails.
 */
void D4FunctionEvaluator::eval(DMR *function_result) {
    if (!d_result)
        throw InternalErr(__FILE__, __LINE__, "Must parse() the function expression before calling eval()");

    D4Group *root = function_result->root(); // Load everything in the root group
    for (auto result : *d_result) {
        // Copy the BaseTypes; this means all the function results can
        // be deleted, which addresses the memory leak issue with function
        // results. This should also copy the D4Dimensions. jhrg 3/17/14
        root->add_var(result->value(*d_dmr));
    }

    delete d_result; // The parser/function allocates the BaseType*s that hold the results.
    d_result = nullptr;

    // Variables can use Dimensions and Enumerations, so those need to be copied
    // from the source dataset to the result. NB: The variables that refer to these
    // use weak pointers.

    // Make a set of D4Dimensions. For each variable in 'function_result', look
    // for its dimensions in 'dataset' (by name) and add a pointer to those to the
    // set. Then copy all the stuff in the set into the root group of 'function_
    // result.'
    list<D4Dimension *> dim_set;

    for (auto i = root->var_begin(), ie = root->var_end(); i != ie; ++i) {
        if ((*i)->is_vector_type()) {
            auto a = static_cast<Array *>(*i);
            for (auto d = a->dim_begin(), de = a->dim_end(); d != de; ++d) {
                // Only add Dimensions that are not already present; share dims are not repeated. jhrg 2/7/18
                D4Dimension *d4_dim = a->dimension_D4dim(d);
                if (d4_dim) {
                    bool found = (std::find(dim_set.begin(), dim_set.end(), d4_dim) != dim_set.end());
                    if (!found)
                        dim_set.push_back(a->dimension_D4dim(d));
                }
            }
        }
    }

    // Copy the D4Dimensions and EnumDefs because this all goes in a new DMR - we don't
    // want to share those across DMRs because the DMRs delete those (so sharing htem
    // across DMRs would lead to dangling pointers.
    for (auto dim : dim_set) {
        root->dims()->add_dim(dim);
    }

    // Now let's do the enumerations....
    list<D4EnumDef *> enum_def_set;
    for (auto i = root->var_begin(), ie = root->var_end(); i != ie; ++i) {
        if ((*i)->type() == dods_enum_c) {
            enum_def_set.push_back(static_cast<D4Enum *>(*i)->enumeration());
        }
    }

    for (auto enum_def : enum_def_set) {
        root->enum_defs()->add_enum(enum_def);
    }
}

// libdap contains functions (in parser-util.cc) that test if a string
// can be converted to an int32, e.g., but I used a more streamlined
// approach here. 3/13/14 jhrg
/**
 * Build and return a new RValue. Allocates the new D4RValue object.
 * The code tries first to find the id in the DMR - that is, it checks
 * first to see if it is a variable in the current dataset. If that
 * fails it will try to build an unsigned long long, a long long or
 * a double from the string (in that order). If that fails the code
 * converts the id into a string.
 *
 * @note Constants are returned as the libdap4 types for int64_t,
 * uint64_t, double, or string (dods_int64_c, ...)
 *
 * @param id An identifier (really a string) parsed from the function
 * expression. May contain quotes.
 * @return Return a pointer to the new allocated D4RValue object.
 */
D4RValue *D4FunctionEvaluator::build_rvalue(const std::string &id) {
    BaseType *btp = nullptr;

    // Look for the id in the dataset first
    if (top_basetype()) {
        btp = top_basetype()->var(id);
    } else {
        btp = dmr()->root()->find_var(id);
    }

    if (btp)
        return new D4RValue(btp);

    // If the id is not a variable, try to turn it into a constant,
    // otherwise, it's an error.
    char *end_ptr = nullptr;

    errno = 0;
    long long ll_val = strtoll(id.c_str(), &end_ptr, 0);
    if (*end_ptr == '\0' && errno == 0)
        return new D4RValue(ll_val);

    // Test for unsigned after signed since strtoull() accepts a minus sign
    // (and will return a huge number if that's the case). jhrg 3/13/14
    errno = 0;
    unsigned long long ull_val = strtoull(id.c_str(), &end_ptr, 0);
    if (*end_ptr == '\0' && errno == 0)
        return new D4RValue(ull_val);

    errno = 0;
    double d_val = strtod(id.c_str(), &end_ptr);
    if (*end_ptr == '\0' && errno == 0)
        return new D4RValue(d_val);

    // To be a valid string, the id must be quoted (using double quotes)
    if (is_quoted(id))
        return new D4RValue(www2id(id));

    // if it's none of these, return null
    return nullptr;
}

// TODO Make the arg_list a unique_ptr through out. jhrg 2/22/24
template <typename T> vector<T> *D4FunctionEvaluator::init_arg_list(T val) {
    auto arg_list = make_unique<vector<T>>();
    if (get_arg_length_hint() > 0)
        arg_list->reserve(get_arg_length_hint());

    arg_list->push_back(val);

    return arg_list.release();
}

// Force an instantiation so this can be called from within the d4_function.yy
// parser.
template std::vector<dods_byte> *D4FunctionEvaluator::init_arg_list(dods_byte val);
template std::vector<dods_int8> *D4FunctionEvaluator::init_arg_list(dods_int8 val);
template std::vector<dods_uint16> *D4FunctionEvaluator::init_arg_list(dods_uint16 val);
template std::vector<dods_int16> *D4FunctionEvaluator::init_arg_list(dods_int16 val);
template std::vector<dods_uint32> *D4FunctionEvaluator::init_arg_list(dods_uint32 val);
template std::vector<dods_int32> *D4FunctionEvaluator::init_arg_list(dods_int32 val);
template std::vector<dods_uint64> *D4FunctionEvaluator::init_arg_list(dods_uint64 val);
template std::vector<dods_int64> *D4FunctionEvaluator::init_arg_list(dods_int64 val);
template std::vector<dods_float32> *D4FunctionEvaluator::init_arg_list(dods_float32 val);
template std::vector<dods_float64> *D4FunctionEvaluator::init_arg_list(dods_float64 val);

// This method is called from the parser (see d4_function_parser.yy, down in the code
// section). This will be called during the call to D4FunctionParser::parse(), that
// is inside D4FunctionEvaluator::parse(...)
[[noreturn]] void D4FunctionEvaluator::error(const libdap::location &l, const std::string &m) {
    ostringstream oss;
    oss << l << ": " << m << ends;
    throw Error(malformed_expr, oss.str());
}

} /* namespace libdap */
