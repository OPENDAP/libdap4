
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

// (c) COPYRIGHT URI/MIT 1995-1996,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for TestByte. See the comments in TestByte.h
// For each of the `variable classes' (e.g., Byte, ... Array, ... Grid) you
// *must* define a ctor, dtor, ptr_duplicate and read mfunc. In addition, you
// must edit the definition of New<class name> so that it creates the correct
// type of object. for example, edit NewByte() so that it creates and returns
// a TestByte pointer (see util.cc).
//
// jhrg 1/12/95
//
// NB: It is no longer true that you must subclass the Byte, ..., Grid
// classes in order to use the DAP. Those classes are no longer abstract. For
// many client-side uses, the classes will work just fine as they are. To
// build a server, it is still necessary to subclass and define a read()
// method for each of the data type classes. 01/22/03 jhrg

#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestByte.h"
#include "debug.h"

// The NewByte `helper function' creates a pointer to the a TestByte and
// returns that pointer. It takes the same arguments as the class's ctor. If
// any of the variable classes are subclassed (e.g., to make a new Byte like
// HDFByte) then the corresponding function here, and in the other class
// definition files, needs to be changed so that it creates an instance of
// the new (sub)class. Continuing the earlier example, that would mean that
// NewByte() would return a HDFByte, not a Byte.
//
// It is important that these function's names and return types do not change
// - they are called by the parser code (for the dds, at least) so if their
// names changes, that will break.
//
// The declarations for these functions (in util.h) should *not* need
// changing.

extern int test_variable_sleep_interval;

void
TestByte::_duplicate(const TestByte &ts)
{
    d_series_values = ts.d_series_values;
}

TestByte::TestByte(const string &n) : Byte(n), d_series_values(false)
{
    d_buf = 255;
}

TestByte::TestByte(const string &n, const string &d)
    : Byte(n, d), d_series_values(false)
{
    d_buf = 255;
}

BaseType *
TestByte::ptr_duplicate()
{
    return new TestByte(*this);
}

TestByte::TestByte(const TestByte &rhs) : Byte(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestByte &
TestByte::operator=(const TestByte &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Byte &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}
#if 1
void
TestByte::output_values(std::ostream &out)
{
    // value is a method where each return value is a different type so we have
    // to make calls to it from objects/methods where the type is statically
    // known.
    print_val(out, "", false);
}
#endif

bool
TestByte::read()
{
    DBG(cerr << "Entering TestByte::read for " << name() << endl);
    if (read_p())
	return true;

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
         d_buf++;
    }
    else {
        d_buf = 255;
    }

    set_read_p(true);

    DBG(cerr << "In TestByte::read, _buf = " << (int)d_buf << endl);

    return true;
}
