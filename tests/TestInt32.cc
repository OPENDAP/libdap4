
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
 
// (c) COPYRIGHT URI/MIT 1995-1996,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for TestInt32. See TestByte.cc
//
// jhrg 1/12/95


#include "config.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

//#define DODS_DEBUG 

#include "TestInt32.h"
#include "debug.h"

extern int test_variable_sleep_interval;

void
TestInt32::_duplicate(const TestInt32 &ts)
{
    d_series_values = ts.d_series_values;
}

TestInt32::TestInt32(const string &n) : Int32(n), d_series_values(false)
{
    d_buf = 1;
}

TestInt32::TestInt32(const string &n, const string &)
    : Int32(n), d_series_values(false)
{
    d_buf = 1;
}

TestInt32::TestInt32(const TestInt32 &rhs) : Int32(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestInt32 &
TestInt32::operator=(const TestInt32 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Int32 &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

BaseType *
TestInt32::ptr_duplicate()
{
    return new TestInt32(*this);
}

void 
TestInt32::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestInt32::read()
{
    DBG(cerr << "Entering TestInt32::read for " << name() << endl);
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
        d_buf = 32 * d_buf;
        if (!d_buf)
            d_buf = 32;
    }
    else {
        d_buf = 123456789;
    }

    set_read_p(true);

    DBG(cerr << "In TestInt32::read, _buf = " << d_buf << endl);
    
    return true;
}
