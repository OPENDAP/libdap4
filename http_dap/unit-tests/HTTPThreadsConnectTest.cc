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

#include <future>
#include <iterator>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "HTTPCache.h"
#include "HTTPConnect.h"
#include "HTTPResponse.h"
#include "RCReader.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

#define CATCH_ALL_TEST_EXCEPTIONS                                                                                      \
    catch (const InternalErr &e) {                                                                                     \
        CPPUNIT_FAIL("Caught an InternalErr from fetch_url: " + e.get_error_message());                                \
    }                                                                                                                  \
    catch (const Error &e) {                                                                                           \
        CPPUNIT_FAIL("Caught an Error from fetch_url: " + e.get_error_message());                                      \
    }                                                                                                                  \
    catch (const std::exception &e) {                                                                                  \
        CPPUNIT_FAIL(string("Caught an std::exception from fetch_url: ") + e.what());                                  \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        cerr << "Caught unknown exception" << endl;                                                                    \
        throw;                                                                                                         \
    }

using namespace CppUnit;
using namespace std;

const static string cache_dir{string(TEST_BUILD_DIR) + "/cache-testsuite/http_mt_cache/"};
constexpr auto THREAD_COUNT = 4; // Edit the code below if this is changed. jhrg 7/31/25

#define prolog std::string("HTTPThreadsConnectTest::").append(__func__).append("() - ")

namespace libdap {
// The response size varies because sometimes a zero-length end-chunk is sent, so four bytes for
// the chunk header appear for the variable.
const auto dap_url_1 = 26221;
const auto dap_url_2 = 26201;
const auto dmr_url = 3103;
const auto dds_url = 197;
const auto das_url = 927;

inline static uint64_t file_size(FILE *fp) {
    struct stat s {};
    fstat(fileno(fp), &s);
    return s.st_size;
}

class HTTPThreadsConnectTest : public TestFixture {
private:
    HTTPCache *d_cache = HTTPCache::instance(cache_dir);
    unique_ptr<HTTPConnect> http{nullptr};
    string url_304{"http://test.opendap.org/test-304.html"};
    string basic_pw_url{"http://jimg:dods_test@test.opendap.org/basic/page.txt"};
    string basic_digest_pw_url{"http://jimg:dods_digest@test.opendap.org/basic/page.txt"};
    // The etag value needs to be updated when the server changes, etc.
    // Also, the value looks like a 'secret' to git secrets, which will complain. jhrg 2/23/23
    string etag{"\"157-3df0e26958000\""}; // New httpd (dockerized), new etag. ndp - 12/06/22
    string lm{"Wed, 13 Jul 2005 19:32:26 GMT"};
    string netcdf_das_url{"http://test.opendap.org/dap/data/nc/fnoc1.nc.das"};

public:
    HTTPThreadsConnectTest() = default;

    ~HTTPThreadsConnectTest() override = default;

    void setUp() override {
        setenv("DODS_CONF", (string(TEST_SRC_DIR) + "/cache-testsuite/dodsrc_w_caching").c_str(), 1);
        // This is coupled with the cache name in cache-testsuite/dodsrc_w_caching
        if (access(cache_dir.c_str(), F_OK) == 0) {
            CPPUNIT_ASSERT_MESSAGE("The HTTPCache::instance() is null!", d_cache);
            DBG(cerr << "The HTTPCache root is: " << d_cache->get_cache_root() << endl);
            DBG(cerr << "The HTTPCache directory is: " << cache_dir << endl);
            CPPUNIT_ASSERT_MESSAGE("The HTTPCache directory is not correct", d_cache->get_cache_root() == cache_dir);
            // Some tests disable the cache, so we need to make sure it's enabled.
            d_cache->set_cache_enabled(true);
            d_cache->purge_cache();
        } else {
            DBG(cerr << "Creating cache directory: " << cache_dir << endl);
            system(("mkdir -p " + cache_dir).c_str());
            DBG(system("ls -l cache-testsuite/http_*"));
        }
    }

    CPPUNIT_TEST_SUITE(HTTPThreadsConnectTest);

    CPPUNIT_TEST(fetch_url_test);
    CPPUNIT_TEST(fetch_url_test_304_mt);
    CPPUNIT_TEST(fetch_url_test_304_mt_w_cache);
    CPPUNIT_TEST(fetch_url_test_nc_mt);
    CPPUNIT_TEST(fetch_url_test_nc_mt_w_cache);

    // See HYRAX-1849
    CPPUNIT_TEST(fetch_url_test_dap_url_no_crc_mt_w_cache);
    CPPUNIT_TEST(fetch_url_test_dap_url_no_crc_no_key_mt_w_cache);
    CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache);
    CPPUNIT_TEST(fetch_url_test_diff_urls_mt_w_cache_multi_access);

