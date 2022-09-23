// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

#include <cstring>

#include <iostream>
#include <fstream>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>



#include "DMR.h"
#include "D4Group.h"
#include "XMLWriter.h"
#include "Array.h"

#include "D4BaseTypeFactory.h"
#include "D4ParserSax2.h"
#include "D4Maps.h"

#include "InternalErr.h"
#include "debug.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

#include "testFile.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;


static bool parser_debug = false;


class D4ParserSax2Test: public TestFixture {
private:
    D4ParserSax2 *parser;
    DMR *dmr;
    D4BaseTypeFactory *btf;
    XMLWriter *xml;

public:
    D4ParserSax2Test() :
        parser(0), dmr(0), btf(0), xml(0)
    {
    }
    ~D4ParserSax2Test()
    {
    }

    void setUp()
    {
        parser = new D4ParserSax2();
        btf = new D4BaseTypeFactory;
        dmr = new DMR(btf);
        xml = new XMLWriter("    ");
    }

    void tearDown()
    {
        delete parser;
        delete dmr;
        delete btf;
        delete xml;
    }

    void compare_dmr_round_trip(const string &src, const string &bl)
    {
        try {
            string name = string(TEST_SRC_DIR) + src;
            ifstream ifile(name.c_str(), ifstream::in);
            if (!ifile) throw InternalErr(__FILE__, __LINE__, "Could not open file: " + src);

            parser->intern(ifile, dmr, parser_debug);

            ifile.close();

            dmr->print_dap4(*xml, false);
            string doc = xml->get_doc();
            string baseline = read_test_baseline(string(TEST_SRC_DIR) + bl);
            DBG(cerr << "RESULT DMR: " << doc << endl);
            CPPUNIT_ASSERT(doc == baseline);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message().c_str());
        }
    }

    void compare_dmr_round_trip_string_version(const string &src, const string &bl)
    {
        try {
            string document = read_test_baseline(string(TEST_SRC_DIR) + src);
            DBG(cerr << "Parsing: " << document << endl);

            parser->intern(document, dmr, parser_debug);

            dmr->print_dap4(*xml, false);
            string doc = xml->get_doc();
            string baseline = read_test_baseline(string(TEST_SRC_DIR) + bl);
            DBG(cerr << "RESULT DMR: " << doc << endl);
            CPPUNIT_ASSERT(doc == baseline);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message().c_str());
        }
    }

    void test_empty_dmr()
    {
        compare_dmr_round_trip("/D4-xml/DMR_empty.xml", "/D4-xml/DMR_empty_baseline.xml");

    }

    void test_empty_dmr_string_version()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_empty.xml", "/D4-xml/DMR_empty_baseline.xml");
    }

    void test_attribute_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_0.xml", "/D4-xml/DMR_0_baseline.xml");

    }

    void test_attribute_def_string_version()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_0.xml", "/D4-xml/DMR_0_baseline.xml");
    }

    void test_nested_attribute_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_0.1.xml", "/D4-xml/DMR_0.1_baseline.xml");
    }

    void test_dimension_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_1.xml", "/D4-xml/DMR_1_baseline.xml");
    }

    void test_enum_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_2.xml", "/D4-xml/DMR_2_baseline.xml");
    }

    void test_enum_def2()
    {
        compare_dmr_round_trip("/D4-xml/DMR_2.1.xml", "/D4-xml/DMR_2.1_baseline.xml");
    }

    void test_simple_var_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_3.xml", "/D4-xml/DMR_3_baseline.xml");
    }

    void test_simple_var_with_attributes_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_3.1.xml", "/D4-xml/DMR_3.1_baseline.xml");
    }

    void test_array_var_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_3.2.xml", "/D4-xml/DMR_3.2_baseline.xml");
    }

    void test_array_var_def2()
    {
        compare_dmr_round_trip("/D4-xml/DMR_3.3.xml", "/D4-xml/DMR_3.3_baseline.xml");
    }

    void test_array_var_def3()
    {
        compare_dmr_round_trip("/D4-xml/DMR_3.4.xml", "/D4-xml/DMR_3.4_baseline.xml");
    }

    void test_array_var_def4()
    {
        compare_dmr_round_trip("/D4-xml/DMR_3.5.xml", "/D4-xml/DMR_3.5_baseline.xml");
    }

    void test_array_var_def4_string_version()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_3.5.xml", "/D4-xml/DMR_3.5_baseline.xml");
    }

    void test_all_simple_var_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_4.xml", "/D4-xml/DMR_4_baseline.xml");
    }

    void test_opaque_var_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_4.1.xml", "/D4-xml/DMR_4.1_baseline.xml");
    }

    void test_structure_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_5.xml", "/D4-xml/DMR_5_baseline.xml");
    }

    void test_structure_with_attributes_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_5.1.xml", "/D4-xml/DMR_5.1_baseline.xml");
    }

    void test_structure_with_attributes_def_string_version()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_5.1.xml", "/D4-xml/DMR_5.1_baseline.xml");
    }

    void test_group_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_6.xml", "/D4-xml/DMR_6_baseline.xml");
    }

    void test_group_with_attributes_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_6.1.xml", "/D4-xml/DMR_6.1_baseline.xml");
    }

    void test_group_with_enums_def()
    {
        compare_dmr_round_trip("/D4-xml/DMR_6.2.xml", "/D4-xml/DMR_6.2_baseline.xml");
    }

    void test_group_with_attributes_def_string_version()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_6.1.xml", "/D4-xml/DMR_6.1_baseline.xml");
    }

    void test_array_1()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_7.xml", "/D4-xml/DMR_7_baseline.xml");
    }

    void test_array_2()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_7.1.xml", "/D4-xml/DMR_7.1_baseline.xml");
    }

    void test_array_3()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_7.2.xml", "/D4-xml/DMR_7.2_baseline.xml");
    }

    void test_array_4()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_7.3.xml", "/D4-xml/DMR_7.3_baseline.xml");
    }

    void test_array_5()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_7.4.xml", "/D4-xml/DMR_7.4_baseline.xml");
    }

    void test_array_6()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_7.5.xml", "/D4-xml/DMR_7.5_baseline.xml");
    }

    void test_map_1()
    {
        compare_dmr_round_trip_string_version("/D4-xml/DMR_8.xml", "/D4-xml/DMR_8_baseline.xml");

        // NB: dmr is global
        Array *b1 = dynamic_cast<Array*>(dmr->root()->var("b1"));
        CPPUNIT_ASSERT(b1 && b1->name() == "b1");
        Array *x = dynamic_cast<Array*>(dmr->root()->var("x"));	// this is the map
        CPPUNIT_ASSERT(x && x->name() == "x");

        D4Maps::D4MapsIter m = b1->maps()->map_begin(); // there's only one map...
        CPPUNIT_ASSERT((*m)->name() == "/x");
        CPPUNIT_ASSERT((*m)->array() == x);
        // 'parent' is no longer in the class. jhrg 9/16/22 CPPUNIT_ASSERT((*m)->parent() == b1);
    }

    CPPUNIT_TEST_SUITE (D4ParserSax2Test);

    CPPUNIT_TEST (test_empty_dmr);
    CPPUNIT_TEST (test_dimension_def);
    CPPUNIT_TEST (test_attribute_def);
    CPPUNIT_TEST (test_nested_attribute_def);
    CPPUNIT_TEST (test_enum_def);
    CPPUNIT_TEST (test_enum_def2);
    CPPUNIT_TEST (test_simple_var_def);
    CPPUNIT_TEST (test_simple_var_with_attributes_def);
    CPPUNIT_TEST (test_array_var_def);
    CPPUNIT_TEST (test_array_var_def2);
    CPPUNIT_TEST (test_array_var_def3);
    CPPUNIT_TEST (test_array_var_def4);
    CPPUNIT_TEST (test_all_simple_var_def);
    CPPUNIT_TEST (test_opaque_var_def);

    CPPUNIT_TEST (test_structure_def);
    CPPUNIT_TEST (test_structure_with_attributes_def);
    CPPUNIT_TEST (test_group_def);
    CPPUNIT_TEST (test_group_with_attributes_def);
    CPPUNIT_TEST (test_group_with_enums_def);

    CPPUNIT_TEST (test_empty_dmr_string_version);
    CPPUNIT_TEST (test_attribute_def_string_version);
    CPPUNIT_TEST (test_group_with_attributes_def_string_version);
    CPPUNIT_TEST (test_structure_with_attributes_def_string_version);
    CPPUNIT_TEST (test_array_var_def4_string_version);

    CPPUNIT_TEST (test_array_1);
    CPPUNIT_TEST (test_array_2);
    CPPUNIT_TEST (test_array_3);
    CPPUNIT_TEST (test_array_4);
    CPPUNIT_TEST (test_array_5);
    CPPUNIT_TEST (test_array_6);

    CPPUNIT_TEST (test_map_1);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4ParserSax2Test);

int main(int argc, char*argv[])
{
    return run_tests<D4ParserSax2Test>(argc, argv) ? 0: 1;
}
