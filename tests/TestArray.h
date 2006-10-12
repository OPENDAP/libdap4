
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
 
// (c) COPYRIGHT URI/MIT 1995-1997,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface definition for TestArray. See TestByte.h for more information
//
// jhrg 1/12/95

#ifndef _testarray_h
#define _testarray_h 1

#include "Array.h"
#include "TestCommon.h"

class TestArray: public Array, public TestCommon {
    bool d_series_values;
    void _duplicate(const TestArray &ts);
    int m_offset(int y, Dim_iter Y, int x);

public:
    TestArray(const string &n = "", BaseType *v = 0);
    TestArray(const TestArray &rhs);

    virtual ~TestArray();

    TestArray &operator=(const TestArray &rhs);

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
    virtual bool name_is_special();
    virtual void build_special_values();    
    virtual void constrained_matrix(const string &dataset, char *constrained_array);
    
    void set_series_values(bool);
    bool get_series_values() { return d_series_values; }
};

#endif // _testarray_h


