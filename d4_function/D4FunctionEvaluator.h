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

#ifndef D4_FUNCTION_DRIVER_H_
#define D4_FUNCTION_DRIVER_H_

#include <stack>
#include <string>
#include <vector>

namespace libdap {

class location;

class BaseType;
class Array;
class ServerFunctionsList;

class DMR;
class D4Dimension;
class D4RValue;
class D4RValueList;

/**
 * Driver for the DAP4 Functional expression parser.
 */
class D4FunctionEvaluator {
    bool d_trace_scanning = false;
    bool d_trace_parsing = false;
    std::string d_expr;

    DMR *d_dmr = nullptr;
    ServerFunctionsList *d_sf_list = nullptr;

    D4RValueList *d_result = nullptr;

    std::stack<BaseType *> d_basetype_stack;

    unsigned long long d_arg_length_hint = 0;

    // d_expr should be set by parse! Its value is used by the parser right before
    // the actual parsing operation starts. jhrg 11/26/13
    std::string *expression() { return &d_expr; }

    void push_basetype(BaseType *btp) { d_basetype_stack.push(btp); }
    BaseType *top_basetype() const { return d_basetype_stack.empty() ? nullptr : d_basetype_stack.top(); }
    void pop_basetype() { d_basetype_stack.pop(); }

    D4RValue *build_rvalue(const std::string &id);

    friend class D4FunctionParser;

public:
    D4FunctionEvaluator() = default;
    /**
     * @brief Builds a function evaluator.
     * @param dmr Dataset metadata root.
     * @param sf_list Registry of callable server functions.
     */
    D4FunctionEvaluator(DMR *dmr, ServerFunctionsList *sf_list) : d_dmr(dmr), d_sf_list(sf_list) {}

    virtual ~D4FunctionEvaluator() = default;

    /**
     * @brief Parses a DAP4 function-expression string.
     * @param expr Function expression text.
     * @return True when parsing succeeds.
     */
    bool parse(const std::string &expr);

    /** @brief Returns whether scanner tracing is enabled. */
    bool trace_scanning() const { return d_trace_scanning; }
    /** @brief Enables or disables scanner tracing.
     * @param ts True to enable scanner tracing.
     */
    void set_trace_scanning(bool ts) { d_trace_scanning = ts; }

    /** @brief Returns whether parser tracing is enabled. */
    bool trace_parsing() const { return d_trace_parsing; }
    /** @brief Enables or disables parser tracing.
     * @param tp True to enable parser tracing.
     */
    void set_trace_parsing(bool tp) { d_trace_parsing = tp; }

    /** Get the result of parsing the function(s)
     *
     * @return The result(s) packages in a D4RValueList
     */
    D4RValueList *result() const { return d_result; }
    /**
     * @brief Sets the parsed result list.
     * @param rv_list Result list ownership pointer.
     */
    void set_result(D4RValueList *rv_list) { d_result = rv_list; }

    /**
     * @brief Evaluates the parsed function expression against a DMR.
     * @param dmr Destination or context DMR for function results.
     * @param function_result Alias name used by some implementations for the destination DMR.
     */
    void eval(DMR *dmr);

    /** @brief Returns the current argument-length hint used for list constants. */
    unsigned long long get_arg_length_hint() const { return d_arg_length_hint; }
    /** @brief Sets the argument-length hint used for list constants.
     * @param alh Hint value.
     */
    void set_arg_length_hint(unsigned long long alh) { d_arg_length_hint = alh; }

    /** @brief Returns the bound DMR pointer. */
    DMR *dmr() const { return d_dmr; }
    /** @brief Binds this evaluator to a DMR.
     * @param dmr Dataset metadata root.
     */
    void set_dmr(DMR *dmr) { d_dmr = dmr; }

    /** @brief Returns the server-function registry used for lookup. */
    ServerFunctionsList *sf_list() const { return d_sf_list; }
    /** @brief Sets the server-function registry used for lookup.
     * @param sf_list Function registry.
     */
    void set_sf_list(ServerFunctionsList *sf_list) { d_sf_list = sf_list; }

    /**
     * @brief Initializes an argument vector with one seed value.
     * @tparam t Argument element type.
     * @param val First value inserted in the returned vector.
     * @return Newly allocated argument vector.
     */
    template <typename t> std::vector<t> *init_arg_list(t val);

    /**
     * @brief Throws a parse/evaluation error with source location context.
     * @param l Parser/scanner location of the error.
     * @param m Error message.
     */
    [[noreturn]] static void error(const libdap::location &l, const std::string &m);
};

} /* namespace libdap */
#endif /* D4_FUNCTION_DRIVER_H_ */
