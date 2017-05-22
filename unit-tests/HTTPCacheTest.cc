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

#include <unistd.h>   // for access stat
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdio>     // for create_cache_root_test
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "HTTPCache.h"
#include "HTTPConnect.h"	// Used to generate a response to cache.
#ifndef WIN32			// Signals are exquisitely non-portable.
#include "SignalHandler.h"	// Needed to clean up this singleton.
#endif
#include "RCReader.h"		// ditto
#include"GetOpt.h"

// #define DODS_DEBUG

#include "debug.h"

#if defined(DODS_DEBUG) || defined(DODS_DEBUG2)
#include <iterator>
#endif

using namespace CppUnit;
using namespace std;

#ifdef WIN32
#define F_OK 0
#define W_OK 2
#endif

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

namespace libdap {

inline static int file_size(string name)
{
    struct stat s;
    stat(name.c_str(), &s);
    return s.st_size;
}

#if 0
inline static void
print_entry(HTTPCache *, HTTPCacheTable::CacheEntry **e)
{
    cerr << "Entry: " << (*e)->get_cachename() << endl;
}
#endif

// Note that because this test class uses the fixture 'hc' we must always
// force access to the single user/process lock for the cache. This is
// because a fixture is always created (by setUp) *before* the body of the
// test is run. So by the time we're at the first line of the test, The
// persistent store's lock has already been grabbed. 10/14/02 jhrg

class HTTPCacheTest: public TestFixture {
private:
    HTTPCache *hc;
    HTTPConnect *http_conn;
    string index_file_line;
    string localhost_url;
    string expired;
    int hash_value;
    vector<string> h;

protected:

public:
    HTTPCacheTest() :
        hc(0), http_conn(0)
    {
        putenv((char*) "DODS_CONF=./cache-testsuite/dodsrc");
        http_conn = new HTTPConnect(RCReader::instance());

        DBG2(cerr << "Entering HTTPCacheTest ctor... ");
        hash_value = 656;
        localhost_url = "http://test.opendap.org/test-304.html";
        index_file_line =
            "http://test.opendap.org/test-304.html cache-testsuite/dods_cache/656/dodsKbcD0h \"3f62c-157-139c2680\" 1121283146 -1 343 0 656 1 7351 1121360379 3723 0";

        expired = "http://test.opendap.org/cgi-bin/expires.sh";

        h.push_back("ETag: jhrgjhrgjhrg");
        h.push_back("Last-Modified: Sat, 05 Nov 1994 08:49:37 GMT");
        h.push_back("Expires: Mon, 07 Nov 1994 08:49:37 GMT");
        h.push_back("Date: Sun, 06 Nov 1994 08:49:37 GMT");
        DBG2(cerr << "exiting." << endl);
    }

    ~HTTPCacheTest()
    {
        delete http_conn;
        http_conn = 0;
        DBG2(cerr << "Entering the HTTPCacheTest dtor... ");DBG2(cerr << "exiting." << endl);
    }
#if 0
    static inline bool
    is_hop_by_hop_header(const string &header) {
        return header.find("Connection") != string::npos
        || header.find("Keep-Alive") != string::npos
        || header.find("Proxy-Authenticate") != string::npos
        || header.find("Proxy-Authorization") != string::npos
        || header.find("Transfer-Encoding") != string::npos
        || header.find("Upgrade") != string::npos;
    }
#endif
    void setUp()
    {
        // Called before every test.
        DBG2(cerr << "Entering HTTPCacheTest::setUp... " << endl);
        hc = new HTTPCache("cache-testsuite/dods_cache/", true);
        DBG2(cerr << "exiting setUp" << endl);
    }

    void tearDown()
    {
        // Called after every test.
        DBG2(cerr << "Entering HTTPCacheTest::tearDown... " << endl);
        delete hc;
        hc = 0;
        DBG2(cerr << "exiting tearDown" << endl);
    }

    CPPUNIT_TEST_SUITE (HTTPCacheTest);

    CPPUNIT_TEST (constructor_test);
    CPPUNIT_TEST (cache_index_read_test);
    CPPUNIT_TEST (cache_index_parse_line_test);
    CPPUNIT_TEST (get_entry_from_cache_table_test);
    CPPUNIT_TEST (cache_index_write_test);
    CPPUNIT_TEST (create_cache_root_test);
    CPPUNIT_TEST (set_cache_root_test);
    CPPUNIT_TEST (get_single_user_lock_test);

