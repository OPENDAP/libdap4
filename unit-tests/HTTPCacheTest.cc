
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#include <stdio.h>		// for create_cache_root_test
#include <unistd.h>		// ditto

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

// #define DODS_DEBUG 1

#include "HTTPCache.h"
#include "HTTPConnect.h"	// Used to generate a response to cache.
#ifndef WIN32			// Signals are exquisitely non-portable.
#include "SignalHandler.h"	// Needed to clean up this singleton.
#endif
#include "RCReader.h"		// ditto
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

inline static int
file_size(string name)
{
    struct stat s;
    stat(name.c_str(), &s);
    return s.st_size;
}

inline static void 
print_entry(HTTPCache *, HTTPCache::CacheEntry **e) 
{
    cerr << "Entry: " << (*e)->cachename << endl;
}

// Note that because this test class uses the fixture 'hc' we must always
// force access to the single user/process lock for the cache. This is
// because a fixture is always created (by setUp) *before* the body of the
// test is run. So by the time we're at the first line of the test, The
// persistent store's lock has already been grabbed. 10/14/02 jhrg

class HTTPCacheTest : public TestFixture {
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
    HTTPCacheTest() : http_conn(0) {
	putenv("DODS_CONF=./cache-testsuite/dodsrc");
	http_conn = new HTTPConnect(RCReader::instance());

	DBG2(cerr << "Entring HTTPCacheTest ctor... ");
	hash_value = 656;
	localhost_url = "http://test.opendap.org/test-304.html";
	index_file_line = "http://test.opendap.org/test-304.html cache-testsuite/dods_cache/656/dodsKbcD0h \"3f62c-157-139c2680\" 1121283146 -1 343 0 656 1 7351 1121360379 3723 0";

	expired = "http://test.opendap.org/cgi-bin/expires.sh";

	h.push_back("ETag: jhrgjhrgjhrg");
	h.push_back("Last-Modified: Sat, 05 Nov 1994 08:49:37 GMT");
	h.push_back("Expires: Mon, 07 Nov 1994 08:49:37 GMT");
	h.push_back("Date: Sun, 06 Nov 1994 08:49:37 GMT");
	DBG2(cerr << "exiting." << endl);
    }

    ~HTTPCacheTest() { 
	delete http_conn; http_conn = 0;
	DBG2(cerr << "Entering the HTTPCacheTest dtor... ");
	DBG2(cerr << "exiting." << endl);
    }
     
    static inline bool
    is_hop_by_hop_header(const string &header) {
	return header.find("Connection") != string::npos
	    || header.find("Keep-Alive") != string::npos
	    || header.find("Proxy-Authenticate") != string::npos
	    || header.find("Proxy-Authorization") != string::npos
	    || header.find("Transfer-Encoding") != string::npos
	    || header.find("Upgrade") != string::npos;
    }
    
    void setUp () {
	// Called before every test.
	DBG2(cerr << "Entering HTTPCacheTest::setUp... " << endl);
	hc = new HTTPCache("cache-testsuite/dods_cache/", true);
	DBG2(cerr << "exiting setUp" << endl);
    }

    void tearDown() {
	// Called after every test.
	DBG2(cerr << "Entering HTTPCacheTest::tearDown... " << endl);
	delete hc; hc = 0;
	DBG2(cerr << "exiting tearDown" << endl);
    }

    CPPUNIT_TEST_SUITE(HTTPCacheTest);

    CPPUNIT_TEST(constructor_test);
    CPPUNIT_TEST(cache_index_read_test);
    CPPUNIT_TEST(cache_index_parse_line_test);
    CPPUNIT_TEST(get_entry_from_cache_table_test);
    CPPUNIT_TEST(cache_index_write_test);
    CPPUNIT_TEST(create_cache_root_test);
    CPPUNIT_TEST(set_cache_root_test);
    CPPUNIT_TEST(get_single_user_lock_test);
    CPPUNIT_TEST(release_single_user_lock_test);

    CPPUNIT_TEST(create_hash_directory_test);

