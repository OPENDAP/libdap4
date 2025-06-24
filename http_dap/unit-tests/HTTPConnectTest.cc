// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2013 OPeNDAP, Inc.
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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <iterator>
#include <string>

#include "GNURegex.h"
#include "HTTPCache.h"
#include "HTTPConnect.h"
#include "HTTPResponse.h"
#include "RCReader.h"
#include "debug.h"

#include "remove_directory.h"
#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

#define prolog std::string("HTTPConnectTest::").append(__func__).append("() - ")

namespace libdap {

class HTTPConnectTest : public TestFixture {
private:
    unique_ptr<HTTPConnect> http = make_unique<HTTPConnect>(RCReader::instance());
    string localhost_url{"http://test.opendap.org/test-304.html"};
    string localhost_pw_url{"http://jimg:dods_test@test.opendap.org/basic/page.txt"};
    string localhost_digest_pw_url{"http://jimg:dods_digest@test.opendap.org/basic/page.txt"};
    string etag{"\"157-5ef05adba5432\""};
    string lm{"Sun, 04 Dec 2022 19:35:52 GMT"};
    string netcdf_das_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.das"};

    bool re_match(Regex &r, const char *s) { return r.match(s, strlen(s)) == (int)strlen(s); }

    struct REMatch {
        Regex &d_re;
        explicit REMatch(Regex &re) : d_re(re) {}
        ~REMatch() = default;

        bool operator()(const string &str) {
            const char *s = str.c_str();
            return d_re.match(s, strlen(s)) == (int)strlen(s);
        }
    };

public:
    HTTPConnectTest() = default;
    ~HTTPConnectTest() override = default;

    void setUp() override {
        DBG(cerr << endl);
        DBG(cerr << prolog << "Setting the DODS_CONF env var" << endl);
        setenv("DODS_CONF", "cache-testsuite/dodsrc", 1);
        // This is coupled with the cache name in cache-testsuite/dodsrc
        if (access("cache-testsuite/http_connect_cache/", F_OK) == 0) {
            auto cache = HTTPCache::instance("cache-testsuite/http_connect_cache/");
            cache->purge_cache();
        }

        DBG(cerr << prolog << "localhost_url: " << localhost_url << endl);

        // Two request header values that will generate a 304 response to the
        // above URL. The values below much match the etag and last-modified
        // time returned by the server. Run this test with DODS_DEBUG defined
        // to see the values it's returning.
        //
        // etag = "\"a10df-157-139c2680\"";
        // etag = "\"2a008e-157-3fbcd139c2680\"";
        // etag = "\"181893-157-3fbcd139c2680\""; // On 10/13/14 we moved to a new httpd and the etag value changed.
        // etag ="\"157-3df1e87884680\""; // New httpd service, new etag, ndp - 01/11/21
        // etag = "\"157-3df0e26958000\"";// New httpd (dockerized), new etag. ndp - 12/06/22
        DBG(cerr << prolog << "etag: " << etag << endl);
        DBG(cerr << prolog << "lm: " << lm << endl);
        DBG(cerr << prolog << "localhost_pw_url: " << localhost_pw_url << endl);
        DBG(cerr << prolog << "localhost_digest_pw_url: " << localhost_digest_pw_url << endl);
        DBG(cerr << prolog << "netcdf_das_url: " << netcdf_das_url << endl);
    }

    CPPUNIT_TEST_SUITE(HTTPConnectTest);

    CPPUNIT_TEST(read_url_test);

    CPPUNIT_TEST(fetch_url_test_1);
    CPPUNIT_TEST(fetch_url_test_2);
    CPPUNIT_TEST(fetch_url_test_3);
    CPPUNIT_TEST(fetch_url_test_4);

    CPPUNIT_TEST(fetch_url_test_1_cpp);
    CPPUNIT_TEST(fetch_url_test_2_cpp);
    CPPUNIT_TEST(fetch_url_test_3_cpp);
    CPPUNIT_TEST(fetch_url_test_4_cpp);

    CPPUNIT_TEST(get_response_headers_test);
    CPPUNIT_TEST(server_version_test);
    CPPUNIT_TEST(type_test);