    CPPUNIT_TEST(fetch_url_test_302_urls_mt_w_cache_multi_access);

    CPPUNIT_TEST(fetch_url_test_cpp);

    CPPUNIT_TEST_SUITE_END();

    void fetch_url_test() {
        DBG(cerr << "Entering " << __func__ << endl);
        try {
            http = std::make_unique<HTTPConnect>(RCReader::instance());
            // Disable the cache for this test.
            http->set_cache_enabled(false);
            DBG(cerr << "hc->is_cache_enabled()? " << boolalpha << http->is_cache_enabled() << endl);

            unique_ptr<HTTPResponse> stuff(http->fetch_url(url_304));
            char c;
            CPPUNIT_ASSERT(fread(&c, 1, 1, stuff->get_stream()) == 1 && !ferror(stuff->get_stream()) &&
                           !feof(stuff->get_stream()));
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_cpp() {
        DBG(cerr << "Entering " << __func__ << endl);
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
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_304_mt() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < THREAD_COUNT; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            // Launch async tasks
            vector<future<void>> futures;
            futures.reserve(THREAD_COUNT);
            for (int i = 0; i < THREAD_COUNT; ++i) {
                futures.emplace_back(async(launch::async, [&, i]() {
                    auto &hc = *conns[i];
                    hc.set_cache_enabled(false);
                    unique_ptr<HTTPResponse> resp(hc.fetch_url(url_304));
                    char c;
                    CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1 && c == '<' &&
                                   !ferror(resp->get_stream()) && !feof(resp->get_stream()));
                    DBG(cerr << "hc[" << i << "] fetched " << url_304 << ", char=" << c << endl);
                }));
            }

            // Check for exceptions
            for (auto &f : futures) {
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("fetch_url_test_304_mt async failed", f.get());
            }
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_304_mt_w_cache() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < THREAD_COUNT; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            vector<future<void>> futures;
            futures.reserve(THREAD_COUNT);
            for (int i = 0; i < THREAD_COUNT; ++i) {
                futures.emplace_back(async(launch::async, [&, i]() {
                    auto &hc = *conns[i];
                    unique_ptr<HTTPResponse> resp(hc.fetch_url(url_304));
                    char c;
                    CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1 && c == '<' &&
                                   !ferror(resp->get_stream()) && !feof(resp->get_stream()));
                    DBG(cerr << "hc[" << i << "] fetched " << url_304 << ", char=" << c << endl);
                }));
            }

            for (auto &f : futures) {
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("fetch_url_test_304_mt_w_cache async failed", f.get());
            }
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_nc_mt() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < THREAD_COUNT; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            vector<future<void>> futures;
            futures.reserve(THREAD_COUNT);
            for (int i = 0; i < THREAD_COUNT; ++i) {
                futures.emplace_back(async(launch::async, [&, i]() {
                    auto &hc = *conns[i];
                    hc.set_cache_enabled(false);
                    unique_ptr<HTTPResponse> resp(hc.fetch_url(netcdf_das_url));
                    char c;
                    CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1 && c == 'A' &&
                                   !ferror(resp->get_stream()) && !feof(resp->get_stream()));
                    DBG(cerr << "hc[" << i << "] fetched " << netcdf_das_url << ", char=" << c << endl);
                }));
            }
            for (auto &f : futures) {
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("fetch_url_test_nc_mt async failed", f.get());
            }
            int cached = 0;
            for (auto &u : conns)
                if (u->is_cached_response())
                    ++cached;
            CPPUNIT_ASSERT_MESSAGE("None should be cached; got " + to_string(cached), cached == 0);
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_nc_mt_w_cache() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < THREAD_COUNT; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            vector<future<void>> futures;
            futures.reserve(THREAD_COUNT);
            for (int i = 0; i < THREAD_COUNT; ++i) {
                futures.emplace_back(async(launch::async, [&, i]() {
                    auto &hc = *conns[i];
                    unique_ptr<HTTPResponse> resp(hc.fetch_url(netcdf_das_url));
                    char c;
                    CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1);
                    CPPUNIT_ASSERT(!ferror(resp->get_stream()));
                    CPPUNIT_ASSERT(!feof(resp->get_stream()));
                    CPPUNIT_ASSERT_MESSAGE("response size: " + to_string(file_size(resp->get_stream())),
                                           file_size(resp->get_stream()) == das_url);
                }));
            }
            for (auto &f : futures) {
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("fetch_url_test_nc_mt_w_cache async failed", f.get());
            }
            int cached = 0;
            for (auto &u : conns)
                if (u->is_cached_response())
                    ++cached;
            CPPUNIT_ASSERT_MESSAGE("Three should be cached; got " + to_string(cached), cached == 3);
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_dap_url_no_crc_mt_w_cache() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < THREAD_COUNT; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            struct Job {
                string url;
                uint32_t sz_1;
                uint32_t sz_2;
                HTTPConnect *hc;
            };
            vector<Job> jobs = {{string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap?dap4.checksum=false"),
                                 dap_url_1, dap_url_2, conns[0].get()},
                                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap?dap4.checksum=false"),
                                 dap_url_1, dap_url_2, conns[1].get()},
                                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap?dap4.checksum=false"),
                                 dap_url_1, dap_url_2, conns[2].get()},
                                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap?dap4.checksum=false"),
                                 dap_url_1, dap_url_2, conns[3].get()}};

            vector<future<void>> futures;
            futures.reserve(jobs.size());
            for (auto &job : jobs) {
                futures.emplace_back(async(launch::async, [=]() {
                    unique_ptr<HTTPResponse> resp(job.hc->fetch_url(job.url));
                    char c;
                    CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1);
                    CPPUNIT_ASSERT(!ferror(resp->get_stream()));
                    CPPUNIT_ASSERT(!feof(resp->get_stream()));
                    auto size = file_size(resp->get_stream());
                    DBG(cerr << "Response file size: " << size << ", expected size: " << job.sz_1 << " or " << job.sz_2
                             << "\n");
                    CPPUNIT_ASSERT_MESSAGE("response size: " + to_string(size), size == job.sz_1 || size == job.sz_2);
                }));
            }
            for (auto &f : futures)
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("fetch_url_test_diff_urls_mt_w_cache async failed", f.get());
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_dap_url_no_crc_no_key_mt_w_cache() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < THREAD_COUNT; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            struct Job {
                string url;
                uint32_t sz_1;
                uint32_t sz_2;
                HTTPConnect *hc;
            };
            vector<Job> jobs = {
                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap"), dap_url_1, dap_url_2, conns[0].get()},
                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap"), dap_url_1, dap_url_2, conns[1].get()},
                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap"), dap_url_1, dap_url_2, conns[2].get()},
                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dap"), dap_url_1, dap_url_2, conns[3].get()}};

            vector<future<void>> futures;
            futures.reserve(jobs.size());
            for (auto &job : jobs) {
                futures.emplace_back(async(launch::async, [=]() {
                    unique_ptr<HTTPResponse> resp(job.hc->fetch_url(job.url));
                    char c;
                    CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1);
                    CPPUNIT_ASSERT(!ferror(resp->get_stream()));
                    CPPUNIT_ASSERT(!feof(resp->get_stream()));
                    CPPUNIT_ASSERT_MESSAGE("response size: " + to_string(file_size(resp->get_stream())),
                                           file_size(resp->get_stream()) == job.sz_1 ||
                                               file_size(resp->get_stream()) == job.sz_2);
                }));
            }
            for (auto &f : futures)
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("fetch_url_test_diff_urls_mt_w_cache async failed", f.get());
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_diff_urls_mt_w_cache() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < 3; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            struct Job {
                string url;
                uint32_t sz;
                HTTPConnect *hc;
            };
            vector<Job> jobs = {{netcdf_das_url, das_url, conns[0].get()},
                                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dds"), dds_url, conns[1].get()},
                                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dmr"), dmr_url, conns[2].get()}};

            vector<future<void>> futures;
            futures.reserve(jobs.size());
            for (auto &job : jobs) {
                futures.emplace_back(async(launch::async, [=]() {
                    unique_ptr<HTTPResponse> resp(job.hc->fetch_url(job.url));
                    char c;
                    CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1);
                    CPPUNIT_ASSERT(!ferror(resp->get_stream()));
                    CPPUNIT_ASSERT(!feof(resp->get_stream()));
                    CPPUNIT_ASSERT_MESSAGE("response size: " + to_string(file_size(resp->get_stream())),
                                           file_size(resp->get_stream()) == job.sz);
                }));
            }
            for (auto &f : futures)
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("fetch_url_test_diff_urls_mt_w_cache async failed", f.get());

            for (int i = 0; i < 3; ++i)
                CPPUNIT_ASSERT_MESSAGE("Response should not be cached", !conns[i]->is_cached_response());
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_diff_urls_mt_w_cache_multi_access() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < 3; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            struct Job {
                string url;
                uint32_t sz;
                HTTPConnect *hc;
            };
            vector<Job> jobs_first = {
                {netcdf_das_url, das_url, conns[0].get()},
                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dds"), dds_url, conns[1].get()},
                {string("http://test.opendap.org/dap/data/nc/fnoc1.nc.dmr"), dmr_url, conns[2].get()}};

            auto run_jobs = [&](const vector<Job> &jobs) {
                vector<future<void>> futs;
                futs.reserve(jobs.size());
                for (auto &job : jobs)
                    futs.emplace_back(async(launch::async, [=]() {
                        job.hc->set_verbose_runtime(false);
                        unique_ptr<HTTPResponse> resp(job.hc->fetch_url(job.url));
                        char c;
                        CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1);
                        CPPUNIT_ASSERT(!ferror(resp->get_stream()));
                        CPPUNIT_ASSERT(!feof(resp->get_stream()));
                        CPPUNIT_ASSERT_MESSAGE("response size: " + to_string(file_size(resp->get_stream())),
                                               file_size(resp->get_stream()) == job.sz);
                    }));
                for (auto &f : futs)
                    CPPUNIT_ASSERT_NO_THROW_MESSAGE("async job failed", f.get());
            };

            // first access: not cached
            run_jobs(jobs_first);
            for (int i = 0; i < jobs_first.size(); ++i)
                CPPUNIT_ASSERT_MESSAGE("Should not be cached", !conns[i]->is_cached_response());

            // second access: cached
            vector<Job> jobs_repeat = jobs_first;
            run_jobs(jobs_repeat);
            for (int i = 0; i < jobs_first.size(); ++i)
                CPPUNIT_ASSERT_MESSAGE("Should be cached", conns[i]->is_cached_response());

            // new instances: still cached
            vector<unique_ptr<HTTPConnect>> conns2;
            for (int i = 0; i < jobs_first.size(); ++i)
                conns2.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));
            vector<Job> jobs_new = jobs_first;
            for (int i = 0; i < jobs_first.size(); ++i)
                jobs_new[i].hc = conns2[i].get();
            run_jobs(jobs_new);
            for (int i = 0; i < jobs_first.size(); ++i)
                CPPUNIT_ASSERT_MESSAGE("Should be cached on new instance", conns2[i]->is_cached_response());
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }

    void fetch_url_test_302_urls_mt_w_cache_multi_access() {
        DBG(cerr << prolog << endl);
        try {
            vector<unique_ptr<HTTPConnect>> conns;
            for (int i = 0; i < THREAD_COUNT; ++i)
                conns.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));

            string opendap_url{"http://test.opendap.org/opendap"};

            auto run_simple = [&](int idx) {
                unique_ptr<HTTPResponse> resp(conns[idx]->fetch_url(opendap_url));
                char c;
                CPPUNIT_ASSERT(fread(&c, 1, 1, resp->get_stream()) == 1);
                CPPUNIT_ASSERT(!ferror(resp->get_stream()));
                CPPUNIT_ASSERT(!feof(resp->get_stream()));
                DBG(cerr << "hc[" << idx << "] got '" << c << "'" << endl);
            };

            // first wave
            vector<future<void>> futs;
            for (int i = 0; i < THREAD_COUNT; ++i)
                futs.emplace_back(async(launch::async, run_simple, i));
            for (auto &f : futs)
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("first 302 wave failed", f.get());

            int cached = 0;
            for (auto &u : conns)
                if (u->is_cached_response())
                    ++cached;
            CPPUNIT_ASSERT_MESSAGE("Expected 3 cached; got " + to_string(cached), cached == 3);

            // second wave
            futs.clear();
            for (int i = 0; i < 2; ++i)
                futs.emplace_back(async(launch::async, run_simple, i));
            for (auto &f : futs)
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("second 302 wave failed", f.get());
            for (int i = 0; i < 2; ++i)
                CPPUNIT_ASSERT_MESSAGE("Should be cached", conns[i]->is_cached_response());

            // new instances
            vector<unique_ptr<HTTPConnect>> conns2;
            for (int i = 0; i < 2; ++i)
                conns2.emplace_back(make_unique<HTTPConnect>(RCReader::instance()));
            futs.clear();
            for (int i = 0; i < 2; ++i)
                futs.emplace_back(async(
                    launch::async,
                    [&](int idx) {
                        unique_ptr<HTTPResponse> r(conns2[idx]->fetch_url(opendap_url));
                        char c;
                        CPPUNIT_ASSERT(fread(&c, 1, 1, r->get_stream()) == 1);
                    },
                    i));
            for (auto &f : futs)
                CPPUNIT_ASSERT_NO_THROW_MESSAGE("new instance 302 wave failed", f.get());
            for (int i = 0; i < 2; ++i)
                CPPUNIT_ASSERT_MESSAGE("Should be cached on new instance", conns2[i]->is_cached_response());
        }
        CATCH_ALL_TEST_EXCEPTIONS
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPThreadsConnectTest);
} // namespace libdap

int main(int argc, char *argv[]) {
    bool passed = run_tests<libdap::HTTPThreadsConnectTest>(argc, argv) ? 0 : 1;

    system(("rm -rf " + cache_dir).c_str());

    return passed;
}
