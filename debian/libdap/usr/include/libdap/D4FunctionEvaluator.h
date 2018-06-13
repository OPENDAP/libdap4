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

#include <string>
#include <vector>
#include <stack>

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
class D4FunctionEvaluator
{
    bool d_trace_scanning;
    bool d_trace_parsing;
    std::string d_expr;

    DMR *d_dmr;
    ServerFunctionsList *d_sf_list;

    D4RValueList *d_result;

    std::stack<BaseType*> d_basetype_stack;

    unsigned long long d_arg_length_hint;

    // d_expr should be set by parse! Its value is used by the parser right before
    // the actual parsing operation starts. jhrg 11/26/13
    std::string *expression()
    {
        return &d_expr;
    }

    void push_basetype(BaseType *btp)
    {
        d_basetype_stack.push(btp);
    }
    BaseType *top_basetype() const
    {
        return d_basetype_stack.empty() ? 0 : d_basetype_stack.top();
    }
    void pop_basetype()
    {
        d_basetype_stack.pop();
    }

    D4RValue *build_rvalue(const std::string &id);

    friend class D4FunctionParser;

public:
    D4FunctionEvaluator() :
            d_trace_scanning(false), d_trace_parsing(false), d_expr(""), d_dmr(0), d_sf_list(0), d_result(0), d_arg_length_hint(
                    0)
    {
    }
    D4FunctionEvaluator(DMR *dmr, ServerFunctionsList *sf_list) :
            d_trace_scanning(false), d_trace_parsing(false), d_expr(""), d_dmr(dmr), d_sf_list(sf_list), d_result(0), d_arg_length_hint(
                    0)
    {
    }

    virtual ~D4FunctionEvaluator()
    {
    }

    bool parse(const std::string &expr);

    bool trace_scanning() const
    {
        return d_trace_scanning;
    }
    void set_trace_scanning(bool ts)
    {
        d_trace_scanning = ts;
    }

    bool trace_parsing() const
    {
        return d_trace_parsing;
    }
    void set_trace_parsing(bool tp)
    {
        d_trace_parsing = tp;
    }

    /** Get the result of parsing the function(s)
     *
     * @return The result(s) packages in a D4RValueList
     */
    D4RValueList *result() const
    {
        return d_result;
    }
    void set_result(D4RValueList *rv_list)
    {
        d_result = rv_list;
    }

    void eval(DMR *dmr);

    unsigned long long get_arg_length_hint() const
    {
        return d_arg_length_hint;
    }
    void set_arg_length_hint(unsigned long long alh)
    {
        d_arg_length_hint = alh;
    }

    DMR *dmr() const
    {
        return d_dmr;
    }
    void set_dmr(DMR *dmr)
    {
        d_dmr = dmr;
    }

    ServerFunctionsList *sf_list() const
    {
        return d_sf_list;
    }
    void set_sf_list(ServerFunctionsList *sf_list)
    {
        d_sf_list = sf_list;
    }

    template<typename t> std::vector<t> *init_arg_list(t val);

    void error(const libdap::location &l, const std::string &m);
};

} /* namespace libdap */
#endif /* D4_FUNCTION_DRIVER_H_ */
