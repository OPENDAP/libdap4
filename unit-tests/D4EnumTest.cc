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
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Enum_1.xml");
        DBG(cerr << "test_print: doc: " << doc << endl);
        DBG(cerr << "test_print: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
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
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Enum_2.txt");
        DBG(cerr << "test_print: doc: " << doc << endl);
        DBG(cerr << "test_print: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
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

    CPPUNIT_TEST (test_value);
    CPPUNIT_TEST (test_value2);

    CPPUNIT_TEST (test_copy_ctor);
    CPPUNIT_TEST (test_assignment);

    CPPUNIT_TEST (test_print);
    CPPUNIT_TEST (test_print_val);

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