    CPPUNIT_TEST(create_location_test);
    CPPUNIT_TEST(parse_headers_test);
    CPPUNIT_TEST(calculate_time_test);
    CPPUNIT_TEST(write_metadata_test);
    CPPUNIT_TEST(cache_response_test);
    CPPUNIT_TEST(is_url_valid_test);
    CPPUNIT_TEST(get_cached_response_test);
    CPPUNIT_TEST(perform_garbage_collection_test);
    CPPUNIT_TEST(purge_cache_and_release_cached_response_test);
    CPPUNIT_TEST(instance_test);
    CPPUNIT_TEST(get_conditional_response_headers_test);
    CPPUNIT_TEST(update_response_test);

#if 0
    CPPUNIT_TEST(interrupt_test);
#endif

    CPPUNIT_TEST(cache_gc_test);

    CPPUNIT_TEST_SUITE_END();

    void constructor_test() {
        DBG(cerr << "hc->cache_index: " << hc->d_cache_index << endl);
	CPPUNIT_ASSERT(hc->d_cache_index=="cache-testsuite/dods_cache/.index");
	CPPUNIT_ASSERT(hc->d_cache_root == "cache-testsuite/dods_cache/");
	DBG(cerr << "Current size: " << hc->d_current_size << endl);
	DBG(cerr << "Block size: " << hc->d_block_size << endl);
	CPPUNIT_ASSERT(hc->d_current_size == hc->d_block_size);
    }

    void cache_index_read_test() {
	CPPUNIT_ASSERT(hc->cache_index_read());

	HTTPCache::CacheEntry *e = hc->get_entry_from_cache_table(localhost_url);

	CPPUNIT_ASSERT(e);
	CPPUNIT_ASSERT(e->url == localhost_url);
    }

    void cache_index_parse_line_test() {
	HTTPCache::CacheEntry *e = hc->cache_index_parse_line(index_file_line.c_str());

	CPPUNIT_ASSERT(e->url == localhost_url);
	CPPUNIT_ASSERT(e->cachename 
		       == "cache-testsuite/dods_cache/656/dodsKbcD0h");
#ifdef WIN32
	char *tmpstr = "\"3f62c-157-139c2680\"";
	CPPUNIT_ASSERT(e->etag == tmpstr);
#else
	CPPUNIT_ASSERT(e->etag == "\"3f62c-157-139c2680\"");
#endif
	CPPUNIT_ASSERT(e->lm == 1121283146);
	// Skip ahead ...
	CPPUNIT_ASSERT(e->must_revalidate == false);

	delete e; e = 0;
    }

    // This will also test the add_entry_to_cache_table() method.
    void get_entry_from_cache_table_test() {
	HTTPCache::CacheEntry *e 
	    = hc->cache_index_parse_line(index_file_line.c_str());

	// Test adding an entry and getting it back.
	hc->add_entry_to_cache_table(e);

	HTTPCache::CacheEntry *e2 
	    = hc->get_entry_from_cache_table(localhost_url);
	CPPUNIT_ASSERT(e2);
	CPPUNIT_ASSERT(e2->url == localhost_url);

	// Now test what happens when two entries collide.
	HTTPCache::CacheEntry *e3
	    = hc->cache_index_parse_line(index_file_line.c_str());

	// Change the url so we can tell the difference (the hash is the same)
	e3->url = "http://new.url.same.hash/test/collisions.gif";

	hc->add_entry_to_cache_table(e3);

	// Use the version of get_entry... that lets us pass in the hash
	// value (as opposed to the normal version which calculates the hash
	// from the url. 10/01/02 jhrg
	HTTPCache::CacheEntry *g = hc->get_entry_from_cache_table(hash_value, e3->url);
	CPPUNIT_ASSERT(g);
	CPPUNIT_ASSERT(g->url == e3->url);

	g = hc->get_entry_from_cache_table("http://not.in.table/never.x");
	CPPUNIT_ASSERT(g == 0);
    }

