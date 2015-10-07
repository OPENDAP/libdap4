
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

#include "TestD4Enum.h"
#include "debug.h"

extern int test_variable_sleep_interval;

void
TestD4Enum::_duplicate(const TestD4Enum &ts)
{
    d_series_values = ts.d_series_values;
}

TestD4Enum::TestD4Enum(const string &n, Type t) : D4Enum(n, t), TestCommon(), d_series_values(false)
{
    set_value(0);
}

TestD4Enum::TestD4Enum(const string &n, const string &d, Type t)
    : D4Enum(n, d, t), TestCommon(), d_series_values(false)
{
    set_value(0);
}

TestD4Enum::TestD4Enum(const TestD4Enum &rhs) : D4Enum(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestD4Enum &
TestD4Enum::operator=(const TestD4Enum &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<D4Enum &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

BaseType *
TestD4Enum::ptr_duplicate()
{
    return new TestD4Enum(*this);
}

void
TestD4Enum::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool TestD4Enum::read() {
    DBG(cerr << "Entering TestD4Enum::read for " << name() << endl);
    if (read_p()) return true;

    if (test_variable_sleep_interval > 0) sleep(test_variable_sleep_interval);

    if (get_series_values()) {
        int64_t v;
        value(&v);
        if (v == 3)
            set_value(1);
        else
            set_value(v + 1);
    }
    else {
        set_value(1);
    }

    set_read_p(true);

    DBG(cerr << "In TestD4Enum::read, _buf = " << d_buf << endl);

    return true;
}
