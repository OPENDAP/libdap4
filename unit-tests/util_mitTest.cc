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

// Tests for the util functions in util.cc and escaping.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstring>
#include <string>

#include "util_mit.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace std;
using namespace CppUnit;
using namespace libdap;

class util_mitTest: public TestFixture {
private:

public:
    util_mitTest() = default;
    ~util_mitTest() = default;

    CPPUNIT_TEST_SUITE(util_mitTest);

    CPPUNIT_TEST(test_parse_time_1);
    CPPUNIT_TEST(test_parse_time_2);
    CPPUNIT_TEST(test_parse_time_3);
    CPPUNIT_TEST(test_parse_time_4);
    CPPUNIT_TEST(test_parse_time_5);
    CPPUNIT_TEST(test_parse_time_6);
    CPPUNIT_TEST(test_parse_time_7);
    CPPUNIT_TEST(test_parse_time_8);
    CPPUNIT_TEST(test_parse_time_9);
    CPPUNIT_TEST(test_parse_time_10);
    CPPUNIT_TEST(test_parse_time_11);
    CPPUNIT_TEST(test_parse_time_12);

    CPPUNIT_TEST(test_date_time_str_1);
    CPPUNIT_TEST(test_date_time_str_2);
    CPPUNIT_TEST(test_date_time_str_3);
    CPPUNIT_TEST(test_date_time_str_4);
    CPPUNIT_TEST(test_date_time_str_5);

    CPPUNIT_TEST_SUITE_END();

    void test_date_time_str_1() {
        time_t t = 726974999;
        DBG(cerr << "date_time_str(" << t << "): " << date_time_str(&t) << endl);
        CPPUNIT_ASSERT_MESSAGE("date_time_str(726974999) should return a string", date_time_str(&t) == "Thu, 14 Jan 1993 01:29:59 GMT");
    }

    void test_date_time_str_2() {
        time_t t = 0;
        DBG(cerr << "date_time_str(" << t << "): " << date_time_str(&t) << endl);
        CPPUNIT_ASSERT_MESSAGE("date_time_str(726974999) should return a string", date_time_str(&t) == "Thu, 01 Jan 1970 00:00:00 GMT");
    }

    // Tricky, in localtime, 0 it before 1 Jan 1970
    void test_date_time_str_3() {
        time_t t = 0;
        DBG(cerr << "date_time_str(" << t << ", true): " << date_time_str(&t, true) << endl);
        CPPUNIT_ASSERT_MESSAGE("date_time_str(0, true) should return a string", date_time_str(&t, true) == "Wed, 31 Dec 1969 17:00:00");
    }

    void test_date_time_str_4() {
        DBG(cerr << "date_time_str(" << nullptr << "): " << date_time_str(nullptr) << endl);
        CPPUNIT_ASSERT_MESSAGE("date_time_str(726974999) should return a string", date_time_str(nullptr) == "");
    }

    void test_date_time_str_5() {
        time_t t = 0xFFFFFFFF;
        DBG(cerr << "date_time_str(" << t << "): " << date_time_str(&t) << endl);
        CPPUNIT_ASSERT_MESSAGE("date_time_str(726974999) should return a string", date_time_str(&t) == "Sun, 07 Feb 2106 06:28:15 GMT");
    }

    void test_parse_time_1() {
        CPPUNIT_ASSERT_MESSAGE("parse_time(nullptr) should return zero.", parse_time(nullptr) == 0);
        CPPUNIT_ASSERT_MESSAGE("parse_time(nullptr, false) should return zero.", parse_time(nullptr, false) == 0);
    }

    // Thursday, 10-Jun-93 01:29:59 GMT, Thu, 10 Jan 1993 01:29:59 GMT == 726974999
    // Thu Jan 01 1970 00:00:01 GMT == 1