    void cache_index_write_test() {
	HTTPCache *hc_3 = new HTTPCache("cache-testsuite/dods_cache/", true);
	hc_3->add_entry_to_cache_table(hc->cache_index_parse_line(index_file_line.c_str()));

	hc_3->d_cache_index = hc->d_cache_root + "test_index";
	hc_3->cache_index_write();

	HTTPCache *hc_4 = new HTTPCache("cache-testsuite/dods_cache/", true);
	hc_4->d_cache_index = hc_3->d_cache_root + "test_index";
	hc_4->cache_index_read();

	HTTPCache::CacheEntry *e 
	    = hc_4->get_entry_from_cache_table(localhost_url);
	CPPUNIT_ASSERT(e);
	CPPUNIT_ASSERT(e->url == localhost_url);

	delete hc_3; hc = 0;
	delete hc_4; hc = 0;
    }

    void create_cache_root_test() {
	hc->create_cache_root("/tmp/silly/");
	CPPUNIT_ASSERT(access("/tmp/silly/", F_OK) == 0);
	remove("/tmp/silly");
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
    }

    void set_cache_root_test() {
	putenv("DODS_CACHE=/home/jimg");
	hc->set_cache_root();
	CPPUNIT_ASSERT(hc->d_cache_root == "/home/jimg/dods-cache/");
	remove("/home/jimg/w3c-cache/");

	hc->set_cache_root("/home/jimg/test_cache");
	CPPUNIT_ASSERT(hc->d_cache_root == "/home/jimg/test_cache/");
	remove("/home/jimg/test_cache/");
    }

    void get_single_user_lock_test() {
	hc->set_cache_root("/tmp/dods_test_cache");
	hc->release_single_user_lock();

	CPPUNIT_ASSERT(hc->get_single_user_lock());
	CPPUNIT_ASSERT(access("/tmp/dods_test_cache/.lock", F_OK) == 0);

	// Second time should fail
	CPPUNIT_ASSERT(!hc->get_single_user_lock());
    }
	
