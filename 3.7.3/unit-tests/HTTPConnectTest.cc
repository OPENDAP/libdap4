
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

#include <iterator>
#include <string>
#include <algorithm>

//#define DODS_DEBUG

#include "GNURegex.h"
#include "HTTPConnect.h"
#include "RCReader.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

class HTTPConnectTest:public TestFixture {
  private:
    HTTPConnect * http;
    string localhost_url, localhost_pw_url, localhost_digest_pw_url;
    string etag;
    string lm;
    string netcdf_das_url;
    
  protected:
     bool re_match(Regex & r, const char *s) {
        return r.match(s, strlen(s)) == (int) strlen(s);
  } public:
     HTTPConnectTest() {
    }
    ~HTTPConnectTest() {
    }

    void setUp() {
        putenv("DODS_CONF=cache-testsuite/dodsrc");
        http = new HTTPConnect(RCReader::instance());

        localhost_url = "http://test.opendap.org/test-304.html";

        // Two request header values that will generate a 304 response to the
        // above URL. The values below much match the etag and last-modified
        // time returned by the server. Run this test with DODS_DEBUG defined
        // to see the values it's returning.
        etag = "\"928ec-157-139c2680\"";
        lm = "Wed, 13 Jul 2005 19:32:26 GMT";

        localhost_pw_url =
            "http://jimg:dods_test@test.opendap.org/basic/page.txt";
        localhost_digest_pw_url =
            "http://jimg:dods_digest@test.opendap.org/digest/page.txt";
        netcdf_das_url =
            "http://test.opendap.org/opendap/nph-dods/data/nc/fnoc1.nc.das";
    }

    void tearDown() {
        delete http;
        http = 0;
    }

    CPPUNIT_TEST_SUITE(HTTPConnectTest);

    CPPUNIT_TEST(read_url_test);
    CPPUNIT_TEST(fetch_url_test);
    CPPUNIT_TEST(get_response_headers_test);
    CPPUNIT_TEST(server_version_test);
    CPPUNIT_TEST(type_test);

    CPPUNIT_TEST(cache_test);

    CPPUNIT_TEST(set_accept_deflate_test);
    CPPUNIT_TEST(read_url_password_test);
    CPPUNIT_TEST(read_url_password_test2);

#if 0
    CPPUNIT_TEST(read_url_password_proxy_test);
#endif

    CPPUNIT_TEST_SUITE_END();

    void read_url_test() {
        vector < string > *resp_h = new vector < string >;;

        try {
            FILE *dump = fopen("/dev/null", "w");
            long status = http->read_url(localhost_url, dump, resp_h);
            CPPUNIT_ASSERT(status == 200);

            vector < string > request_h;

            // First test using a time with if-modified-since
            request_h.push_back(string("If-Modified-Since: ") + lm);
            status =
                http->read_url(localhost_url, dump, resp_h, &request_h);
            DBG(cerr << "If modified since test, status: " << status <<
                endl);
            DBG(copy
                (resp_h->begin(), resp_h->end(),
                 ostream_iterator < string > (cerr, "\n")));
            CPPUNIT_ASSERT(status == 304);

            // Now test an etag
            resp_h->clear();
            request_h.clear();
            request_h.push_back(string("If-None-Match: ") + etag);
            status =
                http->read_url(localhost_url, dump, resp_h, &request_h);
            DBG(cerr << "If none match test, status: " << status << endl);
            DBG(copy(resp_h->begin(), resp_h->end(),
                     ostream_iterator < string > (cerr, "\n")));
            CPPUNIT_ASSERT(status == 304);

            // now test a combined etag and time4
            resp_h->clear();
            request_h.clear();
            request_h.push_back(string("If-None-Match: ") + etag);
            request_h.push_back(string("If-Modified-Since: ") + lm);
            status =
                http->read_url(localhost_url, dump, resp_h, &request_h);
            DBG(cerr << "Combined test, status: " << status << endl);
            DBG(copy(resp_h->begin(), resp_h->end(),
                     ostream_iterator < string > (cerr, "\n")));
            CPPUNIT_ASSERT(status == 304);

            delete resp_h;
            resp_h = 0;

        }
        catch(Error & e) {
            cerr << e.get_error_message() << endl;
            delete resp_h;
            resp_h = 0;
            CPPUNIT_ASSERT(!"Should not get an Error");
        }
    }

    void fetch_url_test() {
        HTTPResponse *stuff = http->fetch_url(localhost_url);
        try {
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;

            stuff = http->fetch_url(netcdf_das_url);
            DBG2(char ln[1024]; while (!feof(stuff->get_stream())) {
                 fgets(ln, 1024, stuff->get_stream()); cerr << ln;}
                 rewind(stuff->get_stream()));

            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;

            stuff = http->fetch_url("file:///etc/passwd");
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;

            stuff = http->fetch_url("file://HTTPConnectTest.cc");
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;
        }
        catch(InternalErr & e) {
            delete stuff;
            stuff = 0;
            cerr << "InternalErr: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Caught a DODS exception from fetch_url");
        }
        catch(Error & e) {
            delete stuff;
            stuff = 0;
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Caught a DODS exception from fetch_url");
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (...) {
            delete stuff; stuff = 0;
            throw;
        }
    }

