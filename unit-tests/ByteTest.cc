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

// Tests for Byte. Tests features of BaseType, too. 7/19/2001 jhrg

#include <sstream>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>

#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "ce_expr.tab.hh"
#include "crc.h"

#include "GetOpt.h" // Added jhrg

#include "testFile.h"

/// Added jhrg
static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);
/// jhrg

using namespace CppUnit;
using namespace std;
using namespace libdap;

class ByteTest: public TestFixture {
private:
    Byte * tb1;
    Byte *tb2;
    Byte *tb3;
    Byte *tb4;
    Byte *tb5;
    char a[1024];    

public:
    ByteTest()
    {
    }
    ~ByteTest()
    {
    }

    void setUp()
    {
        tb1 = new Byte("tb1");
        tb2 = new Byte("tb2 name with spaces");
        tb3 = new Byte("tb3 %");
        tb4 = new Byte("tb4 #");
        tb5 = new Byte("a", "b");
    }

    void tearDown()
    {
        delete tb1;
        tb1 = 0;
        delete tb2;
        tb2 = 0;
        delete tb3;
        tb3 = 0;
        delete tb4;
        tb4 = 0;
        delete tb5;
        tb5 = 0;
    }

    CPPUNIT_TEST_SUITE(ByteTest);

    CPPUNIT_TEST(cons_test);
    CPPUNIT_TEST(equals_test);
    CPPUNIT_TEST(checksum_test);
    CPPUNIT_TEST(val2buf_test);
    // CPPUNIT_TEST(buf2val_test);
    CPPUNIT_TEST(dump_test);
    CPPUNIT_TEST(print_test);
    CPPUNIT_TEST(type_compare_test);
    CPPUNIT_TEST(name_mangling_test);
    CPPUNIT_TEST(decl_mangling_test);
    CPPUNIT_TEST(basetype_test);

    CPPUNIT_TEST_SUITE_END();

    void cons_test()
    {
        CPPUNIT_ASSERT(tb5->value() == 0 && tb5->dataset() == "b" && tb5->name() == "a" &&
                       tb5->type() == dods_byte_c);
        CPPUNIT_ASSERT(tb2->value() == 0);
    }

    void equals_test()
    {
        Byte b3 = Byte("a", "b");
        Byte b4 = Byte("e");
        
        CPPUNIT_ASSERT(b3.set_value(42) && b3.value() == 42);
        b4 = b3;
        CPPUNIT_ASSERT(b3.value() == 42);
        b4 = b4;
        CPPUNIT_ASSERT(b4.value() == 42);
    }

    void checksum_test()
    {
        Crc32 cs;
        tb5->compute_checksum(cs);
    }

    void val2buf_test()
    {
        char i = 3;
        tb3->val2buf(&i, true);
        CPPUNIT_ASSERT(tb3->value() == 3);        
        CPPUNIT_ASSERT_THROW(tb3->val2buf(NULL, true), InternalErr);
    }

    void buf2val_test()
    {
        char i = 42;
        void *v = &i;
        void *v2 = NULL;
        CPPUNIT_ASSERT(tb1->set_value(6));
        CPPUNIT_ASSERT(tb1->buf2val(&v) == 1 && i == 6);
        CPPUNIT_ASSERT_THROW(tb1->buf2val(NULL), InternalErr);
        CPPUNIT_ASSERT(tb1->buf2val(&v2) == 1 && *(int *)v2 == 6);
    }

    void dump_test()
    {
        ofstream ofs("ByteTest_dump.output", ios::trunc);
        tb1->set_value(21);
        tb1->dump(ofs);
        ofs.close();
        ifstream ifs("ByteTest_dump.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(a[0] == 21);
    }

    void print_test()
    {
        FILE *fp;
        CPPUNIT_ASSERT(fp = fopen("ByteTest.output", "w"));
        tb1->set_value(22);
        tb1->print_val(fp, " ", true);
        fclose(fp);
        ifstream ifs("ByteTest.output");
        while(!ifs.eof())
            ifs >> a;
        ifs.close();
        CPPUNIT_ASSERT(!strcmp(a, "22;"));
    }

    void type_compare_test()
    {
        Byte b1 = Byte("a");
        Int8 i8 = Int8("a");
        Int16 i16 = Int16("a");
        UInt16 ui16 = UInt16("a");
        Int32 i32 = Int32("a", "b");
        UInt32 ui32 = UInt32("a", "b");
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
        f32.set_value(42);
        f64.set_value(42);
        CPPUNIT_ASSERT(b1.value() == 42);
        CPPUNIT_ASSERT(b1.ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&b1, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&i8, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&i16, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&ui16, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&i32, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&ui32, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&f32, SCAN_EQUAL));
        CPPUNIT_ASSERT(b1.d4_ops(&f64, SCAN_EQUAL));

        CPPUNIT_ASSERT_THROW(b1.d4_ops(&url, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT_THROW(b1.d4_ops(&str, SCAN_EQUAL), Error);
        CPPUNIT_ASSERT_THROW(b1.d4_ops(&array, SCAN_EQUAL), Error);
    }    

    void name_mangling_test()
    {
        CPPUNIT_ASSERT(tb1->name() == "tb1");
        CPPUNIT_ASSERT(tb2->name() == "tb2 name with spaces");
        CPPUNIT_ASSERT(tb3->name() == "tb3 %");
        CPPUNIT_ASSERT(tb4->name() == "tb4 #");
    }

    void decl_mangling_test()
    {
        ostringstream sof;
        tb1->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb1") != string::npos);

        tb2->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb2%20name%20with%20spaces") != string::npos);

        tb3->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb3%20%") != string::npos);

        tb4->print_decl(sof, "", false);
        CPPUNIT_ASSERT(sof.str().find("Byte tb4%20%23") != string::npos);
    }

    void basetype_test()
    {
        BaseType *bt = static_cast<BaseType*>(tb1);
        bt->width(false);
        CPPUNIT_ASSERT(!bt->synthesized_p());
        bt->set_synthesized_p(true);
        CPPUNIT_ASSERT(bt->synthesized_p());
        CPPUNIT_ASSERT_THROW(bt->add_var(bt), InternalErr);
        CPPUNIT_ASSERT_THROW(bt->add_var_nocopy(bt), InternalErr);
        CPPUNIT_ASSERT_THROW(bt->ops(bt, SCAN_EQUAL), InternalErr);
//        CPPUNIT_ASSERT_THROW(bt->width(false), InternalErr);        
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (ByteTest);

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
            cerr << "Usage: ByteTest has the following tests:" << endl;
            // Since the ByteTest class is not in the namespace 'libdap' using the ns
            // prefix is a compiler error. Somce of the tests do put the code in a ns
            // and then you will need to use the match prefix. jhrg
            const std::vector<Test*> &tests = /*libdap:: jhrg*/ByteTest::suite()->getTests();
            unsigned int prefix_len = /*libdap:: jhrg*/ByteTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
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
            test = /*libdap:: jhrg*/ByteTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
