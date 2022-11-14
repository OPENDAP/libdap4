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
#include "Int16.h"
#include "Float32.h"
#include "Int64.h"
#include "D4Enum.h"
#include "D4Dimensions.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

class BigArrayTest: public TestFixture {
private:
    Byte *d_uint8 = nullptr;
    uint64_t num_eles;
    vector<unsigned char>buf_int8;

    
public:
    BigArrayTest() = default;

    ~BigArrayTest() = default;

    void setUp()
    {
        num_eles=1024*1024*1024;
        //num_eles=num_eles*2;
        num_eles = num_eles*5;
        num_eles = 4;
        buf_int8.resize(num_eles);
        buf_int8[0] = 0;
        //cout<<"buf_int8 0 is "<<(int)(buf_int8[0]) <<endl;
        buf_int8[num_eles/2] = 128;
        buf_int8[num_eles-1] = 255;

#if 0
        auto d4_dim_byte = new D4Dimension("dimension",num_eles);
        d_cardinal_byte->append_dim(d4_dim_byte);
        vector<unsigned char> buffer_u8;
        buffer_u8.resize(num_eles);
        d_cardinal_byte->val2buf(buffer_u8.data());
        delete d4_dim_byte;
        delete d_uint8;
#endif
 
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
        Array d4_ar_uint8 = Array("Byte_array",d_uint8.get());
        unique_ptr<D4Dimension> d4_dim_byte(new D4Dimension("dimension",num_eles));
        d4_ar_uint8.append_dim(d4_dim_byte.get());
        d4_ar_uint8.val2buf(buf_int8.data());
        cout<<"the first value is "<<(int)(buf_int8[0]) <<endl;
        cout<<"the middle value is "<<(int)(buf_int8[num_eles/2]) <<endl;
        vector<unsigned char>d4_ar_val;
        d4_ar_val.resize(num_eles);

        // The following doesn't work for 64-bit integer
        void* d4_ar_val_ptr = (void*)(d4_ar_val.data());    
        d4_ar_uint8.buf2val(&d4_ar_val_ptr);
        cout<<"the first value buf2val is "<<(int)(d4_ar_val[0]) <<endl;
        cout<<"the middle value buf2val is "<<(int)(d4_ar_val[num_eles/2]) <<endl;
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
        d4_ar_uint8.val2buf(buf_int8.data());
        cout<<"the first value is "<<(int)(buf_int8[0]) <<endl;
        cout<<"the middle value is "<<(int)(buf_int8[num_eles/2]) <<endl;
        vector<unsigned char>d4_ar_val;
        d4_ar_val.resize(num_eles);

        // The following doesn't work for 64-bit integer
        d4_ar_uint8.value(d4_ar_val.data());
        cout<<"the first value buf2val is "<<(int)(d4_ar_val[0]) <<endl;
        cout<<"the middle value buf2val is "<<(int)(d4_ar_val[num_eles/2]) <<endl;
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
        d_ar_uint8.set_value((dods_byte *)buf_int8.data(),num_eles);
        cout<<"the first value is "<<(int)(buf_int8[0]) <<endl;
        cout<<"the middle value is "<<(int)(buf_int8[num_eles/2]) <<endl;
        vector<unsigned char>d_ar_val;
        d_ar_val.resize(num_eles);
        d_ar_uint8.value(d_ar_val.data());
        cout<<"the first value buf2val is "<<(int)(d_ar_val[0]) <<endl;
        cout<<"the middle value buf2val is "<<(int)(d_ar_val[num_eles/2]) <<endl;
        CPPUNIT_ASSERT(buf_int8[0] == d_ar_val[0]);
        CPPUNIT_ASSERT(buf_int8[num_eles/2] == d_ar_val[num_eles/2]);
        CPPUNIT_ASSERT(buf_int8[num_eles-1] == d_ar_val[num_eles-1]);
 
    }
#if 0

    void error_handling_test()
    {
        Array a1 = Array("a", d_int16);
        Array::Dim_iter i = a1.dim_begin();
        string msg;
        CPPUNIT_ASSERT_THROW(a1.dimension_name(i), InternalErr);
        CPPUNIT_ASSERT(!a1.check_semantics(msg));
    }

    void error_handling_2_test()
    {
        Array a1 = Array("a", d_int16);
        a1.append_dim(2, "dim_a");
        Array::Dim_iter i = a1.dim_begin();
        CPPUNIT_ASSERT(a1.dimension_size(i) == 2);
        CPPUNIT_ASSERT_THROW(a1.add_constraint(i, 2, 1, 1), Error);
        CPPUNIT_ASSERT_THROW(a1.add_constraint(i, 0, 1, 2), Error);
        CPPUNIT_ASSERT_THROW(a1.add_constraint(i, 0, 3, 1), Error);
    }


#endif

};

CPPUNIT_TEST_SUITE_REGISTRATION (BigArrayTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    return run_tests<libdap::BigArrayTest>(argc, argv) ? 0: 1;
}
