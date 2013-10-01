
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
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for TestFloat32. See TestByte.cc
//
// 3/22/99 jhrg


#include "config.h"

#include <math.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestFloat32.h"
#include "debug.h"

extern int test_variable_sleep_interval;

#if ( defined(__sun__) && ( HOST_SOLARIS < 10 ))
double trunc(double x)
{
       return x < 0 ? -floor(-x) : floor(x);
}
#endif

void
TestFloat32::_duplicate(const TestFloat32 &ts)
{
    d_series_values = ts.d_series_values;
}

TestFloat32::TestFloat32(const string &n) : Float32(n), d_series_values(false)
{
    d_buf = 0.0;
}

TestFloat32::TestFloat32(const string &n, const string &d)
    : Float32(n, d), d_series_values(false)
{
    d_buf = 0.0;
}

TestFloat32::TestFloat32(const TestFloat32 &rhs) : Float32(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestFloat32 &
TestFloat32::operator=(const TestFloat32 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Float32 &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

BaseType *
TestFloat32::ptr_duplicate()
{
    return new TestFloat32(*this); // Copy ctor calls duplicate to do the work
}
 
void 
TestFloat32::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestFloat32::read()
{
    DBG(cerr << "Entering TestFloat32::read for " << name() << endl);
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
        d_buf += 10.0;
        d_buf = (float)(trunc(10000 * sin(trunc(d_buf))) / 100);
	/*
	d_buf -= 0.11 ;
	*/
    }
    else {
        d_buf = (float)99.999;
    }
    
    set_read_p(true);

    DBG(cerr << "In TestFloat32::read, _buf = " << d_buf << endl);
    
    return true;
}
