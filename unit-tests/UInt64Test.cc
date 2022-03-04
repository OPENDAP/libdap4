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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sstream>
#include <string.h>

#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Int64.h"
#include "UInt64.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"
#include "crc.h"

#include "DDS.h"

#include "GNURegex.h"

#include "util.h"
#include "debug.h"
#include "ce_expr.tab.hh"

#include "run_tests_cppunit.h"
#include "testFile.h"
#include "run_tests_cppunit.h"
#include "test_config.h"


using namespace CppUnit;
using namespace std;

namespace libdap {

class UInt64Test: public TestFixture {
private:
    UInt64 *i1, *i2;
    char a[1024];
    
public:
    UInt64Test() : i1(0), i2(0)
    {
    }
    ~UInt64Test()
    {
    }

    void setUp()
    {
        i1 = new UInt64("a", "b");
        i2 = new UInt64("e");
    }

    void tearDown()
    {
        delete i1;
        delete i2;
    }

    CPPUNIT_TEST_SUITE(UInt64Test);

    CPPUNIT_TEST(cons_UInt64_test);
    CPPUNIT_TEST(checksum_test);
    // CPPUNIT_TEST(val2buf_test);
    // CPPUNIT_TEST(buf2val_test);
    CPPUNIT_TEST(set_value_test);
    CPPUNIT_TEST(equals_test);
    CPPUNIT_TEST(type_compare_test);
    CPPUNIT_TEST(ops_exception_1_test);
    CPPUNIT_TEST(ops_exception_2_test);
    CPPUNIT_TEST(dump_test);
    // CPPUNIT_TEST(print_test);
    CPPUNIT_TEST(check_types);

    CPPUNIT_TEST_SUITE_END();

    void cons_UInt64_test()
    {
        CPPUNIT_ASSERT(i1->value() == 0 && i1->dataset() == "b" && i1->name() == "a" &&
                       i1->type() == dods_uint64_c);
        CPPUNIT_ASSERT(i2->value() == 0);
    }

    void checksum_test()
    {
        Crc32 cs;
        i2->compute_checksum(cs);
    }

    // void val2buf_test()
    // {
    //     int i = 42;
    //     i2->val2buf(&i, true);
    //     CPPUNIT_ASSERT(i2->value() == 42);        
    //     CPPUNIT_ASSERT_THROW(i2->val2buf(NULL, true), InternalErr);
    // }

    // void buf2val_test()
    // {
    //     int i = 42;
    //     void *v = &i;
    //     void *v2 = NULL;
    //     CPPUNIT_ASSERT(i2->set_value(0));
    //     CPPUNIT_ASSERT(i2->buf2val(&v) == 4 && i == 0);
    //     CPPUNIT_ASSERT_THROW(i2->buf2val(NULL), InternalErr);
    //     CPPUNIT_ASSERT(i2->buf2val(&v2) == 4 && *(int *)v2 == 0);
    // }

    void set_value_test()
    {
        CPPUNIT_ASSERT(i2->set_value(42) && i2->value() == 42);        
    }

    void equals_test()
    {
        UInt64 i3 = UInt64("a", "b");
        UInt64 i4 = UInt64("e");
        CPPUNIT_ASSERT(i4.set_value(42) && i4.value() == 42);
        i3 = i4;
        CPPUNIT_ASSERT(i3.value() == 42);
        i3 = i3;
    }    

    void type_compare_test()
    {
        Byte b1 = Byte("a");
        Int8 i8 = Int8("a");
        Int16 i16 = Int16("a");
        UInt16 ui16 = UInt16("a");
        Int32 i32 = Int32("a", "b");
        UInt32 ui32 = UInt32("a", "b");
        Int64 i64 = Int64("a", "b");
        UInt64 ui64 = UInt64("a", "b");
        Float32 f32 = Float32("a");
        Float64 f64 = Float64("a");
        Url url = Url("a");
        Str str = Str("a");
        Array array = Array("a", &i16, true);
        
        b1.set_value(42);
        i8.set_value(42);
        i16.set_value(42);
        ui16.set_value(42);
        i32.set_value(42);
        ui32.set_value(42);
        i64.set_value(42);
        ui64.set_value(42);
        f32.set_value(42);
        f64.set_value(42);
        CPPUNIT_ASSERT(ui64.value() == 42);
        CPPUNIT_ASSERT(ui64.ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&i8, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&i16, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&ui16, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&i32, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&ui32, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&i64, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&ui64, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&f32, SCAN_EQUAL));
        CPPUNIT_ASSERT(ui64.d4_ops(&f64, SCAN_EQUAL));

        // CPPUNIT_ASSERT_THROW(ui64.d4_ops(&url, SCAN_EQUAL), Error);
        // CPPUNIT_ASSERT_THROW(ui64.d4_ops(&str, SCAN_EQUAL), Error);
        // CPPUNIT_ASSERT_THROW(ui64.d4_ops(&array, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT(!ui64.d4_ops(&url, SCAN_EQUAL));
        CPPUNIT_ASSERT(!ui64.d4_ops(&str, SCAN_EQUAL));
        CPPUNIT_ASSERT(!ui64.d4_ops(&array, SCAN_EQUAL));
        CPPUNIT_ASSERT_THROW(ui64.ops(0, SCAN_EQUAL), Error);
    }    

    void ops_exception_1_test()
    {
        Byte b1 = Byte("a");
        UInt64 ui64 = UInt64("a", "b");
        b1.set_read_p(false);
        CPPUNIT_ASSERT_THROW(ui64.ops(&b1, SCAN_EQUAL), InternalErr);        
    }    

    void ops_exception_2_test()
    {
        Byte b1 = Byte("a");
        UInt64 ui64 = UInt64("a", "b");
        ui64.set_read_p(false);
        CPPUNIT_ASSERT_THROW(ui64.ops(&b1, SCAN_EQUAL), InternalErr);        
    }    

    void dump_test()
    {
        ofstream ofs("UInt64Test_dump.output", ios::trunc);
        i1->set_value(21);
        i1->dump(ofs);
        ofs.close();
        ifstream ifs("UInt64Test_dump.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "21"));
    }

    // void print_test()
    // {
    //     FILE *fp;
    //     CPPUNIT_ASSERT(fp = fopen("UInt64Test.output", "w"));
    //     i1->set_value(22);
    //     i1->print_val(fp, " ", true);
    //     fclose(fp);
    //     ifstream ifs("UInt64Test.output");
    //     while(!ifs.eof())
    //         ifs >> a;
    //     ifs.close();
    //     CPPUNIT_ASSERT(!strcmp(a, "22;"));
    // }

    void check_types()
    {
        Byte *b1 = new Byte("b");
        b1->set_value(14);
        i1->set_value(14);
//        CPPUNIT_ASSERT(b1 == i1);
        delete b1;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(UInt64Test);

} // namespace libdap

int main(int argc, char *argv[])
{
    return run_tests<UInt64Test>(argc, argv) ? 0: 1;
}
