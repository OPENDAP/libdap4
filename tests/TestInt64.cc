
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#ifndef WIN64
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

//#define DODS_DEBUG

#include "TestInt64.h"
#include "debug.h"

extern int test_variable_sleep_interval;

void
TestInt64::_duplicate(const TestInt64 &ts)
{
    d_series_values = ts.d_series_values;
}

TestInt64::TestInt64(const string &n) : Int64(n), d_series_values(false)
{
    d_buf = 1;
}

TestInt64::TestInt64(const string &n, const string &)
    : Int64(n), d_series_values(false)
{
    d_buf = 1;
}

TestInt64::TestInt64(const TestInt64 &rhs) : Int64(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestInt64 &
TestInt64::operator=(const TestInt64 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Int64 &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

BaseType *
TestInt64::ptr_duplicate()
{
    return new TestInt64(*this);
}

void
TestInt64::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestInt64::read()
{
    DBG(cerr << "Entering TestInt64::read for " << name() << endl);
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
    	// was d_buf = 64 * d_buf; but a change in the compiler broke eqiv code in Int32.cc
    	// jhrg 3/12/14
    	d_buf <<= 6;
        if (!d_buf)
            d_buf = 64;
    }
    else {
        d_buf = 0x00ffffffffffffff;
    }

    set_read_p(true);

    DBG(cerr << "In TestInt64::read, _buf = " << d_buf << endl);

    return true;
}