    void get_response_headers_test() {
        HTTPResponse *r = http->fetch_url(netcdf_das_url);

        try {
            vector < string > *h = r->get_headers();

            DBG(copy(h->begin(), h->end(),
                     ostream_iterator < string > (cerr, "\n")));

            // Should get five or six headers back.
            Regex header("X.*-Server: .*/.*");
            DBG(cerr << "get_response_headers_test(), (*h)[0]: "
                << (*h)[0] << endl);
            CPPUNIT_ASSERT(re_match(header, (*h)[0].c_str()));

            Regex protocol_header("XDAP: .*");
            unsigned int num_headers;
            if (re_match(protocol_header, (*h)[2].c_str()))
                num_headers = 7;
            else
                num_headers = 5;

            DBG(cerr << "num_headers: " << num_headers << endl);
            DBG(cerr << "h->size(): " << h->size() << endl);
            DBG(cerr << "(*h)[num_headers-1]: " << (*h)[num_headers - 1] <<
                endl);
            CPPUNIT_ASSERT((*h)[num_headers - 1] ==
                           "Content-Description: dods_das");
#if 1
            CPPUNIT_ASSERT(h->size() == num_headers);
#endif
            delete r;
            r = 0;
        }
        catch(InternalErr & e) {
            delete r;
            r = 0;
            CPPUNIT_FAIL("Caught an exception from get_response_headers");
        }
        catch (...) {
            delete r; r = 0;
            throw;
        }
    }

    void server_version_test() {
        Response *r = http->fetch_url(netcdf_das_url);
        Regex version("dap[0-4]?/[0-9]+\\.[0-9]+\\.[0-9]+");
        try {
            DBG(cerr << "r->get_version().c_str(): "
                << r->get_version().c_str() << endl);

            CPPUNIT_ASSERT(re_match(version, r->get_version().c_str()));
            delete r;
            r = 0;
        }
        catch(InternalErr & e) {
            delete r;
            r = 0;
            CPPUNIT_ASSERT(!"Caught an exception from server_version");
        }
        catch (...) {
            delete r; r = 0;
            throw;
        }

    }

    void type_test() {
        Response *r = http->fetch_url(netcdf_das_url);
        try {
            CPPUNIT_ASSERT(r->get_type() == dods_das);
            delete r;
            r = 0;
        }
        catch(InternalErr & e) {
            delete r;
            r = 0;
            CPPUNIT_ASSERT(!"Caught an exception from type()");
        }

    }

    void set_credentials_test() {
        http->set_credentials("jimg", "was_quit");
        Response *stuff = http->fetch_url("http://localhost/secret");

        try {
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;
        }
        catch(InternalErr & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_ASSERT(!"Caught exception from output");
        }
    }

    void cache_test() {
        DBG(cerr << endl << "Entering Caching tests." << endl);

        // The cache-testsuite/dodsrc file turns this off; all the other
        // params are set up. It used to be that HTTPConnect had an option to
        // turn caching on, but that no longer is present. This hack enables
        // caching for this test. 06/18/04 jhrg
        http->d_http_cache
            =
            HTTPCache::instance(http->d_rcr->get_dods_cache_root(), false);
        CPPUNIT_ASSERT(http->d_http_cache != 0);
        http->d_http_cache->set_cache_enabled(true);

        fetch_url_test();
        get_response_headers_test();
        server_version_test();
        type_test();
    }

    void set_accept_deflate_test() {
        http->set_accept_deflate(false);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(),
                             http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 0);

        http->set_accept_deflate(true);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(),
                             http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 1);

        http->set_accept_deflate(true);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(),
                             http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 1);

        http->set_accept_deflate(false);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(),
                             http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 0);
    }

    void read_url_password_test() {
        FILE *dump = fopen("/dev/null", "w");
        vector < string > *resp_h = new vector < string >;
        long status = http->read_url(localhost_pw_url, dump, resp_h);

        DBG(cerr << endl << http->d_upstring << endl);
        CPPUNIT_ASSERT(http->d_upstring == "jimg:dods_test");
        DBG(cerr << "Status: " << status << endl);
        CPPUNIT_ASSERT(status == 200);
        delete resp_h;
        resp_h = 0;
    }

    void read_url_password_test2() {
        FILE *dump = fopen("/dev/null", "w");
        vector < string > *resp_h = new vector < string >;
        long status =
            http->read_url(localhost_digest_pw_url, dump, resp_h);

        DBG(cerr << endl << http->d_upstring << endl);
        CPPUNIT_ASSERT(http->d_upstring == "jimg:dods_digest");
        DBG(cerr << "Status: " << status << endl);
        CPPUNIT_ASSERT(status == 200);
        delete resp_h;
        resp_h = 0;
    }

    // I'm going to remove this test. All of the other tests now work off of
    // test.opendap.org. jhrg 7/13/05
    void read_url_password_proxy_test() {
        cerr << endl <<
            "This test will fail if localhost is not configured as a proxy server\n\
with authentication. The username must be jimg and the password must be\n\
dods_test." << endl;

        delete http;
        http = 0;               // get rid of the default object; build a
        // special one. 
        RCReader::delete_instance();
        // this dodsrc directs all access through a proxy server. The
        // localhost must be configured as such.
        putenv("DODS_CONF=cache-testsuite/dodsrc_proxy");
        try {
            RCReader::initialize_instance();    // work-around pthreads for tests
            http = new HTTPConnect(RCReader::instance());

            fetch_url_test();
        }
        catch(Error & e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error");
        }
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPConnectTest);

int main(int, char **)
{
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    cerr << "These tests require a working network connection." << endl;

    bool wasSuccessful = runner.run("", false);

    return wasSuccessful ? 0 : 1;
}
