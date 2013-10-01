
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

// Implementation for TestStr. See TestByte.cc
//
// jhrg 1/12/95


#include "config.h"

#include <string>

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestStr.h"
#include "util.h"

extern int test_variable_sleep_interval;

void
TestStr::_duplicate(const TestStr &ts)
{
    d_series_values = ts.d_series_values;
}


TestStr::TestStr(const string &n) : Str(n), d_series_values(false)
{
}

TestStr::TestStr(const string &n, const string &d)
    : Str(n, d), d_series_values(false)
{
}

TestStr::TestStr(const TestStr &rhs) : Str(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestStr &
TestStr::operator=(const TestStr &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Str &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

BaseType *
TestStr::ptr_duplicate()
{
    return new TestStr(*this);
}

void 
TestStr::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestStr::read()
{
    static int count = 0;
    
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    string dods_str_test = "Silly test string: " + long_to_string(++count);
    (void) val2buf(&dods_str_test);

    set_read_p(true);

    return true;
}
