
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

// (c) COPYRIGHT URI/MIT 1996,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for TestUInt32. See TestByte.cc
//
// jhrg 10/27/96


#include "config.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestUInt32.h"

extern int test_variable_sleep_interval;

void
TestUInt32::_duplicate(const TestUInt32 &ts)
{
    d_series_values = ts.d_series_values;
}


TestUInt32::TestUInt32(const string &n) : UInt32(n), d_series_values(false)
{
    d_buf = 1;
}

TestUInt32::TestUInt32(const string &n, const string &d)
    : UInt32(n, d), d_series_values(false)
{
    d_buf = 1;
}

TestUInt32::TestUInt32(const TestUInt32 &rhs) : UInt32(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestUInt32 &
TestUInt32::operator=(const TestUInt32 &rhs)
{
    if (this == &rhs)
	return *this;

    UInt32::operator=(rhs); // run Constructor=

    _duplicate(rhs);

    return *this;
}


BaseType *
TestUInt32::ptr_duplicate()
{
    return new TestUInt32(*this);
}

void
TestUInt32::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestUInt32::read()
{
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
   		// This line stopped working when I upgraded the compiler on osx 10.9.
		// to version Apple LLVM version 5.1 (clang-503.0.38) (based on LLVM 3.4svn)
		// jhrg 3/12/14
		// d_buf = d_buf * 32;
		d_buf <<= 5;
		if (!d_buf)
			d_buf = 32;
    }
    else {
        d_buf = 0xf0000000;		// about 4 billion
    }

    set_read_p(true);

    return true;
}
