
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

#include <iostream>
#include <fstream>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#define DODS_DEBUG 1

#include "D4ParserSax2.h"
#include "DMR.h"
#include "XMLWriter.h"

#include "InternalErr.h"
#include "debug.h"

#include "test_config.h"
#include "testFile.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

namespace libdap {

class D4ParserSax2Test : public TestFixture {
private:
    D4ParserSax2 *parser;
    DMR *dmr;
    D4BaseTypeFactory *btf;
    XMLWriter *xml;

public:
    D4ParserSax2Test() {}
    ~D4ParserSax2Test() {}

    void setUp() {
        parser = new D4ParserSax2;
        btf = new D4BaseTypeFactory;
        dmr = new DMR(btf);
        xml = new XMLWriter("    ");
    }

    void tearDown() {
        delete parser;
        delete dmr;
        delete btf;
        delete xml;
    }

    void test_empty_dmr() {
        try {
            string name = string(TEST_SRC_DIR) + "/D4-xml/DMR_empty.xml";
            ifstream ifile(name.c_str(), ifstream::in);
            if (!ifile)
                throw InternalErr(__FILE__, __LINE__, "Could not open file");

            parser->intern(ifile, dmr);

            ifile.close();

            dmr->print_dap4(*xml, false);
            string doc = xml->get_doc();
            string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/DMR_empty_baseline.xml");
            DBG(cerr << "test_dimension_def DMR: " << doc << endl);
            CPPUNIT_ASSERT(doc == baseline);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message().c_str());
        }
    }

    void test_dimension_def() {
        try {
            string name = string(TEST_SRC_DIR) + "/D4-xml/DMR_1.xml";
            ifstream ifile(name.c_str(), ifstream::in);
            if (!ifile)
                throw InternalErr(__FILE__, __LINE__, "Could not open file");

            parser->intern(ifile, dmr);

            ifile.close();

            dmr->print_dap4(*xml, false);

            string doc = xml->get_doc();
            string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/DMR_1_baseline.xml");
            DBG(cerr << "test_dimension_def DMR: " << doc << endl);
            CPPUNIT_ASSERT(doc == baseline);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message().c_str());
        }
    }

    void test_enum_def() {
        try {
            string name = string(TEST_SRC_DIR) + "/D4-xml/DMR_2.xml";
            ifstream ifile(name.c_str(), ifstream::in);
            if (!ifile)
                throw InternalErr(__FILE__, __LINE__, "Could not open file");

            parser->intern(ifile, dmr);

            ifile.close();

            dmr->print_dap4(*xml, false);
            string doc = xml->get_doc();
            string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/DMR_2_baseline.xml");
            DBG(cerr << "test_dimension_def DMR: " << doc << endl);
            CPPUNIT_ASSERT(doc == baseline);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message().c_str());
        }
    }

    void test_simple_var_def() {
        try {
            string name = string(TEST_SRC_DIR) + "/D4-xml/DMR_3.xml";
            ifstream ifile(name.c_str(), ifstream::in);
            if (!ifile)
                throw InternalErr(__FILE__, __LINE__, "Could not open file");

            parser->intern(ifile, dmr);

            ifile.close();

            dmr->print_dap4(*xml, false);
            string doc = xml->get_doc();
            string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/DMR_3_baseline.xml");
            DBG(cerr << "test_dimension_def DMR: " << doc << endl);
            CPPUNIT_ASSERT(doc == baseline);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message().c_str());
        }
    }

    void test_all_simple_var_def() {
        try {
            string name = string(TEST_SRC_DIR) + "/D4-xml/DMR_4.xml";
            ifstream ifile(name.c_str(), ifstream::in);
            if (!ifile)
                throw InternalErr(__FILE__, __LINE__, "Could not open file");

            parser->intern(ifile, dmr);

            ifile.close();

            dmr->print_dap4(*xml, false);
            string doc = xml->get_doc();
            string baseline = readTestBaseline(string(TEST_SRC_DIR) + "/D4-xml/DMR_4_baseline.xml");
            DBG(cerr << "test_dimension_def DMR: " << doc << endl);
            CPPUNIT_ASSERT(doc == baseline);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message().c_str());
        }
    }

    CPPUNIT_TEST_SUITE( D4ParserSax2Test );

    CPPUNIT_TEST(test_empty_dmr);
    CPPUNIT_TEST(test_dimension_def);
    CPPUNIT_TEST(test_enum_def);
    CPPUNIT_TEST(test_simple_var_def);
    CPPUNIT_TEST(test_all_simple_var_def);

    CPPUNIT_TEST_SUITE_END();

};

}

CPPUNIT_TEST_SUITE_REGISTRATION( D4ParserSax2Test );

int
main( int, char** )
{
    //xmlInitParser();

    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    //xmlCleanupParser();

    return (wasSuccessful) ? 0 : 1;
}
