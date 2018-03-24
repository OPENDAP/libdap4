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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

//#define DODS_DEBUG

#include "D4Enum.h"
#include "D4EnumDefs.h"
#include "XMLWriter.h"
#include "debug.h"
#include "GetOpt.h"

#include "testFile.h"
#include "dods-limits.h"
#include "test_config.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

using namespace CppUnit;
using namespace std;
using namespace libdap;

namespace libdap {
class D4EnumTest: public TestFixture {
private:
    XMLWriter *xml;
    D4Enum *d;

public:
    D4EnumTest() :
        xml(0), d(0)
    {
    }

    ~D4EnumTest()
    {
    }

    void setUp()
    {
        d = new D4Enum("test", "Byte");
        xml = new XMLWriter;
    }

    void tearDown()
    {
        delete xml;
        delete d;
    }

    void test_first_ctor()
    {
        D4Enum e("first", "Byte");
        CPPUNIT_ASSERT(e.d_element_type == dods_byte_c);
        CPPUNIT_ASSERT(e.name() == "first");
    }

    // Tests bogus type name
    void test_first_ctor2()
    {
        D4Enum e("bogus", "String");
        CPPUNIT_ASSERT(e.d_element_type == dods_uint64_c);
        CPPUNIT_ASSERT(e.name() == "bogus");
    }

    void test_second_ctor()
    {
        D4Enum e("second", dods_byte_c);
        CPPUNIT_ASSERT(e.d_element_type == dods_byte_c);
        CPPUNIT_ASSERT(e.name() == "second");
    }

    // Tests bogus type name
    void test_second_ctor2()
    {
        D4Enum e("bogus", dods_str_c);
        CPPUNIT_ASSERT(e.d_element_type == dods_uint64_c);
        CPPUNIT_ASSERT(e.name() == "bogus");
    }

    void test_set_value()
    {
        D4Enum e("second", dods_byte_c);
        dods_byte db = 200;
        e.set_value(db);
        CPPUNIT_ASSERT(e.d_buf == 200);
    }

    void test_set_value2()
    {
        D4Enum e("second", dods_byte_c);
        e.set_value(200);
        CPPUNIT_ASSERT(e.d_buf == 200);
    }

    void test_set_value3()
    {
        D4Enum e("third", dods_int32_c);
        e.set_value(-65535);
        CPPUNIT_ASSERT((dods_int32)e.d_buf == -65535);
    }

