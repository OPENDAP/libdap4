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

using namespace CppUnit;
using namespace std;
using namespace libdap;

class D4EnumDefsTest: public TestFixture {
private:
    XMLWriter *xml;
    D4EnumDefs *d;

    enumValues e, e2;

public:
    D4EnumDefsTest() {
    }

    ~D4EnumDefsTest() {
    }

    void setUp() {
        d = new D4EnumDefs;
        xml = new XMLWriter;

        e.add_value("red", 1);
        e.add_value("blue", 2);

        e2.add_value("snow", 0);
        e2.add_value("ice", 10000);
    }

    void tearDown() {
        delete xml;
        delete d;
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_print_empty() {
        d->print(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_empty.xml");
        DBG(cerr << "test_print_empty: doc: " << doc << endl);
        DBG(cerr << "test_print_empty: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_enum_values_only() {
        enumValues e;
        e.add_value("red", 1);
        e.add_value("blue", 2);

        e.print(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_values_1.xml");
        DBG(cerr << "test_print_enum_values_only: doc: " << doc << endl);
        DBG(cerr << "test_print_enum_values_only: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);

    }

    void test_print_1() {
        d->add_enum("first", dods_byte_c, e);

        d->print(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_1.xml");
        DBG(cerr << "test_print_1: doc: " << doc << endl);
        DBG(cerr << "test_print_1: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }


    void test_print_2() {
        d->add_enum("first", dods_byte_c, e);
        d->add_enum("second", dods_int32_c, e2);

        d->print(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_2.xml");
        DBG(cerr << "test_print_2: doc: " << doc << endl);
        DBG(cerr << "test_print_2: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_insert_enum() {
        d->add_enum("first", dods_byte_c, e);

        // "second' winds up before 'first'
        D4EnumDefs::D4EnumIter i = d->enum_begin();
        d->insert_enum("second", dods_int32_c, e2, i);

        d->print(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_3.xml");
        DBG(cerr << "test_print_insert_enum: doc: " << doc << endl);
        DBG(cerr << "test_print_insert_enum: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_assignment() {
        d->add_enum("first", dods_byte_c, e);
        d->add_enum("second", dods_int32_c, e2);

        D4EnumDefs lhs = *d;

        lhs.print(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_2.xml");
        DBG(cerr << "test_print_assignment: doc: " << doc << endl);
        DBG(cerr << "test_print_assignment: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_copy_ctor() {
        d->add_enum("first", dods_byte_c, e);
        d->add_enum("second", dods_int32_c, e2);

        D4EnumDefs lhs(*d);

        lhs.print(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4EnumDefs_2.xml");
        DBG(cerr << "test_print_copy_ctor: doc: " << doc << endl);
        DBG(cerr << "test_print_copy_ctor: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    CPPUNIT_TEST_SUITE( D4EnumDefsTest );

        CPPUNIT_TEST(test_print_empty);
        CPPUNIT_TEST(test_print_enum_values_only);
        CPPUNIT_TEST(test_print_1);
        CPPUNIT_TEST(test_print_2);

        CPPUNIT_TEST(test_print_insert_enum);
        CPPUNIT_TEST(test_print_assignment);
        CPPUNIT_TEST(test_print_copy_ctor);

        CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(D4EnumDefsTest);

int main(int, char**) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = runner.run("", false);

    return wasSuccessful ? 0 : 1;
}

