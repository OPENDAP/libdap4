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

#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

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

#define prolog std::string("HTTPProcConnectTest::").append(__func__).append("() - ")

namespace libdap {

inline static uint64_t file_size(FILE *fp)
{
    struct stat s;
    fstat(fileno(fp), &s);
    return s.st_size;
}

#define CHILD_ASSERT_MESSAGE(os, message, condition) \
    if (!(condition)) {         \
        os << message << endl;  \
        exit(1);                \
    }

#define CHILD_DBG(os, message) \
    os << message << endl;

/**
 * Per-process test code for HTTPConnect::fetch_url()
 * @param hc The HTTPConnect object to use.
 * @param url The URL to fetch.
 * @param expected_size The expected size of the response.
 * @return True if the response was cached, false otherwise.
 */
bool fetch_url_test(HTTPConnect *hc, const string &url, uint64_t expected_size) {
    auto ofs = ofstream("http_proc_" + std::to_string(getpid()) + ".out");
    CHILD_DBG(ofs, "hc->is_cache_enabled()? " << boolalpha << hc->is_cache_enabled() << endl);

    unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
    CHILD_DBG(ofs, url << ", response size: " << file_size(stuff->get_stream()) << endl);

    // CPPUNIT_FAIL("What happens when the child process fails an assert?");

    CHILD_ASSERT_MESSAGE(ofs, "Error from the response stream", !ferror(stuff->get_stream()));
    CHILD_ASSERT_MESSAGE(ofs, "EOF from the response stream", !feof(stuff->get_stream()));

    char c;
    CHILD_ASSERT_MESSAGE(ofs, "Did not read a character from the response stream",
                         fread(&c, 1, 1, stuff->get_stream()) == 1);
    CHILD_ASSERT_MESSAGE(ofs, "Wrong response size: " + std::to_string(file_size(stuff->get_stream())),
                         file_size(stuff->get_stream()) == expected_size);

    CHILD_DBG(ofs, "is_cached_response: " << boolalpha << hc->is_cached_response());
    return hc->is_cached_response();
}

class HTTPProcConnectTest : public TestFixture {
private:
    HTTPCache *d_cache = HTTPCache::instance("cache-testsuite/http_mp_cache/");
    unique_ptr<HTTPConnect> http{nullptr};
    string url_304{"http://test.opendap.org/test-304.html"};
    string netcdf_das_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.das"};

public:
    HTTPProcConnectTest() = default;

    ~HTTPProcConnectTest() override = default;

    void setUp() override
    {
        setenv("DODS_CONF", "cache-testsuite/dodsrc_mp_caching", 1);
        // This is coupled with the cache name in cache-testsuite/dodsrc_mp_caching
        if (access("cache-testsuite/http_mp_cache/", F_OK) == 0) {
            CPPUNIT_ASSERT_MESSAGE("The HTTPCache::instance() is null!", d_cache);
            CPPUNIT_ASSERT_MESSAGE("The HTTPCache directory is not correct",
                                   d_cache->get_cache_root() == "cache-testsuite/http_mp_cache/");
            // Some tests disable the cache, so we need to make sure it's enabled.
            d_cache->set_cache_enabled(true);
            d_cache->purge_cache();
        }
    }

    CPPUNIT_TEST_SUITE (HTTPProcConnectTest);

        CPPUNIT_TEST(fetch_url_test_mp_url_already_cached);
        CPPUNIT_TEST(fetch_url_test_mp_url_serial_access);
        CPPUNIT_TEST(fetch_url_test_mp_url_parallel_access);
#if 0
        CPPUNIT_TEST(fetch_url_test_304_mt);
        CPPUNIT_TEST(fetch_url_test_304_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_nc_mt);
        CPPUNIT_TEST(fetch_url_test_nc_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache);
        CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache_multi_access);
        CPPUNIT_TEST(fetch_url_test_302_urls_mt_w_cache_multi_access);

        CPPUNIT_TEST(fetch_url_test_cpp);
#endif

    CPPUNIT_TEST_SUITE_END();

