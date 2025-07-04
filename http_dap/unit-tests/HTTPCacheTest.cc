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

#include "config.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <cstdio> // for create_cache_root_test
#include <sys/stat.h>
#include <unistd.h> // for access stat

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "HTTPCache.h"
#include "HTTPCacheTable.h"
#include "HTTPConnect.h" // Used to generate a response to cache.
#include "HTTPResponse.h"
#include "RCReader.h"

#include "remove_directory.h"
#include "run_tests_cppunit.h"

using namespace CppUnit;
using namespace std;

#define prolog string("HTTPCacheTest::").append(__func__).append("() - ")

namespace libdap {

inline static uint64_t file_size(const string &name) {
    struct stat s;
    stat(name.c_str(), &s);
    return s.st_size;
}

// Note that because this test class uses the fixture 'hc' we must always
// force access to the single user/process lock for the cache. This is
// because a fixture is always created (by setUp) *before* the body of the
// test is run. So by the time we're at the first line of the test, The
// persistent store's lock has already been grabbed. 10/14/02 jhrg

class HTTPCacheTest : public TestFixture {
private:
    unique_ptr<HTTPCache> hc_p = nullptr;
    unique_ptr<HTTPConnect> http_conn_p = nullptr;
    string index_file_line{
        R"(http://test.opendap.org/test-304.html cache-testsuite/dods_cache/656/dodsKbcD0h "3f62c-157-139c2680" 1121283146 -1 343 0 656 1 7351 1121360379 3723 0)"};
    string localhost_url{"http://test.opendap.org/test-304.html"};
    string expired{"http://test.opendap.org/cgi-bin/expires.sh"};
    int hash_value = 656;
    vector<string> h;

public:
    HTTPCacheTest() {
        http_conn_p = std::make_unique<HTTPConnect>(RCReader::instance());

        h.emplace_back("ETag: jhrgjhrgjhrg");
        h.emplace_back("Last-Modified: Sat, 05 Nov 1994 08:49:37 GMT");
        h.emplace_back("Expires: Mon, 07 Nov 1994 08:49:37 GMT");
        h.emplace_back("Date: Sun, 06 Nov 1994 08:49:37 GMT");
    }

    ~HTTPCacheTest() override = default;

    void setUp() override {
        // Here we use reset because std::make_unique<>() cannot access the private constructor. jhrg 2/15/23
        hc_p.reset(new HTTPCache("cache-testsuite/dods_cache/"));
    }

    CPPUNIT_TEST_SUITE(HTTPCacheTest);

    CPPUNIT_TEST(constructor_test);
    CPPUNIT_TEST(cache_index_read_test);
    CPPUNIT_TEST(cache_index_parse_line_test);
    CPPUNIT_TEST(get_entry_from_cache_table_test);
    CPPUNIT_TEST(cache_index_write_test);
    CPPUNIT_TEST(create_cache_root_test);
    CPPUNIT_TEST(set_cache_root_test);
    CPPUNIT_TEST(initialize_cache_lock_test);

#if 0
    // Removed release_single_user_lock() from the class
    CPPUNIT_TEST (release_single_user_lock_test);
#endif
    CPPUNIT_TEST(create_hash_directory_test);
    CPPUNIT_TEST(create_location_test);
    CPPUNIT_TEST(parse_headers_test);

    CPPUNIT_TEST(calculate_time_test);
    CPPUNIT_TEST(write_metadata_test);
    CPPUNIT_TEST(cache_response_test);
    CPPUNIT_TEST(get_cached_response_test);

    CPPUNIT_TEST(perform_garbage_collection_test);
    CPPUNIT_TEST(purge_cache_and_release_cached_response_test);
    CPPUNIT_TEST(get_conditional_response_headers_test);
    CPPUNIT_TEST(update_response_test);
    CPPUNIT_TEST(cache_gc_test);

    // Make this the last test because when distcheck is run, running
    // it before other tests will break them.
    CPPUNIT_TEST(instance_test);

    CPPUNIT_TEST_SUITE_END();

