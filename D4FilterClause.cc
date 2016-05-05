
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

// (c) COPYRIGHT URI/MIT 1996,1998,1999
// Please first read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
// jhrg,jimg James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the CE Clause class.


#include "config.h"

#include "D4RValue.h"
#include "D4FilterClause.h"

using namespace std;

namespace libdap {

void
D4FilterClauseList::m_duplicate(const D4FilterClauseList &src)
{
    //D4FilterClauseList &non_c_src = const_cast<D4FilterClauseList &>(src);

    for (D4FilterClauseList::citer i = src.cbegin(), e = src.cend(); i != e; ++i) {
        D4FilterClause *fc = *i;
        d_clauses.push_back(new D4FilterClause(*fc));
    }
}

D4FilterClauseList::~D4FilterClauseList()
{
    for (D4FilterClauseList::iter i = d_clauses.begin(), e = d_clauses.end(); i != e; ++i) {
        delete *i;
    }
}

/**
 * @brief Evaluate the list of clauses
 *
 * Evaluate the list of clauses and return false when/if one is found to be false.
 * This evaluates the clauses in the order they are stored and stops evaluation a
 * the first false clause.
 *
 * @param dmr Use this DMR when evaluating clauses - for clauses that contain functions,
 * not currently in the DAP4 specification.
 * @return True if each of the clauses' value is true, otherwise false
 */
bool
D4FilterClauseList::value(DMR &dmr)
{
    for (D4FilterClauseList::iter i = d_clauses.begin(), e = d_clauses.end(); i != e; ++i) {
        if ((*i)->value(dmr) == false)
            return false;
    }

    return true;
}

/**
 * @brief Evaluate the list of clauses
 *
 * This version of value() does not need a DMR parameter (but will not work
 * if the clauses contain a function call (which is not currently supported
 * by the spec).
 *
 * @return True if each clauses' value is true, false otherwise
 * @see D4FilterClauseList::value(DMR &dmr)
 */
bool
D4FilterClauseList::value()
{
    for (D4FilterClauseList::iter i = d_clauses.begin(), e = d_clauses.end(); i != e; ++i) {
        if ((*i)->value() == false)
            return false;
    }

    return true;
}

void D4FilterClause::m_duplicate(const D4FilterClause &rhs) {
    d_op = rhs.d_op;

    d_arg1 = new D4RValue(*rhs.d_arg1);
    d_arg2 = new D4RValue(*rhs.d_arg2);

#if 0
    // Copy the D4RValue pointer if the 'value_kind' is a basetype,
    // but build a new D4RValue if it is a constant (because the
    // basetype is a weak pointer.
    switch (rhs.d_arg1->get_kind()) {
    case D4RValue::basetype:
        d_arg1 = rhs.d_arg1;
        break;
    case D4RValue::constant:
        d_arg1 = new D4RValue(*(rhs.d_arg1));
        break;
    default:
        throw Error(malformed_expr, "found a filter clause with a function call.");
    }

    switch (rhs.d_arg2->get_kind()) {
    case D4RValue::basetype:
        d_arg2 = rhs.d_arg2;
        break;
    case D4RValue::constant:
        d_arg2 = new D4RValue(*(rhs.d_arg2));
        break;
    default:
        throw Error(malformed_expr, "found a filter clause with a function call.");
    }
#endif
}

/**
 * @brief Get the value of this relational expression.
 * This version of value() works for function clauses, although that's
 * not supported by the syntax at this time.
 * @param dmr The DMR to use when evaluating a function
 * @return True if the clause is true, false otherwise.
 */
bool D4FilterClause::value(DMR &dmr)
{
	switch (d_op) {
	case null:
		throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Found a null operator");

	case less:
	case greater:
	case less_equal:
	case greater_equal:
	case equal:
	case not_equal:
	case match:
		return cmp(d_op, d_arg1->value(dmr), d_arg2->value(dmr));

	case ND:
	case map:
		throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Filter operator not implemented");

	default:
		throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Unrecognized operator");
	}
}

/**
 * @brief Get the value of this relational expression.
 * This version of value() will not work for clauses where one of the
 * rvalues is a function call. This is not currently supported by the
 * DAP4 specification, so it's probably no great loss.
 * @return True if the clause is true, false otherwise.
 */
bool D4FilterClause::value()
{
    switch (d_op) {
    case null:
        throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Found a null operator");

    case less:
    case greater:
    case less_equal:
    case greater_equal:
    case equal:
    case not_equal:
    case match:
        return cmp(d_op, d_arg1->value(), d_arg2->value());

    case ND:
    case map:
        throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Filter operator not implemented");

    default:
        throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Unrecognized operator");
    }
}

// It may be better to use the code in the Byte, ..., classes that was
// impl'd for DAP2 (with extensions). For now, test this and build the
// rest of the filter implementation. But there is certainly a more _compact_
// way to code this!
//
// Optimize the extraction of constant values.
bool D4FilterClause::cmp(ops op, BaseType *arg1, BaseType *arg2)
{
    return arg1->d4_ops(arg2, op);
}

} // namespace libdap
