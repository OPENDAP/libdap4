// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2005 OPeNDAP, Inc.
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "GNURegex.h"
#include "Error.h"
#include "debug.h"
#include "GetOpt.h"

using namespace CppUnit;
using namespace libdap;

static bool debug = false;

class RegexTest: public TestFixture {
private:

public:
    RegexTest()
    {
    }
    ~RegexTest()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE (RegexTest);

    CPPUNIT_TEST (ctor_test);
    CPPUNIT_TEST (match_test);
#if 1
    CPPUNIT_TEST (search_test);
#endif

    CPPUNIT_TEST_SUITE_END();

    void ctor_test()
    {
        try {
            Regex r("abc");
            CPPUNIT_ASSERT(1);
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error building object");
        }

        try {
            Regex r("");
            CPPUNIT_ASSERT(1);
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error building object");
        }

        try {
            Regex r(".*");
            CPPUNIT_ASSERT(1);
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error building object");
        }
    }

    void match_test()
    {
        Regex simple("abc");
        string s1 = "123abcdef";
        DBG(cerr << "simple.match(s1.c_str(), s1.length()): "
            << simple.match(s1.c_str(), s1.length()) << endl);

        CPPUNIT_ASSERT(simple.match(s1.c_str(), s1.length()) == 3);
        CPPUNIT_ASSERT(simple.match(s1.c_str(), s1.length(), 4) == -1);

        Regex pattern("3.b");
        DBG(cerr << "pattern.match(s1.c_str(), s1.length()): "
            << pattern.match(s1.c_str(), s1.length()) << endl);

        CPPUNIT_ASSERT(pattern.match(s1.c_str(), s1.length()) == 3);
        string s2 = "123acd";
        CPPUNIT_ASSERT(pattern.match(s2.c_str(), s2.length()) == -1);

        Regex exclusion("[^123]+");
        DBG(cerr << "exclusion.match(s1.c_str(), s1.length()): "
            << exclusion.match(s1.c_str(), s1.length()) << endl);

        CPPUNIT_ASSERT(exclusion.match(s1.c_str(), s1.length()) == 6);

        Regex exclusion_range("[^0-9]+");
        CPPUNIT_ASSERT(exclusion_range.match(s1.c_str(), s1.length()) == 6);
    }

    void search_test()
    {
        int matchlen;

        Regex simple("abc");
        string s1 = "123abcdef";
        CPPUNIT_ASSERT(simple.search(s1.c_str(), s1.length(), matchlen, 0) == 3);
        CPPUNIT_ASSERT(matchlen == 3);
        CPPUNIT_ASSERT(simple.search(s1.c_str(), s1.length(), matchlen, 4) == -1);

        Regex pattern("[a-z]+");
        CPPUNIT_ASSERT(pattern.search(s1.c_str(), s1.length(), matchlen) == 3);
        CPPUNIT_ASSERT(matchlen == 6);

        string s2 = "123abc123abcd";
        CPPUNIT_ASSERT(pattern.search(s2.c_str(), s2.length(), matchlen) == 3);
        CPPUNIT_ASSERT(matchlen == 3);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (RegexTest);

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
            cerr << "Usage: RegexTest has the following tests:" << endl;
            const std::vector<Test*> &tests = RegexTest::suite()->getTests();
            unsigned int prefix_len = RegexTest::suite()->getName().append("::").length();
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
            test = RegexTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
