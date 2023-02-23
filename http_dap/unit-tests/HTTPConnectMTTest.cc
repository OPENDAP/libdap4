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

#include <cstring>
#include <sys/stat.h>

#include <iterator>
#include <memory>
#include <string>
#include <algorithm>
#include <thread>

#include "GNURegex.h"
#include "HTTPConnect.h"
#include "debug.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

#include "remove_directory.h"

using namespace CppUnit;
using namespace std;

#define prolog std::string("HTTPConnectMTTest::").append(__func__).append("() - ")

namespace libdap {

inline static uint64_t file_size(FILE *fp)
{
    struct stat s;
    fstat(fileno(fp), &s);
    return s.st_size;
}

class HTTPConnectMTTest : public TestFixture {
private:
    unique_ptr<HTTPConnect> http{nullptr};
    string url_304{"http://test.opendap.org/test-304.html"};
    string basic_pw_url{"http://jimg:dods_test@test.opendap.org/basic/page.txt"};
    string basic_digest_pw_url{"http://jimg:dods_digest@test.opendap.org/basic/page.txt"};
    string etag{"\"157-3df0e26958000\""};   // New httpd (dockerized), new etag. ndp - 12/06/22
    string lm{"Wed, 13 Jul 2005 19:32:26 GMT"};
    string netcdf_das_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.das"};

    static bool re_match(const Regex &r, const char *s)
    {
        return r.match(s, (int) strlen(s)) == (int) strlen(s);
    }

    struct REMatch : public unary_function<const string &, bool> {
        Regex &d_re;

        explicit REMatch(Regex &re) :
                d_re(re)
        {
        }

        ~REMatch() = default;

        bool operator()(const string &str) const
        {
            const char *s = str.c_str();
            return d_re.match(s, (int) strlen(s)) == (int) strlen(s);
        }
    };

    // This is defined in HTTPConnect.cc but has to be defined here as well.
    // Don't know why... jhrg
    class HeaderMatch : public unary_function<const string &, bool> {
        const string &d_header;
    public:
        explicit HeaderMatch(const string &header) :
                d_header(header)
        {
        }

        bool operator()(const string &arg) const
        {
            return arg.find(d_header) == 0;
        }
    };

    static bool is_prime(int n) {
        if (n <= 1) {
            return false;
        }
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return true;
    }

    static int count_primes(int start, int end) {
        int count = 0;
        for (int i = start; i <= end; i++) {
            if (is_prime(i)) {
                count++;
            }
        }
        return count;
    }

public:
    HTTPConnectMTTest() = default;

    ~HTTPConnectMTTest() override = default;

    void setUp() override
    {
        DBG(cerr << endl);
        DBG(cerr << prolog << "Setting the DODS_CONF env var" << endl);

        setenv("DODS_CONF", "cache-testsuite/dodsrc", 1);
        if (access("cache-testsuite/http_connect_cache/", F_OK) == 0)
            remove_directory("cache-testsuite/http_connect_cache/");

        DBG(cerr << prolog << "url_304: " << url_304 << endl);
        // Two request header values that will generate a 304 response to the
        // above URL. The values below much match the etag and last-modified
        // time returned by the server. Run this test with DODS_DEBUG defined
        // to see the values it's returning.
        //
        // etag = "\"a10df-157-139c2680\"";
        // etag = "\"2a008e-157-3fbcd139c2680\"";
        // etag = "\"181893-157-3fbcd139c2680\""; // On 10/13/14 we moved to a new httpd and the etag value changed.
        // etag ="\"157-3df1e87884680\""; // New httpd service, new etag, ndp - 01/11/21
        DBG(cerr << prolog << "etag: " << etag << endl);
        DBG(cerr << prolog << "lm: " << lm << endl);
        DBG(cerr << prolog << "basic_pw_url: " << basic_pw_url << endl);
        DBG(cerr << prolog << "basic_digest_pw_url: " << basic_digest_pw_url << endl);
        DBG(cerr << prolog << "netcdf_das_url: " << netcdf_das_url << endl);
    }

    CPPUNIT_TEST_SUITE (HTTPConnectMTTest);

#if 0
        CPPUNIT_TEST (read_url_test);
#endif

        CPPUNIT_TEST(count_primes_test_mt);

        CPPUNIT_TEST(fetch_url_test);
        CPPUNIT_TEST(fetch_url_test_304_mt);
        CPPUNIT_TEST(fetch_url_test_304_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_nc_mt);
        CPPUNIT_TEST(fetch_url_test_nc_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache_multi_access);
#if 0
        CPPUNIT_TEST (fetch_url_test_3);
        CPPUNIT_TEST (fetch_url_test_4);
#endif

