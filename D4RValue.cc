
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

#include "config.h"

#include <cassert>
#include <iostream>

#include "BaseType.h"
#include "Array.h"
#include "Byte.h"
#include "Int8.h"
#include "UInt16.h"
#include "Int16.h"
#include "UInt32.h"
#include "Int32.h"
#include "UInt64.h"
#include "Int64.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"

#include "D4RValue.h"
#include "InternalErr.h"

#include "dods-datatypes.h"
#include "dods-limits.h"
#include "util.h"

using namespace std;

namespace libdap {

D4RValueList::~D4RValueList()
{
	for (std::vector<D4RValue *>::iterator i = d_rvalues.begin(), e = d_rvalues.end(); i != e; ++i)
		delete *i;
}

template<typename T, class DAP_TYPE>
static BaseType *
build_constant_array(vector<T> &values, DAP_TYPE &dt)
{
    Array *array = new Array("", &dt);
    array->append_dim(values.size());

    // TODO Make set_value_nocopy() methods so that values' pointers can be copied
    // instead of allocating memory twice. jhrg 7/5/13

    array->set_value(values, values.size());

    array->set_read_p(true);

    static unsigned long counter = 1;
    array->set_name(string("g") + long_to_string(counter++));

    return array;
}

D4RValue::D4RValue(unsigned long long ull) : d_variable(0), d_func(0), d_args(0), d_constant(0), d_value_kind(constant)
{
	UInt64 *ui = new UInt64("constant");
	ui->set_value(ull);
	d_constant = ui;
}

D4RValue::D4RValue(long long ll) : d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Int64 *i = new Int64("constant");
	i->set_value(ll);
	d_constant = i;
}

D4RValue::D4RValue(double r) : d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Float64 *f = new Float64("constant");
	f->set_value(r);
	d_constant = f;
}

D4RValue::D4RValue(std::string cpps) : d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Str *s = new Str("constant");
	s->set_value(remove_quotes(cpps));
	d_constant = s;
}

D4RValue::D4RValue(std::vector<dods_byte> &byte_args)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Byte b("");
	d_constant = build_constant_array(byte_args, b);
}

D4RValue::D4RValue(std::vector<dods_int8> &byte_int8)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Int8 b("");
	d_constant = build_constant_array(byte_int8, b);
}

D4RValue::D4RValue(std::vector<dods_uint16> &byte_uint16)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	UInt16 b("");
	d_constant = build_constant_array(byte_uint16, b);
}

D4RValue::D4RValue(std::vector<dods_int16> &byte_int16)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Int16 b("");
	d_constant = build_constant_array(byte_int16, b);
}

D4RValue::D4RValue(std::vector<dods_uint32> &byte_uint32)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	UInt32 b("");
	d_constant = build_constant_array(byte_uint32, b);
}

D4RValue::D4RValue(std::vector<dods_int32> &byte_int32)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Int32 b("");
	d_constant = build_constant_array(byte_int32, b);
}

D4RValue::D4RValue(std::vector<dods_uint64> &byte_uint64)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	UInt64 b("");
	d_constant = build_constant_array(byte_uint64, b);
}

D4RValue::D4RValue(std::vector<dods_int64> &byte_int64)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Int64 b("");
	d_constant = build_constant_array(byte_int64, b);
}

D4RValue::D4RValue(std::vector<dods_float32> &byte_float32)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Float32 b("");
	d_constant = build_constant_array(byte_float32, b);
}

D4RValue::D4RValue(std::vector<dods_float64> &byte_float64)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Float64 b("");
	d_constant = build_constant_array(byte_float64, b);
}

D4RValue::~D4RValue() {
	// d_variable and d_func are weak pointers; don't delete.
	delete d_args;
	delete d_constant;
}

/** Return the BaseType * for a given RValue.
 *
 * @note Unlike the DAP2 functions, we have an easier-to-follow memory model for
 * function values. The values (BaseType*) returned by this method will be packaged
 * up in a RValueList and deleted when that list is deleted. Constant values and
 * function result values will be deleted at that time; variables will not. Thus
 * Server Functions should always allocate storage for their return values.
 *
 * @param dmr The DMR to pass to a function.
 * @return A BaseType* that holds the value.
 */
BaseType *
D4RValue::value(DMR &dmr)
{
	switch (d_value_kind) {
	case basetype:
		d_variable->read();
		d_variable->set_read_p(true);
		return d_variable;

	case function:
		return (*d_func)(d_args, dmr);

	case constant:
		return d_constant;

	default:
		throw InternalErr(__FILE__, __LINE__, "Unknown rvalue type.");
	};
}

} // namespace libdap