    void constructor_test() {
        DBG(cerr << "hc_p->cache_index: " << hc_p->d_http_cache_table->d_cache_index << endl);
        CPPUNIT_ASSERT(hc_p->d_http_cache_table->d_cache_index == "cache-testsuite/dods_cache/.index");
        CPPUNIT_ASSERT(hc_p->d_cache_root == "cache-testsuite/dods_cache/");
        DBG(cerr << "Current size: " << hc_p->d_http_cache_table->d_current_size << endl);
        DBG(cerr << "Block size: " << hc_p->d_http_cache_table->d_block_size << endl);
        CPPUNIT_ASSERT(hc_p->d_http_cache_table->d_current_size == hc_p->d_http_cache_table->d_block_size);
    }

    void cache_index_read_test() {
        CPPUNIT_ASSERT(hc_p->d_http_cache_table->cache_index_read());

        HTTPCacheTable::CacheEntry *e = hc_p->d_http_cache_table->get_read_locked_entry_from_cache_table(localhost_url);

        CPPUNIT_ASSERT(e);
        CPPUNIT_ASSERT(e->url == localhost_url);
        e->unlock_read_response();
    }

    void cache_index_parse_line_test() {
        HTTPCacheTable::CacheEntry *e = hc_p->d_http_cache_table->cache_index_parse_line(index_file_line.c_str());

        CPPUNIT_ASSERT(e->url == localhost_url);
        CPPUNIT_ASSERT(e->cachename == "cache-testsuite/dods_cache/656/dodsKbcD0h");
        CPPUNIT_ASSERT(e->etag == "\"3f62c-157-139c2680\"");
        CPPUNIT_ASSERT(e->lm == 1121283146);
        // Skip ahead ...
        CPPUNIT_ASSERT(e->must_revalidate == false);

        delete e;
    }

    // This will also test the add_entry_to_cache_table() method.
    void get_entry_from_cache_table_test() {
        HTTPCacheTable::CacheEntry *e = hc_p->d_http_cache_table->cache_index_parse_line(index_file_line.c_str());

        // Test adding an entry and getting it back.
        hc_p->d_http_cache_table->add_entry_to_cache_table(e);

        HTTPCacheTable::CacheEntry *e2 =
            hc_p->d_http_cache_table->get_read_locked_entry_from_cache_table(localhost_url);
        CPPUNIT_ASSERT(e2);
        CPPUNIT_ASSERT(e2->url == localhost_url);
        e2->unlock_read_response();

        // Now test what happens when two entries collide.
        HTTPCacheTable::CacheEntry *e3 = hc_p->d_http_cache_table->cache_index_parse_line(index_file_line.c_str());

        // Change the url so we can tell the difference (the hash is the same)
        e3->url = "http://new.url.same.hash/test/collisions.gif";

        hc_p->d_http_cache_table->add_entry_to_cache_table(e3);

        // Use the version of get_entry... that lets us pass in the hash
        // value (as opposed to the normal version which calculates the hash
        // from the url. 10/01/02 jhrg
        HTTPCacheTable::CacheEntry *g =
            hc_p->d_http_cache_table->get_read_locked_entry_from_cache_table(hash_value, e3->url);
        CPPUNIT_ASSERT(g);
        CPPUNIT_ASSERT(g->url == e3->url);
        g->unlock_read_response();

        g = hc_p->d_http_cache_table->get_read_locked_entry_from_cache_table("http://not.in.table/never.x");
        CPPUNIT_ASSERT(g == nullptr);
    }

