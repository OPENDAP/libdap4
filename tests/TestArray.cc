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

// Implementation for TestArray. See TestByte.cc
//
// jhrg 1/12/95

#include "config.h"

#include <cstring>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef WIN32
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

//#define DODS_DEBUG

//#include "ce_functions.h"
#include "util.h"
#include "debug.h"

#include "TestInt8.h"

#include "TestByte.h"
#include "TestInt16.h"
#include "TestUInt16.h"
#include "TestInt32.h"
#include "TestUInt32.h"

#include "TestInt64.h"
#include "TestUInt64.h"

#include "TestFloat32.h"
#include "TestFloat64.h"

#include "TestStr.h"

#include "TestArray.h"
#include "TestCommon.h"

using std::cerr;
using std::endl;

extern int test_variable_sleep_interval;

void TestArray::_duplicate(const TestArray &ts)
{
    d_series_values = ts.d_series_values;
}

BaseType *
TestArray::ptr_duplicate()
{
    return new TestArray(*this);
}

TestArray::TestArray(const string &n, BaseType *v, bool is_dap4) :
    Array(n, v, is_dap4), d_series_values(false)
{
}

TestArray::TestArray(const string &n, const string &d, BaseType *v, bool is_dap4) :
    Array(n, d, v, is_dap4), d_series_values(false)
{
}

TestArray::TestArray(const TestArray &rhs) :
    Array(rhs), TestCommon(rhs)
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

    dynamic_cast<Array &> (*this) = rhs;

    _duplicate(rhs);

    return *this;
}


// This code calls 'output_values()' because print_val() does not test
// the value of send_p(). We need to wrap a method around the calls to
// print_val() to ensure that only values for variables with send_p() set
// are called. In the serialize/deserialize case, the 'client' DDS only
// has variables sent by the 'server' but in the intern_data() case, the
// whole DDS is still present but only variables selected in the CE have
// values.

unsigned int TestArray::m_print_array(ostream &out, unsigned int index, unsigned int dims, unsigned int shape[])
{
    if (dims == 1) {
        out << "{";
        for (unsigned i = 0; i < shape[0] - 1; ++i) {
            dynamic_cast<TestCommon&> (*var(index++)).output_values(out);
            out << ", ";
        }
        dynamic_cast<TestCommon&> (*var(index++)).output_values(out);
        out << "}";

        return index;
    }
    else {
        out << "{";
        // Fixed an off-by-one error in the following loop. Since the array
        // length is shape[dims-1]-1 *and* since we want one less dimension
        // than that, the correct limit on this loop is shape[dims-2]-1. From
        // Todd Karakasian.
        // The saga continues; the loop test should be `i < shape[0]-1'. jhrg
        // 9/12/96.
        for (unsigned i = 0; i < shape[0] - 1; ++i) {
            index = m_print_array(out, index, dims - 1, shape + 1);
            out << ",";
        }
        index = m_print_array(out, index, dims - 1, shape + 1);
        out << "}";

        return index;
    }
}

void TestArray::output_values(std::ostream &out)
{
    unsigned int *shape = new unsigned int[dimensions(true)];
    unsigned int index = 0;
    for (Dim_iter i = dim_begin(); i != dim_end() && index < dimensions(true); ++i)
        shape[index++] = dimension_size(i, true);

    m_print_array(out, 0, dimensions(true), shape);

    delete[] shape;
    shape = 0;
}

/** Special names are ones that start with 'lat' or 'lon'. These indicate
 that the vector (this is only for vectors) is a vector of latitude or
 longitude values. */
bool TestArray::m_name_is_special()
{
    return (name().find("lat") != string::npos || name().find("lon") != string::npos);
}

void TestArray::m_build_special_values()
{
    if (name().find("lat_reversed") != string::npos) {
        int array_len = length();
        double *lat_data = new double[array_len];
        for (int i = 0; i < array_len; ++i) {
            lat_data[i] = -89 + (180 / array_len) * (i + 1);
        }
        libdap::set_array_using_double(this, lat_data, array_len);
    }
    else if (name().find("lat") != string::npos) {
        int array_len = length();
        double *lat_data = new double[array_len];
        for (int i = 0; i < array_len; ++i) {
            lat_data[i] = 90 - (180 / array_len) * (i + 1);
        }
        libdap::set_array_using_double(this, lat_data, array_len);
    }
    else if (name().find("lon") != string::npos) {
        int array_len = length();
        double *lon_data = new double[array_len];
        for (int i = 0; i < array_len; ++i) {
            lon_data[i] = (360 / array_len) * (i + 1);
        }
        libdap::set_array_using_double(this, lon_data, array_len);
    }
    else {
        throw InternalErr(__FILE__, __LINE__, "Unrecognized name");
    }
}