        CPPUNIT_TEST(fetch_url_test_cpp);
#if 0
        CPPUNIT_TEST (fetch_url_test_2_cpp);
        CPPUNIT_TEST (fetch_url_test_3_cpp);
        CPPUNIT_TEST (fetch_url_test_4_cpp);
#endif

#if 0
        CPPUNIT_TEST (get_response_headers_test);
        CPPUNIT_TEST (server_version_test);
        CPPUNIT_TEST (type_test);

        CPPUNIT_TEST (cache_test);
        CPPUNIT_TEST (cache_test_cpp);

        CPPUNIT_TEST (set_accept_deflate_test);
        CPPUNIT_TEST (set_xdap_protocol_test);
        CPPUNIT_TEST (read_url_password_test);
        CPPUNIT_TEST (read_url_password_test2);
#endif

    CPPUNIT_TEST_SUITE_END();

    void read_url_test()
    {
        vector<string> resp_h;

        try {
            DBG(cerr << prolog << "BEGIN" << endl);
            DBG(cerr << prolog << "Testing with URL: " << url_304 << endl);

            FILE *dump = fopen("/dev/null", "w");
            long status = http->read_url(url_304, dump, resp_h);
            CPPUNIT_ASSERT(status == 200);

            vector<string> request_h;

            // First test using a time with if-modified-since
            DBG(cerr << prolog << "If-Modified-Since test. BEGIN " << endl);
            request_h.push_back(string("If-Modified-Since: ") + lm);
            status = http->read_url(url_304, dump, resp_h, request_h);
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
            status = http->read_url(url_304, dump, resp_h, request_h);
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
            status = http->read_url(url_304, dump, resp_h, request_h);
            DBG(cerr << prolog << "Combined test. Returned http status: " << status << endl);
            DBG(copy(resp_h.begin(), resp_h.end(), ostream_iterator<string>(cerr, "\n")));
            CPPUNIT_ASSERT(status == 304);
        }
        catch (const Error &e) {
            CPPUNIT_FAIL(prolog + "Error: " + e.get_error_message());
        }
    }

    void count_primes_test_mt() {
        int start1 = 1;
        int end1 = 10000;
        int start2 = 10001;
        int end2 = 20000;

        int count1, count2;
        std::thread thread1([&count1, start1, end1]() {
            count1 = count_primes(start1, end1);
        });
        std::thread thread2([&count2, start2, end2]() {
            count2 = count_primes(start2, end2);
        });
        thread1.join();
        thread2.join();

        int total_count = count1 + count2;
        int expected_count = count_primes(start1, end2);
        DBG(cerr << "Expected count: " << expected_count << endl);
        DBG(cerr << "Total count: " << total_count << endl);
        DBG(cerr << "Count1: " << count1 << endl);
        DBG(cerr << "Count2: " << count2 << endl);
        CPPUNIT_ASSERT_EQUAL(expected_count, total_count);
    }

