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

//#define DODS_DEBUG

#include "D4EnumDefs.h"
#include "XMLWriter.h"
#include "debug.h"

#include "testFile.h"
#include "test_config.h"

#include "GetOpt.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

static bool debug = false;

class D4EnumDefsTest: public TestFixture {
private:
    XMLWriter *xml;
    D4EnumDefs *d;

    D4EnumDef e, e2;

public:
    D4EnumDefsTest()
    {
    }

    ~D4EnumDefsTest()
    {
    }

    void setUp()
    {
        d = new D4EnumDefs;
        xml = new XMLWriter;

        e.set_name("first");
        e.set_type(dods_byte_c);
        e.add_value("red", 1);
        e.add_value("blue", 2);

        e2.set_name("second");
        e2.set_type(dods_int32_c);
        e2.add_value("snow", 0);
        e2.add_value("ice", 10000);
    }

    void tearDown()
    {
        delete xml;
        delete d;
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_print_empty()
    {
        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_empty.xml");
        DBG(cerr << "test_print_empty: doc: " << doc << endl);DBG(cerr << "test_print_empty: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_enum_values_only()
    {
        D4EnumDef e;
        e.add_value("red", 1);
        e.add_value("blue", 2);

        e.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_values_1.xml");
        DBG(cerr << "test_print_enum_values_only: doc: " << doc << endl);DBG(cerr << "test_print_enum_values_only: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);

    }

    void test_print_1()
    {
        d->add_enum(&e);

        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_1.xml");
        DBG(cerr << "test_print_1: doc: " << doc << endl);DBG(cerr << "test_print_1: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_2()
    {
        d->add_enum(&e);
        d->add_enum(&e2);

        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_2.xml");
        DBG(cerr << "test_print_2: doc: " << doc << endl);DBG(cerr << "test_print_2: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_insert_enum()
    {
        d->add_enum(&e);

        // "second' winds up before 'first'
        D4EnumDefs::D4EnumDefIter i = d->enum_begin();
        d->insert_enum(&e2, i);

        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_3.xml");
        DBG(cerr << "test_print_insert_enum: doc: " << doc << endl);DBG(cerr << "test_print_insert_enum: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_assignment()
    {
        d->add_enum(&e);
        d->add_enum(&e2);

        D4EnumDefs lhs = *d;

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_2.xml");
        DBG(cerr << "test_print_assignment: doc: " << doc << endl);DBG(cerr << "test_print_assignment: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_copy_ctor()
    {
        d->add_enum(&e);
        d->add_enum(&e2);

        D4EnumDefs lhs(*d);

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_2.xml");
        DBG(cerr << "test_print_copy_ctor: doc: " << doc << endl);DBG(cerr << "test_print_copy_ctor: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    CPPUNIT_TEST_SUITE (D4EnumDefsTest);

    CPPUNIT_TEST (test_print_empty);
    CPPUNIT_TEST (test_print_enum_values_only);
    CPPUNIT_TEST (test_print_1);
    CPPUNIT_TEST (test_print_2);

    CPPUNIT_TEST (test_print_insert_enum);
    CPPUNIT_TEST (test_print_assignment);
    CPPUNIT_TEST (test_print_copy_ctor);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4EnumDefsTest);

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
            cerr << "Usage: D4EnumDefsTest has the following tests:" << endl;
            const std::vector<Test*> &tests = D4EnumDefsTest::suite()->getTests();
            unsigned int prefix_len = D4EnumDefsTest::suite()->getName().append("::").length();
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
            test = D4EnumDefsTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

