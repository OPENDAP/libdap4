
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

#ifndef _test_d4_group_h
#define _test_d4_group_h 1

#include "D4Group.h"
#include "TestCommon.h"

using namespace libdap ;

class TestD4Group: public D4Group, public TestCommon {
    bool d_series_values;
    void m_duplicate(const TestD4Group &ts);

public:
    TestD4Group(const string &n);
    TestD4Group(const string &n, const string &d);
    TestD4Group(const TestD4Group &rhs);

    virtual ~TestD4Group();

    TestD4Group &operator=(const TestD4Group &rhs);

    virtual TestD4Group *ptr_duplicate();

    virtual bool read();

    virtual void output_values(std::ostream &out);

    void set_series_values(bool);
    bool get_series_values() { return d_series_values; }
};

#endif // _test_d4_group_h
