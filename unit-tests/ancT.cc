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

#include "Ancillary.h"
#include "DDS.h"
#include "debug.h"

#include "run_tests_cppunit.h"
#include "run_tests_cppunit.h"
#include "test_config.h"


using namespace CppUnit;
using namespace std;
using namespace libdap;

class ancT: public TestFixture {
private:

protected:

public:
    ancT() = default;
    ~ancT() = default;

    CPPUNIT_TEST_SUITE (ancT);

    CPPUNIT_TEST (find_ancillary_file_test);
    CPPUNIT_TEST (find_group_ancillary_file_test);
    CPPUNIT_TEST (read_ancillary_das_file_test);

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
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testste/test_1", "das", (string) TEST_SRC_DIR + "/das-testsuite/", "")
            == (string) TEST_SRC_DIR + "/das-testsuite/test_1.das");
        Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testsuite/te1", "das", (string) TEST_SRC_DIR + "/das-testite/", "");        
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testsuite/te1", "das", (string) TEST_SRC_DIR + "/das-testite/", "")
            == (string) TEST_SRC_DIR + "/das-testsuite/das");
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testste/test1", "das", (string) TEST_SRC_DIR + "/das-testsuite/", "")
            == (string) TEST_SRC_DIR + "/das-testsuite/das");
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testste/test1", "das", (string) TEST_SRC_DIR + "/das-testsuite/", "test.1")
            == (string) TEST_SRC_DIR + "/das-testsuite/test.1.das");
        CPPUNIT_ASSERT(
            Ancillary::find_ancillary_file((string) TEST_SRC_DIR + "/das-testste/test1", "ds", (string) TEST_SRC_DIR + "/das-testsuite/", "")
            == "");
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

    void read_ancillary_das_file_test()
    {
        DAS das;
        string dir = "";
        string file = "";
        Ancillary::read_ancillary_das(das, (string) TEST_SRC_DIR + "/das-testsuite/test.1.das", dir, file);
        CPPUNIT_ASSERT(das.get_size() == 1);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (ancT);

int main(int argc, char *argv[])
{
    return run_tests<ancT>(argc, argv) ? 0: 1;
}