    void test_parse_time_2() {
        const char *time = "Thursday, 14-Jan-93 01:29:59 GMT";
        const unsigned int time_val = 726974999; // see https://www.unixtimestamp.com/
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thursday, 14-Jan-93 01:29:59 GMT) should return a time.",
                               parse_time(time) == time_val);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thursday, 14-Jan-93 01:29:59 GMT) should return a time.",
                               parse_time(time, false) == time_val);
    }

    void test_parse_time_3() {
        // The leading comma is needed to use the first block of conversion code.
        const char *time = ",14-Jan-93 01:29:59";
        const unsigned int time_val = 726974999; // see https://www.unixtimestamp.com/
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14-Jan-93 01:29:59) should return a time.",
                               parse_time(time) == time_val);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14-Jan-93 01:29:59) should return a time.",
                               parse_time(time, false) == time_val);
    }

    void test_parse_time_4() {
        const char *time = ",14-Jan-9301:29:59";
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14-Jan-9301:29:59) should return 0 (<18 chars).",
                               parse_time(time) == 0);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14-Jan-9301:29:59) should return 0 (<18 chars).",
                               parse_time(time, false) == 0);
    }

    void test_parse_time_5() {
        const char *time = "Thu, 14 Jan 1993 01:29:59 GMT";
        const unsigned int time_val = 726974999;
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thu, 14 Jan 1993 01:29:59 GMT) should return a time.",
                               parse_time(time) == time_val);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thu, 14 Jan 1993 01:29:59 GMT) should return a time.",
                               parse_time(time, false) == time_val);
    }

    void test_parse_time_6() {
        // The leading comma is needed to use the first block of conversion code.
        const char *time = ",14 Jan 1993 01:29:59";
        const unsigned int time_val = 726974999;
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14 Jan 1993 01:29:59) should return a time.",
                               parse_time(time) == time_val);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14 Jan 1993 01:29:59) should return a time.",
                               parse_time(time, false) == time_val);
    }

    void test_parse_time_7() {
        const char *time = ",14 Jan 199301:29:59";
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14 Jan 199301:29:59) should return 0 (<20 chars).",
                               parse_time(time) == 0);
        CPPUNIT_ASSERT_MESSAGE("parse_time(14 Jan 199301:29:59) should return 0 (<20 chars).",
                               parse_time(time, false) == 0);
    }

    // YYYY.MM.DDThh:mmStzWkd
    void test_parse_time_8() {
        const char *time = "1993.01.14T01:29:59+00:00"; // cf. https://en.wikipedia.org/wiki/ISO_8601
        const unsigned int time_val = 726974999;
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thu, 14 Jan 1993 01:29:59 GMT) should return 0 (length < 21).",
                               parse_time(time) == time_val);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thu, 14 Jan 1993 01:29:59 GMT) should return 0 (length < 21).",
                               parse_time(time, false) == time_val);
    }

    void test_parse_time_9() {
        const char *time = "1993.01.14T01:29:59";
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thu, 14 Jan 1993 01:29:59 GMT) should return 0 (length < 21).",
                               parse_time(time) == 0);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thu, 14 Jan 1993 01:29:59 GMT) should return 0 (length < 21).",
                               parse_time(time, false) == 0);
    }

    void test_parse_time_10() {
        const char *time = "Thursday, 14-Jan-93 24:29:59 GMT";  // 24 is not a valid hour
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(<hour value too large>) should return 0.",
                               parse_time(time) == 0);
        CPPUNIT_ASSERT_MESSAGE("parse_time(<hour value too large>) should return 0.",
                               parse_time(time, false) == 0);
    }

    // Mon 00 00:00:00 0000 GMT
    void test_parse_time_11() {
        const char *time = "Jan 14 01:29:59 1993 GMT";
        const unsigned int time_val = 726974999;
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thursday Jan 14 01:29:59 1993 GMT) should return a time.",
                               parse_time(time) == time_val);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Thursday Jan 14 01:29:59 1993 GMT) should return a time.",
                               parse_time(time, false) == time_val);
    }

    // Mon 00 00:00:00 0000 GMT
    void test_parse_time_12() {
        const char *time = "Jan 14 01:29:59 2022 GMT";
        const unsigned int time_val = 1642123799;
        DBG(cerr << "parse_time(" << time << "): " << parse_time(time) << ", string length: " << strnlen(time, 64) << endl);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Jan 14 01:29:59 2022 GMT) should return a time.",
                               parse_time(time) == time_val);
        CPPUNIT_ASSERT_MESSAGE("parse_time(Jan 14 01:29:59 2022 GMT) should return a time.",
                               parse_time(time, false) == time_val);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(util_mitTest);

int main(int argc, char*argv[])
{
    return run_tests<util_mitTest>(argc, argv) ? 0: 1;
}
