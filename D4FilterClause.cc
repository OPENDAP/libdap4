
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

#include "D4FilterClause.h"

using namespace std;

namespace libdap {

template<typename T1, typename T2> inline bool D4FilterClause::cmp(ops op, T1 arg1, T2 arg2)
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
inline bool D4FilterClause::cmp(ops op, const string &arg1, const string &arg2)
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
		return r.match(arg1.c_str(), arg1.length()) > 0;
	}

	case ND:
	case map:
		throw Error("While evaluating a constraint filter clause: Operator not valid for a string");
	default:
		throw Error("While evaluating a constraint filter clause: Unrecognized operator");
	}

	return false;
}

template<typename T> inline bool D4FilterClause::cmp(ops op, BaseType *arg1, T arg2)
{
	switch (arg1->type()) {
	case dods_byte_c:
		return cmp(op, static_cast<Byte*>(arg1)->value(), arg2);
	case dods_int8_c:
		return cmp(op, static_cast<Int8*>(arg1)->value(), arg2);
	case dods_int16_c:
		return cmp(op, static_cast<Int16*>(arg1)->value(), arg2);
	case dods_uint16_c:
		return cmp(op, static_cast<UInt16*>(arg1)->value(), arg2);
	case dods_int32_c:
		return cmp(op, static_cast<Int32*>(arg1)->value(), arg2);
	case dods_uint32_c:
		return cmp(op, static_cast<UInt32*>(arg1)->value(), arg2);
	case dods_int64_c:
		return cmp(op, static_cast<Int64*>(arg1)->value(), arg2);
	case dods_uint64_c:
		return cmp(op, static_cast<UInt64*>(arg1)->value(), arg2);
	case dods_float32_c:
		return cmp(op, static_cast<Float32*>(arg1)->value(), arg2);
	case dods_float64_c:
		return cmp(op, static_cast<Float64*>(arg1)->value(), arg2);
	default:
		return false;
	}
}

inline bool D4FilterClause::cmp(ops op, BaseType *arg1, BaseType *arg2)
{
	switch (arg2->type()) {
	case dods_byte_c:
		return cmp(op, arg1, static_cast<Byte*>(arg2)->value());
	case dods_int8_c:
		return cmp(op, arg1, static_cast<Int8*>(arg2)->value());
	case dods_int16_c:
		return cmp(op, arg1, static_cast<Int16*>(arg2)->value());
	case dods_uint16_c:
		return cmp(op, arg1, static_cast<UInt16*>(arg2)->value());
	case dods_int32_c:
		return cmp(op, arg1, static_cast<Int32*>(arg2)->value());
	case dods_uint32_c:
		return cmp(op, arg1, static_cast<UInt32*>(arg2)->value());
	case dods_int64_c:
		return cmp(op, arg1, static_cast<Int64*>(arg2)->value());
	case dods_uint64_c:
		return cmp(op, arg1, static_cast<UInt64*>(arg2)->value());
	case dods_float32_c:
		return cmp(op, arg1, static_cast<Float32*>(arg2)->value());
	case dods_float64_c:
		return cmp(op, arg1, static_cast<Float64*>(arg2)->value());

	case dods_str_c:
	case dods_url_c:
		if (arg1->type() != dods_str_c || arg1->type() != dods_url_c)
			throw Error("While evaluating a constraint filter clause: both operands of the match operator must be strings or URLs.");
		return cmp(op, static_cast<Str*>(arg1)->value(), static_cast<Str*>(arg1)->value());
	default:
		return false;
	}
}

bool D4FilterClause::value(DMR &dmr)
{
	switch (d_op) {
	case null:
		assert("Found a null operator");
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
		assert(false && "Filter operator not implemented");
		throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Filter operator not implemented");
	default:
		assert(false && "Unrecognized operator");
		throw InternalErr(__FILE__, __LINE__, "While evaluating a constraint filter clause: Unrecognized operator");
	}
}



// I don't think there's any real benefit to this code... jhrg 3/24/15
#if 0
bool D4FilterClause::cmp(ops op, BaseType *arg1, BaseType *arg2)
{
	switch (arg1->type()) {
	case dods_byte_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_int8_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_int16_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_uint16_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_int32_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_uint32_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_int64_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_uint64_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_float32_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	case dods_float64_c:
		switch (arg2->type()) {
		case dods_byte_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Byte*>(arg2)->value());
		case dods_int8_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int8*>(arg2)->value());
		case dods_int16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int16*>(arg2)->value());
		case dods_uint16_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt16*>(arg2)->value());
		case dods_int32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int32*>(arg2)->value());
		case dods_uint32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt32*>(arg2)->value());
		case dods_int64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Int64*>(arg2)->value());
		case dods_uint64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<UInt64*>(arg2)->value());
		case dods_float32_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float32*>(arg2)->value());
		case dods_float64_c:
			return cmp(op, static_cast<Byte*>(arg1)->value(), static_cast<Float64*>(arg2)->value());
		}
		break;
	}
}
#endif

} // namespace libdap
