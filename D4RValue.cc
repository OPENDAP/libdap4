
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
#include "Float64.h"
#include "Str.h"

#include "UInt64.h"
#include "Int64.h"

#include "D4RValue.h"
#include "InternalErr.h"

#include "dods-datatypes.h"
#include "dods-limits.h"
#include "util.h"

using namespace std;

namespace libdap {

template<typename t, class DAP_TYPE>
static BaseType *
build_constant_array(vector<t> &values, DAP_TYPE &dt)
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
	s->set_value(cpps);
	d_constant = s;
}

D4RValue::D4RValue(std::vector<dods_byte> &byte_args)
	: d_variable(0), d_func(0), d_args(0),  d_constant(0), d_value_kind(constant)
{
	Byte b("");
	d_constant = build_constant_array(byte_args, b);
}

D4RValue::~D4RValue() {
	// d_variable and d_func are weak pointers; don't delete.
	delete d_args;
	delete d_constant;
}

/** Return the BaseType * for a given RValue.
 *
 * @note A lingering issue with server functions is: Who or what is responsible
 * for deleting the BaseType* this method returns? Copying the BaseType wastes
 * memory but not copying it creates ambiguity since the BaseType* may point into
 * the DMR adn thus should not be freed or it might be newly allocated storage
 * (in which case it should). For the function result, we have no way of knowing
 * how/if to dispose of the BaseType*. Sort this out ... (reference counting,
 * mark and sweep, ...). Maybe duplicate it before it contains data? Then it can be
 * deleted without affecting the DMR
 * jhrg 3/10/14
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

	// FIXME Memory leak: when composition is used the result of the inner function
	// calls will not be deleted. jhrg 3/13/14
	case function: {
		return (*d_func)(d_args, dmr);
	}

	case constant:
		return d_constant;

#if 0
	case uinteger: {
		UInt64 *ui = new UInt64("constant");
		ui->set_value(d_u_int_val);
		return ui;
	}

	case integer: {
		Int64 *i = new Int64("constant");
		i->set_value(d_int_val);
		return i;
	}

	case real: {
		Float64 *f = new Float64("constant");
		f->set_value(d_real_val);
		return f;
	}

	case string: {
		Str *s = new Str("constant");
		s->set_value(d_string_val);
		return s;
	}
#endif
	default:
		throw InternalErr(__FILE__, __LINE__, "Unknown rvalue type.");
	};
}

} // namespace libdap

