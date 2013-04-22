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

#include <GetOpt.h> // Part of libdap

//#define DODS_DEBUG

#include "D4Dimensions.h"
#include "XMLWriter.h"

#include "Error.h"
#include "debug.h"

#include "testFile.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

class D4DimensionsTest: public TestFixture {
private:
    XMLWriter *xml;
    D4Dimensions *d;

public:
    D4DimensionsTest() {
    }

    ~D4DimensionsTest() {
    }

    void setUp() {
        d = new D4Dimensions;
        xml = new XMLWriter;
    }

    void tearDown() {
        delete xml;
        delete d;
    }

    // An empty D4Dimensions object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_print_empty() {
        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Dimensions_empty.xml");
        DBG(cerr << "test_print_empty: doc: " << doc << endl);
        DBG(cerr << "test_print_empty: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_1() {
        d->add_dim_nocopy(new D4Dimension("first", 10));

        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Dimensions_1.xml");
        DBG(cerr << "test_print_1: doc: " << doc << endl);
        DBG(cerr << "test_print_1: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_2() {
        d->add_dim_nocopy(new D4Dimension("first", 10));
        d->add_dim_nocopy(new D4Dimension("second", 100));

        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Dimensions_2.xml");
        DBG(cerr << "test_print_2: doc: " << doc << endl);
        DBG(cerr << "test_print_2: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_error() {
        D4Dimension *d = new D4Dimension();
        d->set_name("error");
        d->set_size("10");

        D4Dimension *d2 = new D4Dimension();
        d2->set_name("error");
        d2->set_size("20.0");
        CPPUNIT_FAIL("Should throw an Error");
    }

    void test_error_2() {
        D4Dimension *d3 = new D4Dimension();
        d3->set_name("error");
        d3->set_size("bad");
        CPPUNIT_FAIL("Should throw an Error");
    }

    void test_print_varying() {
        d->add_dim_nocopy(new D4Dimension("first", 10));
        d->add_dim_nocopy(new D4Dimension("second", 100));
        d->add_dim_nocopy(new D4Dimension("third"));

        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Dimensions_3.xml");
        DBG(cerr << "test_print_varying: doc: " << doc << endl);
        DBG(cerr << "test_print_varying: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_insert_dim() {
        d->add_dim_nocopy(new D4Dimension("first", 10));
        d->add_dim_nocopy(new D4Dimension("second", 100));
        d->add_dim_nocopy(new D4Dimension("third"));

        //vector<D4Dimensions::dimension>::iterator i = d->dim_begin() + 1;
        D4Dimensions::D4DimensionsIter i = d->dim_begin() + 1;
        d->insert_dim_nocopy(new D4Dimension("odd", 20), i);

        d->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Dimensions_4.xml");
        DBG(cerr << "test_print_insert_dim: doc: " << doc << endl);
        DBG(cerr << "test_print_insert_dim: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_assignment() {
        d->add_dim_nocopy(new D4Dimension("first", 10));
        d->add_dim_nocopy(new D4Dimension("second", 100));
        d->add_dim_nocopy(new D4Dimension("third"));

        D4Dimensions lhs = *d;

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Dimensions_3.xml");
        DBG(cerr << "test_print_assignment: doc: " << doc << endl);
        DBG(cerr << "test_print_assignment: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_copy_ctor() {
        d->add_dim_nocopy(new D4Dimension("first", 10));
        d->add_dim_nocopy(new D4Dimension("second", 100));
        d->add_dim_nocopy(new D4Dimension("third"));

        D4Dimensions lhs(*d);

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Dimensions_3.xml");
        DBG(cerr << "test_print_copy_ctor: doc: " << doc << endl);
        DBG(cerr << "test_print_copy_ctor: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    CPPUNIT_TEST_SUITE( D4DimensionsTest );

        CPPUNIT_TEST(test_print_empty);
        CPPUNIT_TEST(test_print_1);
        CPPUNIT_TEST(test_print_2);

        CPPUNIT_TEST_EXCEPTION( test_error, Error );
        CPPUNIT_TEST_EXCEPTION( test_error_2, Error );

        CPPUNIT_TEST(test_print_varying);
        CPPUNIT_TEST(test_print_insert_dim);
        CPPUNIT_TEST(test_print_assignment);
        CPPUNIT_TEST(test_print_copy_ctor);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(D4DimensionsTest);

int main(int argc, char*argv[]) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    GetOpt getopt(argc, argv, "d");
    char option_char;

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        default:
            break;
        }

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        while (i < argc) {
            test = string("D4DimensionsTest::") + argv[i++];

            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

