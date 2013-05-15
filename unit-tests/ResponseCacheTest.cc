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

#include "config.h"

#include <dirent.h>

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "ConstraintEvaluator.h"
#include "ResponseBuilder.h"
#include "DDS.h"
#include "DDXParserSax2.h"
#include "ResponseCache.h"

#include "GetOpt.h"
#include "GNURegex.h"
#include "util.h"
#include "debug.h"

#include "../tests/TestTypeFactory.h"

#include "testFile.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;
using namespace libdap;

int test_variable_sleep_interval = 0;

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

namespace libdap {

class ResponseCacheTest: public TestFixture {
private:
	TestTypeFactory ttf;
	DDXParser dp;
	ConstraintEvaluator eval;
	ResponseBuilder rb;
    DDS dds;

    string d_response_cache;
    ResponseCache *cache;

public:
    ResponseCacheTest(): dds(&ttf), dp(&ttf), d_response_cache(string(TEST_SRC_DIR) + "/response_cache") {
    }

    ~ResponseCacheTest() {
    }

    void clean_cache(const string &directory, const string &prefix) {
        DIR *dip = opendir(directory.c_str());
        if (!dip)
            throw InternalErr(__FILE__, __LINE__, "Unable to open cache directory " + directory);

        struct dirent *dit;
        // go through the cache directory and collect all of the files that
        // start with the matching prefix
        while ((dit = readdir(dip)) != NULL) {
            string dirEntry = dit->d_name;
            if (dirEntry.compare(0, prefix.length(), prefix) == 0) {
            	unlink(string(directory + "/" + dit->d_name).c_str());
                //files.push_back(d_cache_dir + "/" + dirEntry);
            }
        }

        closedir(dip);
    }

    void setUp() {
    	string cid;
    	//dds = new DDS(&ttf, "");
    	dp.intern((string) TEST_SRC_DIR + "/ddx-testsuite/test.05.ddx", &dds, cid);
    	// cid == http://dods.coas.oregonstate.edu:8080/dods/dts/test.01.blob
    	DBG(cerr << "DDS Name: " << dds.get_dataset_name() << endl);
    	DBG(cerr << "Intern CID: " << cid << endl);
    }

    void tearDown() {
    	//delete dds; dds = 0;
		clean_cache(d_response_cache, "rc");
    }

    bool re_match(Regex &r, const string &s) {
        DBG(cerr << "s.length(): " << s.length() << endl);
        int pos = r.match(s.c_str(), s.length());
        DBG(cerr << "r.match(s): " << pos << endl);
        return pos > 0 && static_cast<unsigned> (pos) == s.length();
    }

    bool re_match_binary(Regex &r, const string &s) {
        DBG(cerr << "s.length(): " << s.length() << endl);
        int pos = r.match(s.c_str(), s.length());
        DBG(cerr << "r.match(s): " << pos << endl);
        return pos > 0;
    }

    void ctor_test_1() {
    	// The directory 'never' does not exist; the cache won't be initialized
    	cache = new ResponseCache(string(TEST_SRC_DIR) + "/never", "rc", 1000);
    	CPPUNIT_ASSERT(!cache->is_available());
    }

    void ctor_test_2() {
    	//cache = new ResponseCache(TEST_SRC_DIR + "response_cache", "rc", 1000);
    	cache = new ResponseCache(d_response_cache, "rc", 1000);
    	CPPUNIT_ASSERT(cache->is_available());
    }

	void cache_a_response()
	{
		//cache = new ResponseCache(TEST_SRC_DIR + "response_cache", "rc", 1000);
		cache = new ResponseCache(d_response_cache, "rc", 1000);
		string token;
		try {
			DDS *cache_dds = cache->read_cached_dataset(dds, "", &rb, &eval, token);
			cache->unlock_and_close(token);

			DBG(cerr << "Cached response token: " << token << endl);
			CPPUNIT_ASSERT(cache_dds);
			CPPUNIT_ASSERT(token == d_response_cache + "/rc#SimpleTypes#");
			delete cache_dds;
		}
		catch (Error &e) {
			CPPUNIT_FAIL(e.get_error_message());
		}
    }

	void cache_and_read_a_response()
	{
		//cache = new ResponseCache(TEST_SRC_DIR + "response_cache", "rc", 1000);
		cache = new ResponseCache(d_response_cache, "rc", 1000);
		string token;
		try {
			DDS *cache_dds = cache->read_cached_dataset(dds, "", &rb, &eval, token);
			cache->unlock_and_close(token);

			DBG(cerr << "Cached response token: " << token << endl);
			CPPUNIT_ASSERT(cache_dds);
			CPPUNIT_ASSERT(token == d_response_cache + "/rc#SimpleTypes#");
			delete cache_dds; cache_dds = 0;

			cache_dds = cache->read_cached_dataset(dds, "", &rb, &eval, token);
			cache->unlock_and_close(token);
			CPPUNIT_ASSERT(cache_dds);
			CPPUNIT_ASSERT(token == d_response_cache + "/rc#SimpleTypes#");
			delete cache_dds; cache_dds = 0;
		}
		catch (Error &e) {
			CPPUNIT_FAIL(e.get_error_message());
		}

    }
    CPPUNIT_TEST_SUITE( ResponseCacheTest );

    CPPUNIT_TEST(ctor_test_1);
    CPPUNIT_TEST(ctor_test_2);
    CPPUNIT_TEST(cache_a_response);
    CPPUNIT_TEST(cache_and_read_a_response);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ResponseCacheTest);
}

int main(int argc, char*argv[]) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    GetOpt getopt(argc, argv, "d");
    char option_char;
    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        default:
            break;
        }

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        while (i < argc) {
            test = string("libdap::ResponseCacheTest::") + argv[i++];

            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
