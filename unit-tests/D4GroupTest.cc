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

//#define DODS_DEBUG 1

#include "D4Group.h"

#include "Byte.h"
#include "Int64.h"

#include "XMLWriter.h"
#include "debug.h"

#include "testFile.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

class D4GroupTest: public TestFixture {
private:
    XMLWriter *xml;
    D4Group *root, *named;

public:
    D4GroupTest() {
    }

    ~D4GroupTest() {
    }

    void setUp() {
        root = new D4Group("");
        named = new D4Group("test");
        xml = new XMLWriter;
    }

    void tearDown() {
        delete xml;
        delete root;
        delete named;
    }

    void load_group_with_scalars(D4Group *g) {
        g->add_var_nocopy(new Byte("b"));
        g->add_var_nocopy(new Int64("i64"));
    }

    void load_group_with_stuff(D4Group *g) {
        g->dims()->add_dim_nocopy(new D4Dimension("lat", 1024));
        g->dims()->add_dim_nocopy(new D4Dimension("lon", 1024));
        g->dims()->add_dim_nocopy(new D4Dimension("time"));

        D4EnumDef *color_values = new D4EnumDef("colors", dods_byte_c);
        color_values->add_value("red", 1);
        color_values->add_value("green", 2);
        color_values->add_value("blue", 3);
        g->enum_defs()->add_enum_nocopy(color_values);

        g->get_attr_table().append_attr("test", "Int16", "1");
    }

    // An empty D4Group object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_print_empty() {
        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_empty.xml");
        DBG(cerr << "test_print_empty: doc: " << doc << endl);
        DBG(cerr << "test_print_empty: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_named_empty() {
        named->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_named_empty.xml");
        DBG(cerr << "test_print_named_empty: doc: " << doc << endl);
        DBG(cerr << "test_print_named_empty: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_with_vars() {
        load_group_with_scalars(root);

        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_with_scalars.xml");
        DBG(cerr << "test_print_with_vars: doc: " << doc << endl);
        DBG(cerr << "test_print_with_vars: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_named_with_vars() {
        load_group_with_scalars(named);

        named->print_dap4(*xml);

        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_named_with_scalars.xml");
        DBG(cerr << "test_print_named_with_vars: doc: " << doc << endl);
        DBG(cerr << "test_print_named_with_vars: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_with_vars_and_stuff() {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_with_scalars_and_stuff.xml");
        DBG(cerr << "test_print_with_vars_and_stuff: doc: " << doc << endl);
        DBG(cerr << "test_print_with_vars_and_stuff: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_named_with_vars_and_stuff() {
        load_group_with_scalars(named);
        load_group_with_stuff(named);

        named->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_named_with_scalars_and_stuff.xml");
        DBG(cerr << "test_print_named_with_vars_and_stuff: doc: " << doc << endl);
        DBG(cerr << "test_print_named_with_vars_and_stuff: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_everything() {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        D4Group *child = new D4Group("child");
        load_group_with_scalars(child);
        load_group_with_stuff(child);
        root->add_group(child);

        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_everything.xml");
        DBG(cerr << "test_print_with_vars_and_stuff: doc: " << doc << endl);
        DBG(cerr << "test_print_with_vars_and_stuff: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_copy_ctor() {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        D4Group *child = new D4Group("child");
        load_group_with_scalars(child);
        load_group_with_stuff(child);
        root->add_group(child);

        D4Group lhs(*root);

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_everything.xml");
        DBG(cerr << "test_print_copy_ctor: doc: " << doc << endl);
        DBG(cerr << "test_print_copy_ctor: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_assignment() {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        D4Group *child = new D4Group("child");
        load_group_with_scalars(child);
        load_group_with_stuff(child);
        root->add_group(child);

        D4Group lhs = *root;

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_everything.xml");
        DBG(cerr << "test_print_assignment: doc: " << doc << endl);
        DBG(cerr << "test_print_assignment: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    CPPUNIT_TEST_SUITE( D4GroupTest );

        CPPUNIT_TEST(test_print_empty);
        CPPUNIT_TEST(test_print_named_empty);
        CPPUNIT_TEST(test_print_with_vars);
        CPPUNIT_TEST(test_print_named_with_vars);

        CPPUNIT_TEST(test_print_with_vars_and_stuff);

        CPPUNIT_TEST(test_print_named_with_vars_and_stuff);
        CPPUNIT_TEST(test_print_everything);

        CPPUNIT_TEST(test_print_copy_ctor);
        CPPUNIT_TEST(test_print_assignment);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(D4GroupTest);

int main(int, char**) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = runner.run("", false);

    return wasSuccessful ? 0 : 1;
}

