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
#include "D4Dimensions.h"
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

#if 0
void
D4CEDriver::set_array_slices(const std::string &id, Array *a)
{
    // Test that the indexes and dimensions match in number
    if (d_indexes.size() != a->dimensions())
        throw Error("The index constraint for '" + id + "' does not match its rank.");

    Array::Dim_iter d = a->dim_begin();
    for (vector<index>::iterator i = d_indexes.begin(), e = d_indexes.end(); i != e; ++i) {
        if ((*i).stride > (unsigned long long)a->dimension_stop(d, false))
            throw Error("For '" + id + "', the index stride value is greater than the number of elements in the Array");
        if (!(*i).rest && ((*i).stop) > (unsigned long long)a->dimension_stop(d, false))
            throw Error("For '" + id + "', the index stop value is greater than the number of elements in the Array");

        D4Dimension *dim = a->dimension_D4dim(d);

        // In a DAP4 CE, specifying '[]' as an array dimension slice has two meanings.
        // It can mean 'all the elements' of the dimension or 'apply the slicing inherited
        // from the shared dimension'. The latter might be provide 'all the elements'
        // but regardless, the Array object must record the CE correctly.

        if (dim && (*i).empty) {
            a->add_constraint(d, dim);
        }
        else {
            a->add_constraint(d, (*i).start, (*i).stride, (*i).rest ? -1 : (*i).stop);
        }

        ++d;
    }

    d_indexes.clear();
}
#endif

/**
 * When an identifier is used in a CE, is becomes part of the 'current projection,'
 * which means it is part of the set of variable to be sent back to the client. This
 * method sets a flag in the variable (send_p: send predicate) indicating that.
 *
 * @note This will check if the variable is an array and set it's slices accordingly
 * @param id
 * @return The BaseType* to the variable; the send_p flag is set as a side effect.
 */
BaseType *
D4CEDriver::mark_variable(BaseType *btp)
{
    if (btp) {
        btp->set_send_p(true);

        // Now set the parent variables
        BaseType *parent = btp->get_parent();
		while (parent) {
			parent->BaseType::set_send_p(true); // Just set the parent
			parent = parent->get_parent();
        }
        return btp;
    }
    else {
    	throw Error(d_expr + ": The variable " + btp->name() + " was not found in the dataset.");
    }
}

/**
 * Add an array to the current projection with slicing. Calling this method will result
 * in the array being returned with anonymous dimensions.
 *
 * @note If id is an array that has shared dimensions and uses '[]' where a shared dimension
 * is found and if that shared dimension has been sliced, then the slice is used as the array's
 * slice for that dimension (there must be an easier way to explain that...)
 *
 * @param id
 * @return The BaseType* to the Array variable; the send_p and slicing information is
 * set as a side effect.
 */
BaseType *
D4CEDriver::mark_array_variable(BaseType *btp)
{
    mark_variable(btp);

	if (btp->type() != dods_array_c)
		throw Error("The variable '" + btp->name() + "' is not an Array variable.");

	Array *a = static_cast<Array*>(btp);

	// If an array appears in a CE without the slicing operators ([]) we still have to
	// call set_user_by_projected_var(true) for all of it's sdims for them to appear in
	// the CDMR.
	if (d_indexes.empty()) {
	    for (Array::Dim_iter d = a->dim_begin(), de = a->dim_end(); d != de; ++d) {
	        D4Dimension *dim = a->dimension_D4dim(d);
	        if (dim) {
	            dim->set_used_by_projected_var(true);
	        }
	    }
	}
    else {
        // Test that the indexes and dimensions match in number
        if (d_indexes.size() != a->dimensions())
            throw Error("The index constraint for '" + btp->name() + "' does not match its rank.");

        Array::Dim_iter d = a->dim_begin();
        for (vector<index>::iterator i = d_indexes.begin(), e = d_indexes.end(); i != e; ++i) {
            if ((*i).stride > (unsigned long long) a->dimension_stop(d, false))
                throw Error(
                        "For '" + btp->name()
                                + "', the index stride value is greater than the number of elements in the Array");
            if (!(*i).rest && ((*i).stop) > (unsigned long long) a->dimension_stop(d, false))
                throw Error(
                        "For '" + btp->name()
                                + "', the index stop value is greater than the number of elements in the Array");

            D4Dimension *dim = a->dimension_D4dim(d);

            // In a DAP4 CE, specifying '[]' as an array dimension slice has two meanings.
            // It can mean 'all the elements' of the dimension or 'apply the slicing inherited
            // from the shared dimension'. The latter might be provide 'all the elements'
            // but regardless, the Array object must record the CE correctly.

            if (dim && (*i).empty) {
                a->add_constraint(d, dim);  // calls set_used_by_projected_var(true) + more
            }
            else {
                a->add_constraint(d, (*i).start, (*i).stride, (*i).rest ? -1 : (*i).stop);
            }

            ++d;
        }

        d_indexes.clear();
    }

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
D4CEDriver::slice_dimension(const std::string &id, const index &i)
{
    D4Dimension *dim = dmr()->root()->find_dim(id);

    if (i.stride > dim->size())
        throw Error("For '" + id + "', the index stride value is greater than the size of the dimension");
    if (!i.rest && (i.stop > dim->size() - 1))
        throw Error("For '" + id + "', the index stop value is greater than the size of the dimension");

    dim->set_constraint(i.start, i.stride, i.rest ? dim->size() - 1: i.stop);

    return dim;
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i)
{
	unsigned long long v = get_ull(i.c_str());
	return index(v, 1, v, false, false /*empty*/);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, const std::string &s, const std::string &e)
{
	return index(get_ull(i.c_str()), get_ull(s.c_str()), get_ull(e.c_str()), false, false /*empty*/);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, unsigned long long s, const std::string &e)
{
	return index(get_ull(i.c_str()), s, get_ull(e.c_str()), false, false /*empty*/);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, const std::string &s)
{
	return index(get_ull(i.c_str()), get_ull(s.c_str()), 0, true, false /*empty*/);
}

D4CEDriver::index
D4CEDriver::make_index(const std::string &i, unsigned long long s)
{
	return index(get_ull(i.c_str()), s, 0, true, false /*empty*/);
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