int TestArray::m_offset(int y, Dim_iter X, int x)
{
    return y * dimension_size(X, false) + x;
}

/**
 * @brief Load an 2D array with values.
 * Use the read() function for the prototype element of the array to
 * get values and load them into an array, then constrain the array.
 * Thus if 'series values' are used and the array is constrained, the
 * result will 'make sense'
 *
 * @param constrained_array
 */
template <typename T, class C>
void TestArray::m_constrained_matrix(vector<T>&constrained_array)
{
    int unconstrained_size = 1;
    Dim_iter d = dim_begin();
    while (d != dim_end())
        unconstrained_size *= dimension_size(d++, false);

    vector<T> whole_array(unconstrained_size);
#if 0
    char *whole_array = new char[unconstrained_size * width(true)];
    DBG(cerr << "unconstrained size: " << unconstrained_size << endl);

    int elem_width = var()->width(true); // size of an element
    char *elem_val = new char[elem_width];
#endif
    for (int i = 0; i < unconstrained_size; ++i) {
        var()->read();
        whole_array[i] = static_cast<C*>(var())->value();
#if 0
        var()->buf2val((void **) &elem_val); //FIXME use value() instead

        memcpy(whole_array + i * elem_width, elem_val, elem_width);
#endif
        var()->set_read_p(false); // pick up the next value
    }

#if 0
    DBG(cerr << "whole_array: ";
            for (int i = 0; i < unconstrained_size; ++i) {
                cerr << (int)*(dods_byte*)(whole_array + (i * elem_width)) << ", ";
            }
            cerr << endl);
#endif
    DBG(cerr << "whole_array: "; copy(whole_array.begin(), whole_array.end(), ostream_iterator<T>(cerr, ", ")); cerr << endl);

    Dim_iter Y = dim_begin();
    Dim_iter X = Y + 1;
    //char *dest = constrained_array;

    DBG(cerr << "dimension_start(Y): " << dimension_start(Y) << endl);
    DBG(cerr << "dimension_stop(Y): " << dimension_stop(Y) << endl);
    DBG(cerr << "dimension_start(X): " << dimension_start(X) << endl);
    DBG(cerr << "dimension_stop(X): " << dimension_stop(X) << endl);
    // int elem_width = var()->width(true); // size of an element
    int constrained_size = 0;
    int y = dimension_start(Y);
    while (y < dimension_stop(Y) + 1) {

        int x = dimension_start(X);
        while (x < dimension_stop(X) + 1) {
#if 0
            DBG2(cerr << "whole[" << y << "][" << x << "]: ("
                    << m_offset(y, Y, x) << ") "
                    << *(dods_byte*)(whole_array + m_offset(y, X, x)*elem_width)
                    << endl);
#endif
#if 0
            memcpy(dest, whole_array + m_offset(y, X, x) * elem_width, elem_width);
#endif
            //memcpy(dest, &whole_array[m_offset(y, X, x)], elem_width);

            constrained_array[constrained_size++] = whole_array[m_offset(y, X, x)];

            //dest += elem_width;
            x += dimension_stride(X);
            //constrained_size++;
        }

        y += dimension_stride(Y);
    }
#if 0
    DBG(cerr << "constrained size: " << constrained_size << endl); DBG(cerr << "constrained_array: ";
            for (int i = 0; i < constrained_size; ++i) {
                cerr << (int)*(dods_byte*)(constrained_array + (i * elem_width)) << ", ";
            }
            cerr << endl);
#endif
#if 0
    delete[] whole_array;
    delete[] elem_val;
#endif
}

/**
 * Load the variable's internal data buffer with values, simulating a read()
 * call to some data store. A private method.
 */
