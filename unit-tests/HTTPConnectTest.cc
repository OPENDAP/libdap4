
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

#include <cstring>
#include <iterator>
#include <string>
#include <algorithm>
#include <functional>

//#define DODS_DEBUG

#include "GNURegex.h"
#include "HTTPConnect.h"
#include "RCReader.h"

#include "debug.h"

#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap
{

class HTTPConnectTest: public TestFixture {
  private:
    HTTPConnect * http;
    string localhost_url, localhost_pw_url, localhost_digest_pw_url;
    string etag;
    string lm;
    string netcdf_das_url;

  protected:
    bool re_match(Regex & r, const char *s) {
        return r.match(s, strlen(s)) == (int) strlen(s);
    }

    struct REMatch: public unary_function<const string &, bool> {
        Regex &d_re;
        REMatch(Regex &re) : d_re(re) {}
        ~REMatch() {}
        bool operator()(const string &str) {
            const char *s = str.c_str();
            return d_re.match(s, strlen(s)) == (int) strlen(s);
        }
    };

    // This is defined in HTTPConnect.cc but has to be defined here as well.
    // Don't know why... jhrg
    class HeaderMatch : public unary_function<const string &, bool> {
        const string &d_header;
        public:
            HeaderMatch(const string &header) : d_header(header) {}
            bool operator()(const string &arg) { return arg.find(d_header) == 0; }
    };

  public:
     HTTPConnectTest() {
    }
    ~HTTPConnectTest() {
    }

    void setUp() {
        putenv((char*)"DODS_CONF=cache-testsuite/dodsrc");
        http = new HTTPConnect(RCReader::instance());

        localhost_url = "http://test.opendap.org/test-304.html";

        // Two request header values that will generate a 304 response to the
        // above URL. The values below much match the etag and last-modified
        // time returned by the server. Run this test with DODS_DEBUG defined
        // to see the values it's returning.
        etag = "\"a10df-157-139c2680\""; // a10df-157-139c2680a
        lm = "Wed, 13 Jul 2005 19:32:26 GMT";

        localhost_pw_url =
            "http://jimg:dods_test@test.opendap.org/basic/page.txt";
        localhost_digest_pw_url =
            "http://jimg:dods_digest@test.opendap.org/digest/page.txt";
        netcdf_das_url =
            "http://test.opendap.org/dap/data/nc/fnoc1.nc.das";
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
    CPPUNIT_TEST(set_xdap_protocol_test);
    CPPUNIT_TEST(read_url_password_test);
    CPPUNIT_TEST(read_url_password_test2);

  // CPPUNIT_TEST(read_url_password_proxy_test);

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
            DBG(cerr << "If modified since test, status: " << status << endl);
            DBG(copy(resp_h->begin(), resp_h->end(),
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
            delete resp_h;
            resp_h = 0;
            CPPUNIT_FAIL("Error: " + e.get_error_message() );
        }
    }

    void fetch_url_test() {
        DBG(cerr << "Entering fetch_url_test" << endl);
        HTTPResponse *stuff = 0;
        char c;
        try {
            DBG(cerr << "    First request..." << endl;)
            stuff = http->fetch_url(localhost_url);
            DBG(cerr << "    Back from first request." << endl);
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;

        }
        catch(InternalErr & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch(Error & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            delete stuff; stuff = 0;
            throw;
        }

        try {
            stuff = http->fetch_url(netcdf_das_url);
            DBG2(char ln[1024]; while (!feof(stuff->get_stream())) {
                 fgets(ln, 1024, stuff->get_stream()); cerr << ln;}
                 rewind(stuff->get_stream()));

            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;
        }
        catch(InternalErr & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch(Error & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (...) {
            delete stuff; stuff = 0;
            throw;
        }

        try {
            stuff = http->fetch_url("file:///etc/passwd");
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;
        }
        catch(InternalErr & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch(Error & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (...) {
            delete stuff; stuff = 0;
            throw;
        }

        try {
	    string url = (string)"file://test_config.h" ;
            stuff = http->fetch_url(url);
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                           && !ferror(stuff->get_stream())
                           && !feof(stuff->get_stream()));
            delete stuff;
            stuff = 0;
        }
        catch(InternalErr & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch(Error & e) {
            delete stuff;
            stuff = 0;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
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
        HTTPResponse *r = 0;

        try {
            r = http->fetch_url(netcdf_das_url);
            vector < string > *h = r->get_headers();

            DBG(copy(h->begin(), h->end(),
                     ostream_iterator < string > (cerr, "\n")));

            // Should get five or six headers back.
            Regex header("X.*-Server: .*/.*");

            CPPUNIT_ASSERT(find_if(h->begin(), h->end(), REMatch(header)) != h->end());

            Regex protocol_header("XDAP: .*");
            CPPUNIT_ASSERT(find_if(h->begin(), h->end(), REMatch(protocol_header)) != h->end());

            delete r;
            r = 0;
        }
        catch(InternalErr & e) {
            delete r;
            r = 0;
            CPPUNIT_FAIL("Caught an InternalErr exception from get_response_headers: " + e.get_error_message() );
        }
        catch (...) {
            delete r; r = 0;
            throw;
        }
    }

    void server_version_test() {
        Response *r = 0;
        Regex protocol("^[0-9]+\\.[0-9]+$");
        try {
            r = http->fetch_url(netcdf_das_url);

            DBG(cerr << "r->get_version().c_str(): "
                << r->get_protocol().c_str() << endl);

            CPPUNIT_ASSERT(re_match(protocol, r->get_protocol().c_str()));
            delete r;
            r = 0;
        }
        catch(InternalErr & e) {
            delete r;
            r = 0;
            CPPUNIT_FAIL("Caught an InternalErr exception from server_version: " + e.get_error_message() );
        }
        catch (...) {
            delete r; r = 0;
            throw;
        }

    }

    void type_test() {
        Response *r = 0;
        try {
            r = http->fetch_url(netcdf_das_url);
            DBG(cerr << "r->get_type(): " << r->get_type() << endl);
            CPPUNIT_ASSERT(r->get_type() == dods_das);
            delete r;
            r = 0;
        }
        catch(InternalErr & e) {
            delete r;
            r = 0;
            CPPUNIT_FAIL("Caught an InternalErr exception from type(): " + e.get_error_message() );
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
            CPPUNIT_FAIL("Caught an InternalErrexception from output: " + e.get_error_message());
        }
    }

    void cache_test() {
        DBG(cerr << endl << "Entering Caching tests." << endl);
	try {
	    // The cache-testsuite/dodsrc file turns this off; all the other
	    // params are set up. It used to be that HTTPConnect had an option to
	    // turn caching on, but that no longer is present. This hack enables
	    // caching for this test. 06/18/04 jhrg
	    http->d_http_cache = HTTPCache::instance(http->d_rcr->get_dods_cache_root(), true);
	    DBG(cerr << "Instantiate the cache" << endl);

	    CPPUNIT_ASSERT(http->d_http_cache != 0);
	    http->d_http_cache->set_cache_enabled(true);
	    DBG(cerr << "Enable the cache" << endl);

	    fetch_url_test();
	    DBG(cerr << "fetch_url_test" << endl);
	    get_response_headers_test();
	    DBG(cerr << "get_response_headers_test" << endl);
	    server_version_test();
	    DBG(cerr << "server_version_test" << endl);
	    type_test();
	    DBG(cerr << "type_test" << endl);
	}
	catch (Error &e) {
	    CPPUNIT_FAIL((string)"Error: " + e.get_error_message());
	}
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

    void set_xdap_protocol_test() {
        // Initially there should be no header and the protocol should be 2.0
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 2
                       && http->d_dap_client_protocol_minor == 0);

        CPPUNIT_ASSERT(count_if(http->d_request_headers.begin(),
                             http->d_request_headers.end(),
                             HeaderMatch("XDAP-Accept:")) == 0);

        http->set_xdap_protocol(8, 9);
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 8
                       && http->d_dap_client_protocol_minor == 9);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(),
                             http->d_request_headers.end(),
                             "XDAP-Accept: 8.9") == 1);

        http->set_xdap_protocol(3, 2);
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 3
                       && http->d_dap_client_protocol_minor == 2);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(),
                             http->d_request_headers.end(),
                             "XDAP-Accept: 3.2") == 1);
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
        putenv((char*)"DODS_CONF=cache-testsuite/dodsrc_proxy");
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

}

int main(int, char **)
{
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    cerr << "These tests require a working network connection." << endl;

    bool wasSuccessful = runner.run("", false);

    return wasSuccessful ? 0 : 1;
}
