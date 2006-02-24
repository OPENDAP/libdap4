
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
 
// (c) COPYRIGHT URI/MIT 1995-1996,1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for TestArray. See TestByte.cc
//
// jhrg 1/12/95

#include "config.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestArray.h"
#include "TestCommon.h"


using std::cerr;
using std::endl;

extern int test_variable_sleep_interval;

void
TestArray::_duplicate(const TestArray &ts)
{
    d_series_values = ts.d_series_values;
}

BaseType *
TestArray::ptr_duplicate()
{
    return new TestArray(*this);
}

TestArray::TestArray(const string &n, BaseType *v) : Array(n, v),
        d_series_values(false)
{
}

TestArray::TestArray(const TestArray &rhs) : Array(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestArray::~TestArray()
{
}

TestArray &
TestArray::operator=(const TestArray &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Array &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

// This read mfunc does some strange things to get a value - a real program
// would never get values this way. For testing this is OK.

bool
TestArray::read(const string &dataset)
{
    if (read_p())
		return true;

    if (test_variable_sleep_interval > 0)
		sleep(test_variable_sleep_interval);

    unsigned i;

    // run read() on the contained variable to get, via the read() mfuncs
    // defined in the other Test classes, a value in the *contained* object.
    var()->read(dataset);

    unsigned int array_len = length(); // elements in the array

    switch (var()->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c: {

	// String and Url are grouped with dods_byte, ... because val2buf works
	// for these types.

	unsigned int elem_wid = var()->width(); // size of an element

	char *tmp = new char[width()];
	// elem_val was a void pointer; delete complained. 6/4/2001 jhrg
	char *elem_val = 0;	// NULL init gets read_val() to alloc space

	// Added cast as temporary fix. 6/4/2001 jhrg
	var()->buf2val((void **)&elem_val); // internal buffer to ELEM_VAL

	for (i = 0; i < array_len; ++i)
	    memcpy(tmp + i * elem_wid, elem_val, elem_wid);


	val2buf(tmp);

	delete elem_val; elem_val = 0; // alloced in buf2val()
	delete[] tmp; tmp = 0;	// alloced above

	break;
      }

      case dods_structure_c:
      case dods_sequence_c:
      case dods_grid_c:
	
	// Arrays of Structure, ... must load each element into the array 
	// manually. Because these are stored as C++/DODS objects, there is
	// no need to manipulate blocks of memory by hand as in the above
	// case. 
        // NB: Strings are handled like Byte, etc. because, even though they
	// are represented using C++ objects they are *not* represented using
	// objects defined by DODS, while Structure, etc. are.

	for (i = 0; i < array_len; ++i) {

	    // Create a new object that is a copy of `var()' (whatever that
	    // is). The copy will have the value read in by the read() mfunc
	    // executed before this switch stmt.

	    BaseType *elem = var()->ptr_duplicate(); 

	    // read values into the new instance.
	    
	    elem->read(dataset);

	    // now load the new instance in the array.

	    set_vec(i, elem);
	}

	break;
	
      case dods_array_c:
      case dods_null_c:
      default:
	throw InternalErr(__FILE__, __LINE__, "Bad DODS data type");
	break;
    }

    set_read_p(true);

    return true;
}

void
TestArray::set_series_values(bool sv)
{
    dynamic_cast<TestCommon&>(*var()).set_series_values(sv);    
    d_series_values = sv;
}