template <typename T, class C>
void TestArray::m_cardinal_type_read_helper()
{
    if (get_series_values()) {
        // Special case code for vectors that have specific names.
        // This is used to test code that works with lat/lon data.
        if (dimensions() == 1 && m_name_is_special()) {
            m_build_special_values();
        }
        else if (dimensions() == 2) {
#if 0
            vector<char> tmp(width(true));
            m_constrained_matrix<T, C>(&tmp[0]);
            val2buf(&tmp[0]);	// This is a call to Array::val2buf which defaults to Vector::val2buf
#endif
            vector<T> tmp(length());
            m_constrained_matrix<T, C>(tmp);
            set_value(tmp, length());
        }
        else {
            vector<T> tmp(length());
            for (int64_t i = 0, end = length(); i < end; ++i) {
                var()->read();
#if 0
                T value = static_cast<C*>(var())->value();
                tmp[i] = value;
#endif
                tmp[i] = static_cast<C*>(var())->value();

                var()->set_read_p(false); // pick up the next value
            }
            set_value(tmp, length());
        }
    }
    else {
    	// read a value into the Array's prototype element
        var()->read();
        T value = static_cast<C*>(var())->value();

        vector<T> tmp(length());
        for (int64_t i = 0, end = length(); i < end; ++i) {
        	tmp[i] = value;
        }

        set_value(tmp, length());
    }
}

bool TestArray::read()
{
    if (read_p())
        return true;

    if (test_variable_sleep_interval > 0)
        sleep(test_variable_sleep_interval);

    int64_t array_len = length(); // elements in the array

    switch (var()->type()) {
		// These are the DAP2 types and the classes that implement them all define
		// the old buf2val() and val2buf() methods. For the new DAP4 types see below.
        //case dods_byte_c:
        //case dods_uint8_c:
        case dods_int16_c:
        	m_cardinal_type_read_helper<dods_int16, Int16>();
        	break;

        case dods_uint16_c:
        	m_cardinal_type_read_helper<dods_uint16, UInt16>();
        	break;

        case dods_int32_c:
        	m_cardinal_type_read_helper<dods_int32, Int32>();
        	break;

       case dods_uint32_c:
        	m_cardinal_type_read_helper<dods_uint32, UInt32>();
        	break;

        case dods_float32_c:
        	m_cardinal_type_read_helper<dods_float32, Float32>();
        	break;

        case dods_float64_c:
        	m_cardinal_type_read_helper<dods_float64, Float64>();
        	break;

        case dods_int8_c:
        	m_cardinal_type_read_helper<dods_int8, Int8>();
        	break;

        case dods_byte_c:
        case dods_uint8_c:
        	m_cardinal_type_read_helper<dods_byte, Byte>();
        	break;

        case dods_int64_c:
        	m_cardinal_type_read_helper<dods_int64, Int64>();
        	break;

        case dods_uint64_c:
        	m_cardinal_type_read_helper<dods_uint64, UInt64>();
        	break;

        case dods_str_c:
        case dods_url_c: {
        	vector<string> tmp(array_len);

			if (get_series_values()) {
				for (int64_t i = 0; i < array_len; ++i) {
					var()->read();
					// URL isa Str
					tmp[i] = static_cast<Str*>(var())->value();
					var()->set_read_p(false); // pick up the next value
				}
			}
			else {
				var()->read();
				string value = static_cast<Str*>(var())->value();

				for (unsigned i = 0; i < array_len; ++i)
					tmp[i] = value;
			}

			set_value(tmp, array_len);
			break;
        }

        case dods_opaque_c:
        case dods_structure_c:
            for (unsigned i = 0; i < array_len; ++i) {
            	// Copy the prototype and read a value into it
                BaseType *elem = var()->ptr_duplicate();
                elem->read();
                // Load the new value into this object's array
                set_vec(i, elem);
            }

            break;

        case dods_sequence_c:
        	if (!is_dap4())
        		throw InternalErr(__FILE__, __LINE__, "Bad data type");

            for (unsigned i = 0; i < array_len; ++i) {
            	// Copy the prototype and read a value into it
                BaseType *elem = var()->ptr_duplicate();
                //elem->read();
                // Load the new value into this object's array
                set_vec(i, elem);
            }

            break;

        case dods_grid_c:
        case dods_array_c:
        case dods_null_c:
        default:
            throw InternalErr(__FILE__, __LINE__, "Bad data type");
            break;
    }

    set_read_p(true);

    return true;
}

void TestArray::set_series_values(bool sv)
{
    dynamic_cast<TestCommon&> (*var()).set_series_values(sv);
    d_series_values = sv;
}