    CPPUNIT_TEST(cache_test);
    CPPUNIT_TEST(cache_test_cpp);

    CPPUNIT_TEST(set_accept_deflate_test);
    CPPUNIT_TEST(header_match_test);
    CPPUNIT_TEST(set_xdap_protocol_test);
    CPPUNIT_TEST(read_url_password_test);
    CPPUNIT_TEST(read_url_password_test2);

    CPPUNIT_TEST_SUITE_END();

    void read_url_test() {
        vector<string> resp_h;

        try {
            DBG(cerr << prolog << "BEGIN" << endl);
            DBG(cerr << prolog << "Testing with URL: " << localhost_url << endl);

            FILE *dump = fopen("/dev/null", "w");
            long status = http->read_url(localhost_url, dump, resp_h);
            CPPUNIT_ASSERT(status == 200);

            vector<string> request_h;

            // First test using a time with if-modified-since
            DBG(cerr << prolog << "If-Modified-Since test. BEGIN " << endl);
            request_h.push_back(string("If-Modified-Since: ") + lm);
            status = http->read_url(localhost_url, dump, resp_h, request_h);
            DBG(cerr << prolog << "If modified since test. Returned http status: " << status << endl);
            DBG(cerr << prolog << "Response Headers: " << endl);
            DBG(copy(resp_h.begin(), resp_h.end(), ostream_iterator<string>(cerr, "\n")));
            DBG(cerr << endl);
            CPPUNIT_ASSERT(status == 304);
            DBG(cerr << prolog << "If-Modified-Since test. END " << endl);

            // Now test an etag
            DBG(cerr << prolog << "ETag (If-None_Match) test. BEGIN (etag:" << etag << ")" << endl);
            resp_h.clear();
            request_h.clear();
            request_h.push_back(string("If-None-Match: ") + etag);
            status = http->read_url(localhost_url, dump, resp_h, request_h);
            DBG(cerr << prolog << "ETag (If-None_Match) test. Returned http status: " << status << endl);
            DBG(cerr << prolog << "Response Headers: " << endl);
            DBG(copy(resp_h.begin(), resp_h.end(), ostream_iterator<string>(cerr, "\n")));
            DBG(cerr << endl);
            CPPUNIT_ASSERT(status == 304);
            DBG(cerr << prolog << "ETag test. END " << endl);

            // now test a combined etag and time4
            resp_h.clear();
            request_h.clear();
            request_h.push_back(string("If-None-Match: ") + etag);
            request_h.push_back(string("If-Modified-Since: ") + lm);
            status = http->read_url(localhost_url, dump, resp_h, request_h);
            DBG(cerr << prolog << "Combined test. Returned http status: " << status << endl);
            DBG(copy(resp_h.begin(), resp_h.end(), ostream_iterator<string>(cerr, "\n")));
            CPPUNIT_ASSERT(status == 304);
        } catch (const Error &e) {
            CPPUNIT_FAIL(prolog + "Error: " + e.get_error_message());
        }
    }

