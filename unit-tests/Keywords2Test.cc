// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2006 OPeNDAP, Inc.
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

//#define DODS_DEBUG

#include "BaseType.h"
#include "Int32.h"
#include "Float64.h"
#include "Str.h"
#include "Array.h"
#include "Grid.h"
#include "DDS.h"
#include "DAS.h"
//#include "ce_functions.h"
#include "Keywords2.h"

#include <test_config.h>

#include "../tests/TestTypeFactory.h"

#include "debug.h"

using namespace CppUnit;
using namespace libdap;
using namespace std;

int test_variable_sleep_interval = 0;

class KeywordsTest: public TestFixture {
private:
    Keywords *k;
public:
    KeywordsTest()
    {
    }

    ~KeywordsTest()
    {
    }

    void setUp()
    {
        k = new Keywords();
    }

    void tearDown()
    {
        delete k;
        k = 0;
    }

    CPPUNIT_TEST_SUITE (KeywordsTest);

    CPPUNIT_TEST (no_keywords_test_1);
    CPPUNIT_TEST (no_keywords_test_2);

    CPPUNIT_TEST (one_keyword_test_1);
    CPPUNIT_TEST (one_keyword_test_2);

    CPPUNIT_TEST (two_keyword_test_1);
    CPPUNIT_TEST (two_keyword_test_2);

    CPPUNIT_TEST (bad_keyword_test_1);
    CPPUNIT_TEST (bad_keyword_test_2);
    CPPUNIT_TEST (bad_keyword_test_3);
    CPPUNIT_TEST (bad_keyword_test_4);

    CPPUNIT_TEST_SUITE_END();

    void no_keywords_test_1()
    {
        string ce = k->parse_keywords("");
        CPPUNIT_ASSERT(!k->has_keyword("dap"));
        CPPUNIT_ASSERT(ce == "");
        CPPUNIT_ASSERT(k->get_keywords().size() == 0);
    }

    void no_keywords_test_2()
    {
        string ce = k->parse_keywords("u,v");
        CPPUNIT_ASSERT(!k->has_keyword("dap"));
        CPPUNIT_ASSERT(ce == "u,v");
        CPPUNIT_ASSERT(k->get_keywords().size() == 0);
    }

    void one_keyword_test_1()
    {
        CPPUNIT_ASSERT(k->is_known_keyword("dap"));

        string ce = k->parse_keywords("dap(2)");
        CPPUNIT_ASSERT(ce == "");
        CPPUNIT_ASSERT(k->has_keyword("dap"));
        CPPUNIT_ASSERT(k->get_keyword_value("dap") == "2");
        CPPUNIT_ASSERT(k->get_keywords().size() == 1);
    }

    void one_keyword_test_2()
    {
        string ce = k->parse_keywords("dap(2),u,v&v<7");
        CPPUNIT_ASSERT(ce == "u,v&v<7");
        CPPUNIT_ASSERT(k->has_keyword("dap"));
        CPPUNIT_ASSERT(k->get_keyword_value("dap") == "2");
        CPPUNIT_ASSERT(k->get_keywords().size() == 1);
    }

    void two_keyword_test_1()
    {
        string ce = k->parse_keywords("dap(2),dap(3.2)");
        CPPUNIT_ASSERT(ce == "");
        CPPUNIT_ASSERT(k->has_keyword("dap"));
        CPPUNIT_ASSERT(k->get_keyword_value("dap") == "3.2");
        CPPUNIT_ASSERT(k->get_keywords().size() == 1);

        CPPUNIT_ASSERT(k->is_known_keyword("dap"));
    }

    void two_keyword_test_2()
    {
        string ce = k->parse_keywords("dap(2),dap(3.2),u,v&v<7");
        CPPUNIT_ASSERT(ce == "u,v&v<7");
        CPPUNIT_ASSERT(k->has_keyword("dap"));
        CPPUNIT_ASSERT(k->get_keyword_value("dap") == "3.2");
        CPPUNIT_ASSERT(k->get_keywords().size() == 1);
    }

    void bad_keyword_test_1()
    {
        try {
            string ce = k->parse_keywords("dap7");
            // Even though this is pretty obviously wrong, we soldier on because
            // The keyword processing code has no way of knowing what will be
            // valid variable names.
            CPPUNIT_ASSERT(ce == "dap7");
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Should not get here");
        }
    }
    void bad_keyword_test_2()
    {
        try {
            string ce = k->parse_keywords("dap(7");
            // Even though this is pretty obviously wrong, we soldier on because
            // The keyword processing code has no way of knowing what will be
            // valid variable names.
            CPPUNIT_ASSERT(ce == "dap(7");
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Should not get here");
        }
    }
    void bad_keyword_test_3()
    {
        try {
            string ce = k->parse_keywords("dap7)");
            // Even though this is pretty obviously wrong, we soldier on because
            // The keyword processing code has no way of knowing what will be
            // valid variable names.
            CPPUNIT_ASSERT(ce == "dap7)");
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Should not get here");
        }
    }
    void bad_keyword_test_4()
    {
        try {
            string ce = k->parse_keywords("dap(7)");
            // Even though this is pretty obviously wrong, we soldier on because
            // The keyword processing code has no way of knowing what will be
            // valid variable names.
            CPPUNIT_FAIL("Should throw - bad value to keyword/function");
        }
        catch (Error &e) {
            CPPUNIT_ASSERT("Should throw - bad value to keyword/function");
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (KeywordsTest);

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
            cerr << "Usage: KeywordsTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::KeywordsTest::suite()->getTests();
            unsigned int prefix_len = libdap::KeywordsTest::suite()->getName().append("::").length();
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
            test = libdap::KeywordsTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    xmlMemoryDump();

    return wasSuccessful ? 0 : 1;
}
