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

#include "GNURegex.h"

#include "Error.h"
#include "mime_util.h"
#include "debug.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

static bool debug = false;

class cgiUtilTest: public TestFixture {
private:

protected:
    bool re_match(Regex &r, const string &s)
    {
        int match = r.match(s.c_str(), s.length());
        DBG(cerr << "RE Match: " << match << endl);
        return match == (int) s.length();
    }

public:
    cgiUtilTest()
    {
    }
    ~cgiUtilTest()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE (cgiUtilTest);
#if 1
    CPPUNIT_TEST (name_path_test);
    CPPUNIT_TEST (set_mime_text_test);
    CPPUNIT_TEST (rfc822_date_test);
    CPPUNIT_TEST (last_modified_time_test);
    CPPUNIT_TEST (read_multipart_headers_test);
#endif
    CPPUNIT_TEST (get_next_mime_header_test);

    CPPUNIT_TEST_SUITE_END();

    void name_path_test()
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

    // See note above. jhrg 1/18/06
    void set_mime_text_test()
    {
        Regex r1(
            "HTTP/1.0 200 OK\r\n\
XDODS-Server: dods-test/0.00\r\n\
XOPeNDAP-Server: dods-test/0.00\r\n\
XDAP: .*\r\n\
Date: (.*)\r\n\
Last-Modified: \\1\r\n\
Content-Type: text/plain\r\n\
Content-Description: dods_das\r\n\
\r\n.*");
        ostringstream oss;
        set_mime_text(oss, dods_das, "dods-test/0.00");
        DBG(cerr << "DODS DAS" << endl << oss.str());
        CPPUNIT_ASSERT(re_match(r1, oss.str()));

        Regex r2(
            "HTTP/1.0 200 OK\r\n\
XDODS-Server: dods-test/0.00\r\n\
XOPeNDAP-Server: dods-test/0.00\r\n\
XDAP: .*\r\n\
Date: (.*)\r\n\
Last-Modified: \\1\r\n\
Content-Type: text/plain\r\n\
Content-Description: dods_dds\r\n\
\r\n.*");

        oss.str("");
        set_mime_text(oss, dods_dds, "dods-test/0.00");
        DBG(cerr << "DODS DDS" << endl << oss);
        CPPUNIT_ASSERT(re_match(r2, oss.str()));

        struct tm tm = { 0, 0, 0, 1, 0, 100, 0, 0, 0, 0, 0 }; // 1 Jan 2000
        time_t t = mktime(&tm);
        Regex r3(
            "HTTP/1.0 200 OK\r\n\
XDODS-Server: dods-test/0.00\r\n\
XOPeNDAP-Server: dods-test/0.00\r\n\
XDAP: .*\r\n\
Date: .*\r\n\
Last-Modified: (Sat, 01 Jan 2000 ..:00:00 GMT|Fri, 31 Dec 1999 ..:00:00 GMT)\r\n\
Content-Type: text/plain\r\n\
Content-Description: dods_dds\r\n\
\r\n.*");

        oss.str("");
        set_mime_text(oss, dods_dds, "dods-test/0.00", x_plain, t);
        DBG(cerr << "set_mime_text_test: oss: " << oss.str() << endl);
        CPPUNIT_ASSERT(re_match(r3, oss.str()));
    }

    void rfc822_date_test()
    {
        time_t t = 0;
        CPPUNIT_ASSERT(rfc822_date(t) == "Thu, 01 Jan 1970 00:00:00 GMT");
        struct tm tm = { 0, 0, 0, 1, 0, 100, 0, 0, 0, 0, 0 }; // 1 Jan 2000
        t = mktime(&tm);
        // This test may fail for some locations since mktime interprets t as
        // the local time and returns the corresponding GMT time.
        Regex r1("(Sat, 01 Jan 2000 ..:00:00 GMT|Fri, 31 Dec 1999 ..:00:00 GMT)");
        CPPUNIT_ASSERT(re_match(r1, rfc822_date(t)));
    }

    void last_modified_time_test()
    {
        time_t t = time(0);
        CPPUNIT_ASSERT(last_modified_time("no-such-file") == t);
        struct stat st;
        stat("test_config.h", &st);
        CPPUNIT_ASSERT(last_modified_time("test_config.h") == st.st_mtime);
        stat("/etc/passwd", &st);
        CPPUNIT_ASSERT(last_modified_time("/etc/passwd") == st.st_mtime);
    }

    void get_next_mime_header_test()
    {
        string test_file = (string) TEST_SRC_DIR + "/cgi-util-tests/multipart_mime_header1.txt";
        FILE *in = fopen(test_file.c_str(), "r");
        if (!in) CPPUNIT_FAIL("Could not open the mime header file.");

        try {
            CPPUNIT_ASSERT(get_next_mime_header(in) == "--my_boundary");
            CPPUNIT_ASSERT(get_next_mime_header(in) == "Content-Type: Text/xml; charset=iso-8859-1");
            CPPUNIT_ASSERT(get_next_mime_header(in) == "Content-Description: dods-ddx");
            CPPUNIT_ASSERT(get_next_mime_header(in) == "Content-Id: <1234@opendap.org>");
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }

        fclose(in);
    }

    void read_multipart_headers_test()
    {
        string test_file = (string) TEST_SRC_DIR + "/cgi-util-tests/multipart_mime_header1.txt";
        FILE *in = fopen(test_file.c_str(), "r");
        if (!in) CPPUNIT_FAIL("Could not open the mime header file.");

        try {
            read_multipart_headers(in, "text/xml", dods_ddx);
            CPPUNIT_ASSERT(true);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }

        fclose(in);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (cgiUtilTest);

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
            cerr << "Usage: cgiUtilTest has the following tests:" << endl;
            const std::vector<Test*> &tests = cgiUtilTest::suite()->getTests();
            unsigned int prefix_len = cgiUtilTest::suite()->getName().append("::").length();
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
            test = cgiUtilTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
