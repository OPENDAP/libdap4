
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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
#include <strstream.h>

#include "Regex.h"

#include "cgi_util.h"

using namespace CppUnit;
using std::streampos ;
using std::ends ;

class cgiUtilTest : public TestFixture {
private:
    ostrstream oss;
    streampos start_oss;

protected:
    void reset_oss() {
	oss.freeze(0);
	oss.seekp(start_oss);
    }	

    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }



public:
    cgiUtilTest() {}
    ~cgiUtilTest() {}
    
    void setUp () {
	start_oss = oss.tellp();
    }

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
	CPPUNIT_ASSERT(find_ancillary_file("das-testsuite/test.1", "das", "", "") 
		       == "das-testsuite/test.1.das");
	CPPUNIT_ASSERT(find_ancillary_file("das-testsuite/special.test.hdf", "das", "", "") 
		       == "das-testsuite/special.test.das");
	CPPUNIT_ASSERT(find_ancillary_file("das-testsuite/test.2", "das", "", "") 
		       == "das-testsuite/das");
	CPPUNIT_ASSERT(find_ancillary_file("das-testsuite/test.1.gz", "das", "", "") 
		       == "das-testsuite/test.1.das");
	CPPUNIT_ASSERT(find_ancillary_file("das-testsuite/test.3.Z", "das", "", "") 
		       == "das-testsuite/test.3.Z.das");
    }
 
    void find_group_ancillary_file_test() {
	CPPUNIT_ASSERT(find_group_ancillary_file("cgi-util-tests/02group.hdf", ".htm")
		       == "cgi-util-tests/group.htm");
	CPPUNIT_ASSERT(find_group_ancillary_file("cgi-util-tests/group01.hdf", ".htm")
		       == "cgi-util-tests/group.htm");
	CPPUNIT_ASSERT(find_group_ancillary_file("cgi-util-tests/group.hdf", ".htm")
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

    void set_mime_text_test() {
	Regex r1("HTTP/1.0 200 OK
XDODS-Server: dods-test/0.00
Date: \\(.*\\)
Last-Modified: \\1
Content-type: text/plain
Content-Description: dods_das

");
	set_mime_text(oss, dods_das, "dods-test/0.00");	oss << ends;
	CPPUNIT_ASSERT(re_match(r1, oss.str()));
	reset_oss();

	Regex r2("HTTP/1.0 200 OK
XDODS-Server: dods-test/0.00
Date: \\(.*\\)
Last-Modified: \\1
Content-type: text/plain
Content-Description: dods_dds

");
	set_mime_text(oss, dods_dds, "dods-test/0.00");	oss << ends;
	CPPUNIT_ASSERT(re_match(r2, oss.str()));
	reset_oss();

	struct tm tm = {0, 0, 0, 1, 0, 100, 0, 0, 0}; // 1 Jan 2000
	time_t t = mktime(&tm);
	Regex r3("HTTP/1.0 200 OK
XDODS-Server: dods-test/0.00
Date: \\(.*\\)
Last-Modified: Sat, 01 Jan 2000 08:00:00 GMT
Content-type: text/plain
Content-Description: dods_dds

");
	set_mime_text(oss, dods_dds, "dods-test/0.00", x_plain, t);
	oss << ends;
	CPPUNIT_ASSERT(re_match(r3, oss.str()));
	reset_oss();
    }
 
    void rfc822_date_test() {
	time_t t = 0;
	CPPUNIT_ASSERT(rfc822_date(t) == "Thu, 01 Jan 1970 00:00:00 GMT");
	struct tm tm = {0, 0, 0, 1, 0, 100, 0, 0, 0}; // 1 Jan 2000
	t = mktime(&tm);
	CPPUNIT_ASSERT(rfc822_date(t) == "Sat, 01 Jan 2000 08:00:00 GMT");
    }

    void last_modified_time_test() {
	time_t t = time(0);
	CPPUNIT_ASSERT(last_modified_time("no-such-file") == t);
	struct stat st;
	stat("Makefile.in", &st);
	CPPUNIT_ASSERT(last_modified_time("Makefile.in") == st.st_mtime);
	stat("/etc/passwd", &st);
	CPPUNIT_ASSERT(last_modified_time("/etc/passwd") == st.st_mtime);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(cgiUtilTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

// $Log: cgiUtilTest.cc,v $
// Revision 1.6  2003/02/03 21:42:36  pwest
// added tests/Makefile.in and rearranged test files so no duplication
//
// Revision 1.5  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.4  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.8  2002/12/24 00:55:34  jimg
// Fixed the rfc822_date test.
//
// Revision 1.1.2.7  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.1.2.6  2002/10/23 17:40:38  jimg
// Added compile-time switch for time.h versus sys/time.h.
//
// Revision 1.1.2.5  2002/06/19 06:05:36  jimg
// Built a single file version of the cgi_util tests. This should be easy to
// clone for other test suites.
//
