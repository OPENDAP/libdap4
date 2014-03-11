
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
#include "UInt64.h"
#include "Int64.h"
#include "Float64.h"
#include "Str.h"

#include "D4RValue.h"
#include "InternalErr.h"

#include "dods-limits.h"
#include "util.h"

using namespace std;

namespace libdap {

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
	// FIXME Memory leak. This returns a weak pointer while the other
	// cases return new storage that the caller must free. jhrg 3/10/14
	case basetype:
		return d_variable;

	case function:
		return (*d_func)(d_args, dmr);

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

	default:
		throw InternalErr(__FILE__, __LINE__, "Unknown rvalue type.");
	};
}

} // namespace libdap

