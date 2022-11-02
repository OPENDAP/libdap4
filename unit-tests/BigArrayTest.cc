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

class ArrayTest: public TestFixture {
private:
    Array *d_cardinal = nullptr;
    Array *d_card_dap4 = nullptr;

    Array *d_cardinal_byte = nullptr;
    Array *d_card_dap4_byte = nullptr;

    Int16 *d_int16 = nullptr;
    Byte *d_uint8 = nullptr;

    
public:
    ArrayTest() = default;

    ~ArrayTest() = default;

    void setUp()
    {
        d_int16 = new Int16("Int16");
        DBG(cerr << "d_int16: " << d_int16 << endl);
        d_cardinal = new Array("Array_of_Int16", d_int16);
        d_cardinal->append_dim(4, "dimension");
        dods_int16 buffer[4] = { 0, 1, 2, 3 };
        d_cardinal->val2buf(buffer);

        // Added to test if Arrays with DAP4 protos will have is_dap4() == true. jhrg 7/27/22
        Int64 i64("");
        d_card_dap4 = new Array("Array_of_Int64", &i64);
        d_card_dap4->append_dim(4, "dimension");
        dods_int64 buffer64[4] = { 0, 1, 2, 3 };
        d_card_dap4->val2buf(buffer64);

        delete d_int16;
        d_int16 = 0;
    }

    void tearDown()
    {
        delete d_cardinal;
        delete d_card_dap4;
    }


    CPPUNIT_TEST_SUITE (ArrayTest);

    CPPUNIT_TEST (cons_test);
#if 0
    CPPUNIT_TEST (test_is_dap4_1);
    CPPUNIT_TEST (test_is_dap4_2);
    CPPUNIT_TEST (test_is_dap4_3);
    CPPUNIT_TEST (test_is_dap4_4);
    CPPUNIT_TEST (assignment_test_1);
    CPPUNIT_TEST (assignment_test_2);
    CPPUNIT_TEST (assignment_test_3);
    CPPUNIT_TEST (prepend_dim_test);
    CPPUNIT_TEST (prepend_dim_2_test);
    CPPUNIT_TEST (clear_dims_test);
    CPPUNIT_TEST (error_handling_test);
    CPPUNIT_TEST (error_handling_2_test);
    CPPUNIT_TEST (duplicate_cardinal_test);
#endif

    CPPUNIT_TEST_SUITE_END();

    void cons_test()
    {
        Array a1 = Array("a", "b", d_int16, true);
        CPPUNIT_ASSERT(a1.name() == "a");
    }

    void test_is_dap4_1() {
        DBG(d_card_dap4->dump(cerr));
        CPPUNIT_ASSERT_MESSAGE("This Array should register as DAP4", d_card_dap4->is_dap4());
    }

    void test_is_dap4_2() {
        DBG(d_cardinal->dump(cerr));
        CPPUNIT_ASSERT_MESSAGE("This Array should not register as DAP4", !d_cardinal->is_dap4());
    }

    void test_is_dap4_3() {
        // An array Enum is always DAP4, even when the underlying type of the enum is not
        D4Enum enum16("", dods_int16_c);
        unique_ptr<Array> enum_array_dap4(new Array("Array_of_Enum", &enum16));
        enum_array_dap4->append_dim(4, "dimension");
        dods_int16 buffer16[4] = { 0, 1, 2, 3 };
        enum_array_dap4->val2buf(buffer16);

        DBG(enum_array_dap4->dump(cerr));
        CPPUNIT_ASSERT_MESSAGE("This Array should register as DAP4", enum_array_dap4->is_dap4());
    }

    void test_is_dap4_4() {
        D4Enum enum64("", dods_int64_c);
        unique_ptr<Array> enum_array_dap4(new Array("Array_of_Enum", &enum64));
        enum_array_dap4->append_dim(4, "dimension");
        dods_int64 buffer64[4] = { 0, 1, 2, 3 };
        enum_array_dap4->val2buf(buffer64);

        DBG(enum_array_dap4->dump(cerr));
        CPPUNIT_ASSERT_MESSAGE("This Array should register as DAP4", enum_array_dap4->is_dap4());
    }