    // This is a test that builds the HTTPConnect objects in two batches. The first one will cache the
    // response and the second set will see that and read from the cache.
    void fetch_url_test_mp_url_already_cached()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            // Run the test the simple way first...
            http = std::make_unique<HTTPConnect>(RCReader::instance());
            if (debug) http->set_verbose_runtime(true);
            auto res = fetch_url_test(http.get(), netcdf_das_url, 927);
            CPPUNIT_ASSERT_MESSAGE("The response should not be cached", !res);

            // Create child processes to concurrently access the cache. Note that these instances
            // of HTTPConnect all share the same HTTPCacheTable. That means that they 'see' the cached
            // data retrieved in the above access.
            const int num_processes = 4;
            unique_ptr<HTTPConnect> hc_mp[num_processes] = {std::make_unique<HTTPConnect>(RCReader::instance()),
                                                std::make_unique<HTTPConnect>(RCReader::instance()),
                                                std::make_unique<HTTPConnect>(RCReader::instance()),
                                                std::make_unique<HTTPConnect>(RCReader::instance())};

            pid_t pid[num_processes];
            for (int i = 0; i < num_processes; i++) {
                if (debug) hc_mp[i]->set_verbose_runtime(true);
                pid[i] = fork();
                if (pid[i] == 0) {
                    // Child process, fetch_url_test() returns true if the URL was in the cache.
                    bool result = fetch_url_test(hc_mp[i].get(), netcdf_das_url, 927);
                    exit(result ? 1 : 0);   // 1 = true, 0 = false, not the usual EXIT_SUCCESS/EXIT_FAILURE
                } else if (pid[i] < 0) {
                    // Error: failed to fork process
                    CPPUNIT_FAIL("Failed to fork child process");
                }
            }

            // Wait for child processes to complete and collect their exit status
            int status;
            for (int i = 0; i < num_processes; i++) {
                waitpid(pid[i], &status, 0);
                DBG(cerr << "Child process " << i << " (" << getpid() << ") exited with status " << WEXITSTATUS(status) << endl);
                CPPUNIT_ASSERT_MESSAGE("All these responses should be in the cache", WEXITSTATUS(status));
            }
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

    // All HTTPConnect processes made in one batch but use the cache serially.
    void fetch_url_test_mp_url_serial_access()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            // Create child processes to concurrently access the cache. The first access will cache the data.
            // Subsequent accesses will need to have a freshly made HTTPCacheTable object by reading the cache
            // index before looking for the response in the cache
            const int num_processes = 4;
            unique_ptr<HTTPConnect> hc_mp[num_processes] = {std::make_unique<HTTPConnect>(RCReader::instance()),
                                                            std::make_unique<HTTPConnect>(RCReader::instance()),
                                                            std::make_unique<HTTPConnect>(RCReader::instance()),
                                                            std::make_unique<HTTPConnect>(RCReader::instance())};
            pid_t pid[num_processes];
            int status;
            int num_cached = 0;

            for (int i = 0; i < num_processes; i++) {
                if (debug) hc_mp[i]->set_verbose_runtime(true);
                pid[i] = fork();
                if (pid[i] == 0) {
                    try {
                        // Child process, fetch_url_test() returns true if the URL was in the cache.
                        bool result = fetch_url_test(hc_mp[i].get(), netcdf_das_url, 927);
                        exit(result ? 1 : 0);   // 1 = true, 0 = false, not the usual EXIT_SUCCESS/EXIT_FAILURE
                    }
                    catch (Error &e) {
                        CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
                    }
                    catch (const std::exception &e) {
                        CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
                    }
                } else if (pid[i] > 0) {
                    // Parent process
                    waitpid(pid[i], &status, 0);
                    DBG(cerr << "Child process " << i << " (" << pid[i] << ") exited with status " << WEXITSTATUS(status) << endl);
                    if (WEXITSTATUS(status) == 1)
                        num_cached++;
                } else if (pid[i] < 0) {
                    // Error: failed to fork process
                    CPPUNIT_FAIL("Failed to fork child process");
                }
            }

            CPPUNIT_ASSERT_MESSAGE("Three responses should be in the cache", num_cached == 3);
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

