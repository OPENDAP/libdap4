
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

#ifndef _testuint64_h
#define _testuint64_h 1

#include <string>

#include "UInt64.h"
#include "TestCommon.h"

// using namespace libdap ;

class TestUInt64: public libdap::UInt64, public TestCommon {
    bool d_series_values;
    void _duplicate(const TestUInt64 &ts);

public:
    TestUInt64(const std::string &n);
    TestUInt64(const std::string &n, const std::string &d);
    TestUInt64(const TestUInt64 &rhs);

    virtual ~TestUInt64() {}

    TestUInt64 &operator=(const TestUInt64 &rhs);

    virtual libdap::BaseType *ptr_duplicate();
    
    virtual bool read();
    
    virtual void output_values(std::ostream &out);

    void set_series_values(bool sv) { d_series_values = sv; }
    bool get_series_values() { return d_series_values; }
};

#endif // _testuint64_h

