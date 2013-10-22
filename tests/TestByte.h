
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

// (c) COPYRIGHT URI/MIT 1995-1997,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// TestByte is a demonstration of subclassing a class in the hierarchy of
// DODS data types. It does not do much of anything - a real subclass would
// add specifics for an API or format (e.g., a read mfunc for netcdf, HDF,
// ...).  The class is used by some of the test code (hence the name) as well
// as serving as a template for others who need to subclass the hierarchy.
//
// Since the class Byte is an abstract class (as are all the other
// `variable type' classes), the hierarchy *must* be subclassed in order to
// be used.
//
// jhrg 1/12/95
//
// NB: It is no longer true that Byte, ..., Grid are abstract classes. They
// are now concrete ad do not necessarily need to be subclassed. 01/22/03 jhrg

#ifndef _testbyte_h
#define _testbyte_h 1

#include "Byte.h"
#include "TestCommon.h"

using namespace libdap ;

class TestByte: public Byte, public TestCommon {
    bool d_series_values;
    void _duplicate(const TestByte &ts);

public:
    TestByte(const string &n);
    TestByte(const string &n, const string &d);
    TestByte(const TestByte &rhs);

    virtual ~TestByte() {}

    TestByte &operator=(const TestByte &rhs);

    virtual BaseType *ptr_duplicate();

    virtual bool read();

    virtual void output_values(std::ostream &out);

    virtual void set_series_values(bool sv) { d_series_values = sv; }
    virtual bool get_series_values() { return d_series_values; }
};

#endif // _testbyte_h

