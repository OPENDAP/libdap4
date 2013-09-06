
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

//#define DODS_DEBUG

#include "config.h"

#include "TestD4Group.h"

#include "debug.h"

void
TestD4Group::m_duplicate(const TestD4Group &ts)
{
    d_series_values = ts.d_series_values;
}

TestD4Group *
TestD4Group::ptr_duplicate()
{
    return new TestD4Group(*this);
}

TestD4Group::TestD4Group(const TestD4Group &rhs) : D4Group(rhs), TestCommon(rhs)
{
    m_duplicate(rhs);
}

TestD4Group &
TestD4Group::operator=(const TestD4Group &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<D4Group &>(*this) = rhs; // run Constructor=

    m_duplicate(rhs);

    return *this;
}

TestD4Group::TestD4Group(const string &n) : D4Group(n), d_series_values(false)
{
}

TestD4Group::TestD4Group(const string &n, const string &d)
    : D4Group(n, d), d_series_values(false)
{
}

TestD4Group::~TestD4Group()
{
}

void
TestD4Group::output_values(std::ostream &out)
{
    out << "{ " ;

    bool value_written = false;
    Vars_citer i = var_begin();

    // Write the first (and maybe only) value.
    while(i != var_end() && ! value_written) {
        if ((*i)->send_p()) {
            (*i++)->print_val(out, "", false);
            value_written = true;
        }
        else {
            ++i;
        }
    }
    // Each subsequent value will be preceded by a comma
    while(i != var_end()) {
        if ((*i)->send_p()) {
            out << ", ";
            (*i++)->print_val(out, "", false);
        }
        else {
            ++i;
        }
    }

    out << " }" ;
}

bool TestD4Group::read()
{
	if (read_p()) return true;

	for (Vars_iter i = var_begin(); i != var_end(); i++) {
		if (!(*i)->read()) {
			return false;
		}
	}

	set_read_p(true);

	return true;
}

void TestD4Group::set_series_values(bool sv)
{
	Vars_iter i = var_begin();
	while (i != var_end()) {
		dynamic_cast<TestCommon*>(*i)->set_series_values(sv);
		++i;
	}

	d_series_values = sv;
}