    void test_set_value_all()
    {
        D4Enum b("b", dods_byte_c);
        b.set_value(100);
        CPPUNIT_ASSERT((dods_byte)b.d_buf == 100);
        CPPUNIT_ASSERT_THROW(b.set_value((dods_int64)DODS_UCHAR_MAX + 1), Error);
        CPPUNIT_ASSERT_THROW(b.set_value(-100), Error);
        dods_byte uchar = 42, uchar_in;
        void *uchar_inp = &uchar_in;
        b.val2buf(&uchar, true);        
        CPPUNIT_ASSERT((dods_byte)b.d_buf == uchar);
        b.buf2val(&uchar_inp);
        CPPUNIT_ASSERT(uchar_in == uchar);

        D4Enum i8("i8", dods_int8_c);
        i8.set_value(100);
        CPPUNIT_ASSERT((dods_int8)i8.d_buf == 100);
        CPPUNIT_ASSERT_THROW(i8.set_value((dods_int64)DODS_SCHAR_MAX + 1), Error);
        CPPUNIT_ASSERT_THROW(i8.set_value((dods_int64)DODS_SCHAR_MIN - 1), Error);
        dods_int8 int8 = -43, int8_in;
        void *int8_inp = &int8_in;
        i8.val2buf(&int8, true);        
        CPPUNIT_ASSERT((dods_int8)i8.d_buf == int8);
        i8.buf2val(&int8_inp);
        CPPUNIT_ASSERT(int8_in == int8);

        D4Enum i16("i16", dods_int16_c);
        i16.set_value(100);        
        CPPUNIT_ASSERT((dods_int16)i16.d_buf == 100);
        CPPUNIT_ASSERT_THROW(i16.set_value((dods_int64)DODS_SHRT_MAX + 1), Error);
        CPPUNIT_ASSERT_THROW(i16.set_value((dods_int64)DODS_SHRT_MIN - 1), Error);
        dods_int16 int16 = -44, int16_in;
        void *int16_inp = &int16_in;
        i16.val2buf(&int16, true);        
        CPPUNIT_ASSERT((dods_int16)i16.d_buf == int16);
        i16.buf2val(&int16_inp);
        CPPUNIT_ASSERT(int16_in == int16);

        D4Enum ui16("ui16", dods_uint16_c);
        ui16.set_value(100);        
        CPPUNIT_ASSERT((dods_uint16)ui16.d_buf == 100);
        CPPUNIT_ASSERT_THROW(ui16.set_value((dods_int64)DODS_USHRT_MAX + 1), Error);
        CPPUNIT_ASSERT_THROW(ui16.set_value((dods_int64)-1), Error);
        dods_uint16 uint16 = 45, uint16_in;
        void *uint16_inp = &uint16_in;
        ui16.val2buf(&uint16, true);        
        CPPUNIT_ASSERT((dods_uint16)ui16.d_buf == uint16);
        ui16.buf2val(&uint16_inp);
        CPPUNIT_ASSERT(uint16_in == uint16);

        D4Enum i32("i32", dods_int32_c);
        i32.set_value(100);        
        CPPUNIT_ASSERT((dods_int32)i32.d_buf == 100);
        CPPUNIT_ASSERT_THROW(i32.set_value((dods_int64)DODS_INT_MAX + 1), Error);
        CPPUNIT_ASSERT_THROW(i32.set_value((dods_int64)DODS_INT_MIN - 1), Error);
        dods_int32 int32 = -46, int32_in;
        void *int32_inp = &int32_in;
        i32.val2buf(&int32, true);        
        CPPUNIT_ASSERT((dods_int32)i32.d_buf == int32);
        i32.buf2val(&int32_inp);
        CPPUNIT_ASSERT(int32_in == int32);

        D4Enum ui32("ui32", dods_uint32_c);
        ui32.set_value(100);        
        CPPUNIT_ASSERT((dods_uint32)ui32.d_buf == 100);
        CPPUNIT_ASSERT_THROW(ui32.set_value((dods_int64)DODS_UINT_MAX + 1), Error);
        CPPUNIT_ASSERT_THROW(ui32.set_value((dods_int64)-1), Error);
        dods_uint32 uint32 = 47, uint32_in;
        void *uint32_inp = &uint32_in;
        ui32.val2buf(&uint32, true);        
        CPPUNIT_ASSERT((dods_uint32)ui32.d_buf == uint32);
        ui32.buf2val(&uint32_inp);
        CPPUNIT_ASSERT(uint32_in == uint32);

        D4Enum i64("i64", dods_int64_c);
        i64.set_value(100);        
        CPPUNIT_ASSERT((dods_int64)i64.d_buf == 100);
        dods_int64 int64 = -48, int64_in;
        void *int64_inp = &int64_in;
        i64.val2buf(&int64, true);        
        CPPUNIT_ASSERT((dods_int64)i64.d_buf == int64);
        i64.buf2val(&int64_inp);
        CPPUNIT_ASSERT(int64_in == int64);

        D4Enum ui64("ui64", dods_uint64_c);
        ui64.set_value(100);        
        CPPUNIT_ASSERT((dods_uint64)ui64.d_buf == 100);
        dods_uint64 uint64 = 49, uint64_in;
        void *uint64_inp = &uint64_in;
        ui64.val2buf(&uint64, true);        
        CPPUNIT_ASSERT((dods_uint64)ui64.d_buf == uint64);
        ui64.buf2val(&uint64_inp);
        CPPUNIT_ASSERT(uint64_in == uint64);
    }

    void test_ctor()
    {
        D4Enum ui64("ui64", "dataset", dods_uint64_c);
        CPPUNIT_ASSERT(ui64.dataset() == "dataset");
        CPPUNIT_ASSERT_THROW(ui64.set_is_signed(dods_float32_c), InternalErr);
        CPPUNIT_ASSERT_THROW(ui64.val2buf(0, true), InternalErr);
        CPPUNIT_ASSERT_THROW(ui64.buf2val(0), InternalErr);
    }

