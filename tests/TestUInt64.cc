
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

#include <string>

#ifndef WIN64
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestUInt64.h"

using namespace std;
using namespace libdap;

extern int test_variable_sleep_interval;

void
TestUInt64::_duplicate(const TestUInt64 &ts)
{
    d_series_values = ts.d_series_values;
}


TestUInt64::TestUInt64(const string &n) : UInt64(n), d_series_values(false)
{
    d_buf = 1;
}

TestUInt64::TestUInt64(const string &n, const string &d)
    : UInt64(n, d), d_series_values(false)
{
    d_buf = 1;
}

TestUInt64::TestUInt64(const TestUInt64 &rhs) : UInt64(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestUInt64 &
TestUInt64::operator=(const TestUInt64 &rhs)
{
    if (this == &rhs)
	return *this;

    UInt64::operator=(rhs); // run Constructor=

    _duplicate(rhs);

    return *this;
}


BaseType *
TestUInt64::ptr_duplicate()
{
    return new TestUInt64(*this);
}

void 
TestUInt64::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestUInt64::read()
{
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
        d_buf = 64 * d_buf;
    }
    else {
        d_buf = 0xffffffffffffffff;		// really big
    }

    set_read_p(true);
    
    return true;
}
