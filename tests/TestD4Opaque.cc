
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

#include "TestD4Opaque.h"
#include "debug.h"

extern int test_variable_sleep_interval;

void
TestD4Opaque::m_duplicate(const TestD4Opaque &ts)
{
    d_series_values = ts.d_series_values;
}

void
TestD4Opaque::m_set_values(int start)
{
    dods_opaque values;
    values.push_back(start);
    values.push_back(2 * start);
    values.push_back(3 * start);
    values.push_back(4 * start);
    values.push_back(5 * start);

    set_value(values);
}

TestD4Opaque::TestD4Opaque(const string &n) : D4Opaque(n), d_series_values(false)
{
}

TestD4Opaque::TestD4Opaque(const string &n, const string &d) : D4Opaque(n, d), d_series_values(false)
{
}

TestD4Opaque::TestD4Opaque(const TestD4Opaque &rhs) : D4Opaque(rhs), TestCommon(rhs)
{
    m_duplicate(rhs);
}

TestD4Opaque &
TestD4Opaque::operator=(const TestD4Opaque &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<D4Opaque &>(*this) = rhs; // run Constructor=

    m_duplicate(rhs);

    return *this;
}

BaseType *
TestD4Opaque::ptr_duplicate()
{
    return new TestD4Opaque(*this);
}

void
TestD4Opaque::output_values(std::ostream &out)
{
    print_val(out, "", false);
}

bool TestD4Opaque::read() {
    DBG(cerr << "Entering TestD4Opaque::read for " << name() << endl);
    if (read_p()) return true;

    if (test_variable_sleep_interval > 0) sleep(test_variable_sleep_interval);

    if (get_series_values()) {
        dods_opaque vals = value();
        m_set_values(vals[0] * 2);
    }
    else {
        m_set_values(1);
    }

    set_read_p(true);

    DBG(cerr << "In TestD4Opaque::read, d_buf = ");
    DBGN(ostream_iterator<uint8_t> out_it (cerr," "));
    DBGN(std::copy ( d_buf.begin(), d_buf.end(), out_it ));
    DBGN(cerr << endl);

    return true;
}