    void assignment_test_1()
    {
        // Are fields from the object copied by the assignment
        Array a1 = Array("a", d_int16);
        a1.append_dim(17, "bob");
        CPPUNIT_ASSERT(a1.dimension_size(a1.dim_begin()) == 17);
        a1 = *d_cardinal;
        CPPUNIT_ASSERT(a1.dimension_size(a1.dim_begin()) == 4);
    }

    void assignment_test_2()
    {
        // Self-assignment; are objects changed by the assignment?
        Array a1 = Array("a", d_int16);
        Array *before = &a1;
        a1 = a1;
        Array *after = &a1;
        CPPUNIT_ASSERT_MESSAGE("The pointers should be the same", before == after);
    }

    // This tests that the assignment operator defined in Array correctly copies
    // information held in a parent class (Vector). jhrg 2/9/22
    void assignment_test_3()
    {
        // Array copies the proto pointer and manages the storage
#if 0       
        unique_ptr<Float32> f32(new Float32("float_proto"));
        Array a1 = Array("a", f32.get());
#endif
        auto f32_ptr = new Float32("float_proto");
        Array a1 = Array("a", f32_ptr);
	delete f32_ptr;
	
        CPPUNIT_ASSERT_MESSAGE("The type of a1.var() should be dods_float32", a1.var()->type() == dods_float32_c);
        a1 = *d_cardinal;
        CPPUNIT_ASSERT_MESSAGE("The type of a1.var() should now be dods_int16_c", a1.var()->type() == dods_int16_c);
    }

    void prepend_dim_test()
    {
        Array a1 = Array("a", d_int16);
        Array::Dim_iter i = a1.dim_begin();
        CPPUNIT_ASSERT(a1.dimension_size(i) == 0);
        a1.prepend_dim(2, "dim_a");
        i = a1.dim_begin();
        CPPUNIT_ASSERT(a1.dimension_size(i) == 2);
        a1.prepend_dim(3, "dim_b");
    }

    void prepend_dim_2_test()
    {
        Array a1 = Array("a", d_int16);
        string expected_name[2] = {"dim_b", "myDim"};
        int j = 0;
        D4Dimensions *dims = new D4Dimensions();
        D4Dimension *d = new D4Dimension("dim_b", 2, dims);
        a1.prepend_dim(2, "dim_a");
        a1.prepend_dim(d);
        a1.rename_dim("dim_a", "myDim");
        for (Array::Dim_iter i = a1.dim_begin(); i != a1.dim_end(); i++, j++) {
            CPPUNIT_ASSERT(a1.dimension_name(i) == expected_name[j]);
        }
        delete dims;
        delete d;
    }

    void clear_dims_test()
    {
        Array a1 = Array("a", d_int16);
        a1.prepend_dim(2, "dim_a");
        a1.prepend_dim(3, "dim_b");
        a1.clear_all_dims();
        Array::Dim_iter i = a1.dim_begin();
        CPPUNIT_ASSERT(a1.dimension_size(i) == 0);
    }

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


    void duplicate_cardinal_test()
    {
        Array::Dim_iter i = d_cardinal->dim_begin();
        CPPUNIT_ASSERT(d_cardinal->dimension_size(i) == 4);
        dods_int16 *b = new dods_int16[4];
        d_cardinal->buf2val((void**) &b);
        for (int i = 0; i < 4; ++i) {
            CPPUNIT_ASSERT(b[i] == i);
            DBG(cerr << "b[" << i << "]: " << b[i] << endl);
        }
        delete[] b;
        b = 0;

        Array a = *d_cardinal;
        i = a.dim_begin();
        CPPUNIT_ASSERT(a.dimension_size(i) == 4);

        dods_int16 *b2 = new dods_int16[4];
        d_cardinal->buf2val((void**) &b2);
        for (int i = 0; i < 4; ++i) {
            CPPUNIT_ASSERT(b2[i] == i);
            DBG(cerr << "b2[" << i << "]: " << b2[i] << endl);
        }
        delete[] b2;
        b2 = 0;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (ArrayTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    return run_tests<libdap::ArrayTest>(argc, argv) ? 0: 1;
}
