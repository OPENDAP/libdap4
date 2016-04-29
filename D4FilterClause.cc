
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

#define DODS_DEBUG

#include <cassert>

#include <string>

#include "BaseType.h"
#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Int64.h"
#include "UInt64.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"

#include "GNURegex.h"

#include "D4RValue.h"
#include "D4FilterClause.h"

#include "debug.h"

using namespace std;

namespace libdap {

void
D4FilterClauseList::m_duplicate(const D4FilterClauseList &src)
{
    D4FilterClauseList &non_c_src = const_cast<D4FilterClauseList &>(src);

    for (D4FilterClauseList::iter i = non_c_src.begin(), e = non_c_src.end(); i != e; ++i) {
        d_clauses.push_back(new D4FilterClause(**i));
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

#if 0
template<typename T1, typename T2> inline bool D4FilterClause::cmp_impl(ops op, T1 arg1, T2 arg2)
{
    DBG(cerr << "arg1: " << arg1 << ", arg2: " << arg2 << endl);
	switch (op) {
	case null:
		assert(false && "Found a null operator");
		break;
	case less:
		return arg1 < arg2;
	case greater:
		return arg1 > arg2;
	case less_equal:
		return arg1 <= arg2;
	case greater_equal:
		return arg1 >= arg2;
	case equal:
		return arg1 == arg2;
	case not_equal:
		return arg1 != arg2;
	case match:
	case ND:
	case map:
		throw Error("While evaluating a constraint filter clause: Operator not valid for a number");
	default:
		throw Error("While evaluating a constraint filter clause: Unrecognized operator");
	}

	return false;
}

// special case strings
inline bool D4FilterClause::cmp_impl(ops op, const string &arg1, const string &arg2)
{
	switch (op) {
	case null:
		assert(false && "Found a null operator");
		break;
	case less:
		return arg1 < arg2;
	case greater:
		return arg1 > arg2;
	case less_equal:
		return arg1 <= arg2;
	case greater_equal:
		return arg1 >= arg2;
	case equal:
		return arg1 == arg2;
	case not_equal:
		return arg1 != arg2;
	case match: {
		Regex r(arg2.c_str());
		return r.match(arg1.c_str(), arg1.length(), 0) > 0;
	}

	case ND:
	case map:
		throw Error("While evaluating a constraint filter clause: Operator not valid for a string");
	default:
		throw Error("While evaluating a constraint filter clause: Unrecognized operator");
	}

	return false;
}
#endif

void D4FilterClause::m_duplicate(const D4FilterClause &rhs) {
    d_op = rhs.d_op;

    // Copy the D4RValue pointer if the 'value_kind' is a basetype,
    // but build a new D4RValue if it is a constant (because the
    // basetype is a weak pointer.
    switch (rhs.d_arg1->get_kind()) {
    case D4RValue::basetype:
        d_arg1 = rhs.d_arg1;
        break;
    case D4RValue::constant:
        d_arg1 = new D4RValue(*rhs.d_arg1);
        break;
    default:
        throw Error(malformed_expr, "found a filter clause with a function call.");
    }

    switch (rhs.d_arg2->get_kind()) {
    case D4RValue::basetype:
        d_arg2 = rhs.d_arg2;
        break;
    case D4RValue::constant:
        d_arg2 = new D4RValue(*rhs.d_arg2);
        break;
    default:
        throw Error(malformed_expr, "found a filter clause with a function call.");
    }
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

#if 0
bool D4FilterClause::cmp(ops op, BaseType *arg1, BaseType *arg2)
{
	switch (arg1->type()) {
	case dods_byte_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
		    throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_int8_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<Int8*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_int16_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<Int16*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_uint16_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<UInt16*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_int32_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<Int32*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_uint32_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<UInt32*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_int64_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<Int64*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_uint64_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<UInt64*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_float32_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<Float32*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;
	case dods_float64_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp_impl(op, static_cast<Float64*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
        case dods_str_c:
        case dods_url_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, string).");
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (number, non-scalar).");
		}
		break;

    case dods_str_c:
    case dods_url_c:
        switch (arg2->type()) {
        case dods_byte_c:
        case dods_int8_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_int64_c:
        case dods_uint64_c:
        case dods_float32_c:
        case dods_float64_c:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (string, number).");
        case dods_str_c:
        case dods_url_c:
            // We can do this because URL/Url is-a Str.
            return cmp_impl(op, static_cast<Str*>(arg1)->value(), static_cast<Str*>(arg2)->value());
        default:
            throw Error(malformed_expr, "Relational operators can only compare compatible types (string, non-scalar).");
        }
        break;

    default:
        throw Error(malformed_expr, "Relational operators only work with scalar types.");
    }   // switch (arg1...)
}
#endif

} // namespace libdap
