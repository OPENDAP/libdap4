
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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
#include <unistd.h>		// ibid [sic]

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "HTTPCache.h"
#include "HTTPConnect.h"	// Used to generate a response to cache.
// #define DODS_DEBUG 1
#include "debug.h"

using namespace CppUnit;

static int
file_size(string name)
{
    struct stat s;
    stat(name.c_str(), &s);
    return s.st_size;
}

void print_entry(HTTPCache *, HTTPCache::CacheEntry **e) {
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
    HTTPConnect http_conn;
    string index_file_line;
    string dodsdev_url;
    string expired;
    int hash_value;
    vector<string> h;

protected:

public:
    HTTPCacheTest() : http_conn(RCReader::instance()) {
	DBG2(cerr << "Entring HTTPCacheTest ctor... ");
	hash_value = 893;	
	dodsdev_url = "http://dodsdev.gso.uri.edu/test.html";
	index_file_line = dodsdev_url + " cache-testsuite/dods_cache/893/file1qr09a @cache@ 930090223 -1 197 0 893 1 172800 1030124259 1 0";
	expired = "http://dodsdev.gso.uri.edu/cgi-bin/expires.sh";

	h.push_back("ETag: jhrgjhrgjhrg");
	h.push_back("Last-Modified: Sat, 05 Nov 1994 08:49:37 GMT");
	h.push_back("Expires: Mon, 07 Nov 1994 08:49:37 GMT");
	h.push_back("Date: Sun, 06 Nov 1994 08:49:37 GMT");
	DBG2(cerr << "exiting." << endl);
    }

    ~HTTPCacheTest() { 
	DBG2(cerr << "Entering the HTTPCacheTest dtor... ");
	DBG2(cerr << "exiting." << endl);
    }
    
    void setUp () {
	// Called before every test.
	DBG(cerr << "Entering HTTPCacheTest::setUp... " << endl);
	hc = new HTTPCache("cache-testsuite/dods_cache/", true);
	DBG(cerr << "exiting setUp" << endl);
    }

    void tearDown() {
	// Called after every test.
	DBG(cerr << "Entering HTTPCacheTest::tearDown... " << endl);
	// Calling delete here fails; don't understand why. 10/03/02 jhrg
	delete hc;
	DBG(cerr << "exiting tearDown" << endl);
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

    CPPUNIT_TEST_SUITE_END();

    void constructor_test() {
	CPPUNIT_ASSERT(hc->d_cache_index=="cache-testsuite/dods_cache/.index");
	CPPUNIT_ASSERT(hc->d_cache_root == "cache-testsuite/dods_cache/");
	CPPUNIT_ASSERT(hc->d_current_size == 703);
    }

    void cache_index_read_test() {
	// It's possible that .index got trashed. Run the tests with
	// something stable.
	// hc->d_cache_index = "cache-testsuite/dods_cache/.index";
	CPPUNIT_ASSERT(hc->cache_index_read());

	HTTPCache::CacheEntry *e = hc->get_entry_from_cache_table(dodsdev_url);

	CPPUNIT_ASSERT(e);
	CPPUNIT_ASSERT(e->url == dodsdev_url);
    }

    void cache_index_parse_line_test() {
	HTTPCache::CacheEntry *e = hc->cache_index_parse_line(index_file_line.c_str());

	CPPUNIT_ASSERT(e->url == dodsdev_url);
	CPPUNIT_ASSERT(e->cachename 
		       == "cache-testsuite/dods_cache/893/file1qr09a");
	CPPUNIT_ASSERT(e->etag == "");
	CPPUNIT_ASSERT(e->lm == 930090223);
	// Skip ahead ...
	CPPUNIT_ASSERT(e->must_revalidate == false);
    }

    // This will also test the add_entry_to_cache_table() method.
    void get_entry_from_cache_table_test() {
	HTTPCache::CacheEntry *e 
	    = hc->cache_index_parse_line(index_file_line.c_str());

	// Test adding an entry and getting it back.
	hc->add_entry_to_cache_table(e);

	HTTPCache::CacheEntry *e2 
	    = hc->get_entry_from_cache_table(dodsdev_url);
	CPPUNIT_ASSERT(e2);
	CPPUNIT_ASSERT(e2->url == dodsdev_url);

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
	    = hc_4->get_entry_from_cache_table(dodsdev_url);
	CPPUNIT_ASSERT(e);
	CPPUNIT_ASSERT(e->url == dodsdev_url);

	delete hc_3;
	delete hc_4;
    }

    void create_cache_root_test() {
	hc->create_cache_root("/tmp/silly/");
	CPPUNIT_ASSERT(access("/tmp/silly/", F_OK) == 0);
	remove("/tmp/silly");
	try {
	    hc->create_cache_root("/root/very_silly/");
	    access("/root/very_silly/", F_OK);
	    remove("/root/very_silly/");
	    CPPUNIT_ASSERT(false && "Should not be able to do this...");
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

	try {
	    hc->set_cache_root("/root/never/ever/");
	    CPPUNIT_ASSERT(false && "Created cache in root's dir. Bad.");
	}
	catch (Error &e) {
	    CPPUNIT_ASSERT("OK, caught an error");
	}

	remove("/root/never/ever/");
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
	    CPPUNIT_ASSERT(false && "Create in bad directory");
	}
	catch (Error &e) {
	}

	remove("/tmp/dods_test_cache/391");

    }

    void create_location_test() {
	hc->set_cache_root("/tmp/dods_test_cache");
	HTTPCache::CacheEntry *e = new HTTPCache::CacheEntry;
	e->url = dodsdev_url;
	e->hash = hash_value;
	try {
	    hc->create_location(e);
	    CPPUNIT_ASSERT(e->cachename != "");
	}
	catch (Error &e) {
	    CPPUNIT_ASSERT(true && "could not create entry file");
	}
	remove(e->cachename.c_str());
    }

    void parse_headers_test() {
	HTTPCache::CacheEntry *e = new HTTPCache::CacheEntry;
	
	hc->parse_headers(e, h);
	CPPUNIT_ASSERT(e->lm == 784025377);
    }

    void calculate_time_test() {
	HTTPCache::CacheEntry *e = new HTTPCache::CacheEntry;
	
	hc->parse_headers(e, h);
	hc->calculate_time(e, time(0));
	CPPUNIT_ASSERT(e->corrected_initial_age > 249300571);
	CPPUNIT_ASSERT(e->freshness_lifetime == 86400);
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
    }

    void cache_response_test() {
	try {
	    time_t now = time(0);
	    FILE *body = http_conn.fetch_url(dodsdev_url);
	    vector<string> headers = http_conn.get_response_headers();
	    hc->cache_response(dodsdev_url, now, headers, body);

	    CPPUNIT_ASSERT(hc->is_url_in_cache(dodsdev_url));

	    HTTPCache::CacheEntry *e = hc->get_entry_from_cache_table(dodsdev_url);
	    CPPUNIT_ASSERT(file_size(e->cachename) == 703);
	}
	catch (Error &e) {
	    cerr << "Error: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false && "Caught unexpected Error/InternalErr");
	}
    }

    void is_url_valid_test() {
	cache_response_test();	// This should get a response into the cache.
	CPPUNIT_ASSERT(hc->is_url_valid(dodsdev_url));
    }

    void get_cached_response_test() {
	cache_response_test();	// Get a response into the cache.
	vector<string> cached_headers;
	FILE *cached_body = hc->get_cached_response(dodsdev_url, 
						    cached_headers);

	FILE *body = http_conn.fetch_url(dodsdev_url);
	vector<string> headers = http_conn.get_response_headers();

	// headers and cached_headers should match, except for the values.
	vector<string>::iterator i, j;
	for (i = cached_headers.begin(), j = headers.begin();
	     i != cached_headers.end() && j != headers.end();
	     ++i, ++j) {
	    string ch = (*i).substr(0, (*i).find(": "));
	    string h = (*j).substr(0, (*j).find(": "));
	    CPPUNIT_ASSERT(ch == h);
	}

	CPPUNIT_ASSERT(i == cached_headers.end() && j == headers.end());

	// every byte of the cached_body and response body should match.
	while (!feof(body) && !feof(cached_body) && !ferror(body) 
	       && !ferror(cached_body)) {
	    char cb, b;
	    int cn = fread(&cb, 1, 1, cached_body);
	    int n = fread(&b, 1, 1, body);
	    CPPUNIT_ASSERT(cn == n);
	    if (cn == 1)
		CPPUNIT_ASSERT(cb == b);
	}
	CPPUNIT_ASSERT(feof(body) && feof(cached_body));
    }

    void perform_garbage_collection_test() {
	try {	
	    auto_ptr<HTTPCache> gc(new HTTPCache("cache-testsuite/gc_cache", true));

	    FILE *resp = http_conn.fetch_url(dodsdev_url);
	    gc->cache_response(dodsdev_url, time(0), 
			       http_conn.get_response_headers(), 
			       resp);

	    CPPUNIT_ASSERT(gc->is_url_in_cache(dodsdev_url));
	    fclose(resp); 

	    resp = http_conn.fetch_url(expired);
	    gc->cache_response(expired, time(0), http_conn.get_response_headers(), 
			       resp);

	    CPPUNIT_ASSERT(gc->is_url_in_cache(expired));
	    fclose(resp); 
	
	    sleep(1);		// Needed for the `expired' response.
	    gc->perform_garbage_collection();

	    CPPUNIT_ASSERT(!gc->is_url_in_cache(expired) 
			   && "This may fail is sleep is not long enough before gc above");
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
	    FILE *resp = http_conn.fetch_url(dodsdev_url);
	    pc->cache_response(dodsdev_url, now, http_conn.get_response_headers(), 
			       resp);

	    CPPUNIT_ASSERT(pc->is_url_in_cache(dodsdev_url));
	    fclose(resp); 

	    string expired = "http://dodsdev.gso.uri.edu/cgi-bin/expires.sh";
	    now = time(0);
	    resp = http_conn.fetch_url(expired);
	    pc->cache_response(expired, now, http_conn.get_response_headers(), 
			       resp);

	    CPPUNIT_ASSERT(pc->is_url_in_cache(expired));
	    fclose(resp); 

	    HTTPCache::CacheEntry *e1 = pc->get_entry_from_cache_table(expired);
	    HTTPCache::CacheEntry *e2 = pc->get_entry_from_cache_table(dodsdev_url);
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

	    CPPUNIT_ASSERT(!pc->is_url_in_cache(dodsdev_url));
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

	    if (!c->is_url_in_cache(dodsdev_url)) {
		FILE *resp = http_conn.fetch_url(dodsdev_url);
		c->cache_response(dodsdev_url, time(0), 
				  http_conn.get_response_headers(), 
				  resp);
		fclose(resp); 
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(dodsdev_url));

	    if (!c->is_url_in_cache(expired)) {
		FILE *resp = http_conn.fetch_url(expired);
		c->cache_response(expired, time(0), 
				  http_conn.get_response_headers(), 
				  resp);
		fclose(resp); 
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(expired));

	    HTTPCache::CacheEntry *e1 = c->get_entry_from_cache_table(expired);
	    HTTPCache::CacheEntry *e2 = c->get_entry_from_cache_table(dodsdev_url);
	    string e1_file = e1->cachename;
	    string e2_file = e2->cachename;

	    c->purge_cache();

	    CPPUNIT_ASSERT(!c->is_url_in_cache(dodsdev_url));
	    CPPUNIT_ASSERT(!c->is_url_in_cache(expired));
	    CPPUNIT_ASSERT(access(e1_file.c_str(), F_OK) != 0);
	    CPPUNIT_ASSERT(access(e2_file.c_str(), F_OK) != 0);
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}
    }
    
    void get_conditional_response_headers_test() {
	try {
	    HTTPCache *c = HTTPCache::instance("cache-testsuite/singleton_cache",
					       true);
	    if (!c->is_url_in_cache(dodsdev_url)) {
		FILE *resp = http_conn.fetch_url(dodsdev_url);
		c->cache_response(dodsdev_url, time(0), 
				  http_conn.get_response_headers(), 
				  resp);
		fclose(resp); 
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(dodsdev_url));

	    if (!c->is_url_in_cache(expired)) {
		FILE *resp = http_conn.fetch_url(expired);
		c->cache_response(expired, time(0), 
				  http_conn.get_response_headers(), 
				  resp);
		fclose(resp); 
	    }
	    CPPUNIT_ASSERT(c->is_url_in_cache(expired));

	    vector<string> h = c->get_conditional_request_headers(dodsdev_url);
	    // copy(h.begin(), h.end(), ostream_iterator<string>(cout, "\n"));
	    // I know what the strings should start with, the values might vary.
	    CPPUNIT_ASSERT(h[0].find("If-None-Match: ") == 0);
	    CPPUNIT_ASSERT(h[1].find("If-Modified-Since: ") == 0);

	    h = c->get_conditional_request_headers(expired);
	    //copy(h.begin(), h.end(), ostream_iterator<string>(cout, "\n"));
	    CPPUNIT_ASSERT(h[0].find("If-Modified-Since: ") == 0);
	}
	catch(Error &e) {
	    cerr << "Exception: " << e.get_error_message() << endl;
	    CPPUNIT_ASSERT(false);
	}
    }

    void update_response_test() {
	try {
	    HTTPCache *c = HTTPCache::instance("cache-testsuite/singleton_cache",
					       true);
	    if (!c->is_url_in_cache(dodsdev_url)) {
		FILE *resp = http_conn.fetch_url(dodsdev_url);
		c->cache_response(dodsdev_url, time(0), 
				  http_conn.get_response_headers(), 
				  resp);
		fclose(resp); 
	    }

	    if (!c->is_url_in_cache(expired)) {
		FILE *resp = http_conn.fetch_url(expired);
		c->cache_response(expired, time(0), 
				  http_conn.get_response_headers(), 
				  resp);
		fclose(resp); 
	    }

	    // Yes, there's stuff here.
	    CPPUNIT_ASSERT(c->is_url_in_cache(dodsdev_url));
	    CPPUNIT_ASSERT(c->is_url_in_cache(expired));

	    vector<string> orig_h;
	    (void) c->get_cached_response(dodsdev_url, orig_h);
	    // copy(orig_h.begin(), orig_h.end(), ostream_iterator<string>(cerr, "\n"));

	    // Before we merge, et c., check that the headers we're going to poke
	    // in aren't already there.
	    CPPUNIT_ASSERT(find(orig_h.begin(), orig_h.end(), 
				"XHTTPCache: 123456789") == orig_h.end());
	    CPPUNIT_ASSERT(find(orig_h.begin(), orig_h.end(), 
				"Date: <invalid date>") == orig_h.end());

	    // Make up some new headers.
	    vector<string> new_h;
	    new_h.push_back("XHTTPCache: 123456789");
	    new_h.push_back("Date: <invalid date>");
	
	    c->update_response(dodsdev_url, time(0), new_h);
	
	    vector<string> updated_h;
	    (void) c->get_cached_response(dodsdev_url, updated_h);
	    // copy(updated_h.begin(), updated_h.end(), ostream_iterator<string>(cerr, "\n"));
	
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
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HTTPCacheTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    cerr.flush();

    // now clean up the directories
    system("cd cache-testsuite && ./cleanup.sh");

    return 0;
}

// $Log: HTTPCacheTest.cc,v $
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
