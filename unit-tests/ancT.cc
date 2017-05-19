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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef TM_IN_SYS_TIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>   // for stat

#include <string>
#include <sstream>

#include "GetOpt.h"

#include "Ancillary.h"

#include "debug.h"
#include <test_config.h>

using namespace CppUnit;
using namespace std;
using namespace libdap;

static bool debug = false;

class ancT: public TestFixture {
private:

protected:

public:
    ancT()
    {
    }
    ~ancT()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE (ancT);

    CPPUNIT_TEST (find_ancillary_file_test);
    CPPUNIT_TEST (find_group_ancillary_file_test);

    CPPUNIT_TEST_SUITE_END();

    void find_ancillary_file_test()
    {
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testsuite/test.1", "das", "", "")
                == (string) TEST_SRC_DIR + "/das-testsuite/test.1.das");
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testsuite/special.test.hdf", "das", "", "")
                == (string) TEST_SRC_DIR + "/das-testsuite/special.test.das");
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testsuite/test.2", "das", "", "")
                == (string) TEST_SRC_DIR + "/das-testsuite/das");
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testsuite/test.1.gz", "das", "", "")
                == (string) TEST_SRC_DIR + "/das-testsuite/test.1.das");
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testsuite/test.3.Z", "das", "", "")
                == (string) TEST_SRC_DIR + "/das-testsuite/test.3.Z.das");
    }

    void find_group_ancillary_file_test()
    {
        CPPUNIT_ASSERT(
            Ancillary::find_group_ancillary_file((string) TEST_SRC_DIR + "/cgi-util-tests/02group.hdf", ".htm")
                == (string) TEST_SRC_DIR + "/cgi-util-tests/group.htm");
        CPPUNIT_ASSERT(
            Ancillary::find_group_ancillary_file((string) TEST_SRC_DIR + "/cgi-util-tests/group01.hdf", ".htm")
                == (string) TEST_SRC_DIR + "/cgi-util-tests/group.htm");
        CPPUNIT_ASSERT(
            Ancillary::find_group_ancillary_file((string) TEST_SRC_DIR + "/cgi-util-tests/group.hdf", ".htm") == "");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (ancT);

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
            cerr << "Usage: ancT has the following tests:" << endl;
            const std::vector<Test*> &tests = ancT::suite()->getTests();
            unsigned int prefix_len = ancT::suite()->getName().append("::").length();
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
            test = ancT::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
