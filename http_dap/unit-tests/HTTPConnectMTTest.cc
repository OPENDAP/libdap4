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

#include <sys/stat.h>

#include <iterator>
#include <memory>
#include <string>
#include <thread>

#include "RCReader.h"
#include "HTTPResponse.h"
#include "HTTPCache.h"
#include "HTTPConnect.h"
#include "debug.h"

#include "run_tests_cppunit.h"

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
    HTTPCache *d_cache = HTTPCache::instance("cache-testsuite/http_mt_cache/");
    unique_ptr<HTTPConnect> http{nullptr};
    string url_304{"http://test.opendap.org/test-304.html"};
    string basic_pw_url{"http://jimg:dods_test@test.opendap.org/basic/page.txt"};
    string basic_digest_pw_url{"http://jimg:dods_digest@test.opendap.org/basic/page.txt"};
    // The etag value needs to be updated when the server changes, etc.
    // Also, the value looks like a 'sectret' to git secrets, which will complain. jhrg 2/23/23
    string etag{"\"157-3df0e26958000\""};   // New httpd (dockerized), new etag. ndp - 12/06/22
    string lm{"Wed, 13 Jul 2005 19:32:26 GMT"};
    string netcdf_das_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.das"};

public:
    HTTPConnectMTTest() = default;

    ~HTTPConnectMTTest() override = default;

    void setUp() override
    {
        setenv("DODS_CONF", "cache-testsuite/dodsrc_w_caching", 1);
        // This is coupled with the cache name in cache-testsuite/dodsrc_w_caching
        if (access("cache-testsuite/http_mt_cache/", F_OK) == 0) {
            CPPUNIT_ASSERT_MESSAGE("The HTTPCache::instance() is null!", d_cache);
            CPPUNIT_ASSERT_MESSAGE("The HTTPCache directory is not correct", d_cache->get_cache_root() == "cache-testsuite/http_mt_cache/");
            // Some tests disable the cache, so we need to make sure it's enabled.
            d_cache->set_cache_enabled(true);
            d_cache->purge_cache();
        }
    }

    CPPUNIT_TEST_SUITE (HTTPConnectMTTest);

        CPPUNIT_TEST(fetch_url_test);
        CPPUNIT_TEST(fetch_url_test_304_mt);
        CPPUNIT_TEST(fetch_url_test_304_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_nc_mt);
        CPPUNIT_TEST(fetch_url_test_nc_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache_multi_access);

        CPPUNIT_TEST(fetch_url_test_cpp);

    CPPUNIT_TEST_SUITE_END();

    void fetch_url_test()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            http = std::make_unique<HTTPConnect>(RCReader::instance());
            // Disable the cache for this test.
            http->set_cache_enabled(false);
            DBG(cerr << "hc->is_cache_enabled()? " << boolalpha << http->is_cache_enabled() << endl);

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
        catch (const std::exception &e) {
            CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
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
            // Disable the cache for this test.
            http->set_cache_enabled(false);
            DBG(cerr << "hc->is_cache_enabled()? " << boolalpha << http->is_cache_enabled() << endl);
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
                // Disable the cache for this test.
                hc->set_cache_enabled(false);
                DBG(cerr << "hc->is_cache_enabled()? " << boolalpha << hc->is_cache_enabled() << endl);
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
        catch (const std::exception &e) {
            CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
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
        catch (const std::exception &e) {
            CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
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
                hc->set_cache_enabled(false);
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

            // None should be cached; the cache is disabled.
            int cached = 0;
            if (http_1->is_cached_response()) cached++;
            if (http_2->is_cached_response()) cached++;
            if (http_3->is_cached_response()) cached++;
            if (http_4->is_cached_response()) cached++;
            CPPUNIT_ASSERT_MESSAGE("Three responses should be cached, one not (" + std::to_string(cached) + " was)", cached == 0);
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
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

    void fetch_url_test_nc_mt_w_cache()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc, uint32_t expected_size) {
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                DBG(cerr << "hc_lambda: " << url << ", response size: " << file_size(stuff->get_stream()) << endl);
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
                CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
                CPPUNIT_ASSERT(!feof(stuff->get_stream()));
                CPPUNIT_ASSERT_MESSAGE("response size: " + std::to_string(file_size(stuff->get_stream())),
                                       file_size(stuff->get_stream()) == expected_size);
            };

            std::thread thread1(hc_lambda, netcdf_das_url, http_1.get(), 927);
            std::thread thread2(hc_lambda, netcdf_das_url, http_2.get(), 927);
            std::thread thread3(hc_lambda, netcdf_das_url, http_3.get(), 927);
            std::thread thread4(hc_lambda, netcdf_das_url, http_4.get(), 927);

            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();

            // Three should be cached; the cache is enabled.
            int cached = 0;
            if (http_1->is_cached_response()) cached++;
            if (http_2->is_cached_response()) cached++;
            if (http_3->is_cached_response()) cached++;
            if (http_4->is_cached_response()) cached++;
            CPPUNIT_ASSERT_MESSAGE("Three responses should be cached; number cached: " + std::to_string(cached) + ".", cached == 3);
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
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

    void fetch_url_test_diff_urls_mt_w_cache()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc, uint32_t expected_size) {
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                DBG(cerr << "hc_lambda: " << url << ", response size: " << file_size(stuff->get_stream()) << endl);
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

            // None should be cached; different URLs, accessed each for the first time.
            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_1->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_2->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_3->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !http_4->is_cached_response());
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
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
    void fetch_url_test_diff_urls_mt_w_cache_multi_access()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc, uint32_t expected_size) {
                hc->set_verbose_runtime(false);
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

            DBG(cerr << "http_1->is_cached_response(): " << boolalpha << http_1->is_cached_response() << endl);
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_1->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_2->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_3->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_4->is_cached_response());

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

            DBG(cerr << "http_5->is_cached_response(): " << boolalpha << http_5->is_cached_response() << endl);
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_5->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_6->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_7->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_8->is_cached_response());
        }
        catch (InternalErr &e) {
            CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());
        }
        catch (Error &e) {
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

#if 0

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


#endif
};

CPPUNIT_TEST_SUITE_REGISTRATION (HTTPConnectMTTest);

}

int main(int argc, char *argv[])
{
    return run_tests<libdap::HTTPConnectMTTest>(argc, argv) ? 0 : 1;
}
