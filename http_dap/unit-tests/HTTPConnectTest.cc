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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iterator>
#include <string>
#include <algorithm>
#include <functional>

#include "GNURegex.h"
#include "RCReader.h"
#include "HTTPResponse.h"
#include "HTTPCache.h"
#include "HTTPConnect.h"
#include "debug.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

#define prolog std::string("HTTPConnectTest::").append(__func__).append("() - ")

namespace libdap {

class HTTPConnectTest: public TestFixture {
private:
    unique_ptr<HTTPConnect> http = make_unique<HTTPConnect>(RCReader::instance());
    string not_modified_304{"http://test.opendap.org/test-304.html"};
    string basic_pw_url{"http://jimg:dods_test@test.opendap.org/basic/page.txt"};
    string digest_pw_url{"http://jimg:dods_digest@test.opendap.org/basic/page.txt"};
    string etag{"\"157-3df0e26958000\""};
    string lm{"Wed, 13 Jul 2005 19:32:26 GMT"};
    string netcdf_das_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.das"};

public:
    HTTPConnectTest() = default;
    ~HTTPConnectTest() override = default;

    void setUp() override
    {
        setenv("DODS_CONF", "cache-testsuite/dodsrc", 1);
        // This is coupled with the cache name in cache-testsuite/dodsrc
        if (access("cache-testsuite/http_connect_cache/", F_OK) == 0) {
            auto cache = HTTPCache::instance("cache-testsuite/http_connect_cache/");
            cache->purge_cache();
        }
    }

    CPPUNIT_TEST_SUITE (HTTPConnectTest);

    CPPUNIT_TEST (read_url_test);

    CPPUNIT_TEST (fetch_url_test_1);
    CPPUNIT_TEST (fetch_url_test_2);
    CPPUNIT_TEST (fetch_url_test_3);
    CPPUNIT_TEST (fetch_url_test_4);

    CPPUNIT_TEST (fetch_url_test_1_cpp);
    CPPUNIT_TEST (fetch_url_test_2_cpp);
    CPPUNIT_TEST (fetch_url_test_3_cpp);
    CPPUNIT_TEST (fetch_url_test_4_cpp);

    CPPUNIT_TEST (get_response_headers_test);
    CPPUNIT_TEST (server_version_test);
    CPPUNIT_TEST (type_test);

    CPPUNIT_TEST (set_accept_deflate_test);
    CPPUNIT_TEST (set_xdap_protocol_test);
    CPPUNIT_TEST (read_url_password_test);
    CPPUNIT_TEST (read_url_password_test2);

    CPPUNIT_TEST_SUITE_END();

    void read_url_test()
    {
        vector<string> resp_h;

        try {
            DBG(cerr << prolog << "BEGIN" << endl);
            DBG(cerr << prolog << "Testing with URL: " << not_modified_304 << endl);

            FILE *dump = fopen("/dev/null", "w");
            long status = http->read_url(not_modified_304, dump, resp_h);
            CPPUNIT_ASSERT(status == 200);

            vector<string> request_h;

            // First test using a time with if-modified-since
            DBG(cerr << prolog << "If-Modified-Since test. BEGIN " << endl);
            request_h.push_back(string("If-Modified-Since: ") + lm);
            status = http->read_url(not_modified_304, dump, resp_h, request_h);
            DBG(cerr << prolog << "If modified since test. Returned http status: " << status << endl);
            DBG(cerr << prolog << "Response Headers: " << endl);
            DBG(copy(resp_h.begin(), resp_h.end(), ostream_iterator<string>(cerr, "\n")));
            DBG(cerr << endl);
            CPPUNIT_ASSERT(status == 304);
            DBG(cerr << prolog << "If-Modified-Since test. END " << endl);

            // Now test an etag
            DBG(cerr << prolog << "ETag (If-None_Match) test. BEGIN (etag:" << etag << ")"<< endl);
            resp_h.clear();
            request_h.clear();
            request_h.push_back(string("If-None-Match: ") + etag);
            status = http->read_url(not_modified_304, dump, resp_h, request_h);
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
            status = http->read_url(not_modified_304, dump, resp_h, request_h);
            DBG(cerr << prolog << "Combined test. Returned http status: " << status << endl);
            DBG(copy(resp_h.begin(), resp_h.end(), ostream_iterator<string>(cerr, "\n")));
            CPPUNIT_ASSERT(status == 304);
        }
        catch (const Error & e) {
            CPPUNIT_FAIL(prolog + "Error: " + e.get_error_message());
        }
    }

