
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for TestUrl. See TestByte.cc
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

#include "TestUrl.h"

extern int test_variable_sleep_interval;

void
TestUrl::_duplicate(const TestUrl &ts)
{
    d_series_values = ts.d_series_values;
}

TestUrl::TestUrl(const string &n) : Url(n), d_series_values(false)
{
}

TestUrl::TestUrl(const string &n, const string &)
    : Url(n), d_series_values(false)
{
}

TestUrl::TestUrl(const TestUrl &rhs) : Url(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestUrl &
TestUrl::operator=(const TestUrl &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Url &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

BaseType *
TestUrl::ptr_duplicate()
{
    return new TestUrl(*this);
}

void 
TestUrl::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool
TestUrl::read()
{
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    string url_test="http://dcz.gso.uri.edu/avhrr-archive/archive.html";

    val2buf(&url_test);

    set_read_p(true);

    return true;
}
