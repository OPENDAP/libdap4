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

// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95

//#define DODS_DEBUG

#include "config.h"
#include "D4Group.h"
#include "Constructor.h"
#include "debug.h"

#include "TestStructure.h"

using namespace libdap;

void TestStructure::m_duplicate(const TestStructure &ts)
{
	d_series_values = ts.d_series_values;
}

BaseType *
TestStructure::ptr_duplicate()
{
	return new TestStructure(*this);
}

TestStructure::TestStructure(const TestStructure &rhs) :
		Structure(rhs), TestCommon(rhs)
{
	m_duplicate(rhs);
}

TestStructure &
TestStructure::operator=(const TestStructure &rhs)
{
	if (this == &rhs) return *this;

	dynamic_cast<Structure &>(*this) = rhs; // run Constructor=

	m_duplicate(rhs);

	return *this;
}

TestStructure::TestStructure(const string &n) :
		Structure(n), d_series_values(false)
{
}

TestStructure::TestStructure(const string &n, const string &d) :
		Structure(n, d), d_series_values(false)
{
}

TestStructure::~TestStructure()
{
}

void TestStructure::output_values(std::ostream &out)
{
	out << "{ ";

	bool value_written = false;
	Vars_citer i = var_begin();

	// Write the first (and maybe only) value.
	while (i != var_end() && !value_written) {
		if ((*i)->send_p()) {
			(*i++)->print_val(out, "", false);
			value_written = true;
		}
		else {
			++i;
		}
	}
	// Each subsequent value will be preceded by a comma
	while (i != var_end()) {
		if ((*i)->send_p()) {
			out << ", ";
			(*i++)->print_val(out, "", false);
		}
		else {
			++i;
		}
	}

	out << " }";
}

void
TestStructure::transform_to_dap4(D4Group *root, Constructor *container)
{
    TestStructure *dest = new TestStructure(name(), dataset());
	Constructor::transform_to_dap4(root, dest);
	container->add_var_nocopy(dest);
}

// For this `Test' class, run the read mfunc for each of variables which
// comprise the structure.

bool TestStructure::read()
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

void TestStructure::set_series_values(bool sv)
{
	Vars_iter i = var_begin();
	while (i != var_end()) {
		dynamic_cast<TestCommon&>(*(*i)).set_series_values(sv);
		++i;
	}

	d_series_values = sv;
}
