
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

// Implementation for TestFloat64. See TestByte.cc
//
// jhrg 1/12/95


#include "config.h"

#include <math.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestFloat64.h"

extern int test_variable_sleep_interval;

#if ( defined(__sun__) && ( HOST_SOLARIS < 10 ))
double trunc(double x)
{
       return x < 0 ? -floor(-x) : floor(x);
}
#endif

void
TestFloat64::_duplicate(const TestFloat64 &ts)
{
    d_series_values = ts.d_series_values;
}

TestFloat64::TestFloat64(const string &n) : Float64(n), d_series_values(false)
{
    d_buf = 0.0;
}

TestFloat64::TestFloat64(const string &n, const string &d)
    : Float64(n, d), d_series_values(false)
{
    d_buf = 0.0;
}

TestFloat64::TestFloat64(const TestFloat64 &rhs) : Float64(rhs) , TestCommon(rhs)
{
    _duplicate(rhs);
}

TestFloat64 &
TestFloat64::operator=(const TestFloat64 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Float64 &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

BaseType *
TestFloat64::ptr_duplicate()
{
    return new TestFloat64(*this); // Copy ctor calls duplicate to do the work
}

void 
TestFloat64::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestFloat64::read()
{
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
        d_buf += 10.0;
        d_buf = (float)(trunc(10000 * cos(trunc(d_buf))) / 100);
    }
    else {
        d_buf = 99.999;
    }
    
    set_read_p(true);

    return true;
}

