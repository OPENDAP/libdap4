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
#include "GetOpt.h"
#include "util.h"
#include "debug.h"
#include "ce_expr.tab.hh"

#include "testFile.h"
#include "test_config.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) {x;} } while(false)

using namespace CppUnit;
using namespace std;

namespace libdap {

class Int16Test: public TestFixture {
private:
    Int16 *i1, *i2;
    char a[1024];
    
public:
    Int16Test() : i1(0), i2(0)
    {
    }
    ~Int16Test()
    {
    }

    void setUp()
    {
        i1 = new Int16("a", "b");
        i2 = new Int16("e");
    }

    void tearDown()
    {
        delete i1;
        delete i2;
    }

    CPPUNIT_TEST_SUITE(Int16Test);

    CPPUNIT_TEST(cons_Int16_test);
    CPPUNIT_TEST(checksum_test);
    CPPUNIT_TEST(val2buf_test);
    CPPUNIT_TEST(buf2val_test);
    CPPUNIT_TEST(set_value_test);
    CPPUNIT_TEST(equals_test);
    CPPUNIT_TEST(type_compare_test);
    CPPUNIT_TEST(ops_exception_1_test);
    CPPUNIT_TEST(ops_exception_2_test);
    CPPUNIT_TEST(dump_test);
    CPPUNIT_TEST(print_test);
    CPPUNIT_TEST(check_types);

    CPPUNIT_TEST_SUITE_END();

    void cons_Int16_test()
    {
        CPPUNIT_ASSERT(i1->value() == 0 && i1->dataset() == "b" && i1->name() == "a" &&
                       i1->type() == dods_int16_c);
        CPPUNIT_ASSERT(i2->value() == 0);
    }

    void checksum_test()
    {
        Crc32 cs;
        i2->compute_checksum(cs);
    }

    void val2buf_test()
    {
        short i = 42;
        i2->val2buf(&i, true);
        CPPUNIT_ASSERT(i2->value() == 42);        
        CPPUNIT_ASSERT_THROW(i2->val2buf(NULL, true), InternalErr);
    }

    void buf2val_test()
    {
        short i = 42;
        void *v = &i;
        short *v2 = NULL;
        CPPUNIT_ASSERT(i2->set_value(0));
        CPPUNIT_ASSERT(i2->buf2val(&v) == 2 && i == 0);
        CPPUNIT_ASSERT_THROW(i2->buf2val(NULL), InternalErr);
        CPPUNIT_ASSERT(i2->buf2val((void **)&v2) == 2 && *v2 == 0);
        delete v2;
    }

    void set_value_test()
    {
        CPPUNIT_ASSERT(i2->set_value(42) && i2->value() == 42);        
    }

    void equals_test()
    {
        Int16 i3 = Int16("a", "b");
        Int16 i4 = Int16("e");
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
        CPPUNIT_ASSERT(i16.value() == 42);
        CPPUNIT_ASSERT(i16.ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&i8, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&i16, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&ui16, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&i32, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&ui32, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&i64, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&ui64, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&f32, SCAN_EQUAL));
        CPPUNIT_ASSERT(i16.d4_ops(&f64, SCAN_EQUAL));

        CPPUNIT_ASSERT_THROW(i16.d4_ops(&url, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT_THROW(i16.d4_ops(&str, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT_THROW(i16.d4_ops(&array, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT_THROW(i16.ops(0, SCAN_EQUAL), Error);
    }    

    void ops_exception_1_test()
    {
        Byte b1 = Byte("a");
        Int16 i16 = Int16("a", "b");
        b1.set_read_p(false);
        CPPUNIT_ASSERT_THROW(i16.ops(&b1, SCAN_EQUAL), InternalErr);        
    }    

    void ops_exception_2_test()
    {
        Byte b1 = Byte("a");
        Int16 i16 = Int16("a", "b");
        i16.set_read_p(false);
        CPPUNIT_ASSERT_THROW(i16.ops(&b1, SCAN_EQUAL), InternalErr);        
    }    

    void dump_test()
    {
        ofstream ofs("Int16Test_dump.output", ios::trunc);
        i1->set_value(21);
        i1->dump(ofs);
        ofs.close();
        ifstream ifs("Int16Test_dump.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "21"));
    }

    void print_test()
    {
        FILE *fp;
        CPPUNIT_ASSERT(fp = fopen("Int16Test.output", "w"));
        i1->set_value(22);
        i1->print_val(fp, " ", true);
        fclose(fp);
        ifstream ifs("Int16Test.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "22;"));
    }

    void check_types()
    {
        Byte *b1 = new Byte("b");
        b1->set_value(14);
        i1->set_value(14);
//        CPPUNIT_ASSERT(b1 == i1);
        delete b1;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(Int16Test);

} // namespace libdap

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;

        case 'h': {     // help - show test names
            cerr << "Usage: Int16Test has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::Int16Test::suite()->getTests();
            unsigned int prefix_len = libdap::Int16Test::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            return 1;
            break;
        }

        default:
            break;
        }

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::Int16Test::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

