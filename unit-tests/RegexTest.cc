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

#include <unistd.h>     // getopt
#include <string>

#include "GNURegex.h"
#include "Error.h"
#include "debug.h"
#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace libdap;



class RegexTest: public TestFixture {
private:
    string s1 = "123abcdef";

public:
    RegexTest() = default;
    ~RegexTest() = default;

    void setUp() {}
    void tearDown() {}

    CPPUNIT_TEST_SUITE (RegexTest);

    CPPUNIT_TEST (litteral_expr_test);
    CPPUNIT_TEST (blank_expr_test);
    CPPUNIT_TEST (simple_regex_test);

    CPPUNIT_TEST (litteral_match_test);
    CPPUNIT_TEST (simple_match_test);
    CPPUNIT_TEST (exclusion_match_test);
    CPPUNIT_TEST (exclusion_range_match_test);

    CPPUNIT_TEST (litteral_search_test);
    CPPUNIT_TEST (pattern_search_test);

    CPPUNIT_TEST_SUITE_END();

    void litteral_expr_test()
    {
        try {
            Regex r("abc");
            CPPUNIT_ASSERT(1);
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Error building object");
        }
    }

    void blank_expr_test() {
        try {
            Regex r("");
            CPPUNIT_ASSERT(1);
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Error building object");
        }
    }

    void simple_regex_test() {
        try {
            Regex r(".*");
            CPPUNIT_ASSERT(1);
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Error building object");
        }
    }

    void litteral_match_test()
    {
        Regex simple("abc");
        DBG(cerr << "simple.match(s1.c_str(), s1.length()): "
        << simple.match(s1.c_str(), s1.length()) << endl);

        CPPUNIT_ASSERT(simple.match(s1.c_str(), s1.length()) == 3);
        CPPUNIT_ASSERT(simple.match(s1.c_str(), s1.length(), 4) == -1);
    }

    void simple_match_test() {
        Regex pattern("3.b");
        DBG(cerr << "pattern.match(s1.c_str(), s1.length()): "
        << pattern.match(s1.c_str(), s1.length()) << endl);

        CPPUNIT_ASSERT(pattern.match(s1.c_str(), s1.length()) == 3);
        string s2 = "123acd";
        CPPUNIT_ASSERT(pattern.match(s2.c_str(), s2.length()) == -1);
    }

    void exclusion_match_test() {
        Regex exclusion("[^123]+");
        DBG(cerr << "exclusion.match(s1.c_str(), s1.length()): "
        << exclusion.match(s1.c_str(), s1.length()) << endl);

        CPPUNIT_ASSERT(exclusion.match(s1.c_str(), s1.length()) == 6);
    }

    void exclusion_range_match_test() {
        Regex exclusion_range("[^0-9]+");
        CPPUNIT_ASSERT(exclusion_range.match(s1.c_str(), s1.length()) == 6);
    }

    void litteral_search_test()
    {
        int matchlen;
        Regex simple("abc");
        // s1 is "123abcdef"

        CPPUNIT_ASSERT(simple.search(s1.c_str(), s1.length(), matchlen, 0) == 3);
        CPPUNIT_ASSERT(matchlen == 3);
        DBG(cerr << "simple.search(s1.c_str(), s1.length(), matchlen, 4): "
            << simple.search(s1.c_str(), s1.length(), matchlen, 4) << endl);
        CPPUNIT_ASSERT(simple.search(s1.c_str(), s1.length(), matchlen, 4) == -1);
    }

    void pattern_search_test() {
        int matchlen;
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
    return run_tests<RegexTest>(argc, argv) ? 0: 1;
}
