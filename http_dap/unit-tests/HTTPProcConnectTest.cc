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
#include "HTTPCacheTable.h"
#include "debug.h"

#include "run_tests_cppunit.h"

using namespace CppUnit;
using namespace std;

#define prolog std::string("HTTPProcConnectTest::").append(__func__).append("() - ")

namespace libdap {

inline static uint64_t file_size(FILE *fp)
{
    struct stat s{};
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

        // clean the http_proc_*.out files that are made when set_verbose_runtime() is true.
        // Running the clean here leaves the files around from the last test.
        system("rm -f http_proc_*.out");
    }

    CPPUNIT_TEST_SUITE (HTTPProcConnectTest);

        CPPUNIT_TEST(fetch_url_test_mp_url_already_cached);
        CPPUNIT_TEST(fetch_url_test_mp_url_serial_access);
        CPPUNIT_TEST(fetch_url_test_mp_url_parallel_access);

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
            pid_t pid[num_processes];
            for (int i = 0; i < num_processes; i++) {
                pid[i] = fork();
                if (pid[i] == 0) {
                    auto hc_mp = std::make_unique<HTTPConnect>(RCReader::instance());
                    if (debug) hc_mp->set_verbose_runtime(true);
                    bool result = fetch_url_test(hc_mp.get(), netcdf_das_url, 927);
                    exit(result ? EXIT_SUCCESS : EXIT_FAILURE);
                } else if (pid[i] < 0) {
                    CPPUNIT_FAIL("Failed to fork child process");
                }
            }

            // Wait for child processes to complete and collect their exit status
            int status;
            for (int i = 0; i < num_processes; i++) {
                waitpid(pid[i], &status, 0);
                DBG(cerr << "Child process " << i << " (" << getpid() << ") exited with status " << WEXITSTATUS(status) << endl);
                CPPUNIT_ASSERT_MESSAGE("All these responses should be in the cache", WEXITSTATUS(status) == EXIT_SUCCESS);
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
            const int num_processes = 1;
            pid_t pid[num_processes];
            int status;
            int num_cached = 0;

            for (int i = 0; i < num_processes; i++) {
                pid[i] = fork();
                if (pid[i] == 0) {
                    auto hc_mp = std::make_unique<HTTPConnect>(RCReader::instance());
                    if (debug) hc_mp->set_verbose_runtime(true);
                    bool result = fetch_url_test(hc_mp.get(), netcdf_das_url, 927);
                    exit(result ? EXIT_SUCCESS : EXIT_FAILURE);
                } else if (pid[i] > 0) {
                    // Parent process
                    waitpid(pid[i], &status, 0);
                    DBG(cerr << "Child process " << i << " (" << pid[i] << ") exited with status " << WEXITSTATUS(status) << endl);
                    if (WEXITSTATUS(status) == EXIT_SUCCESS)
                        num_cached++;
                } else if (pid[i] < 0) {
                    CPPUNIT_FAIL("Failed to fork child process");
                }
            }

            CPPUNIT_ASSERT_MESSAGE("Three responses should be in the cache", num_cached == num_processes - 1);
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
            pid_t pid[num_processes];
            for (int i = 0; i < num_processes; i++) {
                pid[i] = fork();
                if (pid[i] == 0) {
                    if (i != 0)
                        sleep(3);   // Wait for the first process to cache the response
                    auto hc_mp = std::make_unique<HTTPConnect>(RCReader::instance());
                    if (debug) hc_mp->set_verbose_runtime(true);
                    bool result = fetch_url_test(hc_mp.get(), netcdf_das_url, 927);
                    exit(result ? EXIT_SUCCESS : EXIT_FAILURE);
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
                if (WEXITSTATUS(status) == EXIT_SUCCESS)
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
};

CPPUNIT_TEST_SUITE_REGISTRATION (HTTPProcConnectTest);

}

int main(int argc, char *argv[])
{
    return run_tests<libdap::HTTPProcConnectTest>(argc, argv) ? 0 : 1;
}