    void fetch_url_test()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            http = std::make_unique<HTTPConnect>(RCReader::instance());
            unique_ptr<HTTPResponse> stuff(http->fetch_url(url_304));
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                            && !feof(stuff->get_stream()));
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }

    void fetch_url_test_cpp()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            http = std::make_unique<HTTPConnect>(RCReader::instance());
            http->set_use_cpp_streams(true);
            unique_ptr<HTTPResponse> stuff(http->fetch_url(url_304));
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
        catch (const std::exception &e) {
            CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }

    void fetch_url_test_304_mt()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc) {
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                               && c =='<'
                               && !ferror(stuff->get_stream())
                               && !feof(stuff->get_stream()));
                DBG(cerr << "hc_lambda: " << url << ", " << c << endl);
            };

            std::thread thread1(hc_lambda, url_304, http_1.get());
            std::thread thread2(hc_lambda, url_304, http_2.get());
            std::thread thread3(hc_lambda, url_304, http_3.get());
            std::thread thread4(hc_lambda, url_304, http_4.get());

            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }

    void fetch_url_test_304_mt_w_cache()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            setenv("DODS_CONF", "cache-testsuite/dodsrc_w_caching", 1);
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc) {
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                               && c =='<'
                               && !ferror(stuff->get_stream())
                               && !feof(stuff->get_stream()));
                DBG(cerr << "hc_lambda: " << url << ", " << c << endl);
            };

            std::thread thread1(hc_lambda, url_304, http_1.get());
            std::thread thread2(hc_lambda, url_304, http_2.get());
            std::thread thread3(hc_lambda, url_304, http_3.get());
            std::thread thread4(hc_lambda, url_304, http_4.get());

            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }

    void fetch_url_test_nc_mt()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc) {
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                               && c =='A'
                               && !ferror(stuff->get_stream())
                               && !feof(stuff->get_stream()));
                DBG(cerr << "hc_lambda: " << url << ", " << c << endl);
            };

            std::thread thread1(hc_lambda, netcdf_das_url, http_1.get());
            std::thread thread2(hc_lambda, netcdf_das_url, http_2.get());
            std::thread thread3(hc_lambda, netcdf_das_url, http_3.get());
            std::thread thread4(hc_lambda, netcdf_das_url, http_4.get());

            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }

    void fetch_url_test_nc_mt_w_cache()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            setenv("DODS_CONF", "cache-testsuite/dodsrc_w_caching", 1);
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc) {
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1
                                && c =='A'
                                && !ferror(stuff->get_stream())
                                && !feof(stuff->get_stream()));
                DBG(cerr << "hc_lambda: " << url << ", " << c << endl);
            };

            std::thread thread1(hc_lambda, netcdf_das_url, http_1.get());
            std::thread thread2(hc_lambda, netcdf_das_url, http_2.get());
            std::thread thread3(hc_lambda, netcdf_das_url, http_3.get());
            std::thread thread4(hc_lambda, netcdf_das_url, http_4.get());

            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }

    void fetch_url_test_diff_urls_mt_w_cache()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            setenv("DODS_CONF", "cache-testsuite/dodsrc_w_caching", 1);
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc, uint32_t expected_size) {
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                DBG(cerr << "hc_lambda: " << url << ", response size: " << file_size(stuff->get_stream()) << endl);
                DBG2(cerr << "hc_lambda: " << url << ", Response type: " << typeid(stuff.get()).name() << endl);
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
                CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
                CPPUNIT_ASSERT(!feof(stuff->get_stream()));
                CPPUNIT_ASSERT_MESSAGE("response size: " + std::to_string(file_size(stuff->get_stream())),
                                       file_size(stuff->get_stream()) == expected_size);
            };

            string netcdf_dds_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.dds"};
            string netcdf_dmr_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.dmr"};
            string netcdf_dap_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.dap"};

            std::thread thread1(hc_lambda, netcdf_das_url, http_1.get(), 927);
            std::thread thread2(hc_lambda, netcdf_dds_url, http_2.get(), 197);
            std::thread thread3(hc_lambda, netcdf_dmr_url, http_3.get(), 3103);
            std::thread thread4(hc_lambda, netcdf_dap_url, http_4.get(), 26221);

            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }
    void fetch_url_test_diff_urls_mt_w_cache_multi_access()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            setenv("DODS_CONF", "cache-testsuite/dodsrc_w_caching", 1);
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc, uint32_t expected_size) {
                hc->set_verbose_runtime(true);
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                DBG(cerr << "hc_lambda: " << url << ", response size: " << file_size(stuff->get_stream()) << endl);
                DBG2(cerr << "hc_lambda: " << url << ", Response type: " << typeid(stuff.get()).name() << endl);
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
                CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
                CPPUNIT_ASSERT(!feof(stuff->get_stream()));
                CPPUNIT_ASSERT_MESSAGE("response size: " + std::to_string(file_size(stuff->get_stream())),
                                       file_size(stuff->get_stream()) == expected_size);
            };

            string netcdf_dds_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.dds"};
            string netcdf_dmr_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.dmr"};
            string netcdf_dap_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.dap"};

            std::thread thread1(hc_lambda, netcdf_das_url, http_1.get(), 927);
            std::thread thread2(hc_lambda, netcdf_dds_url, http_2.get(), 197);
            std::thread thread3(hc_lambda, netcdf_dmr_url, http_3.get(), 3103);
            std::thread thread4(hc_lambda, netcdf_dap_url, http_4.get(), 26221);

            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();

            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_1->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_2->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_3->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_4->is_cached_response());

            // Now we access the same URLs again. The cache should be used.
            std::thread thread5(hc_lambda, netcdf_das_url, http_1.get(), 927);
            std::thread thread6(hc_lambda, netcdf_dds_url, http_2.get(), 197);
            std::thread thread7(hc_lambda, netcdf_dmr_url, http_3.get(), 3103);
            std::thread thread8(hc_lambda, netcdf_dap_url, http_4.get(), 26221);

            thread5.join();
            thread6.join();
            thread7.join();
            thread8.join();

            // CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_1->is_cached_response());

            // Now we access the same URLs again using new instances of HTTPConnect.
            // The cache should be used
            auto http_5 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_6 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_7 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_8 = std::make_unique<HTTPConnect>(RCReader::instance());

            std::thread thread9(hc_lambda, netcdf_das_url, http_5.get(), 927);
            std::thread thread10(hc_lambda, netcdf_dds_url, http_6.get(), 197);
            std::thread thread11(hc_lambda, netcdf_dmr_url, http_7.get(), 3103);
            std::thread thread12(hc_lambda, netcdf_dap_url, http_8.get(), 26221);

            thread9.join();
            thread10.join();
            thread11.join();
            thread12.join();

            // CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_5->is_cached_response());
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
        catch (...) {
            cerr << "Caught unknown exception" << endl;
            throw;
        }
    }