    CPPUNIT_TEST (release_single_user_lock_test);
    CPPUNIT_TEST (create_hash_directory_test);
    CPPUNIT_TEST (create_location_test);
    CPPUNIT_TEST (parse_headers_test);

    CPPUNIT_TEST (calculate_time_test);
    CPPUNIT_TEST (write_metadata_test);
    CPPUNIT_TEST (cache_response_test);
#if 0
    // This test does not seem to work in New Zealand - maybe because
    // of the dateline??? jhrg 1/31/13
    CPPUNIT_TEST(is_url_valid_test);
#endif
    CPPUNIT_TEST (get_cached_response_test);

    CPPUNIT_TEST (perform_garbage_collection_test);
    CPPUNIT_TEST (purge_cache_and_release_cached_response_test);
    CPPUNIT_TEST (get_conditional_response_headers_test);
    CPPUNIT_TEST (update_response_test);
    CPPUNIT_TEST (cache_gc_test);

    // Make this the last test because when distcheck is run, running
    // it before other tests will break them.
    CPPUNIT_TEST (instance_test);

    CPPUNIT_TEST_SUITE_END();

    void constructor_test()
    {
        DBG(cerr << "hc->cache_index: " << hc->d_http_cache_table->d_cache_index << endl);
        CPPUNIT_ASSERT(hc->d_http_cache_table->d_cache_index == "cache-testsuite/dods_cache/.index");
        CPPUNIT_ASSERT(hc->d_cache_root == "cache-testsuite/dods_cache/");
        DBG(cerr << "Current size: " << hc->d_http_cache_table->d_current_size << endl);
        DBG(cerr << "Block size: " << hc->d_http_cache_table->d_block_size << endl);
        CPPUNIT_ASSERT(hc->d_http_cache_table->d_current_size == hc->d_http_cache_table->d_block_size);
    }

    void cache_index_read_test()
    {
        CPPUNIT_ASSERT(hc->d_http_cache_table->cache_index_read());

        HTTPCacheTable::CacheEntry *e = hc->d_http_cache_table->get_locked_entry_from_cache_table(localhost_url);

        CPPUNIT_ASSERT(e);
        CPPUNIT_ASSERT(e->url == localhost_url);
        e->unlock_read_response();
    }

    void cache_index_parse_line_test()
    {
        HTTPCacheTable::CacheEntry *e = hc->d_http_cache_table->cache_index_parse_line(index_file_line.c_str());

        CPPUNIT_ASSERT(e->url == localhost_url);
        CPPUNIT_ASSERT(e->cachename == "cache-testsuite/dods_cache/656/dodsKbcD0h");
#ifdef WIN32
        char *tmpstr = "\"3f62c-157-139c2680\"";
        CPPUNIT_ASSERT(e->etag == tmpstr);
#else
        CPPUNIT_ASSERT(e->etag == "\"3f62c-157-139c2680\"");
#endif
        CPPUNIT_ASSERT(e->lm == 1121283146);
        // Skip ahead ...
        CPPUNIT_ASSERT(e->must_revalidate == false);

        delete e;
        e = 0;
    }

    // This will also test the add_entry_to_cache_table() method.
    void get_entry_from_cache_table_test()
    {
        HTTPCacheTable::CacheEntry *e = hc->d_http_cache_table->cache_index_parse_line(index_file_line.c_str());

        // Test adding an entry and getting it back.
        hc->d_http_cache_table->add_entry_to_cache_table(e);

        HTTPCacheTable::CacheEntry *e2 = hc->d_http_cache_table->get_locked_entry_from_cache_table(localhost_url);
        CPPUNIT_ASSERT(e2);
        CPPUNIT_ASSERT(e2->url == localhost_url);
        e2->unlock_read_response();

        // Now test what happens when two entries collide.
        HTTPCacheTable::CacheEntry *e3 = hc->d_http_cache_table->cache_index_parse_line(index_file_line.c_str());

        // Change the url so we can tell the difference (the hash is the same)
        e3->url = "http://new.url.same.hash/test/collisions.gif";

        hc->d_http_cache_table->add_entry_to_cache_table(e3);

        // Use the version of get_entry... that lets us pass in the hash
        // value (as opposed to the normal version which calculates the hash
        // from the url. 10/01/02 jhrg
        HTTPCacheTable::CacheEntry *g = hc->d_http_cache_table->get_locked_entry_from_cache_table(hash_value, e3->url);
        CPPUNIT_ASSERT(g);
        CPPUNIT_ASSERT(g->url == e3->url);
        g->unlock_read_response();

        g = hc->d_http_cache_table->get_locked_entry_from_cache_table("http://not.in.table/never.x");
        CPPUNIT_ASSERT(g == 0);
    }

