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

#include <cstdlib>
#include <cerrno>

#include <string>
#include <sstream>
#include <iterator>

//#define DODS_DEBUG

#include "D4FunctionScanner.h"
#include "D4FunctionDriver.h"
#include "d4_function_parser.tab.hh"
#include "DMR.h"
#include "D4RValue.h"

#include "BaseType.h"

#include "escaping.h"
#include "util.h"
#include "debug.h"

namespace libdap {

bool D4FunctionDriver::parse(const std::string &expr)
{
	d_expr = expr;	// set for error messages. See the %initial-action section of .yy

	std::istringstream iss(expr);
	D4FunctionScanner *scanner = new D4FunctionScanner(iss);

	D4FunctionParser *parser = new D4FunctionParser(*scanner, *this /* driver */);

	if (trace_parsing()) {
		parser->set_debug_level(1);
		parser->set_debug_stream(std::cerr);
	}

	return parser->parse() == 0;
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
 * @param id An identifier (really a string) parsed from the function
 * expression. May contain quotes.
 * @return Return a pointer to the new allocated D4RValue object.
 */
D4RValue *
D4FunctionDriver::build_rvalue(const std::string &id)
{
    BaseType *btp = 0;

    // Look for the id in the dataset first
    if (top_basetype()) {
        btp = top_basetype()->var(id);
    }
    else {
        btp = dmr()->root()->find_var(id);
    }

    if (btp)
        return new D4RValue(btp);

    // If the id is not a variable, try to turn it into a constant,
    // otherwise, its an error.
    char *end_ptr = 0;

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
    return 0;
}

#if 0
void
D4FunctionDriver::throw_not_found(const string &id, const string &ident)
{
    throw Error(d_expr + ": The variable " + id + " was not found in the dataset (" + ident + ").");
}

void
D4FunctionDriver::throw_not_array(const string &id, const string &ident)
{
	throw Error(d_expr + ": The variable '" + id + "' is not an Array variable (" + ident + ").");
}
#endif
// This method is called from the parser (see d4_ce_parser.yy, down in the code
// section). This will be called during the call to D4FunctionParser::parse(), that
// is inside D4FunctionDriver::parse(...)
void
D4FunctionDriver::error(const libdap::location &l, const std::string &m)
{
	std::cerr << l << ": " << m << std::endl;
}

} /* namespace libdap */