    void test_value()
    {
        D4Enum e("second", dods_byte_c);
        e.set_value(200);
        dods_byte db;
        e.value(&db);
        CPPUNIT_ASSERT(db == 200);
    }

    void test_value2()
    {
        D4Enum e("third", dods_int32_c);
        e.set_value(-65535);
        int32_t db;
        e.value(&db);
        CPPUNIT_ASSERT(db == -65535);
    }

    void test_copy_ctor()
    {
        D4Enum e("second", dods_byte_c);
        e.set_value(200);

        D4Enum f(e);
        CPPUNIT_ASSERT(f.d_element_type == dods_byte_c);
        CPPUNIT_ASSERT(f.name() == "second");
        CPPUNIT_ASSERT(f.d_buf == 200);
    }

    void test_assignment()
    {
        D4Enum e("second", dods_byte_c);
        e.set_value(200);

        D4Enum f = e;
        CPPUNIT_ASSERT(f.d_element_type == dods_byte_c);
        CPPUNIT_ASSERT(f.name() == "second");
        CPPUNIT_ASSERT(f.d_buf == 200);
    }

    void test_print()
    {
        D4Enum e("test", dods_byte_c);
        D4EnumDef enum_def("Colors", dods_byte_c);

        e.set_enumeration(&enum_def);
        e.set_value((dods_byte) 200);

        XMLWriter xml;

        e.print_dap4(xml);
        string doc = xml.get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Enum_1.xml");
        DBG(cerr << "test_print: doc: " << doc << endl);
        DBG(cerr << "test_print: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_2()
    {
        D4Enum e("test", dods_int32_c);
        D4EnumDef enum_def("Colors", dods_int32_c);

        e.set_enumeration(&enum_def);
        e.set_value(200);

        ostringstream oss;

        e.print_val(oss, "", true);
        CPPUNIT_ASSERT(oss.str().find("Enum test = 200;") != string::npos);

    }

    void test_print_val()
    {
        D4Enum e("test", dods_byte_c);
        D4EnumDef enum_def("Colors", dods_byte_c);

        e.set_enumeration(&enum_def);
        e.set_value(200);

        ostringstream oss;

        e.print_val(oss, "", true);
        string doc = oss.str();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Enum_2.txt");
        DBG(cerr << "test_print: doc: " << doc << endl);
        DBG(cerr << "test_print: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_dump()
    {
        D4Enum e("spock", dods_byte_c);
        D4EnumDef enum_def("Colors", dods_byte_c);

        e.set_enumeration(&enum_def);
        e.set_value(200);

        ostringstream sof;        
        e.dump(sof);
        CPPUNIT_ASSERT(sof.str().find("name: spock") != string::npos);
    }

    CPPUNIT_TEST_SUITE (D4EnumTest);

    CPPUNIT_TEST (test_first_ctor);
#ifdef NDEBUG
    CPPUNIT_TEST(test_first_ctor2);
#endif
    CPPUNIT_TEST (test_second_ctor);
#ifdef NDEBUG
    CPPUNIT_TEST(test_second_ctor2);
#endif

    CPPUNIT_TEST (test_set_value);
    CPPUNIT_TEST (test_set_value2);
    CPPUNIT_TEST (test_set_value3);
    CPPUNIT_TEST (test_set_value_all);

    CPPUNIT_TEST (test_ctor);
    CPPUNIT_TEST (test_value);
    CPPUNIT_TEST (test_value2);

    CPPUNIT_TEST (test_copy_ctor);
    CPPUNIT_TEST (test_assignment);

    CPPUNIT_TEST (test_print);
    CPPUNIT_TEST (test_print_2);
    CPPUNIT_TEST (test_print_val);
    CPPUNIT_TEST (test_dump);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4EnumTest);

}

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;
    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: D4EnumTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::D4EnumTest::suite()->getTests();
            unsigned int prefix_len = libdap::D4EnumTest::suite()->getName().append("::").length();
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
            test = libdap::D4EnumTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
