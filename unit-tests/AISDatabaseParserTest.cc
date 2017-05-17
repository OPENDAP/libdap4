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

// Tests for the AISResources class.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "AISDatabaseParser.h"
#include <test_config.h>
#include "debug.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

class AISDatabaseParserTest: public TestFixture {
private:
    string fnoc1, fnoc2, fnoc3, number, bears, three_fnoc;
    string fnoc1_ais, fnoc2_ais, fnoc3_ais, number_ais;
    AISDatabaseParser *ais_parser;
    AISResources *ais;

public:
    AISDatabaseParserTest()
    {
    }
    ~AISDatabaseParserTest()
    {
    }

    void setUp()
    {
        fnoc1 = "http://test.opendap.org/opendap/data/nc/fnoc1.nc";
        fnoc2 = "http://test.opendap.org/opendap/data/nc/fnoc2.nc";
        fnoc3 = "http://test.opendap.org/opendap/data/nc/fnoc3.nc";
        // number is the regular expression that will be in the
        // ais_database.xml file.
        number = "http://test.opendap.org/opendap/data/nc/[0-9]+.*\\.nc";
        bears = "http://test.opendap.org/opendap/data/nc/123bears.nc";
        three_fnoc = "http://test.opendap.org/opendap/data/nc/3fnoc.nc";

        fnoc1_ais = "http://test.opendap.org/ais/fnoc1.nc.das";
        fnoc2_ais = (string) TEST_SRC_DIR + "/ais_testsuite/fnoc2_replace.das";
        fnoc3_ais = "http://test.opendap.org/ais/fnoc3_fallback.das";
        number_ais = (string) TEST_SRC_DIR + "/ais_testsuite/starts_with_number.das";

        ais = new AISResources;
        ais_parser = new AISDatabaseParser();
    }

    void tearDown()
    {
        delete ais_parser;
        ais_parser = 0;
        delete ais;
        ais = 0;
    }

    CPPUNIT_TEST_SUITE (AISDatabaseParserTest);

    CPPUNIT_TEST (intern_test);
    CPPUNIT_TEST (errant_database_test);

    CPPUNIT_TEST_SUITE_END();

    void intern_test()
    {
        try {
            ais_parser->intern("ais_testsuite/ais_database.xml", ais);

            ResourceVector trv1 = ais->get_resource(fnoc1);
            CPPUNIT_ASSERT(trv1.size() == 1);
            CPPUNIT_ASSERT(trv1[0].get_url() == fnoc1_ais);
            CPPUNIT_ASSERT(trv1[0].get_rule() == Resource::overwrite);

            ResourceVector trv2 = ais->get_resource(fnoc2);
            CPPUNIT_ASSERT(trv2.size() == 1);
            CPPUNIT_ASSERT(trv2[0].get_url() == fnoc2_ais);
            CPPUNIT_ASSERT(trv2[0].get_rule() == Resource::replace);

            ResourceVector trv3 = ais->get_resource(fnoc3);
            CPPUNIT_ASSERT(trv3.size() == 1);
            CPPUNIT_ASSERT(trv3[0].get_url() == fnoc3_ais);
            CPPUNIT_ASSERT(trv3[0].get_rule() == Resource::fallback);

            ResourceVector trv4 = ais->get_resource(bears);
            CPPUNIT_ASSERT(trv4.size() == 1);
            CPPUNIT_ASSERT(trv4[0].get_url() == number_ais);
            CPPUNIT_ASSERT(trv4[0].get_rule() == Resource::overwrite);
        }
        catch (AISDatabaseReadFailed &e) {
            cerr << endl << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Parse failed.");
        }
    }

    void errant_database_test()
    {
        try {
            ais_parser->intern((string) TEST_SRC_DIR + "/" + "ais_testsuite/ais_error_1.xml", ais);
            CPPUNIT_ASSERT(!"ais_error_1.xml should fail!");
        }
        catch (AISDatabaseReadFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }

        try {
            ais_parser->intern((string) TEST_SRC_DIR + "/" + "ais_testsuite/ais_error_2.xml", ais);
            CPPUNIT_ASSERT(!"ais_error_2.xml should fail!");
        }
        catch (AISDatabaseReadFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }

        try {
            ais_parser->intern((string) TEST_SRC_DIR + "/" + "ais_testsuite/ais_error_3.xml", ais);
            CPPUNIT_ASSERT(!"ais_error_3.xml should fail!");
        }
        catch (AISDatabaseReadFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }

        try {
            ais_parser->intern((string) TEST_SRC_DIR + "/" + "ais_testsuite/ais_error_4.xml", ais);
            CPPUNIT_ASSERT(!"ais_error_4.xml should fail!");
        }
        catch (AISDatabaseReadFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }

        try {
            ais_parser->intern((string) TEST_SRC_DIR + "/" + "ais_testsuite/ais_error_5.xml", ais);
            CPPUNIT_ASSERT(!"ais_error_5.xml should fail!");
        }
        catch (AISDatabaseReadFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }

        try {
            ais_parser->intern((string) TEST_SRC_DIR + "/" + "ais_testsuite/ais_error_6.xml", ais);
            CPPUNIT_ASSERT(!"ais_error_6.xml should fail!");
        }
        catch (AISDatabaseReadFailed &e) {
            DBG(cerr << "Error: " << e.get_error_message() << endl);
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (AISDatabaseParserTest);

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
            cerr << "Usage: AISDatabaseParserTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::AISDatabaseParserTest::suite()->getTests();
            unsigned int prefix_len = libdap::AISDatabaseParserTest::suite()->getName().append("::").length();
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
            test = libdap::AISDatabaseParserTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
