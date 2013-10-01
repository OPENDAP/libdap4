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

TestArray::TestArray(const string &n, BaseType *v) :
    Array(n, v), d_series_values(false)
{
}

TestArray::TestArray(const string &n, const string &d, BaseType *v) :
    Array(n, d, v), d_series_values(false)
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

    dynamic_cast<Array &> (*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

/** Special names are ones that start with 'lat' or 'lon'. These indicate
 that the vector (this is only for vectors) is a vector of latitude or
 longitude values. */
bool TestArray::name_is_special()
{
    return (name().find("lat") != string::npos || name().find("lon") != string::npos);
}

void TestArray::build_special_values()
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

/** Only call this method for a two dimensional array */
void TestArray::constrained_matrix(char *constrained_array)
{
    int unconstrained_size = 1;
    Dim_iter d = dim_begin();
    while (d != dim_end())
        unconstrained_size *= dimension_size(d++, false);
    char *whole_array = new char[unconstrained_size * width(true)];
    DBG(cerr << "unconstrained size: " << unconstrained_size << endl);

    int elem_width = var()->width(true); // size of an element
    char *elem_val = new char[elem_width];

    for (int i = 0; i < unconstrained_size; ++i) {
        var()->read();
        var()->buf2val((void **) &elem_val);

        memcpy(whole_array + i * elem_width, elem_val, elem_width);
        var()->set_read_p(false); // pick up the next value
    }

    DBG(cerr << "whole_array: ";
            for (int i = 0; i < unconstrained_size; ++i) {
                cerr << (int)*(dods_byte*)(whole_array + (i * elem_width)) << ", ";
            }
            cerr << endl);

    Dim_iter Y = dim_begin();
    Dim_iter X = Y + 1;
    char *dest = constrained_array;

    DBG(cerr << "dimension_start(Y): " << dimension_start(Y) << endl); DBG(cerr << "dimension_stop(Y): " << dimension_stop(Y) << endl); DBG(cerr << "dimension_start(X): " << dimension_start(X) << endl); DBG(cerr << "dimension_stop(X): " << dimension_stop(X) << endl);

    int constrained_size = 0;
    int y = dimension_start(Y);
    while (y < dimension_stop(Y) + 1) {

        int x = dimension_start(X);
        while (x < dimension_stop(X) + 1) {

            DBG2(cerr << "whole[" << y << "][" << x << "]: ("
                    << m_offset(y, Y, x) << ") "
                    << *(dods_byte*)(whole_array + m_offset(y, X, x)*elem_width)
                    << endl);

            memcpy(dest, whole_array + m_offset(y, X, x) * elem_width, elem_width);

            dest += elem_width;
            x += dimension_stride(X);
            constrained_size++;
        }

        y += dimension_stride(Y);
    }

    DBG(cerr << "constrained size: " << constrained_size << endl); DBG(cerr << "constrained_array: ";
            for (int i = 0; i < constrained_size; ++i) {
                cerr << (int)*(dods_byte*)(constrained_array + (i * elem_width)) << ", ";
            }
            cerr << endl);

    delete[] whole_array;
    delete[] elem_val;
}

// This code calls 'output_values()' because print_val() does not test
// the value of send_p(). We need to wrap a method around the calls to
// print_val() to ensure that only values for variables with send_p() set
// are called. In the serialize/deserialize case, the 'client' DDS only
// has variables sent by the 'server' but in the intern_data() case, the
// whole DDS is still present but only variables selected in the CE have
// values.

unsigned int TestArray::print_array(ostream &out, unsigned int index, unsigned int dims, unsigned int shape[])
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
            index = print_array(out, index, dims - 1, shape + 1);
            out << ",";
        }
        index = print_array(out, index, dims - 1, shape + 1);
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

    print_array(out, 0, dimensions(true), shape);

    delete[] shape;
    shape = 0;
}

bool TestArray::read()
{
    if (read_p())
        return true;

    if (test_variable_sleep_interval > 0)
        sleep(test_variable_sleep_interval);

    unsigned int array_len = length(); // elements in the array

    switch (var()->type()) {
        case dods_byte_c:
        case dods_int16_c:
        case dods_uint16_c:
        case dods_int32_c:
        case dods_uint32_c:
        case dods_float32_c:
        case dods_float64_c: {

            //char *tmp = new char[width()];
            vector<char> tmp(width(true));

            unsigned int elem_wid = var()->width(); // size of an element
            char *elem_val = 0; // Null forces buf2val to allocate memory

            if (get_series_values()) {
                // Special case code for vectors that have specific names.
                // This is used to test code that works with lat/lon data.
                if (dimensions() == 1 && name_is_special()) {
                    build_special_values();
                }
                else if (dimensions() == 2) {
                    constrained_matrix(&tmp[0]);
                    val2buf(&tmp[0]);
                }
                else {
                    for (unsigned i = 0; i < array_len; ++i) {
                        var()->read();
                        var()->buf2val((void **) &elem_val); // internal buffer to ELEM_VAL
                        memcpy(&tmp[0] + i * elem_wid, elem_val, elem_wid);
                        var()->set_read_p(false); // pick up the next value
                    }
                    val2buf(&tmp[0]);
                }
            }
            else {
                var()->read();
                var()->buf2val((void **) &elem_val);

                for (unsigned i = 0; i < array_len; ++i) {
                    memcpy(&tmp[0] + i * elem_wid, elem_val, elem_wid);
                }

                val2buf(&tmp[0]);
            }

            delete elem_val;
            elem_val = 0; // alloced in buf2val()
            // delete[] tmp; tmp = 0;	// alloced above

            break;
        }

        case dods_str_c:
        case dods_url_c: {
            // char *tmp = new char[width()];
            vector<char> tmp(width(true));
            unsigned int elem_wid = var()->width(); // size of an element
            char *elem_val = 0; // Null forces buf2val to allocate memory

            if (get_series_values()) {
                for (unsigned i = 0; i < array_len; ++i) {
                    var()->read();
                    var()->buf2val((void **) &elem_val); // internal buffer to ELEM_VAL
                    memcpy(&tmp[0] + i * elem_wid, elem_val, elem_wid);
                    var()->set_read_p(false); // pick up the next value
                }
            }
            else {
                var()->read();
                var()->buf2val((void **) &elem_val);

                for (unsigned i = 0; i < array_len; ++i) {
                    memcpy(&tmp[0] + i * elem_wid, elem_val, elem_wid);
                }
            }

            val2buf(&tmp[0]);

            delete elem_val;
            elem_val = 0; // alloced in buf2val()
            // delete[] tmp; tmp = 0;  // alloced above

            break;
        }

        case dods_structure_c:

            // Arrays of Structure, ... must load each element into the array
            // manually. Because these are stored as C++/DODS objects, there is
            // no need to manipulate blocks of memory by hand as in the above
            // case.
            // NB: Strings are handled like Byte, etc. because, even though they
            // are represented using C++ objects they are *not* represented using
            // objects defined by DODS, while Structure, etc. are.

            for (unsigned i = 0; i < array_len; ++i) {

                // Create a new object that is a copy of `var()' (whatever that
                // is). The copy will have the value read in by the read() mfunc
                // executed before this switch stmt.

                BaseType *elem = var()->ptr_duplicate();

                // read values into the new instance.

                elem->read();

                // now load the new instance in the array.

                set_vec(i, elem);
            }

            break;

        case dods_sequence_c:
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
