
// Tests for the cgi util functions.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include <time.h>
#include "cgi_util.h"
#include <strstream.h>
#include <Regex.h>

class cgiUtilTest : public TestCase {
private:
    ostrstream oss;
    streampos start_oss;

public: 
    cgiUtilTest (string name) : TestCase (name) {}

    void setUp() {
	start_oss = oss.tellp();
    }

    void tearDown() {
    }

    void reset_oss() {
	oss.freeze(0);
	oss.seekp(start_oss);
    }	

    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }

    // Tests for methods/functions
    void find_ancillary_file_test() {
	assert(find_ancillary_file("das-testsuite/test.1", "das", "", "") ==
	       "das-testsuite/test.1.das");
    }

    void name_path_test() {
	assert(name_path(string("stuff")) == "stuff");
	assert(name_path(string("stuff.Z")) == "stuff");
	assert(name_path(string("/usr/local/src/stuff.Z")) == "stuff");
	assert(name_path(string("/usr/local/src/stuff.tar.Z")) == "stuff");
	assert(name_path(string("/usr/local/src/stuff")) == "stuff");
	assert(name_path(string("")) == "");
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
	assert(re_match(r1, oss.str()));
	reset_oss();

	Regex r2("HTTP/1.0 200 OK
XDODS-Server: dods-test/0.00
Date: \\(.*\\)
Last-Modified: \\1
Content-type: text/plain
Content-Description: dods_dds

");
	set_mime_text(oss, dods_dds, "dods-test/0.00");	oss << ends;
	assert(re_match(r2, oss.str()));
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
	assert(re_match(r3, oss.str()));
	reset_oss();
    }

    void rfc822_date_test() {
	time_t t = 0;
	assert(strcmp(rfc822_date(t), "Thu, 01 Jan 1970 00:00:00 GMT") == 0);
	struct tm tm = {0, 0, 0, 1, 0, 100, 0, 0, 0}; // 1 Jan 2000
	t = mktime(&tm);
	assert(strcmp(rfc822_date(t), "Sat, 01 Jan 2000 08:00:00 GMT") == 0);
    }

    void last_modified_time_test() {
	time_t t = time(0);
	assert(last_modified_time("no-such-file") == t);
	struct stat st;
	stat("Makefile.in", &st);
	assert(last_modified_time("Makefile.in") == st.st_mtime);
	stat("/etc/passwd", &st);
	assert(last_modified_time("/etc/passwd") == st.st_mtime);
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("cgiUtilTest");

	s->addTest(new TestCaller<cgiUtilTest>
		   ("find_ancillary_file_test",
		    &cgiUtilTest::find_ancillary_file_test));

	s->addTest(new TestCaller<cgiUtilTest>
		   ("name_path_test", &cgiUtilTest::name_path_test));

	s->addTest(new TestCaller<cgiUtilTest>
		   ("set_mime_text_test", &cgiUtilTest::set_mime_text_test));

	s->addTest(new TestCaller<cgiUtilTest>
		   ("rfc822_date_test", &cgiUtilTest::rfc822_date_test));

	s->addTest(new TestCaller<cgiUtilTest>
		   ("last_modified_time_test",
		    &cgiUtilTest::last_modified_time_test));

	return s;
    }
};


