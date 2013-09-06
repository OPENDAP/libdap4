// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestInt8.h"

extern int test_variable_sleep_interval;

void TestInt8::m_duplicate(const TestInt8 &ts)
{
	d_series_values = ts.d_series_values;
}

TestInt8::TestInt8(const string &n) :
		Int8(n), d_series_values(false)
{
	d_buf = 1;
}

TestInt8::TestInt8(const string &n, const string &d) :
		Int8(n, d), d_series_values(false)
{
	d_buf = 1;
}

TestInt8::TestInt8(const TestInt8 &rhs) :
		Int8(rhs), TestCommon(rhs)
{
	m_duplicate(rhs);
}

TestInt8 &
TestInt8::operator=(const TestInt8 &rhs)
{
	if (this == &rhs) return *this;

	dynamic_cast<Int8 &>(*this) = rhs; // run Constructor=

	m_duplicate(rhs);

	return *this;
}

BaseType *
TestInt8::ptr_duplicate()
{
	return new TestInt8(*this);
}

void TestInt8::output_values(std::ostream &out)
{
	print_val(out, "", false);
}

bool TestInt8::read()
{
	if (read_p()) return true;

	if (test_variable_sleep_interval > 0) sleep (test_variable_sleep_interval);

	if (get_series_values()) {
		d_buf = (dods_int8) (2 * d_buf);
	}
	else {
		d_buf = 128;
	}

	set_read_p(true);

	return true;
}