    void fetch_url_test_1()
    {
        DBG(cerr << "Entering fetch_url_test 1" << endl);
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(not_modified_304));
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
            CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
            CPPUNIT_ASSERT(!feof(stuff->get_stream()));
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (const Error &e) {
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

    void fetch_url_test_1_cpp()
    {
        DBG(cerr << "Entering fetch_url_test 1" << endl);
        http->set_use_cpp_streams(true);
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(not_modified_304));
            char c;
            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()));
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->bad());
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->eof());
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (std::exception &e) {
            CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
        }
    }

    void fetch_url_test_2()
    {
        DBG(cerr << "Entering fetch_url_test 2" << endl);
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(netcdf_das_url));
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
            CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
            CPPUNIT_ASSERT(!feof(stuff->get_stream()));
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (const Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        // Catch the exception from a failed ASSERT and clean up. Deleting a
        // Response object also unlocks the HTTPCache in some cases. If delete
        // is not called, then a failed test can leave the cache with locked
        // entries
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from fetch_url_test_2: " + string(e.what()));        }
    }

    void fetch_url_test_2_cpp()
    {
        DBG(cerr << "Entering fetch_url_test 2" << endl);
        http->set_use_cpp_streams(true);
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url(netcdf_das_url));
            char c;
            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()));
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->bad());
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->eof());

        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (const Error &e) {
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

    void fetch_url_test_3()
    {
        DBG(cerr << "Entering fetch_url_test 3" << endl);
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url("file:///etc/passwd"));
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
            CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
            CPPUNIT_ASSERT(!feof(stuff->get_stream()));
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (const Error &e) {
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

    void fetch_url_test_3_cpp()
    {
        DBG(cerr << "Entering fetch_url_test 3" << endl);
        http->set_use_cpp_streams(true);
        try {
            unique_ptr<HTTPResponse> stuff(http->fetch_url("file:///etc/passwd"));
            char c;
            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()));
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->bad());
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->eof());
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (const Error &e) {
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

    void fetch_url_test_4()
    {
        DBG(cerr << "Entering fetch_url_test 4" << endl);
        try {
            string url = (string) "file://" + TEST_SRC_DIR + "/test_config.h";
            unique_ptr<HTTPResponse> stuff(http->fetch_url(url));
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
            CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
            CPPUNIT_ASSERT(!feof(stuff->get_stream()));
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (const Error &e) {
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

    void fetch_url_test_4_cpp()
    {
        DBG(cerr << "Entering fetch_url_test_4_cpp" << endl);
        http->set_use_cpp_streams(true);
        try {
            string url = (string) "file://" + TEST_SRC_DIR + "/test_config.h";
            unique_ptr<HTTPResponse> stuff(http->fetch_url(url));
            char c;
            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(*(stuff->get_cpp_stream()));
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->bad());
            CPPUNIT_ASSERT(!stuff->get_cpp_stream()->eof());
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (const Error &e) {
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

    void get_response_headers_test()
    {
        try {
            unique_ptr<HTTPResponse> r(http->fetch_url(netcdf_das_url));
            vector<string> &h = r->get_headers();

            DBG(copy(h.begin(), h.end(), ostream_iterator<string>(cerr, "\n")));

            // Should get five or six headers back.
            Regex header("X.*-Server: .*/.*");
            CPPUNIT_ASSERT(find_if(h.begin(), h.end(),
                                   [&header](const string &i) {return header.match(i) == (int)i.size();}) != h.end());

            Regex protocol_header("X.*DAP: .*");	// Matches both XDAP and X-DAP
            CPPUNIT_ASSERT(find_if(h.begin(), h.end(),
                                   [&protocol_header](const string &i) {return protocol_header.match(i) == (int)i.size();}) != h.end());
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from get_response_headers: " + e.get_error_message());
        }
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from get_response_headers: " + string(e.what()));
        }
    }

    void server_version_test()
    {
        Regex protocol("^[0-9]+\\.[0-9]+$");
        try {
            unique_ptr<HTTPResponse> r(http->fetch_url(netcdf_das_url));
            DBG(cerr << "r->get_version().c_str(): " << r->get_protocol().c_str() << endl);

            CPPUNIT_ASSERT(protocol.match(r->get_protocol()) == (int)r->get_protocol().size());
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from server_version: " + e.get_error_message());
        }
        catch (const std::exception &e) {
            CPPUNIT_FAIL("An exception was thrown from server_version_test: " + string(e.what()));
        }
    }

    void type_test()
    {
        try {
            unique_ptr<HTTPResponse> r(http->fetch_url(netcdf_das_url));
            DBG(cerr << "r->get_type(): " << r->get_type() << endl);
            CPPUNIT_ASSERT(r->get_type() == dods_das);
        }
        catch (const InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from type(): " + e.get_error_message());
        }
    }

    void set_accept_deflate_test()
    {
        http->set_accept_deflate(false);
        CPPUNIT_ASSERT(
            count(http->d_request_headers.begin(), http->d_request_headers.end(),
                "Accept-Encoding: deflate, gzip, compress") == 0);

        http->set_accept_deflate(true);
        CPPUNIT_ASSERT(
            count(http->d_request_headers.begin(), http->d_request_headers.end(),
                "Accept-Encoding: deflate, gzip, compress") == 1);

        http->set_accept_deflate(true);
        CPPUNIT_ASSERT(
            count(http->d_request_headers.begin(), http->d_request_headers.end(),
                "Accept-Encoding: deflate, gzip, compress") == 1);

        http->set_accept_deflate(false);
        CPPUNIT_ASSERT(
            count(http->d_request_headers.begin(), http->d_request_headers.end(),
                "Accept-Encoding: deflate, gzip, compress") == 0);
    }

    void set_xdap_protocol_test()
    {
        // Initially there should be no header and the protocol should be 2.0
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 2 && http->d_dap_client_protocol_minor == 0);

        CPPUNIT_ASSERT(count_if(http->d_request_headers.begin(), http->d_request_headers.end(),
            [](const string &header) { return header.find("XDAP-Accept: 2.0") == 0; }) == 0);

        http->set_xdap_protocol(8, 9);
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 8 && http->d_dap_client_protocol_minor == 9);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(), "XDAP-Accept: 8.9") == 1);

        http->set_xdap_protocol(3, 2);
        CPPUNIT_ASSERT(http->d_dap_client_protocol_major == 3 && http->d_dap_client_protocol_minor == 2);
        CPPUNIT_ASSERT(count(http->d_request_headers.begin(), http->d_request_headers.end(), "XDAP-Accept: 3.2") == 1);
    }

    void read_url_password_test()
    {
        FILE *dump = fopen("/dev/null", "w");
        vector<string> resp_h;
        long status = http->read_url(basic_pw_url, dump, resp_h);

        DBG(cerr << endl << http->d_upstring << endl);
        CPPUNIT_ASSERT(http->d_upstring == "jimg:dods_test");
        DBG(cerr << "Status: " << status << endl);
        CPPUNIT_ASSERT(status == 200);
    }

    void read_url_password_test2()
    {
        FILE *dump = fopen("/dev/null", "w");
        vector<string> resp_h;
        long status = http->read_url(digest_pw_url, dump, resp_h);

        DBG(cerr << endl << http->d_upstring << endl);
        CPPUNIT_ASSERT(http->d_upstring == "jimg:dods_digest");
        DBG(cerr << "Status: " << status << endl);
        CPPUNIT_ASSERT(status == 200);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (HTTPConnectTest);

}

int main(int argc, char*argv[])
{
    return run_tests<libdap::HTTPConnectTest>(argc, argv) ? 0: 1;
}
