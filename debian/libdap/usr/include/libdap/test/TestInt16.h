
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
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// TestInt16 interface. See TestByte.h for more info.
//
// 3/22/99 jhrg

#ifndef _testint16_h
#define _testint16_h 1


#include "Int16.h"
#include "TestCommon.h"

using namespace libdap ;

class TestInt16: public Int16, public TestCommon {
    bool d_series_values;
    void _duplicate(const TestInt16 &ts);

public:
    TestInt16(const string &n);
    TestInt16(const string &n, const string &d);
    TestInt16(const TestInt16 &rhs);

    virtual ~TestInt16() {}

    TestInt16 &operator=(const TestInt16 &rhs);

    virtual BaseType *ptr_duplicate();
    
    virtual bool read();
    
    virtual void output_values(std::ostream &out);

    void set_series_values(bool sv) { d_series_values = sv; }
    bool get_series_values() { return d_series_values; }
};

#endif // _testint16_h