    void cache_index_write_test()
    {
        try {
            HTTPCache * hc_3 = new HTTPCache("cache-testsuite/dods_cache/", true);
            hc_3->d_http_cache_table->add_entry_to_cache_table(
                hc->d_http_cache_table->cache_index_parse_line(index_file_line.c_str()));

            hc_3->d_http_cache_table->d_cache_index = hc->d_cache_root + "test_index";
            hc_3->d_http_cache_table->cache_index_write();

            HTTPCache *hc_4 = new HTTPCache("cache-testsuite/dods_cache/", true);
            hc_4->d_http_cache_table->d_cache_index = hc_3->d_cache_root + "test_index";
            hc_4->d_http_cache_table->cache_index_read();

            HTTPCacheTable::CacheEntry *e = hc_4->d_http_cache_table->get_locked_entry_from_cache_table(localhost_url);
            DBG(cerr << "Got locked entry" << endl);
            CPPUNIT_ASSERT(e);
            CPPUNIT_ASSERT(e->url == localhost_url);
            e->unlock_read_response();

            delete hc_3;
            hc_3 = 0;
            delete hc_4;
            hc_4 = 0;
        }
        catch (Error &e) {
            //cerr << "Fail: " << e.get_error_message() << endl;
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    void create_cache_root_test()
    {
        hc->create_cache_root("/tmp/silly/");
        CPPUNIT_ASSERT(access("/tmp/silly/", F_OK) == 0);
        remove("/tmp/silly");
#if 0
        // This test doesn't work on some machines where the build is
        // run as root or where /root is owned by some other user (as is
        // the case with OS/X.
        try {
            hc->create_cache_root("/root/very_silly/");
            access("/root/very_silly/", F_OK);
            remove("/root/very_silly/");
            CPPUNIT_ASSERT(!"Should not be able to do this...");
        }
        catch (Error &e) {
            CPPUNIT_ASSERT("This is where we want to be");
            CPPUNIT_ASSERT(access("/root/very_silly/", F_OK) != 0);
        }
#endif
    }

    void set_cache_root_test()
    {
#if 0
        // env var support removed 3/22/11 jhrg
        putenv("DODS_CACHE=/home/jimg");
        hc->set_cache_root();
        CPPUNIT_ASSERT(hc->d_cache_root == "/home/jimg/dods-cache/");
        remove("/home/jimg/w3c-cache/");
#endif
        hc->set_cache_root("/home/jimg/test_cache");
        CPPUNIT_ASSERT(hc->d_cache_root == "/home/jimg/test_cache/");
        remove("/home/jimg/test_cache/");
    }

    void get_single_user_lock_test()
    {
        hc->set_cache_root("/tmp/dods_test_cache");
        hc->release_single_user_lock();

        CPPUNIT_ASSERT(hc->get_single_user_lock());
        CPPUNIT_ASSERT(access("/tmp/dods_test_cache/.lock", F_OK) == 0);

        // Second time should fail
        CPPUNIT_ASSERT(!hc->get_single_user_lock());
    }

    void release_single_user_lock_test()
    {
        hc->set_cache_root("/tmp/dods_test_cache");
        remove("/tmp/dods_test_cache/.lock"); // in case prev. test fails
        hc->d_locked_open_file = 0;

        CPPUNIT_ASSERT(hc->get_single_user_lock());
        CPPUNIT_ASSERT(access("/tmp/dods_test_cache/.lock", F_OK) == 0);

        hc->release_single_user_lock();
        CPPUNIT_ASSERT(hc->get_single_user_lock());
        CPPUNIT_ASSERT(access("/tmp/dods_test_cache/.lock", F_OK) == 0);

        CPPUNIT_ASSERT(!hc->get_single_user_lock());

        remove("/tmp/dods_test_cache/.lock");
    }

    void create_hash_directory_test()
    {
        hc->set_cache_root("/tmp/dods_test_cache");
        CPPUNIT_ASSERT(hc->d_http_cache_table->create_hash_directory(391) == "/tmp/dods_test_cache/391");
        CPPUNIT_ASSERT(access("/tmp/dods_test_cache/391", W_OK) == 0);
#if 0
        // This test doesn't work on some machines where the build is
        // run as root or where /root is owned by some other user (as is
        // the case with OS/X.
        hc->set_cache_root("/root/");
        try {
            hc->create_hash_directory(391);
            CPPUNIT_ASSERT(!"Create in bad directory");
        }
        catch (Error &e) {
        }
#endif
        remove("/tmp/dods_test_cache/391");

    }

    void create_location_test()
    {
        hc->set_cache_root("/tmp/dods_test_cache");
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;
        e->url = localhost_url;
        e->hash = hash_value;
        try {
            hc->d_http_cache_table->create_location(e);
            CPPUNIT_ASSERT(e->cachename != "");
        }
        catch (Error &e) {
            CPPUNIT_ASSERT(true && "could not create entry file");
        }
        remove(e->cachename.c_str());

        delete e;
        e = 0;
    }

    void parse_headers_test()
    {
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;

        hc->d_http_cache_table->parse_headers(e, hc->d_max_entry_size, h);
        CPPUNIT_ASSERT(e->lm == 784025377);

        delete e;
        e = 0;
    }

    void calculate_time_test()
    {
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;

        hc->d_http_cache_table->parse_headers(e, hc->d_max_entry_size, h);
        hc->d_http_cache_table->calculate_time(e, hc->d_default_expiration, time(0));
        CPPUNIT_ASSERT(e->corrected_initial_age > 249300571);
        CPPUNIT_ASSERT(e->freshness_lifetime == 86400);

        delete e;
        e = 0;
    }

    void write_metadata_test()
    {
        hc->set_cache_root("/tmp/dods_test_cache");
        HTTPCacheTable::CacheEntry *e = new HTTPCacheTable::CacheEntry;
        try {
            e->hash = 101;
            hc->d_http_cache_table->create_location(e);
            CPPUNIT_ASSERT(e->cachename != "");
        }
        catch (Error &e) {
            CPPUNIT_ASSERT(true && "could not create entry file");
        }

        hc->write_metadata(e->cachename, h);
        vector<string> headers;
        hc->read_metadata(e->cachename, headers);

        vector<string>::iterator i, j;
        for (i = headers.begin(), j = h.begin(); i != headers.end() && j != h.end(); ++i, ++j) {
            CPPUNIT_ASSERT(*i == *j);
        }

        remove(e->cachename.c_str());
        remove(string(e->cachename + ".meta").c_str());
        delete e;
        e = 0;
    }

    void cache_response_test()
    {
        HTTPResponse *rs = http_conn->fetch_url(localhost_url);
        try {
            time_t now = time(0);
            vector<string> *headers = rs->get_headers();
            hc->cache_response(localhost_url, now, *headers, rs->get_stream());

            CPPUNIT_ASSERT(hc->is_url_in_cache(localhost_url));

            HTTPCacheTable::CacheEntry *e = hc->d_http_cache_table->get_locked_entry_from_cache_table(localhost_url);
            CPPUNIT_ASSERT(file_size(e->cachename) == 343);
            e->unlock_read_response();
            delete rs;
            rs = 0;
        }
        catch (Error &e) {
            delete rs;
            rs = 0;
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Caught unexpected Error/InternalErr");
        }
    }

    void is_url_valid_test()
    {
        cache_response_test(); // This should get a response into the cache.
        CPPUNIT_ASSERT(hc->is_url_valid(localhost_url));
    }

    void get_cached_response_test()
    {
        cache_response_test(); // Get a response into the cache.
        vector<string> cached_headers;
        FILE *cached_body = hc->get_cached_response(localhost_url, cached_headers);

        HTTPResponse *rs = http_conn->fetch_url(localhost_url);
        vector<string> *headers = rs->get_headers();

        // headers and cached_headers should match, except for the values.
        vector<string>::iterator i, j;
        for (i = cached_headers.begin(), j = headers->begin(); i != cached_headers.end() && j != headers->end();
            ++i, ++j) {
            string ch = (*i).substr(0, (*i).find(": "));
            // Skip over headers that won't be cached. jhrg 7/4/05
            while (is_hop_by_hop_header(*j))
                ++j;
            string h = (*j).substr(0, (*j).find(": "));
            DBG(cerr << "cached: " << ch << ", header: " << h << endl);
            CPPUNIT_ASSERT(ch == h);
        }

#ifdef DODS_DEBUG
        std::ostream_iterator<string> out_it(std::cerr, "\n");
        cerr << "Cached headers: ";
        std::copy(cached_headers.begin(), cached_headers.end(), out_it);
        cerr << "Headers: ";
        std::copy(headers->begin(), headers->end(), out_it);
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
            if (cn == 1) CPPUNIT_ASSERT(cb == b);
        }
        CPPUNIT_ASSERT(feof(rs->get_stream()) && feof(cached_body));

        hc->release_cached_response(cached_body);
        delete rs;
        rs = 0;
    }

    void perform_garbage_collection_test()
    {
        try {
            delete hc;
            hc = 0;
            auto_ptr<HTTPCache> gc(new HTTPCache("cache-testsuite/gc_cache", true));
            DBG(cerr << "get_cache_root: " << gc->get_cache_root() << endl);

            HTTPResponse *rs = http_conn->fetch_url(localhost_url);
            gc->cache_response(localhost_url, time(0), *(rs->get_headers()), rs->get_stream());
            CPPUNIT_ASSERT(gc->is_url_in_cache(localhost_url));
            delete rs;
            rs = 0;

            rs = http_conn->fetch_url(expired);
            gc->cache_response(expired, time(0), *(rs->get_headers()), rs->get_stream());
            CPPUNIT_ASSERT(gc->is_url_in_cache(expired));
            delete rs;
            rs = 0;

            sleep(2);

            gc->perform_garbage_collection();
            gc->d_http_cache_table->cache_index_write();

            CPPUNIT_ASSERT(
                !gc->is_url_in_cache(expired) && "This may fail if sleep is not long enough before gc above");
        }
        catch (Error &e) {
            cerr << "Exception: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(false);
        }
    }

    void purge_cache_and_release_cached_response_test()
    {
        try {
            auto_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache", true));
            DBG(cerr << "get_cache_root: " << pc->get_cache_root() << endl);

            time_t now = time(0);
            HTTPResponse *rs = http_conn->fetch_url(localhost_url);
            pc->cache_response(localhost_url, now, *(rs->get_headers()), rs->get_stream());

            CPPUNIT_ASSERT(pc->is_url_in_cache(localhost_url));
            delete rs;
            rs = 0;

            string expired = "http://test.opendap.org/cgi-bin/expires.sh";
            now = time(0);
            rs = http_conn->fetch_url(expired);
            pc->cache_response(expired, now, *(rs->get_headers()), rs->get_stream());

            CPPUNIT_ASSERT(pc->is_url_in_cache(expired));
            delete rs;
            rs = 0;

            HTTPCacheTable::CacheEntry *e1 = pc->d_http_cache_table->get_locked_entry_from_cache_table(expired);
            HTTPCacheTable::CacheEntry *e2 = pc->d_http_cache_table->get_locked_entry_from_cache_table(localhost_url);
            string e1_file = e1->cachename;
            string e2_file = e2->cachename;
            e1->unlock_read_response();
            e2->unlock_read_response();

            vector<string> headers;
            FILE *b = pc->get_cached_response(expired, headers);

            try {
                pc->purge_cache();
                CPPUNIT_ASSERT(!"This call should throw Error");
            }
            catch (Error &e) {
                CPPUNIT_ASSERT("Caught Error as expected");
            }

            pc->release_cached_response(b);

            pc->purge_cache();

            CPPUNIT_ASSERT(!pc->is_url_in_cache(localhost_url));
            CPPUNIT_ASSERT(!pc->is_url_in_cache(expired));
            CPPUNIT_ASSERT(access(e1_file.c_str(), F_OK) != 0);
            CPPUNIT_ASSERT(access(e2_file.c_str(), F_OK) != 0);
            CPPUNIT_ASSERT(pc->d_http_cache_table->d_current_size == 0);
        }
        catch (Error &e) {
            cerr << "Exception: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(false);
        }
    }

    void instance_test()
    {
        try {
            // FIXME: Explain
            HTTPCache::delete_instance();

            HTTPCache *c = HTTPCache::instance("cache-testsuite/singleton_cache", true);
            DBG(cerr << "get_cache_root: " << c->get_cache_root() << endl);

            if (!c->is_url_in_cache(localhost_url)) {
                HTTPResponse *rs = http_conn->fetch_url(localhost_url);
                c->cache_response(localhost_url, time(0), *(rs->get_headers()), rs->get_stream());
                delete rs;
                rs = 0;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));

            if (!c->is_url_in_cache(expired)) {
                HTTPResponse *rs = http_conn->fetch_url(expired);
                c->cache_response(expired, time(0), *(rs->get_headers()), rs->get_stream());
                delete rs;
                rs = 0;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(expired));

            HTTPCacheTable::CacheEntry *e1 = c->d_http_cache_table->get_locked_entry_from_cache_table(expired);
            HTTPCacheTable::CacheEntry *e2 = c->d_http_cache_table->get_locked_entry_from_cache_table(localhost_url);
            string e1_file = e1->cachename;
            string e2_file = e2->cachename;
            e1->unlock_read_response();
            e2->unlock_read_response();

            c->purge_cache();

            CPPUNIT_ASSERT(!c->is_url_in_cache(localhost_url));
            CPPUNIT_ASSERT(!c->is_url_in_cache(expired));
            CPPUNIT_ASSERT(access(e1_file.c_str(), F_OK) != 0);
            CPPUNIT_ASSERT(access(e2_file.c_str(), F_OK) != 0);
        }
        catch (Error &e) {
            cerr << "Exception: " << e.get_error_message() << endl;
            CPPUNIT_ASSERT(false);
        }

        // Call this here to simulate exiting the program. This ensures that
        // the next test's call to instance() gets a fresh cache. The static
        // method will still be run at exit, but that's OK since it tests the
        // value of _instance and simply returns with it's zero.
        HTTPCache::delete_instance();
#ifndef WIN32
        SignalHandler::delete_instance();
#endif
    }