#if 0
    std::thread thread1([&http_1, this]() {
                unique_ptr<HTTPResponse> stuff(http_1->fetch_url(url_304));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                               && !feof(stuff->get_stream()));
            });
            std::thread thread2([&http_2, this]() {
                unique_ptr<HTTPResponse> stuff(http_2->fetch_url(url_304));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                               && !feof(stuff->get_stream()));
            });
            std::thread thread3([&http_3, this]() {
                unique_ptr<HTTPResponse> stuff(http_3->fetch_url(url_304));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                               && !feof(stuff->get_stream()));
            });
            std::thread thread4([&http_4, this]() {
                unique_ptr<HTTPResponse> stuff(http_4->fetch_url(url_304));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                               && !feof(stuff->get_stream()));
            });
#endif

#if 0

    void fetch_url_test_2_cpp()
    {
        DBG(cerr << "Entering fetch_url_test 2" << endl);
        http->set_use_cpp_streams(true);

        HTTPResponse *stuff = nullptr;
        char c;

        try {
            stuff = http->fetch_url(netcdf_das_url);

            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(
                    *(stuff->get_cpp_stream()) && !stuff->get_cpp_stream()->bad() && !stuff->get_cpp_stream()->eof());

            delete stuff;
            stuff = nullptr;
        }
        catch (InternalErr &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
            // Catch the exception from a failed ASSERT and clean up. Deleting a
            // Response object also unlocks the HTTPCache in some cases. If delete
            // is not called, then a failed test can leave the cache with locked
            // entries
        catch (...) {
            delete stuff;
            stuff = nullptr;
            throw;
        }
    }

    void fetch_url_test_3()
    {
        DBG(cerr << "Entering fetch_url_test 3" << endl);
        HTTPResponse *stuff = nullptr;
        char c;
        try {
            stuff = http->fetch_url("file:///etc/passwd");
            CPPUNIT_ASSERT(
                    fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                    && !feof(stuff->get_stream()));
            delete stuff;
            stuff = nullptr;
        }
        catch (InternalErr &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (Error &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
            // Catch the exception from a failed ASSERT and clean up. Deleting a
            // Response object also unlocks the HTTPCache in some cases. If delete
            // is not called, then a failed test can leave the cache with locked
            // entries
        catch (...) {
            delete stuff;
            stuff = nullptr;
            throw;
        }
    }

    void fetch_url_test_3_cpp()
    {
        DBG(cerr << "Entering fetch_url_test 3" << endl);
        http->set_use_cpp_streams(true);

        HTTPResponse *stuff = nullptr;
        char c;
        try {
            stuff = http->fetch_url("file:///etc/passwd");

            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(
                    *(stuff->get_cpp_stream()) && !stuff->get_cpp_stream()->bad() && !stuff->get_cpp_stream()->eof());

            delete stuff;
            stuff = nullptr;
        }
        catch (InternalErr &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (Error &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
            // Catch the exception from a failed ASSERT and clean up. Deleting a
            // Response object also unlocks the HTTPCache in some cases. If delete
            // is not called, then a failed test can leave the cache with locked
            // entries
        catch (...) {
            delete stuff;
            stuff = nullptr;
            throw;
        }
    }

    void fetch_url_test_4()
    {
        DBG(cerr << "Entering fetch_url_test 4" << endl);
        HTTPResponse *stuff = nullptr;
        char c;
        try {
            string url = (string) "file://" + TEST_SRC_DIR + "/test_config.h";
            stuff = http->fetch_url(url);
            CPPUNIT_ASSERT(
                    fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                    && !feof(stuff->get_stream()));
            delete stuff;
            stuff = nullptr;
        }
        catch (InternalErr &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (Error &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
            // Catch the exception from a failed ASSERT and clean up. Deleting a
            // Response object also unlocks the HTTPCache in some cases. If delete
            // is not called, then a failed test can leave the cache with locked
            // entries
        catch (...) {
            delete stuff;
            stuff = nullptr;
            throw;
        }
    }

    void fetch_url_test_4_cpp()
    {
        DBG(cerr << "Entering fetch_url_test_4_cpp" << endl);
        http->set_use_cpp_streams(true);

        HTTPResponse *stuff = nullptr;
        char c;
        try {
            string url = (string) "file://" + TEST_SRC_DIR + "/test_config.h";
            stuff = http->fetch_url(url);

            stuff->get_cpp_stream()->read(&c, 1);
            CPPUNIT_ASSERT(
                    *(stuff->get_cpp_stream()) && !stuff->get_cpp_stream()->bad() && !stuff->get_cpp_stream()->eof());

            delete stuff;
            stuff = nullptr;
        }
        catch (InternalErr &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url" + e.get_error_message());
        }
        catch (Error &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
        }
            // Catch the exception from a failed ASSERT and clean up. Deleting a
            // Response object also unlocks the HTTPCache in some cases. If delete
            // is not called, then a failed test can leave the cache with locked
            // entries
        catch (...) {
            delete stuff;
            stuff = nullptr;
            throw;
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

            CPPUNIT_ASSERT(find_if(h.begin(), h.end(), REMatch(header)) != h.end());

            Regex protocol_header("X.*DAP: .*");    // Matches both XDAP and X-DAP
            CPPUNIT_ASSERT(find_if(h.begin(), h.end(), REMatch(protocol_header)) != h.end());
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr exception from get_response_headers: " + e.get_error_message());
        }
        catch (...) {
            CPPUNIT_FAIL("No idea what exception was thrown from get_response_headers");
        }
    }

    void server_version_test()
    {
        Response *r = nullptr;
        Regex protocol("^[0-9]+\\.[0-9]+$");
        try {
            r = http->fetch_url(netcdf_das_url);

            DBG(cerr << "r->get_version().c_str(): " << r->get_protocol().c_str() << endl);

            CPPUNIT_ASSERT(re_match(protocol, r->get_protocol().c_str()));
            delete r;
            r = nullptr;
        }
        catch (InternalErr &e) {
            delete r;
            r = nullptr;
            CPPUNIT_FAIL("Caught an InternalErr exception from server_version: " + e.get_error_message());
        }
        catch (...) {
            delete r;
            r = nullptr;
            throw;
        }

    }

    void type_test()
    {
        Response *r = nullptr;
        try {
            r = http->fetch_url(netcdf_das_url);
            DBG(cerr << "r->get_type(): " << r->get_type() << endl);
            CPPUNIT_ASSERT(r->get_type() == dods_das);
            delete r;
            r = nullptr;
        }
        catch (InternalErr &e) {
            delete r;
            r = nullptr;
            CPPUNIT_FAIL("Caught an InternalErr exception from type(): " + e.get_error_message());
        }

    }

    void set_credentials_test()
    {
        http->set_credentials("jimg", "was_quit");
        Response *stuff = http->fetch_url("http://localhost/secret");

        try {
            char c;
            CPPUNIT_ASSERT(
                    fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream())
                    && !feof(stuff->get_stream()));
            delete stuff;
            stuff = nullptr;
        }
        catch (InternalErr &e) {
            delete stuff;
            stuff = nullptr;
            CPPUNIT_FAIL("Caught an InternalErrexception from output: " + e.get_error_message());
        }
    }

    void cache_test()
    {
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
        }
        catch (Error &e) {
            CPPUNIT_FAIL((string) "Error: " + e.get_error_message());
        }
    }

    void cache_test_cpp()
    {
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
        }
        catch (Error &e) {
            CPPUNIT_FAIL((string) "Error: " + e.get_error_message());
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

        CPPUNIT_ASSERT(
                count_if(http->d_request_headers.begin(), http->d_request_headers.end(), HeaderMatch("XDAP-Accept:")) ==
                0);

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
        long status = http->read_url(basic_digest_pw_url, dump, resp_h);

        DBG(cerr << endl << http->d_upstring << endl);
        CPPUNIT_ASSERT(http->d_upstring == "jimg:dods_digest");
        DBG(cerr << "Status: " << status << endl);
        CPPUNIT_ASSERT(status == 200);
    }

#endif
};

CPPUNIT_TEST_SUITE_REGISTRATION (HTTPConnectMTTest);

}

int main(int argc, char *argv[])
{
    return run_tests<libdap::HTTPConnectMTTest>(argc, argv) ? 0 : 1;
}
