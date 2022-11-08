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
#include "Int16.h"
#include "Int64.h"
#include "Float32.h"
#include "Str.h"
#include "D4Enum.h"
#include "Structure.h"
#include "D4Dimensions.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

class ArrayTest: public TestFixture {
private:
    Array *d_cardinal = nullptr;
    Array *d_string = nullptr;
    Array *d_structure = nullptr;
    Array *d_card_dap4 = nullptr;

    Int16 *d_int16 = nullptr;
    Str *d_str = nullptr;
    Structure *d_struct = nullptr;

    string svalues[4] = {"0 String", "1 String", "2 String", "3 String" };
    
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


        d_str = new Str("Str");
        d_string = new Array("Array_of_String", d_str);
        d_string->append_dim(4, "dimension");
        string sbuffer[4] = { "0 String", "1 String", "2 String", "3 String" };
        d_string->val2buf(sbuffer);

        d_struct = new Structure("Structure");
        d_struct->add_var(d_int16);
        d_structure = new Array("Array_of_Strctures", d_struct);
        d_structure->append_dim(4, "dimension");
        ostringstream oss;
        for (int i = 0; i < 4; ++i) {
            oss.str("");
            oss << "field" << i;
            Int16 *n = new Int16(oss.str());
            DBG(cerr << "n " << i << ": " << n << endl);
            oss.str("");
            oss << "element" << i;
            Structure *s = new Structure(oss.str());
            s->add_var(n);
            d_structure->set_vec(i, s);
            delete n;
            n = 0;
            delete s;
            s = 0;
        }

        delete d_int16;
        d_int16 = 0;
        delete d_str;
        d_str = 0;
        delete d_struct;
        d_struct = 0;
    }

    void tearDown()
    {
        delete d_cardinal;
        delete d_card_dap4;
        delete d_string;
        delete d_structure;
    }

    bool re_match(Regex &r, const char *s)
    {
        int match_position = r.match(s, strlen(s));
        DBG(cerr << "match position: " << match_position
            << " string length: " << (int)strlen(s) << endl);
        return match_position == (int) strlen(s);
    }

    CPPUNIT_TEST_SUITE (ArrayTest);

    CPPUNIT_TEST (cons_test);
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
    CPPUNIT_TEST (duplicate_string_test);
    CPPUNIT_TEST (duplicate_structure_test);

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
        unique_ptr<Float32> f32(new Float32("float_proto"));
        Array a1 = Array("a", f32.get());
#if 0
        auto f32_ptr = new Float32("float_proto");
        Array a1 = Array("a", f32_ptr);
	delete f32_ptr;
#endif
	
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
#if 0
        D4Dimensions *dims = new D4Dimensions();
        D4Dimension *d = new D4Dimension("dim_b", 2, dims);
#endif 
        unique_ptr<D4Dimensions> dims(new D4Dimensions());
        D4Dimension *d = new D4Dimension("dim_b", 2, dims.get());
#if 0
        //unique_ptr<D4Dimension> d(new D4Dimension("dim_b",2,dims.get()));
#endif

        a1.prepend_dim(2, "dim_a");
        a1.prepend_dim(d);
        a1.rename_dim("dim_a", "myDim");
        for (Array::Dim_iter i = a1.dim_begin(); i != a1.dim_end(); i++, j++) {
            CPPUNIT_ASSERT(a1.dimension_name(i) == expected_name[j]);
        }
#if 0
        //delete dims;
#endif
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

    void duplicate_structure_test()
    {
        Array::Dim_iter i = d_structure->dim_begin();
        CPPUNIT_ASSERT(d_structure->dimension_size(i) == 4);
#ifdef DODS_DEBUG
        for (int i = 0; i < 4; ++i) {
            Structure *s = dynamic_cast<Structure*>(d_structure->var(i));
            DBG(cerr << "s: " << s << endl);
            if (s)
            s->print_decl(cerr);
        }
#endif

        Array *a = new Array(*d_structure);
        // a = *d_structure; I test operator= in duplicate_cardinal_test().
        i = a->dim_begin();
        CPPUNIT_ASSERT(a->dimension_size(i) == 4);
        for (int i = 0; i < 4; ++i) {
            // The point of this test is to ensure that the const ctor
            // performs a deep copy; first test to make sure the pointers
            // to BaseType instnaces are different in the two objects.
            Structure *src = dynamic_cast<Structure*>(d_structure->var(i));
            Structure *dest = dynamic_cast<Structure*>(a->var(i));
            CPPUNIT_ASSERT(src != dest);

            // However, for the deep copy to be correct, the two arrays must
            // have equivalent elements. We know there's only one field...
            CPPUNIT_ASSERT(src->type() == dods_structure_c && dest->type() == dods_structure_c);
            Constructor::Vars_iter s = src->var_begin();
            Constructor::Vars_iter d = dest->var_begin();
            CPPUNIT_ASSERT((*s)->type() == dods_int16_c && (*d)->type() == dods_int16_c);
            CPPUNIT_ASSERT((*s)->name() == (*d)->name());
        }
        delete a;
        a = 0;
    }

    void duplicate_string_test()
    {
        Array::Dim_iter i = d_string->dim_begin();
        CPPUNIT_ASSERT(d_string->dimension_size(i) == 4);
        string *s = new string[4];
        d_string->buf2val((void**) &s);
        for (int i = 0; i < 4; ++i) {
            CPPUNIT_ASSERT(s[i] == svalues[i]);
            DBG(cerr << "s[" << i << "]: " << s[i] << endl);
        }

        Array a = *d_string;
        i = a.dim_begin();
        CPPUNIT_ASSERT(a.dimension_size(i) == 4);

        string *s2 = new string[4];
        d_string->buf2val((void**) &s2);
        for (int i = 0; i < 4; ++i) {
            CPPUNIT_ASSERT(s2[i] == svalues[i]);
            DBG(cerr << "s2[" << i << "]: " << s2[i] << endl);
        }

        delete[] s;
        s = 0;
        delete[] s2;
        s2 = 0;
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