    void fetch_url_test_1() {
        DBG(cerr << "Entering fetch_url_test 1" << endl);
        char c;
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(localhost_url));
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream()) &&
                           !feof(stuff->get_stream()));
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_1: " + string(e.what()));
        }
    }

    void fetch_url_test_1_cpp() {
        DBG(cerr << "Entering fetch_url_test 1" << endl);
        http->set_use_cpp_streams(true);
        char c;
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(localhost_url));
            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()));
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->bad());
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->eof());
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        } catch (std::exception &e) {
            CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
        }
    }

    void fetch_url_test_2() {
        DBG(cerr << "Entering fetch_url_test 2" << endl);
        char c;
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(netcdf_das_url));
            DBG2(char ln[1024]; while (!feof(stuff->get_stream())) {
                fgets(ln, 1024, stuff->get_stream());
                cerr << ln;
            } rewind(stuff->get_stream()));

            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream()) &&
                           !feof(stuff->get_stream()));
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_2: " + string(e.what()));
        }
    }

    void fetch_url_test_2_cpp() {
        DBG(cerr << "Entering fetch_url_test 2" << endl);
        http->set_use_cpp_streams(true);
        char c;
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(netcdf_das_url));
            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()) && !stuff->get_cpp_stream()->bad() &&
                           !stuff->get_cpp_stream()->eof());
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_2_cpp: " + string(e.what()));
        }
    }

    void fetch_url_test_3() {
        DBG(cerr << "Entering fetch_url_test 3" << endl);
        char c;
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url("file:///etc/passwd"));
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream()) &&
                           !feof(stuff->get_stream()));
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_3: " + string(e.what()));
        }
    }

    void fetch_url_test_3_cpp() {
        DBG(cerr << "Entering fetch_url_test 3" << endl);
        http->set_use_cpp_streams(true);
        char c;
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url("file:///etc/passwd"));

            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()) && !stuff->get_cpp_stream()->bad() &&
                           !stuff->get_cpp_stream()->eof());
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_3_cpp: " + string(e.what()));
        }
    }

    void fetch_url_test_4() {
        DBG(cerr << "Entering fetch_url_test 4" << endl);
        char c;
        try {
            string url = (string) "file://" + TEST_BUILD_DIR + "/test_config.h";
            unique_ptr<HTTPResponse> stuff(http->fetch_url(url));
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream()) &&
                           !feof(stuff->get_stream()));
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_4: " + string(e.what()));
        }
    }

    void fetch_url_test_4_cpp() {
        DBG(cerr << "Entering fetch_url_test_4_cpp" << endl);
        http->set_use_cpp_streams(true);
        char c;
        try {
            string url = (string) "file://" + TEST_BUILD_DIR + "/test_config.h";
            unique_ptr<HTTPResponse> stuff(http->fetch_url(url));

            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()));
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->bad());
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->eof());
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        } catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_4_cpp: " + string(e.what()));
        }
    }

    void get_response_headers_test() {
        try {
            unique_ptr<HTTPResponse> r(http->fetch_url(netcdf_das_url));
            vector<string> &h = r->get_headers();

            DBG(copy(h.begin(), h.end(), ostream_iterator<string>(cerr, "\n")));

            // Should get five or six headers back.
            Regex header("X.*-Server: .*/.*");

            CPPUNIT_ASSERT(find_if(h.begin(), h.end(), REMatch(header)) != h.end());

            Regex protocol_header("X.*DAP: .*"); // Matches both XDAP and X-DAP
            CPPUNIT_ASSERT(find_if(h.begin(), h.end(), REMatch(protocol_header)) != h.end());
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from get_response_headers: " + e.get_error_message());
        } catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from get_response_headers: " + string(e.what()));
        }
    }

    void server_version_test() {
        // Response *r = nullptr;
        Regex protocol("^[0-9]+\\.[0-9]+$");
        try {
            // r = http->fetch_url(netcdf_das_url);
            unique_ptr<HTTPResponse> r(http->fetch_url(netcdf_das_url));
            DBG(cerr << "r->get_version().c_str(): " << r->get_protocol().c_str() << endl);

            CPPUNIT_ASSERT(re_match(protocol, r->get_protocol().c_str()));
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from server_version: " + e.get_error_message());
        } catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from server_version_test: " + string(e.what()));
        }
    }

    void type_test() {
        try {
            unique_ptr<HTTPResponse> r(http->fetch_url(netcdf_das_url));
            DBG(cerr << "r->get_type(): " << r->get_type() << endl);
            CPPUNIT_ASSERT(r->get_type() == dods_das);
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from type(): " + e.get_error_message());
        }
    }

    void set_credentials_test() {
        http->set_credentials("jimg", "was_quit");
        unique_ptr<HTTPResponse> stuff(http->fetch_url("http://localhost/secret"));

        try {
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
            CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
            CPPUNIT_ASSERT(!feof(stuff->get_stream()));
        } catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from output: " + e.get_error_message());
        }
    }

    void cache_test() {
        DBG(cerr << endl << "Entering Caching tests." << endl);
        try {
            // The cache-testsuite/dodsrc file turns this off; all the other
            // params are set up. It used to be that HTTPConnect had an option to
            // turn caching on, but that no longer is present. This hack enables
            // caching for this test. 06/18/04 jhrg
            http->d_http_cache = HTTPCache::instance(http->d_rcr->get_dods_cache_root());
            DBG(cerr << "Instantiate the cache" << endl);

            CPPUNIT_ASSERT(http->d_http_cache != nullptr);
            http->d_http_cache->set_cache_enabled(true);
            DBG(cerr << "Enable the cache" << endl);

            fetch_url_test_4();
            DBG(cerr << "fetch_url_test" << endl);
            get_response_headers_test();
            DBG(cerr << "get_response_headers_test" << endl);
            server_version_test();
            DBG(cerr << "server_version_test" << endl);
            type_test();
            DBG(cerr << "type_test" << endl);
        } catch (Error &e) {
            CPPUNIT_FAIL((string) "Error: " + e.get_error_message());
        }
    }

    void cache_test_cpp() {
        DBG(cerr << endl << "Entering Caching tests." << endl);
        try {
            // The cache-testsuite/dodsrc file turns this off; all the other
            // params are set up. It used to be that HTTPConnect had an option to
            // turn caching on, but that no longer is present. This hack enables
            // caching for this test. 06/18/04 jhrg
            http->d_http_cache = HTTPCache::instance(http->d_rcr->get_dods_cache_root());
            DBG(cerr << "Instantiate the cache" << endl);

            CPPUNIT_ASSERT(http->d_http_cache != nullptr);
            http->d_http_cache->set_cache_enabled(true);
            DBG(cerr << "Enable the cache" << endl);

            fetch_url_test_4_cpp();
            DBG(cerr << "fetch_url_test_4_cpp" << endl);

            get_response_headers_test();
            DBG(cerr << "get_response_headers_test" << endl);

            server_version_test();
            DBG(cerr << "server_version_test" << endl);

            type_test();
            DBG(cerr << "type_test" << endl);
        } catch (Error &e) {
            CPPUNIT_FAIL((string) "Error: " + e.get_error_message());
        }
    }

    void set_accept_deflate_test() {
        http->set_accept_deflate(false);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 0);

        http->set_accept_deflate(true);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 1);

        http->set_accept_deflate(true);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 1);

        http->set_accept_deflate(false);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(),
                             "Accept-Encoding: deflate, gzip, compress") == 0);
    }

    void header_match_test() {
        CPPUNIT_ASSERT(HTTPConnect::header_match("TEST")("TESTING"));
        CPPUNIT_ASSERT(!HTTPConnect::header_match("DUMMY")("TESTING"));
    }

    void set_xdap_protocol_test() {
        // Initially there should be no header and the protocol should be 2.0
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 2 && http->d_dap_client_protocol_minor == 0);

        CPPUNIT_ASSERT(count_if(http->d_request_headers.begin(), http->d_request_headers.end(),
                                HTTPConnect::header_match("XDAP-Accept:")) == 0);

        http->set_xdap_protocol(8, 9);
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 8 && http->d_dap_client_protocol_minor == 9);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(), "XDAP-Accept: 8.9") == 1);

        http->set_xdap_protocol(3, 2);
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 3 && http->d_dap_client_protocol_minor == 2);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(), "XDAP-Accept: 3.2") == 1);
    }

    void read_url_password_test() {
        FILE *dump = fopen("/dev/null", "w");
        vector<string> resp_h;
        long status = http->read_url(localhost_pw_url, dump, resp_h);

        DBG(cerr << endl << http->d_upstring << endl);
        CPPUNIT_ASSERT(http->d_upstring == "jimg:dods_test");
        DBG(cerr << "Status: " << status << endl);
        CPPUNIT_ASSERT(status == 200);
    }

    void read_url_password_test2() {
        FILE *dump = fopen("/dev/null", "w");
        vector<string> resp_h;
        long status = http->read_url(localhost_digest_pw_url, dump, resp_h);

        DBG(cerr << endl << http->d_upstring << endl);
        CPPUNIT_ASSERT(http->d_upstring == "jimg:dods_digest");
        DBG(cerr << "Status: " << status << endl);
        CPPUNIT_ASSERT(status == 200);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPConnectTest);

} // namespace libdap

int main(int argc, char *argv[]) { return run_tests<libdap::HTTPConnectTest>(argc, argv) ? 0 : 1; }