    void release_single_user_lock_test() {
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

    void create_hash_directory_test() {
	hc->set_cache_root("/tmp/dods_test_cache");
	CPPUNIT_ASSERT(hc->create_hash_directory(391)
		       == "/tmp/dods_test_cache/391");
	CPPUNIT_ASSERT(access("/tmp/dods_test_cache/391", W_OK) == 0);

	hc->set_cache_root("/root/");
	try {
	    hc->create_hash_directory(391);
	    CPPUNIT_ASSERT(!"Create in bad directory");
	}
	catch (Error &e) {
	}

	remove("/tmp/dods_test_cache/391");

    }

    void create_location_test() {
	hc->set_cache_root("/tmp/dods_test_cache");
	HTTPCache::CacheEntry *e = new HTTPCache::CacheEntry;
	e->url = localhost_url;
	e->hash = hash_value;
	try {
	    hc->create_location(e);
	    CPPUNIT_ASSERT(e->cachename != "");
	}
	catch (Error &e) {
	    CPPUNIT_ASSERT(true && "could not create entry file");
	}
	remove(e->cachename.c_str());

	delete e; e = 0;
    }

    void parse_headers_test() {
	HTTPCache::CacheEntry *e = new HTTPCache::CacheEntry;
	
	hc->parse_headers(e, h);
	CPPUNIT_ASSERT(e->lm == 784025377);

	delete e; e = 0;
    }

    void calculate_time_test() {
	HTTPCache::CacheEntry *e = new HTTPCache::CacheEntry;
	
	hc->parse_headers(e, h);
	hc->calculate_time(e, time(0));
	CPPUNIT_ASSERT(e->corrected_initial_age > 249300571);
	CPPUNIT_ASSERT(e->freshness_lifetime == 86400);

	delete e; e = 0;
    }

    void write_metadata_test() {
	hc->set_cache_root("/tmp/dods_test_cache");
	HTTPCache::CacheEntry *e = new HTTPCache::CacheEntry;
	try {
	    e->hash = 101;
	    hc->create_location(e);
	    CPPUNIT_ASSERT(e->cachename != "");
	}
	catch (Error &e) {
	    CPPUNIT_ASSERT(true && "could not create entry file");
	}
	
	hc->write_metadata(e->cachename, h);
	vector<string> headers;
	hc->read_metadata(e->cachename, headers);

	vector<string>::iterator i, j;
	for (i = headers.begin(), j = h.begin(); 
	     i != headers.end() && j != h.end(); 
	     ++i, ++j) {
	    CPPUNIT_ASSERT(*i == *j);
	}

	remove(e->cachename.c_str());
	remove(string(e->cachename + ".meta").c_str());
	delete e; e = 0;
    }

    void cache_response_test() {
	HTTPResponse *rs = http_conn->fetch_url(localhost_url);
	try {
	    time_t now = time(0);
	    vector<string> *headers = rs->get_headers();
	    hc->cache_response(localhost_url, now, *headers, rs->get_stream());

	    CPPUNIT_ASSERT(hc->is_url_in_cache(localhost_url));

	    HTTPCache::CacheEntry *e = hc->get_entry_from_cache_table(localhost_url);
	    CPPUNIT_ASSERT(file_size(e->cachename) == 343);
	    delete rs; rs = 0;
	}
	catch (Error &e) {
	    delete rs; rs = 0;
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(!"Caught unexpected Error/InternalErr");
	}
    }

    void is_url_valid_test() {
	cache_response_test();	// This should get a response into the cache.
	CPPUNIT_ASSERT(hc->is_url_valid(localhost_url));
    }

    void get_cached_response_test() {
	cache_response_test();	// Get a response into the cache.
	vector<string> cached_headers;
	FILE *cached_body = hc->get_cached_response(localhost_url, 
						    cached_headers);

	HTTPResponse *rs = http_conn->fetch_url(localhost_url);
	vector<string> *headers = rs->get_headers();

	// headers and cached_headers should match, except for the values.
	vector<string>::iterator i, j;
	for (i = cached_headers.begin(), j = headers->begin();
	     i != cached_headers.end() && j != headers->end();
	     ++i, ++j) {
	    string ch = (*i).substr(0, (*i).find(": "));
	    // Skip over headers that won't be cached. jhrg 7/4/05
	    while (is_hop_by_hop_header(*j)) ++j;
	    string h = (*j).substr(0, (*j).find(": "));
	    DBG(cerr << "cached: " << ch << ", header: " << h << endl);
	    CPPUNIT_ASSERT(ch == h);
	}

	CPPUNIT_ASSERT(i == cached_headers.end() && j == headers->end());

	// every byte of the cached_body and response body should match.
	while (!feof(rs->get_stream()) && !feof(cached_body) 
	       && !ferror(rs->get_stream()) && !ferror(cached_body)) {
	    char cb, b;
	    int cn = fread(&cb, 1, 1, cached_body);
	    int n = fread(&b, 1, 1, rs->get_stream());
	    CPPUNIT_ASSERT(cn == n);
	    if (cn == 1)
		CPPUNIT_ASSERT(cb == b);
	}
	CPPUNIT_ASSERT(feof(rs->get_stream()) && feof(cached_body));

	hc->release_cached_response(cached_body);
	delete rs; rs = 0;
    }

    void perform_garbage_collection_test() {
	try {	
	    delete hc; hc = 0;
	    auto_ptr<HTTPCache> gc(new HTTPCache("cache-testsuite/gc_cache", true));

	    HTTPResponse *rs = http_conn->fetch_url(localhost_url);
	    gc->cache_response(localhost_url, time(0), *(rs->get_headers()),
			       rs->get_stream());
	    CPPUNIT_ASSERT(gc->is_url_in_cache(localhost_url));
	    delete rs; rs = 0;

	    rs = http_conn->fetch_url(expired);
	    gc->cache_response(expired, time(0), *(rs->get_headers()),
			       rs->get_stream());
	    CPPUNIT_ASSERT(gc->is_url_in_cache(expired));
	    delete rs; rs = 0;

	    sleep(2);

	    gc->perform_garbage_collection();
	    gc->cache_index_write();

	    CPPUNIT_ASSERT(!gc->is_url_in_cache(expired) 
			   && "This may fail if sleep is not long enough before gc above");
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}
    }

    void purge_cache_and_release_cached_response_test() {
	try {
	    auto_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache", true));

	    time_t now = time(0);
	    HTTPResponse *rs = http_conn->fetch_url(localhost_url);
	    pc->cache_response(localhost_url, now, *(rs->get_headers()),
			       rs->get_stream());

	    CPPUNIT_ASSERT(pc->is_url_in_cache(localhost_url));
	    delete rs; rs = 0;

	    string expired = "http://dodsdev.gso.uri.edu/cgi-bin/expires.sh";
	    now = time(0);
	    rs = http_conn->fetch_url(expired);
	    pc->cache_response(expired, now, *(rs->get_headers()), 
			       rs->get_stream());

	    CPPUNIT_ASSERT(pc->is_url_in_cache(expired));
	    delete rs; rs = 0;

	    HTTPCache::CacheEntry *e1 = pc->get_entry_from_cache_table(expired);
	    HTTPCache::CacheEntry *e2 = pc->get_entry_from_cache_table(localhost_url);
	    string e1_file = e1->cachename;
	    string e2_file = e2->cachename;

	    vector<string> headers;
	    FILE *b = pc->get_cached_response(expired, headers);

	    try {
		pc->purge_cache();
		CPPUNIT_ASSERT(!"This call should throw Error");
	    }
	    catch(Error &e) {
		CPPUNIT_ASSERT("Caught Error as expected");
	    }

	    pc->release_cached_response(b);

	    pc->purge_cache();

	    CPPUNIT_ASSERT(!pc->is_url_in_cache(localhost_url));
	    CPPUNIT_ASSERT(!pc->is_url_in_cache(expired));
	    CPPUNIT_ASSERT(access(e1_file.c_str(), F_OK) != 0);
	    CPPUNIT_ASSERT(access(e2_file.c_str(), F_OK) != 0);
	    CPPUNIT_ASSERT(pc->d_current_size == 0);
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}
    }

    void instance_test() {
	try {
	    HTTPCache *c = HTTPCache::instance("cache-testsuite/singleton_cache", true);

	    if (!c->is_url_in_cache(localhost_url)) {
		HTTPResponse *rs = http_conn->fetch_url(localhost_url);
		c->cache_response(localhost_url, time(0), 
				  *(rs->get_headers()), rs->get_stream());
		delete rs; rs = 0;
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));

	    if (!c->is_url_in_cache(expired)) {
		HTTPResponse *rs = http_conn->fetch_url(expired);
		c->cache_response(expired, time(0), 
				  *(rs->get_headers()), rs->get_stream());
		delete rs; rs = 0;
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(expired));

	    HTTPCache::CacheEntry *e1 = c->get_entry_from_cache_table(expired);
	    HTTPCache::CacheEntry *e2 = c->get_entry_from_cache_table(localhost_url);
	    string e1_file = e1->cachename;
	    string e2_file = e2->cachename;

	    c->purge_cache();

	    CPPUNIT_ASSERT(!c->is_url_in_cache(localhost_url));
	    CPPUNIT_ASSERT(!c->is_url_in_cache(expired));
	    CPPUNIT_ASSERT(access(e1_file.c_str(), F_OK) != 0);
	    CPPUNIT_ASSERT(access(e2_file.c_str(), F_OK) != 0);
	}
	catch(Error &e) {
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
    
    void get_conditional_response_headers_test() {
	try {
	    HTTPCache *c = HTTPCache::instance("cache-testsuite/header_cache",
					       true);
	    CPPUNIT_ASSERT(c->get_cache_root() 
			   == "cache-testsuite/header_cache/");
	    if (!c->is_url_in_cache(localhost_url)) {
		HTTPResponse *rs = http_conn->fetch_url(localhost_url);
		c->cache_response(localhost_url, time(0), 
				  *(rs->get_headers()), rs->get_stream());
		delete rs; rs = 0;
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));

	    if (!c->is_url_in_cache(expired)) {
		HTTPResponse *rs = http_conn->fetch_url(expired);
		c->cache_response(expired, time(0), 
				  *(rs->get_headers()), rs->get_stream());
		delete rs; rs = 0;
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(expired));

	    vector<string> h = c->get_conditional_request_headers(localhost_url);
	    DBG(copy(h.begin(), h.end(), 
		     ostream_iterator<string>(cout, "\n")));
	    DBG(cerr << "if none match location: " << 
		h[0].find("If-None-Match: ") << endl);
	    // I know what the strings should start with...
	    CPPUNIT_ASSERT(h[0].find("If-None-Match: ") == 0);

	    h = c->get_conditional_request_headers(expired);
	    DBG(cerr << "Number of headers: " << h.size() << endl);
	    DBG(copy(h.begin(), h.end(), 
		     ostream_iterator<string>(cout, "\n")));
	    CPPUNIT_ASSERT(h[0].find("If-Modified-Since: ") == 0);
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}

	HTTPCache::delete_instance();
    }

    void update_response_test() {
	try {
	    HTTPCache *c = new HTTPCache("cache-testsuite/singleton_cache", true);

	    if (!c->is_url_in_cache(localhost_url)) {
		HTTPResponse *rs = http_conn->fetch_url(localhost_url);
		c->cache_response(localhost_url, time(0), 
				  *(rs->get_headers()), rs->get_stream());
		delete rs; rs = 0;
	    }

	    if (!c->is_url_in_cache(expired)) {
		HTTPResponse *rs = http_conn->fetch_url(expired);
		c->cache_response(expired, time(0), 
				  *(rs->get_headers()), rs->get_stream());
		delete rs; rs = 0;
	    }

	    // Yes, there's stuff here.
	    CPPUNIT_ASSERT(c->is_url_in_cache(localhost_url));
	    CPPUNIT_ASSERT(c->is_url_in_cache(expired));

	    vector<string> orig_h;
	    FILE *cr = c->get_cached_response(localhost_url, orig_h);
	    DBG(copy(orig_h.begin(), orig_h.end(), 
		     ostream_iterator<string>(cerr, "\n")));

	    // Before we merge, et c., check that the headers we're going to
	    // poke in aren't already there.
	    CPPUNIT_ASSERT(find(orig_h.begin(), orig_h.end(), 
				"XHTTPCache: 123456789") == orig_h.end());
	    CPPUNIT_ASSERT(find(orig_h.begin(), orig_h.end(), 
				"Date: <invalid date>") == orig_h.end());

	    // Make up some new headers.
	    vector<string> new_h;
	    new_h.push_back("XHTTPCache: 123456789");
	    new_h.push_back("Date: <invalid date>");
	
	    c->release_cached_response(cr);

	    c->update_response(localhost_url, time(0), new_h);
	
	    vector<string> updated_h;
	    cr =  c->get_cached_response(localhost_url, updated_h);
	    c->release_cached_response(cr);
	    DBG(copy(updated_h.begin(), updated_h.end(),
		     ostream_iterator<string>(cerr, "\n")));
	
	    // The XHTTPCacheTest header should be new, Date should replace the
	    // existing Date header.
	    CPPUNIT_ASSERT(orig_h.size() + 1 == updated_h.size());
	    CPPUNIT_ASSERT(find(updated_h.begin(), updated_h.end(), 
				"XHTTPCache: 123456789") != updated_h.end());
	    CPPUNIT_ASSERT(find(updated_h.begin(), updated_h.end(), 
				"Date: <invalid date>") != updated_h.end());
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}

	HTTPCache::delete_instance();
    }

    // Only run this interactively since you need to hit Ctrl-c to generate
    // SIGINT while the cache is doing its thing. 02/10/04 jhrg
    void interrupt_test() {
	try {
	    HTTPCache *c = new HTTPCache("cache-testsuite/singleton_cache", true);
	    string coads = "http://test.opendap.org/data/nc/coads_climatology.nc";
	    if (!c->is_url_in_cache(coads)) {
		HTTPResponse *rs = http_conn->fetch_url(coads);
		cerr << "In interrupt test, hit ctrl-c now... ";
		c->cache_response(coads, time(0), 
				  *(rs->get_headers()), rs->get_stream());
		cerr << "to late.";
		delete rs; rs = 0;
	    }
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}

	// Call this here to simulate exiting the program. This ensures that
	// the next test's call to instance() gets a fresh cache. The static
	// method will still be run at exit, but that's OK since it tests the
	// value of _instance and simply returns with it's zero.
	HTTPCache::delete_instance();
    }
    
    void cache_gc_test() {
	string fnoc1 = "http://test.opendap.org/opendap/nph-dods/data/nc/fnoc1.nc.dds";
	string fnoc2 = "http://test.opendap.org/opendap/nph-dods/data/nc/fnoc2.nc.dds";
	string fnoc3 = "http://test.opendap.org/opendap/nph-dods/data/nc/fnoc3.nc.dds";
	try {
	    auto_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache", true));

	    CPPUNIT_ASSERT(pc->d_block_size == 4096);

	    // Change the size parameters so that we can run some tests
	    pc->d_total_size = 12288; // bytes
	    pc->d_folder_size = pc->d_total_size/10;
	    pc->d_gc_buffer = pc->d_total_size/10;

	    // Thie cache should start empty
	    CPPUNIT_ASSERT(pc->d_current_size == 0);

	    // Get a url
	    HTTPResponse *rs = http_conn->fetch_url(fnoc1);
	    pc->cache_response(fnoc1, time(0), *(rs->get_headers()),
			       rs->get_stream());
	    CPPUNIT_ASSERT(pc->is_url_in_cache(fnoc1));
	    delete rs; rs = 0;
	    // trigger a hit for fnoc1
	    vector<string> h;
	    FILE *f = pc->get_cached_response(fnoc1, h);
	    pc->release_cached_response(f);

	    rs = http_conn->fetch_url(fnoc2);
	    pc->cache_response(fnoc2, time(0), *(rs->get_headers()),
			       rs->get_stream());
	    CPPUNIT_ASSERT(pc->is_url_in_cache(fnoc2));
	    delete rs; rs = 0;
	    // trigger two hits for fnoc2
	    f = pc->get_cached_response(fnoc2, h);
	    pc->release_cached_response(f);
	    f = pc->get_cached_response(fnoc2, h);
	    pc->release_cached_response(f);

	    rs = http_conn->fetch_url(fnoc3);
	    pc->cache_response(fnoc3, time(0), *(rs->get_headers()),
			       rs->get_stream());
	    CPPUNIT_ASSERT(pc->is_url_in_cache(fnoc3));
	    delete rs; rs = 0;
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}

	// now that pc is out of scope, its dtor has been run and GC
	// performed. The fnoc3 URL should have been deleted.

	try {
	    auto_ptr<HTTPCache> pc(new HTTPCache("cache-testsuite/purge_cache", true));
	    CPPUNIT_ASSERT(!pc->is_url_in_cache(fnoc3));
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPCacheTest);

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    cerr.flush();

    // now clean up the directories
    system("cd cache-testsuite && ./cleanup.sh");

    return wasSuccessful ? 0 : 1;
}

// $Log: HTTPCacheTest.cc,v $
// Revision 1.13  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.9.2.11  2005/01/25 00:40:12  jimg
// Fixed a bug where caching small entries broke the GC algorithm.
// The code used the size of the entry as a measure of the actual disk
// space used by the entry. For small entries this was a significant
// error (off by a factor of > 32 for the test.nc dataset). I changed
// the code to use the block size and assume that each entry occupies
// n*blocksize bytes where n >= 1. I added a test to check that the
// purge code works correctly.
//
// Revision 1.12  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.9.2.10  2004/03/11 18:26:31  jimg
// Fixed the tests so that we no longer need to fiddle with the pthread_once_t
// mutex. That just broke too many things...
//
// Revision 1.11  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.9.2.9  2004/02/15 22:48:36  rmorris
// Removed SignalHandler method calls under win32.  Signal handling is not
// portable.
//
// Revision 1.9.2.8  2004/02/11 22:26:46  jimg
// Changed all calls to delete so that whenever we use 'delete x' or
// 'delete[] x' the code also sets 'x' to null. This ensures that if a
// pointer is deleted more than once (e.g., when an exception is thrown,
// the method that throws may clean up and then the catching method may
// also clean up) the second, ..., call to delete gets a null pointer
// instead of one that points to already deleted memory.
//
// Revision 1.9.2.7  2004/02/11 17:34:25  jimg
// Updated to test interrupting the cache during writes. The tests require that
// HTTPCache be recompiled with debugging on.
//
// Revision 1.9.2.6  2004/02/10 20:50:32  jimg
// Test cache behavior when writing a file is interrupted. You must fiddle with
// the code (and HTTPCache.cc) and recompile to run this test.
//
// Revision 1.9.2.5  2004/01/22 20:47:24  jimg
// Fix for bug 689. I added tests to make sure the cache size doesn't wind
// up being set to a negative number. I also changed the types of the cache
// size and entry size from int to unsigned long. Added information to
// the default .dodsrc file explaining the units of the CACHE_SIZE and
// MAX_ENTRY_SIZE parameters.
//
// Revision 1.10  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.9.2.4  2003/09/29 05:11:40  rmorris
// Minor win32 port tweek involving string literals in macros.
//
// Revision 1.9.2.3  2003/09/06 22:11:02  jimg
// Modified so that localhost is used instead of remote hosts. This means
// that the tests don't require Internet access but do require that the
// local machine runs httpd and has it correctly configured.
//
// Revision 1.9.2.2  2003/08/17 20:37:15  rmorris
// Mod's to more-uniformly and simply account for sleep(secs) vs. Sleep(millisecs)
// difference between unix/Linux/OSX and win32.  The ifdef to make win32
// match unixes is now in one single win32-specific place.
//
// Revision 1.9.2.1  2003/05/06 06:45:29  jimg
// Fixed HTTPCacheTest to work with the new HTTPConnect and Response
// classes. These use local copies of the response headers managed by the
// Response class.
//
// Revision 1.9  2003/05/01 22:52:46  jimg
// Corrected the get_conditional_response_headers_test() given the latest mods
// to HTTPCache.cc.
//
// Revision 1.8  2003/04/23 21:33:53  jimg
// Changes for the unit tests. This involved merging Rob's VC++ changes
// and fixing a bug in escaping.cc (a call to string::insert invalidated
// an iterator in a loop).
//
// Revision 1.7  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.3.2.2  2003/04/18 00:54:24  rmorris
// Ported dap unit tests to win32.
//
// Revision 1.6  2003/03/04 23:19:37  jimg
// Fixed some of the unit tests.
//
// Revision 1.5  2003/03/04 17:30:04  jimg
// Now uses Response objects.
//
// Revision 1.4  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.5  2002/12/24 00:51:04  jimg
// Updated for the new (streamlined) HTTPConnect interface.
//
// Revision 1.1.2.4  2002/10/18 23:02:22  jimg
// Massive additions for the latest version of HTTPCache.
//
// Revision 1.1.2.3  2002/10/11 00:19:38  jimg
// Latest version. 22 tests. Note that these tests don't make a cache that
// behaves correctly; all of the test caches grown in size and need to be
// cleaned up. All can be removed *except* cache-testsuite/dods_cache. The
// latter has some files which should not be removed.
//
// Revision 1.1.2.2  2002/10/08 05:24:45  jimg
// This checking matches the version of HTTPCache.cc,h of the same date.
// All the tests pass but they rely on the current contents of
// cache-testsuite.
//
// Revision 1.1.2.1  2002/09/17 00:19:29  jimg
// New tests for cache root and lock code.
//
// Revision 1.1.2.5  2002/06/19 06:05:36  jimg
// Built a single file version of the cgi_util tests. This should be easy to
// clone for other test suites.
//