    void fetch_url_test_mp_url_parallel_access()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            // Create child processes to concurrently access the cache
            const int num_processes = 4;
            unique_ptr<HTTPConnect> hc_mp[num_processes] = {std::make_unique<HTTPConnect>(RCReader::instance()),
                                                            std::make_unique<HTTPConnect>(RCReader::instance()),
                                                            std::make_unique<HTTPConnect>(RCReader::instance()),
                                                            std::make_unique<HTTPConnect>(RCReader::instance())};
            pid_t pid[num_processes];
            for (int i = 0; i < num_processes; i++) {
                hc_mp[i]->set_verbose_runtime(true);
                pid[i] = fork();
                if (pid[i] == 0) {
                    if (i != 0)
                        sleep(2);   // Wait for the first process to cache the response
                    try {
                        // Child process, fetch_url_test() returns true if the URL was in the cache.
                        bool result = fetch_url_test(hc_mp[i].get(), netcdf_das_url, 927);
                        exit(result ? 1 : 0);   // 1 = true, 0 = false, not the usual EXIT_SUCCESS/EXIT_FAILURE
                    }
                    catch (Error &e) {
                        CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());
                    }
                    catch (const std::exception &e) {
                        CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());
                    }
                } else if (pid[i] < 0) {
                    // Error: failed to fork process
                    CPPUNIT_FAIL("Failed to fork child process");
                }
            }

            // Wait for child processes to complete and collect their exit status
            int status;
            int num_cached = 0;
            for (int i = 0; i < num_processes; i++) {
                waitpid(pid[i], &status, 0);
                DBG(cerr << "Child process " << i << " (" << pid[i] << ") exited with status " << WEXITSTATUS(status) << endl);
                if (WEXITSTATUS(status) == 1)
                    num_cached++;
            }

            CPPUNIT_ASSERT_MESSAGE("Three responses should be in the cache", num_cached == 3);
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

    void fetch_url_test_302_urls_mt_w_cache_multi_access()
    {
        DBG(cerr << "Entering " << __func__  << endl);
        try {
            auto http_1 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_2 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_3 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_4 = std::make_unique<HTTPConnect>(RCReader::instance());

            auto hc_lambda = [](const string &url, HTTPConnect *hc) {
                hc->set_verbose_runtime(false);
                unique_ptr<HTTPResponse> stuff(hc->fetch_url(url));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1);
                DBG(cerr << "First character: " << c << endl);
                CPPUNIT_ASSERT(!ferror(stuff->get_stream()));
                CPPUNIT_ASSERT(!feof(stuff->get_stream()));
            };

            string opendap_url{"http://test.opendap.org/opendap"};

            std::thread thread1(hc_lambda, opendap_url, http_1.get());
            std::thread thread2(hc_lambda, opendap_url, http_2.get());
            std::thread thread3(hc_lambda, opendap_url, http_3.get());
            std::thread thread4(hc_lambda, opendap_url, http_4.get());

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

            // Now we access the same URLs again. The cache should be used.
            std::thread thread5(hc_lambda, opendap_url, http_1.get());
            std::thread thread6(hc_lambda, opendap_url, http_2.get());

            thread5.join();
            thread6.join();

            DBG(cerr << "http_1->is_cached_response(): " << boolalpha << http_1->is_cached_response() << endl);
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_1->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_2->is_cached_response());

            // Now we access the same URLs again using new instances of HTTPConnect.
            // The cache should be used
            auto http_5 = std::make_unique<HTTPConnect>(RCReader::instance());
            auto http_6 = std::make_unique<HTTPConnect>(RCReader::instance());

            std::thread thread9(hc_lambda, opendap_url, http_5.get());
            std::thread thread10(hc_lambda, opendap_url, http_6.get());

            thread9.join();
            thread10.join();

            DBG(cerr << "http_5->is_cached_response(): " << boolalpha << http_5->is_cached_response() << endl);
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_5->is_cached_response());
            CPPUNIT_ASSERT_MESSAGE("Response should be cached", http_6->is_cached_response());
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
#endif

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

CPPUNIT_TEST_SUITE_REGISTRATION (HTTPProcConnectTest);

}

int main(int argc, char *argv[])
{
    return run_tests<libdap::HTTPProcConnectTest>(argc, argv) ? 0 : 1;
}
