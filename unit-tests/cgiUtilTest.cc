
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
#include <unistd.h>

#include <string>
#include <sstream>

//#define DODS_DEBUG

#include "GNURegex.h"

#include "cgi_util.h"
#include "debug.h"
#include <test_config.h>

#include "testFile.cc"

using namespace CppUnit;
using namespace std;
using namespace libdap;

class cgiUtilTest : public TestFixture {
private:

protected:
    bool re_match(Regex &r, const string &s) {
	int match = r.match(s.c_str(), s.length());
	DBG(cerr << "RE Match: " << match << endl);
	return match == (int)s.length();
    }

public:
    cgiUtilTest() {}
    ~cgiUtilTest() {}
    
    void setUp () {}

    void tearDown() {}

    CPPUNIT_TEST_SUITE( cgiUtilTest );

    CPPUNIT_TEST(find_ancillary_file_test);
    CPPUNIT_TEST(find_group_ancillary_file_test);
    CPPUNIT_TEST(name_path_test);
    CPPUNIT_TEST(set_mime_text_test);
    CPPUNIT_TEST(rfc822_date_test);
    CPPUNIT_TEST(last_modified_time_test);

    CPPUNIT_TEST_SUITE_END();

    void find_ancillary_file_test() {
	CPPUNIT_ASSERT(find_ancillary_file((string)TEST_SRC_DIR + "/das-testsuite/test.1", "das", "", "") 
		       == (string)TEST_SRC_DIR + "/das-testsuite/test.1.das");
	CPPUNIT_ASSERT(find_ancillary_file((string)TEST_SRC_DIR + "/das-testsuite/special.test.hdf", "das", "", "") 
		       == (string)TEST_SRC_DIR + "/das-testsuite/special.test.das");
	CPPUNIT_ASSERT(find_ancillary_file((string)TEST_SRC_DIR + "/das-testsuite/test.2", "das", "", "") 
		       == (string)TEST_SRC_DIR + "/das-testsuite/das");
	CPPUNIT_ASSERT(find_ancillary_file((string)TEST_SRC_DIR + "/das-testsuite/test.1.gz", "das", "", "") 
		       == (string)TEST_SRC_DIR + "/das-testsuite/test.1.das");
	CPPUNIT_ASSERT(find_ancillary_file((string)TEST_SRC_DIR + "/das-testsuite/test.3.Z", "das", "", "") 
		       == (string)TEST_SRC_DIR + "/das-testsuite/test.3.Z.das");
    }
 
    void find_group_ancillary_file_test() {
	CPPUNIT_ASSERT(find_group_ancillary_file((string)TEST_SRC_DIR + "/cgi-util-tests/02group.hdf", ".htm")
		       == (string)TEST_SRC_DIR + "/cgi-util-tests/group.htm");
	CPPUNIT_ASSERT(find_group_ancillary_file((string)TEST_SRC_DIR + "/cgi-util-tests/group01.hdf", ".htm")
		       == (string)TEST_SRC_DIR + "/cgi-util-tests/group.htm");
	CPPUNIT_ASSERT(find_group_ancillary_file((string)TEST_SRC_DIR + "/cgi-util-tests/group.hdf", ".htm")
		       == "");
    }

    void name_path_test() {
	CPPUNIT_ASSERT(name_path(string("stuff")) == "stuff");
	CPPUNIT_ASSERT(name_path(string("stuff.Z")) == "stuff.Z");
	CPPUNIT_ASSERT(name_path(string("/usr/local/src/stuff.Z")) == "stuff.Z");
	CPPUNIT_ASSERT(name_path(string("/usr/local/src/stuff.tar.Z")) == "stuff.tar.Z");
	CPPUNIT_ASSERT(name_path(string("/usr/local/src/stuff")) == "stuff");
	CPPUNIT_ASSERT(name_path(string("/usr/local/src/#usr#local#src#stuff")) == "stuff");
	CPPUNIT_ASSERT(name_path(string("/usr/local/src/#usr#local#src#stuff.hdf")) == "stuff.hdf");
	CPPUNIT_ASSERT(name_path(string("")) == "");
    }

    // See note above. jhrg 1/18/06
    void set_mime_text_test() {
	Regex r1("HTTP/1.0 200 OK\r\n\
XDODS-Server: dods-test/0.00\r\n\
XOPeNDAP-Server: dods-test/0.00\r\n\
XDAP: .*\r\n\
Date: (.*)\r\n\
Last-Modified: \\1\r\n\
Content-Type: text/plain\r\n\
Content-Description: dods_das\r\n\
\r\n.*");
        string oss;
        FILE2string(oss, tmp, set_mime_text(tmp, dods_das, "dods-test/0.00"));
        DBG(cerr << "DODS DAS" << endl << oss);
      	CPPUNIT_ASSERT(re_match(r1, oss));

	Regex r2("HTTP/1.0 200 OK\r\n\
XDODS-Server: dods-test/0.00\r\n\
XOPeNDAP-Server: dods-test/0.00\r\n\
XDAP: .*\r\n\
Date: (.*)\r\n\
Last-Modified: \\1\r\n\
Content-Type: text/plain\r\n\
Content-Description: dods_dds\r\n\
\r\n.*");
	FILE2string(oss, tmp, set_mime_text(tmp, dods_dds, "dods-test/0.00"));
	DBG(cerr << "DODS DDS" << endl << oss);
	CPPUNIT_ASSERT(re_match(r2, oss));

	struct tm tm = {0, 0, 0, 1, 0, 100, 0, 0, 0, 0, 0}; // 1 Jan 2000
	time_t t = mktime(&tm);
	Regex r3("HTTP/1.0 200 OK\r\n\
XDODS-Server: dods-test/0.00\r\n\
XOPeNDAP-Server: dods-test/0.00\r\n\
XDAP: .*\r\n\
Date: .*\r\n\
Last-Modified: Sat, 01 Jan 2000 ..:00:00 GMT\r\n\
Content-Type: text/plain\r\n\
Content-Description: dods_dds\r\n\
\r\n.*");
	FILE2string(oss, tmp,
                    set_mime_text(tmp, dods_dds, "dods-test/0.00", x_plain, t));
	CPPUNIT_ASSERT(re_match(r3, oss));
    }
 
    void rfc822_date_test() {
	time_t t = 0;
	CPPUNIT_ASSERT(rfc822_date(t) == "Thu, 01 Jan 1970 00:00:00 GMT");
	struct tm tm = {0, 0, 0, 1, 0, 100, 0, 0, 0, 0, 0}; // 1 Jan 2000
	t = mktime(&tm);
	// This test may fail for some locations since mktime interprets t as
	// the local time and returns the corresponding GMT time.
	Regex r1("Sat, 01 Jan 2000 ..:00:00 GMT");
	CPPUNIT_ASSERT(re_match(r1, rfc822_date(t)));
    }

    void last_modified_time_test() {
	time_t t = time(0);
	CPPUNIT_ASSERT(last_modified_time("no-such-file") == t);
	struct stat st;
	stat("test_config.h", &st);
	CPPUNIT_ASSERT(last_modified_time("test_config.h") == st.st_mtime);
	stat("/etc/passwd", &st);
	CPPUNIT_ASSERT(last_modified_time("/etc/passwd") == st.st_mtime);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(cgiUtilTest);

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return wasSuccessful ? 0 : 1;
}
