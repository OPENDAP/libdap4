// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2005,2018 OPeNDAP, Inc.
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstring>
#include <string>

#include <unistd.h>

#include "GNURegex.h"

#include "Array.h"
#include "Byte.h"
#include "D4Dimensions.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

class BigArrayTest: public TestFixture {
private:
    uint64_t num_eles;
    vector<unsigned char>buf_int8;

    
public:
    BigArrayTest() = default;

    ~BigArrayTest() = default;

    void setUp()
    {
        num_eles=1024*1024*1024;
        num_eles = num_eles*5;

        // Uncomment this #if 0 block to test a trivial small array case.
//#if 0
        num_eles = 4;
//#endif 
        buf_int8.resize(num_eles);
        buf_int8[0] = 0;
        buf_int8[num_eles/2] = 128;
        buf_int8[num_eles-1] = 255;

        DBG(cerr<<"Input buffer: The first value is "<<(int)(buf_int8[0]) <<endl);
        DBG(cerr<<"Input buffer: The middle value is "<<(int)(buf_int8[num_eles/2]) <<endl);
        DBG(cerr<<"Input buffer: The last value is "<<(int)(buf_int8[num_eles-1]) <<endl);
 
    }

    void tearDown()
    {
    }


    CPPUNIT_TEST_SUITE (BigArrayTest);

    CPPUNIT_TEST (dap4_val2buf_buf2val);
    CPPUNIT_TEST (dap4_val2buf_value);
    CPPUNIT_TEST (dap2_set_value);

    CPPUNIT_TEST_SUITE_END();

    void dap4_val2buf_buf2val()
    {

        unique_ptr<Byte> d_uint8(new Byte("Byte"));
        Array d4_ar_uint8 = Array("Byte_array",d_uint8.get(),true);
        unique_ptr<D4Dimension> d4_dim_byte(new D4Dimension("dimension",num_eles));
        d4_ar_uint8.append_dim(d4_dim_byte.get());

        DBG(cerr<<"Before testing val2buf() " <<endl);
        // Set the array value via val2buf
        d4_ar_uint8.val2buf(buf_int8.data());

        DBG(cerr<<"Pass testing val2buf() " <<endl);
        vector<unsigned char>d4_ar_val;
        d4_ar_val.resize(num_eles);
        void* d4_ar_val_ptr = (void*)(d4_ar_val.data());    

        // Retrieve the array value via buf2val
        d4_ar_uint8.buf2val(&d4_ar_val_ptr);
        DBG(cerr<<"DAP4 buf2val: The first value is "<<(int)(d4_ar_val[0]) <<endl);
        DBG(cerr<<"DAP4 buf2val: The middle value is "<<(int)(d4_ar_val[num_eles/2]) <<endl);
        DBG(cerr<<"DAP4 buf2val: The last value is "<<(int)(d4_ar_val[num_eles-1]) <<endl);

        CPPUNIT_ASSERT(buf_int8[0] == d4_ar_val[0]);
        CPPUNIT_ASSERT(buf_int8[num_eles/2] == d4_ar_val[num_eles/2]);
        CPPUNIT_ASSERT(buf_int8[num_eles-1] == d4_ar_val[num_eles-1]);
    }

    void dap4_val2buf_value()
    {

        unique_ptr<Byte> d_uint8(new Byte("Byte"));
        Array d4_ar_uint8 = Array("Byte_array",d_uint8.get());
        unique_ptr<D4Dimension> d4_dim_byte(new D4Dimension("dimension",num_eles));
        d4_ar_uint8.append_dim(d4_dim_byte.get());

        // Set the array value via val2buf
        d4_ar_uint8.val2buf(buf_int8.data());

        vector<unsigned char>d4_ar_val;
        d4_ar_val.resize(num_eles);

        // Retrieve the array value via value()
        d4_ar_uint8.value(d4_ar_val.data());
        DBG(cerr<<"DAP4 value(): The first value is "<<(int)(d4_ar_val[0]) <<endl);
        DBG(cerr<<"DAP4 value(): The middle value is "<<(int)(d4_ar_val[num_eles/2]) <<endl);
        DBG(cerr<<"DAP4 value(): The last value is "<<(int)(d4_ar_val[num_eles-1]) <<endl);

        CPPUNIT_ASSERT(buf_int8[0] == d4_ar_val[0]);
        CPPUNIT_ASSERT(buf_int8[num_eles/2] == d4_ar_val[num_eles/2]);
        CPPUNIT_ASSERT(buf_int8[num_eles-1] == d4_ar_val[num_eles-1]);
    }

    void dap2_set_value()
    {
        unique_ptr<Byte> d_uint8(new Byte("Byte"));
        Array d_ar_uint8 = Array("Byte_array",d_uint8.get());
        d_ar_uint8.append_dim(2);
        d_ar_uint8.append_dim(num_eles/2);

        // Set the array value via set_value()
        d_ar_uint8.set_value((dods_byte *)buf_int8.data(),(int64_t)num_eles);

        vector<unsigned char>d_ar_val;
        d_ar_val.resize(num_eles);

        // Retrieve the array value via value()
        d_ar_uint8.value(d_ar_val.data());
        DBG(cerr<<"DAP2 value() via set_value(): The first value is "<<(int)(d_ar_val[0]) <<endl);
        DBG(cerr<<"DAP2 value() via set_value(): The middle value is "<<(int)(d_ar_val[num_eles/2]) <<endl);
        DBG(cerr<<"DAP2 value() via set_value(): The last value is "<<(int)(d_ar_val[num_eles-1]) <<endl);

        CPPUNIT_ASSERT(buf_int8[0] == d_ar_val[0]);
        CPPUNIT_ASSERT(buf_int8[num_eles/2] == d_ar_val[num_eles/2]);
        CPPUNIT_ASSERT(buf_int8[num_eles-1] == d_ar_val[num_eles-1]);
 
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (BigArrayTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    return run_tests<libdap::BigArrayTest>(argc, argv) ? 0: 1;
}
