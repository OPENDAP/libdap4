
// Tests for the cgi util functions.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <time.h>
#include <strstream.h>
#include <Regex.h>

#include "cgi_util.h"

#include "cgiUtilTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(cgiUtilTest);

using namespace CppUnit;

cgiUtilTest::cgiUtilTest()
{
}

cgiUtilTest::~cgiUtilTest()
{
}

void 
cgiUtilTest::setUp() 
{
    start_oss = oss.tellp();
}

void 
cgiUtilTest::tearDown() 
{
}

void 
cgiUtilTest::reset_oss() 
{
    oss.freeze(0);
    oss.seekp(start_oss);
}	

bool 
cgiUtilTest::re_match(Regex &r, const char *s) 
{
    return r.match(s, strlen(s)) == (int)strlen(s);
}

// Tests for methods/functions
void 
cgiUtilTest::find_ancillary_file_test() 
{
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

void cgiUtilTest::find_group_ancillary_file_test() 
{
    CPPUNIT_ASSERT(find_group_ancillary_file("cgi-util-tests/02group.hdf", ".htm")
		   == "cgi-util-tests/group.htm");
    CPPUNIT_ASSERT(find_group_ancillary_file("cgi-util-tests/group01.hdf", ".htm")
		   == "cgi-util-tests/group.htm");
    CPPUNIT_ASSERT(find_group_ancillary_file("cgi-util-tests/group.hdf", ".htm")
		   == "");
}

void 
cgiUtilTest::name_path_test() 
{
    CPPUNIT_ASSERT(name_path(string("stuff")) == "stuff");
    CPPUNIT_ASSERT(name_path(string("stuff.Z")) == "stuff.Z");
    CPPUNIT_ASSERT(name_path(string("/usr/local/src/stuff.Z")) == "stuff.Z");
    CPPUNIT_ASSERT(name_path(string("/usr/local/src/stuff.tar.Z")) == "stuff.tar.Z");
    CPPUNIT_ASSERT(name_path(string("/usr/local/src/stuff")) == "stuff");
    CPPUNIT_ASSERT(name_path(string("/usr/local/src/#usr#local#src#stuff")) == "stuff");
    CPPUNIT_ASSERT(name_path(string("/usr/local/src/#usr#local#src#stuff.hdf")) == "stuff.hdf");
    CPPUNIT_ASSERT(name_path(string("")) == "");
}

void 
cgiUtilTest::set_mime_text_test() 
{
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

void 
cgiUtilTest::rfc822_date_test() 
{
    time_t t = 0;
    CPPUNIT_ASSERT(strcmp(rfc822_date(t), "Thu, 01 Jan 1970 00:00:00 GMT") == 0);
    struct tm tm = {0, 0, 0, 1, 0, 100, 0, 0, 0}; // 1 Jan 2000
    t = mktime(&tm);
    CPPUNIT_ASSERT(strcmp(rfc822_date(t), "Sat, 01 Jan 2000 08:00:00 GMT") == 0);
}

void 
cgiUtilTest::last_modified_time_test() 
{
    time_t t = time(0);
    CPPUNIT_ASSERT(last_modified_time("no-such-file") == t);
    struct stat st;
    stat("Makefile.in", &st);
    CPPUNIT_ASSERT(last_modified_time("Makefile.in") == st.st_mtime);
    stat("/etc/passwd", &st);
    CPPUNIT_ASSERT(last_modified_time("/etc/passwd") == st.st_mtime);
}

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}
