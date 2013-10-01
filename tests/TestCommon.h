
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2005 OPeNDAP, Inc.
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

// An interface to new common methods for the Test*.cc/h classes.

#ifndef _test_common_h
#define _test_common_h 1

#include <stdio.h>
#include <iostream>

/** Get the value of the series_value property. If true, the TestByte, ...,
    classes should produce values that vary in a fashion similar to the DTS.
    If false, they should exhibit the old behavior where the values are static.
    For arrays, grids and structures the 'series_value' doesn't mean much, but
    for sequences it's a big deal since those are constrained by value in
    addition to by position. */
class TestCommon {
public:
    TestCommon();
    virtual ~TestCommon();

    /** Write out values first testing send_p() to ensure the variable has
        data. This method is to be used when the intern_data() method is used
        to read data into variables after the CE is evaluated. In most cases
        variables will be used on either the server- or client-side but not
        both. However, in some cases the same variables are used on both
        sides. The intern_data() method is then used to read data into the
        variables (usually serialize() is used to read the data int a
        variable and send it to a client, then deserialize() is used to read
        those values from a network. The intern_data() method combines those
        operations.

        @note This method is used to test the result of calling
        intern_data(). The print_val() method does not know to check the
        send_p() flag and thus is not useful for testing intern_data()

        @param out Where to write the values */
    virtual void output_values(std::ostream &out) = 0;

    virtual void set_series_values(bool);
    virtual bool get_series_values();
};

#endif // _test_common_h