    void cache_index_write_test() {
        try {
            unique_ptr<HTTPCache> hc_3(new HTTPCache("cache-testsuite/dods_cache/"));
            hc_3->d_http_cache_table->add_entry_to_cache_table(
                hc_p->d_http_cache_table->cache_index_parse_line(index_file_line.c_str()));

            hc_3->d_http_cache_table->d_cache_index = hc_p->d_cache_root + "test_index";
            hc_3->d_http_cache_table->cache_index_write();

            unique_ptr<HTTPCache> hc_4(new HTTPCache("cache-testsuite/dods_cache/"));
            hc_4->d_http_cache_table->d_cache_index = hc_3->d_cache_root + "test_index";
            hc_4->d_http_cache_table->cache_index_read();

            HTTPCacheTable::CacheEntry *e =
                hc_4->d_http_cache_table->get_read_locked_entry_from_cache_table(localhost_url);
            DBG(cerr << "Got locked entry" << endl);
            CPPUNIT_ASSERT(e);
            CPPUNIT_ASSERT(e->url == localhost_url);
            e->unlock_read_response();
        } catch (const Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    void create_cache_root_test() {
        hc_p->create_cache_root("/tmp/silly/");
        CPPUNIT_ASSERT(access("/tmp/silly/", F_OK) == 0);
        // remove("/tmp/silly");
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Directory delete should not throw", remove_directory("/tmp/silly"));
    }

    void set_cache_root_test() {
        hc_p->set_cache_root("/home/jimg/test_cache");
        CPPUNIT_ASSERT(hc_p->d_cache_root == "/home/jimg/test_cache/");
        // CPPUNIT_ASSERT_NO_THROW_MESSAGE("Directory delete should not throw",
        // remove_directory("/home/jimg/test_cache/"));
    }

    void initialize_cache_lock_test() {
        hc_p->set_cache_root("/tmp/dods_test_cache");
        hc_p->create_cache_root(hc_p->get_cache_root());
        close(hc_p->d_cache_lock_fd);

        CPPUNIT_ASSERT(hc_p->m_initialize_cache_lock(hc_p->d_cache_root + ".lock"));
        CPPUNIT_ASSERT(access("/tmp/dods_test_cache/.lock", F_OK) == 0);

        // Second time should return the same fd
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("The second call to initialize...() should return and not throw",
                                        hc_p->m_initialize_cache_lock(hc_p->d_cache_root + ".lock"));

        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Directory delete should not throw", remove_directory("/tmp/dods_test_cache"));
    }

    void create_hash_directory_test() {
        hc_p->set_cache_root("/tmp/dods_test_cache");
        hc_p->create_cache_root(hc_p->get_cache_root());
        CPPUNIT_ASSERT(hc_p->d_http_cache_table->create_hash_directory(391) == "/tmp/dods_test_cache/391");
        CPPUNIT_ASSERT(access("/tmp/dods_test_cache/391", W_OK) == 0);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Directory delete should not throw", remove_directory("/tmp/dods_test_cache/"));
    }

    void create_location_test() {
        hc_p->set_cache_root("/tmp/dods_test_cache");
        hc_p->create_cache_root(hc_p->get_cache_root());
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;
        e->url = localhost_url;
        e->hash = hash_value;
        try {
            hc_p->d_http_cache_table->create_location(e);
            CPPUNIT_ASSERT(e->cachename != "");
        } catch (const Error &e) {
            CPPUNIT_ASSERT(true && "could not create entry file");
        }

        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Directory delete should not throw", remove_directory("/tmp/dods_test_cache/"));

        delete e;
    }

    void parse_headers_test() {
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;

        hc_p->d_http_cache_table->parse_headers(e, hc_p->d_max_entry_size, h);
        CPPUNIT_ASSERT(e->lm == 784025377);

        delete e;
    }

    void calculate_time_test() {
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;

        hc_p->d_http_cache_table->parse_headers(e, hc_p->d_max_entry_size, h);
        hc_p->d_http_cache_table->calculate_time(e, hc_p->d_default_expiration, time(0));
        CPPUNIT_ASSERT(e->corrected_initial_age > 249300571);
        CPPUNIT_ASSERT(e->freshness_lifetime == 86400);

        delete e;
        e = 0;
    }

    void write_metadata_test() {
        hc_p->set_cache_root("/tmp/dods_test_cache");
        hc_p->create_cache_root(hc_p->get_cache_root());
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;
        try {
            e->hash = 101;
            hc_p->d_http_cache_table->create_location(e);
            CPPUNIT_ASSERT(e->cachename != "");
        } catch (const Error &e) {
            CPPUNIT_ASSERT(true && "could not create entry file");
        }

        hc_p->write_metadata(e->cachename, h);
        vector<string> headers;
        hc_p->read_metadata(e->cachename, headers);

        vector<string>::iterator i, j;
        for (i = headers.begin(), j = h.begin(); i != headers.end() && j != h.end(); ++i, ++j) {
            CPPUNIT_ASSERT(*i == *j);
        }

        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Directory delete should not throw", remove_directory("/tmp/dods_test_cache/"));
        delete e;
    }

    void cache_response_test() {
        HTTPResponse *rs = http_conn_p->fetch_url(localhost_url);
        try {
            time_t now = time(nullptr);
            vector<string> &headers = rs->get_headers();
            hc_p->cache_response(localhost_url, now, headers, rs->get_stream());

            CPPUNIT_ASSERT(hc_p->is_url_in_cache(localhost_url));

            HTTPCacheTable::CacheEntry *e =
                hc_p->d_http_cache_table->get_read_locked_entry_from_cache_table(localhost_url);
            CPPUNIT_ASSERT(file_size(e->cachename) == 343);
            e->unlock_read_response();
            delete rs;
            rs = 0;
        } catch (const Error &e) {
            delete rs;
            rs = 0;
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Caught unexpected Error/InternalErr");
        }
    }

    void is_url_valid_test() {
        cache_response_test(); // This should get a response into the cache.
        CPPUNIT_ASSERT(hc_p->is_url_valid(localhost_url));
    }

    void get_cached_response_test() {
        cache_response_test(); // Get a response into the cache.
        vector<string> cached_headers;
        FILE *cached_body = hc_p->get_cached_response(localhost_url, cached_headers);

        HTTPResponse *rs = http_conn_p->fetch_url(localhost_url);
        vector<string> &headers = rs->get_headers();

        // headers and cached_headers should match, except for the values.
        vector<string>::iterator i, j;
        for (i = cached_headers.begin(), j = headers.begin(); i != cached_headers.end() && j != headers.end();
             ++i, ++j) {
            string cached_header = (*i).substr(0, (*i).find(": "));
            // Skip over headers that won't be cached. jhrg 7/4/05
            while (is_hop_by_hop_header(*j))
                ++j;
            string header = (*j).substr(0, (*j).find(": "));
            DBG(cerr << "cached: " << cached_header << ", header: " << header << endl);
            CPPUNIT_ASSERT(cached_header == header);
        }

#ifdef DODS_DEBUG
#include <iterator>
        std::ostream_iterator<string> out_it(std::cerr, "\n");
        cerr << "Cached headers: ";
        std::copy(cached_headers.begin(), cached_headers.end(), out_it);
        cerr << "Headers: ";
        std::copy(headers.begin(), headers.end(), out_it);
#endif

        CPPUNIT_ASSERT(i == cached_headers.end());
        // This may not be true if. For example, keep-alive might appear in the list of headers
        // received, but not in the list of headers cached.
        // CPPUNIT_ASSERT(j == headers->end());

        // every byte of the cached_body and response body should match.
        while (!feof(rs->get_stream()) && !feof(cached_body) && !ferror(rs->get_stream()) && !ferror(cached_body)) {
            char cb, b;
            int cn = fread(&cb, 1, 1, cached_body);
            int n = fread(&b, 1, 1, rs->get_stream());
            CPPUNIT_ASSERT(cn == n);
            if (cn == 1)
                CPPUNIT_ASSERT(cb == b);
        }
        CPPUNIT_ASSERT(feof(rs->get_stream()) && feof(cached_body));

        hc_p->release_cached_response(cached_body);
        delete rs;
        rs = 0;
    }

    void perform_garbage_collection_test() {
        try {
            unique_ptr<HTTPCache> gc(new HTTPCache("cache-testsuite/gc_cache"));
            DBG(cerr << prolog << "gc->get_cache_root(): " << gc->get_cache_root() << endl);

            HTTPResponse *rs = http_conn_p->fetch_url(localhost_url);
            DBG(cerr << prolog << "Retrieved: " << localhost_url << endl);
            gc->cache_response(localhost_url, time(nullptr), rs->get_headers(), rs->get_stream());
            CPPUNIT_ASSERT(gc->is_url_in_cache(localhost_url));
            delete rs;
            rs = nullptr;
            DBG(cerr << prolog << "Cached: " << localhost_url << endl);

            rs = http_conn_p->fetch_url(expired);
            DBG(cerr << prolog << "Retrieved: " << expired << endl);
            gc->cache_response(expired, time(nullptr), rs->get_headers(), rs->get_stream());
            CPPUNIT_ASSERT(gc->is_url_in_cache(expired));
            delete rs;
            rs = nullptr;
            DBG(cerr << prolog << "Cached: " << expired << endl);

            unsigned int nap_time = 2;
            DBG(cerr << prolog << "Sleeping: " << nap_time << " seconds..." << endl);
            sleep(nap_time);
            DBG(cerr << prolog << "Awake." << endl);

            gc->perform_garbage_collection();
            DBG(cerr << prolog << "gc->perform_garbage_collection() completed." << endl);
            gc->d_http_cache_table->cache_index_write();
            DBG(cerr << prolog << "gc->d_http_cache_table->cache_index_write() completed." << endl);

            bool expired_is_in_cache = gc->is_url_in_cache(expired);
            DBG(cerr << prolog << "expired_is_in_cache: " << boolalpha << expired_is_in_cache << endl);
            CPPUNIT_ASSERT(!expired_is_in_cache && "This may fail if sleep is not long enough before gc above");
        } catch (const Error &e) {
            cerr << "Exception: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(false);
        }
    }

    void purge_cache_and_release_cached_response_test() {
        try {
            unique_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache"));
            DBG(cerr << "get_cache_root: " << pc->get_cache_root() << endl);

            time_t now = time(0);
            HTTPResponse *rs = http_conn_p->fetch_url(localhost_url);
            pc->cache_response(localhost_url, now, rs->get_headers(), rs->get_stream());

            CPPUNIT_ASSERT(pc->is_url_in_cache(localhost_url));
            delete rs;
            rs = 0;
#if 0
            string expired = "http://test.opendap.org/cgi-bin/expires.sh";
#endif
            now = time(0);
            rs = http_conn_p->fetch_url(expired);
            pc->cache_response(expired, now, rs->get_headers(), rs->get_stream());

            CPPUNIT_ASSERT(pc->is_url_in_cache(expired));
            delete rs;
            rs = 0;

            HTTPCacheTable::CacheEntry *e1 = pc->d_http_cache_table->get_read_locked_entry_from_cache_table(expired);
            HTTPCacheTable::CacheEntry *e2 =
                pc->d_http_cache_table->get_read_locked_entry_from_cache_table(localhost_url);
            string e1_file = e1->cachename;
            string e2_file = e2->cachename;
            e1->unlock_read_response();
            e2->unlock_read_response();

            vector<string> headers;
            FILE *b = pc->get_cached_response(expired, headers);

            try {
                pc->purge_cache();
                CPPUNIT_ASSERT(!"This call should throw Error");
            } catch (const Error &e) {
                CPPUNIT_ASSERT("Caught Error as expected");
            }

            pc->release_cached_response(b);

            pc->purge_cache();

            CPPUNIT_ASSERT(!pc->is_url_in_cache(localhost_url));
            CPPUNIT_ASSERT(!pc->is_url_in_cache(expired));
            CPPUNIT_ASSERT(access(e1_file.c_str(), F_OK) != 0);
            CPPUNIT_ASSERT(access(e2_file.c_str(), F_OK) != 0);
            CPPUNIT_ASSERT(pc->d_http_cache_table->d_current_size == 0);
        } catch (const Error &e) {
            cerr << "Exception: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(false);
        }
    }

    void instance_test() {
        try {
            HTTPCache *c = HTTPCache::instance("cache-testsuite/singleton_cache");
            DBG(cerr << "get_cache_root: " << c->get_cache_root() << endl);

            if (!c->is_url_in_cache(localhost_url)) {
                HTTPResponse *rs = http_conn_p->fetch_url(localhost_url);
                c->cache_response(localhost_url, time(0), rs->get_headers(), rs->get_stream());
                delete rs;
                rs = 0;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));

            if (!c->is_url_in_cache(expired)) {
                HTTPResponse *rs = http_conn_p->fetch_url(expired);
                c->cache_response(expired, time(0), rs->get_headers(), rs->get_stream());
                delete rs;
                rs = 0;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(expired));

            HTTPCacheTable::CacheEntry *e1 = c->d_http_cache_table->get_read_locked_entry_from_cache_table(expired);
            HTTPCacheTable::CacheEntry *e2 =
                c->d_http_cache_table->get_read_locked_entry_from_cache_table(localhost_url);
            string e1_file = e1->cachename;
            string e2_file = e2->cachename;
            e1->unlock_read_response();
            e2->unlock_read_response();

            c->purge_cache();

            CPPUNIT_ASSERT(!c->is_url_in_cache(localhost_url));
            CPPUNIT_ASSERT(!c->is_url_in_cache(expired));
            CPPUNIT_ASSERT(access(e1_file.c_str(), F_OK) != 0);
            CPPUNIT_ASSERT(access(e2_file.c_str(), F_OK) != 0);
        } catch (const Error &e) {
            CPPUNIT_FAIL("Exception: " + e.get_error_message());
        }
    }

#define IF_NONE_MATCH "If-None-Match: "
#define IF_MODIFIED_SINCE "If-Modified-Since: "

    void get_conditional_response_headers_test() {
        try {
            unique_ptr<HTTPCache> c(new HTTPCache("cache-testsuite/header_cache"));
            DBG(cerr << prolog << "c->get_cache_root(): " << c->get_cache_root() << endl);

            CPPUNIT_ASSERT(c->get_cache_root() == "cache-testsuite/header_cache/");
            if (!c->is_url_in_cache(localhost_url)) {
                HTTPResponse *rs = http_conn_p->fetch_url(localhost_url);
                c->cache_response(localhost_url, time(0), rs->get_headers(), rs->get_stream());
                delete rs;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));

            if (!c->is_url_in_cache(expired)) {
                HTTPResponse *rs = http_conn_p->fetch_url(expired);
                c->cache_response(expired, time(0), rs->get_headers(), rs->get_stream());
                delete rs;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(expired));

            bool found_it = false;
            vector<string> h = c->get_conditional_request_headers(localhost_url);
            DBG(cerr << prolog << "Number of headers: " << h.size() << endl);
            DBG(copy(h.begin(), h.end(), ostream_iterator<string>(cout, "\n")));
            for (const auto &hdr : h) {
                // I know what the strings should start with...
                auto index = hdr.find(IF_NONE_MATCH);
                DBG(cerr << prolog << IF_NONE_MATCH << " location: " << index << endl);
                if (index == 0) {
                    found_it = true;
                    break;
                }
            }
            CPPUNIT_ASSERT(found_it && "Located If-None-Match header.");

            found_it = false;
            h = c->get_conditional_request_headers(expired);
            DBG(cerr << prolog << "Number of headers: " << h.size() << endl);
            DBG(copy(h.begin(), h.end(), ostream_iterator<string>(cout, "\n")));
            for (const auto &hdr : h) {
                // I know what the strings should start with...
                auto index = hdr.find(IF_MODIFIED_SINCE);
                DBG(cerr << prolog << IF_MODIFIED_SINCE << " location: " << index << endl);
                if (index == 0) {
                    found_it = true;
                    break;
                }
            }
            CPPUNIT_ASSERT(found_it && "Located If-Modified-Since header.");
        } catch (const Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    void update_response_test() {
        try {
            unique_ptr<HTTPCache> c(new HTTPCache("cache-testsuite/singleton_cache"));
            DBG(cerr << "get_cache_root: " << c->get_cache_root() << endl);

            if (!c->is_url_in_cache(localhost_url)) {
                HTTPResponse *rs = http_conn_p->fetch_url(localhost_url);
                c->cache_response(localhost_url, time(0), rs->get_headers(), rs->get_stream());
                delete rs;
            }

            if (!c->is_url_in_cache(expired)) {
                HTTPResponse *rs = http_conn_p->fetch_url(expired);
                c->cache_response(expired, time(0), rs->get_headers(), rs->get_stream());
                delete rs;
            }

            // Yes, there's stuff here.
            CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));
            CPPUNIT_ASSERT(c->is_url_in_cache(expired));

            vector<string> orig_h;
            FILE *cr = c->get_cached_response(localhost_url, orig_h);

            DBG(copy(orig_h.begin(), orig_h.end(), ostream_iterator<string>(cerr, "\n")));

            // Before we merge, et c., check that the headers we're going to
            // poke in aren't already there.
            CPPUNIT_ASSERT(find(orig_h.begin(), orig_h.end(), "XHTTPCache: 123456789") == orig_h.end());
            CPPUNIT_ASSERT(find(orig_h.begin(), orig_h.end(), "Date: <invalid date>") == orig_h.end());

            // Make up some new headers.
            vector<string> new_h;
            new_h.push_back("XHTTPCache: 123456789");
            new_h.push_back("Date: <invalid date>");

            c->release_cached_response(cr);

            c->update_response(localhost_url, time(0), new_h);

            vector<string> updated_h;
            cr = c->get_cached_response(localhost_url, updated_h);
            c->release_cached_response(cr);

            DBG(cerr << endl);
            DBG(copy(updated_h.begin(), updated_h.end(), ostream_iterator<string>(cerr, "\n")));

            // The XHTTPCacheTest header should be new, Date should replace the
            // existing Date header.
            // This may not be true when using distcheck and/or when the user
            // has set USE_CACHE to 1 in their .dodsrc. jhrg 9/29/15
            // CPPUNIT_ASSERT(orig_h.size() + 1 == updated_h.size());
            CPPUNIT_ASSERT(find(updated_h.begin(), updated_h.end(), "XHTTPCache: 123456789") != updated_h.end());
            CPPUNIT_ASSERT(find(updated_h.begin(), updated_h.end(), "Date: <invalid date>") != updated_h.end());
        } catch (const Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    // Only run this interactively since you need to hit Ctrl-c to generate
    // SIGINT while the cache is doing its thing. 02/10/04 jhrg
    void interrupt_test() {
        try {
            unique_ptr<HTTPCache> c(new HTTPCache("cache-testsuite/singleton_cache"));
            string coads = "http://test.opendap.org/dap/data/nc/coads_climatology.nc";
            if (!c->is_url_in_cache(coads)) {
                HTTPResponse *rs = http_conn_p->fetch_url(coads);
                cerr << "In interrupt test, hit ctrl-c now... ";
                c->cache_response(coads, time(0), rs->get_headers(), rs->get_stream());
                cerr << "to late.";
                delete rs;
            }
        } catch (const Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    void cache_gc_test() {
        string fnoc1 = "http://test.opendap.org/dap/data/nc/fnoc1.nc.dds";
        string jan = "http://test.opendap.org/dap/data/nc/jan.nc.dds";
        string feb = "http://test.opendap.org/dap/data/nc/feb.nc.dds";
        try {
            unique_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache"));
#if 0
            // This broke Fedora ppc64le system with XFS system
            CPPUNIT_ASSERT(pc->d_http_cache_table->d_block_size == 4096);
#endif
            // Change the size parameters so that we can run some tests
            pc->d_total_size = 12288; // bytes
            pc->d_folder_size = pc->d_total_size / 10;
            pc->d_gc_buffer = pc->d_total_size / 10;

            // The cache should start empty
            CPPUNIT_ASSERT(pc->d_http_cache_table->d_current_size == 0);

            // Get a url
            HTTPResponse *rs = http_conn_p->fetch_url(fnoc1);
            pc->cache_response(fnoc1, time(0), rs->get_headers(), rs->get_stream());
            CPPUNIT_ASSERT(pc->is_url_in_cache(fnoc1));
            delete rs;
            rs = 0;
            // trigger a hit for fnoc1
            vector<string> h;
            FILE *f = pc->get_cached_response(fnoc1, h);
            pc->release_cached_response(f);

            rs = http_conn_p->fetch_url(jan);
            pc->cache_response(jan, time(0), rs->get_headers(), rs->get_stream());
            CPPUNIT_ASSERT(pc->is_url_in_cache(jan));
            delete rs;
            rs = 0;
            // trigger two hits for jan
            f = pc->get_cached_response(jan, h);
            pc->release_cached_response(f);
            f = pc->get_cached_response(jan, h);
            pc->release_cached_response(f);

            rs = http_conn_p->fetch_url(feb);
            pc->cache_response(feb, time(0), rs->get_headers(), rs->get_stream());
            CPPUNIT_ASSERT(pc->is_url_in_cache(feb));
            delete rs;
            rs = 0;
        } catch (const Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }

        // now that pc is out of scope, its dtor has been run and GC
        // performed. The feb URL should have been deleted.

        try {
            unique_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache"));
            CPPUNIT_ASSERT(!pc->is_url_in_cache(feb));
        } catch (const Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPCacheTest);

} // namespace libdap

int main(int argc, char *argv[]) {
    // Run cleanup here, so that the first run works (since this code now
    // sets up the tests).
    // This gives valgrind fits...
    system("cd cache-testsuite && ./cleanup.sh");

    return run_tests<libdap::HTTPCacheTest>(argc, argv) ? 0 : 1;
}
