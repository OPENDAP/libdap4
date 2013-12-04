/*
 * D4CEParserDriver.cc
 *
 *  Created on: Aug 8, 2013
 *      Author: jimg
 */

#include <string>
#include <sstream>
#include <iterator>

#include "D4CEScanner.h"
#include "D4CEDriver.h"
#include "d4_ce_parser.tab.hh"
#include "DMR.h"
#include "BaseType.h"
#include "Array.h"

#include "parser.h"		// for get_ull()

namespace libdap {

bool D4CEDriver::parse(const std::string &expr)
{
	d_expr = expr;	// set for error messages. See the %initial-action section of .yy

	std::istringstream iss(expr);
	D4CEScanner *scanner = new D4CEScanner(iss);

	D4CEParser *parser = new D4CEParser(*scanner, *this /* driver */);

	if (trace_parsing()) {
		parser->set_debug_level(1);
		parser->set_debug_stream(std::cerr);
	}

	return parser->parse() == 0;
}

/**
 * When an identifier is used in a CE, is becomes part of the 'current projection,'
 * which means it is part of the set of variable to be sent back to the client. This
 * method sets a flag in the variable (send_p: send predicate) indicating that.
 * @param id
 * @return The BaseType* to the variable; the send_p flag is set as a side effect.
 */
BaseType *
D4CEDriver::mark_variable(const std::string &id)
{
    BaseType *btp = dmr()->root()->find_var(id);
    if (btp) {
        btp->set_send_p(true);
        return btp;	// the return value is used by the parser logic. Maybe drop this?
    }
    else {
    	throw Error(d_expr + ": The variable " + id + " was not found in the dataset.");
        // return false;
    }
}

/**
 * Add an array to the current projection with slicing. Calling this method will result
 * in the array being returned with anonymous dimensions.
 * @param id
 * @return The BaseType* to the Array variable; the send_p and slicing information is
 * set as a side effect.
 */
BaseType *
D4CEDriver::mark_array_variable(const std::string &id)
{
	BaseType *btp = mark_variable(id);
	if (btp->type() != dods_array_c)
		throw Error("The variable '" + id + "' is not an Array variable.");

	Array *a = static_cast<Array*>(btp);

	// Test that the indexes and dimensions match in number
	if (d_indexes.size() != a->dimensions())
		throw Error("The index constraint for '" + id + "' does not match its rank.");

	Array::Dim_iter d = a->dim_begin();
	for (vector<index>::iterator i = d_indexes.begin(), e = d_indexes.end(); i != e; ++i) {
		if ((*i).stride > (unsigned long long)a->dimension_stop(d, false))
			throw Error("For '" + id + "', the index stride value is greater than the number of elements in the Array");
		if (!(*i).rest && ((*i).stop) > (unsigned long long)a->dimension_stop(d, false))
			throw Error("For '" + id + "', the index stop value is greater than the number of elements in the Array");

		// -1 for a stop value means 'to the end' of the array.
		a->add_constraint(d, (*i).start, (*i).stride, (*i).rest ? -1: (*i).stop);
		++d;
	}

	d_indexes.clear();

	return btp;
}

/**
 * Add an array to the current projection with slicing. Calling this method will result
 * in the array being returned with anonymous dimensions.
 * @param id
 * @return The BaseType* to the Array variable; the send_p and slicing information is
 * set as a side effect.
 */
D4Dimension *
D4CEDriver::slice_dimension(const std::string &id)
{
    D4Dimension *dim = dmr()->root()->find_dim(id);

    // Test that there is only one index slide (Shared Dimensions are always 1D)
    if (d_indexes.size() != 1)
        throw Error("The too many dimensions for the shared dimension '" + id + "'; must be of rank one.");

    index i = *d_indexes.begin();

    if (i.stride > dim->size())
        throw Error("For '" + id + "', the index stride value is greater than the size of the dimension");
    if (!i.rest && (i.stop > dim->size() - 1))
        throw Error("For '" + id + "', the index stop value is greater than the size of the dimension");

    dim->set_constraint(i.start, i.stride, i.rest ? dim->size() - 1: i.stop);

    d_indexes.clear();

    return dim;
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i)
{
	unsigned long long v = get_ull(i.c_str());
	return index(v, 1, v, false);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, const std::string &s, const std::string &e)
{
	return index(get_ull(i.c_str()), get_ull(s.c_str()), get_ull(e.c_str()), false);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, unsigned long long s, const std::string &e)
{
	return index(get_ull(i.c_str()), s, get_ull(e.c_str()), false);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, const std::string &s)
{
	return index(get_ull(i.c_str()), get_ull(s.c_str()), 0, true);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, unsigned long long s)
{
	return index(get_ull(i.c_str()), s, 0, true);
}

// This method is called from the parser (see d4_ce_parser.yy, down in the code
// section). This will be called during the call to D4CEParser::parse(), that
// is inside D4CEDriver::parse(...)
void
D4CEDriver::error(const libdap::location &l, const std::string &m)
{
	std::cerr << l << ": " << m << std::endl;
}

} /* namespace libdap */
