
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// An interface to new common methods for the Test*.cc/h classes. 

#ifndef _test_common_h
#define _test_common_h 1

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

    virtual void set_series_values(bool);
    virtual bool get_series_values();
};

#endif // _test_common_h