    void get_conditional_response_headers_test()
    {
        try {
            auto_ptr<HTTPCache> c(new HTTPCache("cache-testsuite/header_cache", true));
            DBG(cerr << "get_cache_root: " << c->get_cache_root() << endl);

            CPPUNIT_ASSERT(c->get_cache_root() == "cache-testsuite/header_cache/");
            if (!c->is_url_in_cache(localhost_url)) {
                HTTPResponse *rs = http_conn->fetch_url(localhost_url);
                c->cache_response(localhost_url, time(0), *(rs->get_headers()), rs->get_stream());
                delete rs;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));

            if (!c->is_url_in_cache(expired)) {
                HTTPResponse *rs = http_conn->fetch_url(expired);
                c->cache_response(expired, time(0), *(rs->get_headers()), rs->get_stream());
                delete rs;
            }
            CPPUNIT_ASSERT(c->is_url_in_cache(expired));

            vector<string> h = c->get_conditional_request_headers(localhost_url);
            DBG(copy(h.begin(), h.end(), ostream_iterator<string>(cout, "\n")));
            DBG(cerr << "if none match location: " << h[0].find("If-None-Match: ") << endl);
            // I know what the strings should start with...
            CPPUNIT_ASSERT(h[0].find("If-None-Match: ") == 0);

            h = c->get_conditional_request_headers(expired);
            DBG(cerr << "Number of headers: " << h.size() << endl);
            DBG(copy(h.begin(), h.end(), ostream_iterator<string>(cout, "\n")));
            CPPUNIT_ASSERT(h[0].find("If-Modified-Since: ") == 0);
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    void update_response_test()
    {
        try {
            auto_ptr<HTTPCache> c(new HTTPCache("cache-testsuite/singleton_cache", true));
            DBG(cerr << "get_cache_root: " << c->get_cache_root() << endl);

            if (!c->is_url_in_cache(localhost_url)) {
                HTTPResponse *rs = http_conn->fetch_url(localhost_url);
                c->cache_response(localhost_url, time(0), *(rs->get_headers()), rs->get_stream());
                delete rs;
            }

            if (!c->is_url_in_cache(expired)) {
                HTTPResponse *rs = http_conn->fetch_url(expired);
                c->cache_response(expired, time(0), *(rs->get_headers()), rs->get_stream());
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
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    // Only run this interactively since you need to hit Ctrl-c to generate
    // SIGINT while the cache is doing its thing. 02/10/04 jhrg
    void interrupt_test()
    {
        try {
            auto_ptr<HTTPCache> c(new HTTPCache("cache-testsuite/singleton_cache", true));
            string coads = "http://test.opendap.org/dap/data/nc/coads_climatology.nc";
            if (!c->is_url_in_cache(coads)) {
                HTTPResponse *rs = http_conn->fetch_url(coads);
                cerr << "In interrupt test, hit ctrl-c now... ";
                c->cache_response(coads, time(0), *(rs->get_headers()), rs->get_stream());
                cerr << "to late.";
                delete rs;
            }
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

    void cache_gc_test()
    {
        string fnoc1 = "http://test.opendap.org/dap/data/nc/fnoc1.nc.dds";
        string jan = "http://test.opendap.org/dap/data/nc/jan.nc.dds";
        string feb = "http://test.opendap.org/dap/data/nc/feb.nc.dds";
        try {
            auto_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache", true));

            CPPUNIT_ASSERT(pc->d_http_cache_table->d_block_size == 4096);

            // Change the size parameters so that we can run some tests
            pc->d_total_size = 12288; // bytes
            pc->d_folder_size = pc->d_total_size / 10;
            pc->d_gc_buffer = pc->d_total_size / 10;

            // The cache should start empty
            CPPUNIT_ASSERT(pc->d_http_cache_table->d_current_size == 0);

            // Get a url
            HTTPResponse *rs = http_conn->fetch_url(fnoc1);
            pc->cache_response(fnoc1, time(0), *(rs->get_headers()), rs->get_stream());
            CPPUNIT_ASSERT(pc->is_url_in_cache(fnoc1));
            delete rs;
            rs = 0;
            // trigger a hit for fnoc1
            vector<string> h;
            FILE *f = pc->get_cached_response(fnoc1, h);
            pc->release_cached_response(f);

            rs = http_conn->fetch_url(jan);
            pc->cache_response(jan, time(0), *(rs->get_headers()), rs->get_stream());
            CPPUNIT_ASSERT(pc->is_url_in_cache(jan));
            delete rs;
            rs = 0;
            // trigger two hits for jan
            f = pc->get_cached_response(jan, h);
            pc->release_cached_response(f);
            f = pc->get_cached_response(jan, h);
            pc->release_cached_response(f);

            rs = http_conn->fetch_url(feb);
            pc->cache_response(feb, time(0), *(rs->get_headers()), rs->get_stream());
            CPPUNIT_ASSERT(pc->is_url_in_cache(feb));
            delete rs;
            rs = 0;
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }

        // now that pc is out of scope, its dtor has been run and GC
        // performed. The feb URL should have been deleted.

        try {
            auto_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache", true));
            CPPUNIT_ASSERT(!pc->is_url_in_cache(feb));
        }
        catch (Error &e) {
            CPPUNIT_FAIL(e.get_error_message());
        }
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (HTTPCacheTest);

} // namespace libdap

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: HTTPCacheTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::HTTPCacheTest::suite()->getTests();
            unsigned int prefix_len = libdap::HTTPCacheTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            break;
        }
        default:
            break;
        }

    // Run cleanup here, so that the first run works (since this code now
    // sets up the tests).
    // This gives valgrind fits...
    system("cd cache-testsuite && ./cleanup.sh");

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::HTTPCacheTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

