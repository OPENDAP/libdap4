
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

//#define DODS_DEBUG

#include <fcntl.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

#include "GetOpt.h"

#include "chunked_ostream.h"
#include "chunked_istream.h"

#include "InternalErr.h"
#include "test_config.h"
#include "debug.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

using namespace std;
using namespace CppUnit;
using namespace libdap;

/**
 * The intent is to test writing to and reading from a chunked iostream,
 * using various combinations of chunk/buffer sizes and character red/write
 * sizes. There are three write functions and three read functions and
 * all combinations are tested.
 */
class chunked_iostream_test: public TestFixture {
private:
	// This should be big enough to do meaningful timing tests
	string big_file;
	// This should be smaller than a single buffer
	string small_file;
	// A modest sized text file - makes looking at the results easier
	string text_file;
public:
    chunked_iostream_test()
    {
    }
    ~chunked_iostream_test()
    {
    }

    void setUp()
    {
    	big_file = "test_big_binary_file.bin";
    	small_file = "test_small_text_file.txt";
    	text_file = "test_text_file.txt";
    }

    void tearDown()
    {
    }

    void
    single_char_write(const string &file, int buf_size)
    {
    	fstream infile(file.c_str(), ios::in|ios::binary);
    	if (!infile.good())
    		CPPUNIT_FAIL("File not open or eof");

    	string out = file + ".chunked";
    	fstream outfile(out.c_str(), ios::out|ios::binary);

    	chunked_ostream chunked_outfile(outfile, buf_size);

    	char c;
    	infile.read(&c, 1);
    	int num = infile.gcount();
    	while (num > 0 && !infile.eof()) {
    		chunked_outfile.write(&c, num);
    		infile.read(&c, 1);
    		num = infile.gcount();
    	}

    	if (num > 0 && !infile.bad()) {
    		chunked_outfile.write(&c, num);
    	}

    	chunked_outfile.flush();
    }

    void
    write_128char_data(const string &file, int buf_size)
    {
    	fstream infile(file.c_str(), ios::in|ios::binary);
    	if (!infile.good())
    		CPPUNIT_FAIL("File not open or eof");

    	string out = file + ".chunked";
    	fstream outfile(out.c_str(), ios::out|ios::binary);

    	chunked_ostream chunked_outfile(outfile, buf_size);

    	char str[128];
    	infile.read(str, 128);
    	int num = infile.gcount();
    	while (num > 0 && !infile.eof()) {
    		chunked_outfile.write(str, num);
    		infile.read(str, 128);
    		num = infile.gcount();
    	}

    	if (num > 0 && !infile.bad()) {
    		chunked_outfile.write(str, num);
    	}

        chunked_outfile.flush();
    }

    void
    write_24char_data_with_error_option(const string &file, int buf_size, bool error = false)
    {
    	fstream infile(file.c_str(), ios::in|ios::binary);
    	if (!infile.good())
    		CPPUNIT_FAIL("File not open or eof");

    	string out = file + ".chunked";
    	fstream outfile(out.c_str(), ios::out|ios::binary);

    	chunked_ostream chunked_outfile(outfile, buf_size);

    	try {
    		char str[24];
    		infile.read(str, 24);
    		int num = infile.gcount();
    		if (num > 0 && !infile.eof()) {
    			chunked_outfile.write(str, num);
    			chunked_outfile.flush();
    		}

    		infile.read(str, 24);
    		num = infile.gcount();
    		if (num > 0 && !infile.eof()) chunked_outfile.write(str, num);

    		// Send an error chunk; the 24 bytes read here are lost...
    		if (error)
    			throw InternalErr(__FILE__, __LINE__, "Testing error transmission");

    		infile.read(str, 24);
    		num = infile.gcount();
    		while (num > 0 && !infile.eof()) {
    			chunked_outfile.write(str, num);
    			infile.read(str, 24);
    			num = infile.gcount();
    		}

    		if (num > 0 && !infile.bad()) {
    			chunked_outfile.write(str, num);
    		}

            chunked_outfile.flush();
    	}
    	catch (Error &e) {
    		chunked_outfile.write_err_chunk(e.get_error_message());
    	}
    }

    void
    single_char_read(const string &file, int buf_size)
    {
    	string in = file + ".chunked";
    	fstream infile(in.c_str(), ios::in|ios::binary);
    	if (!infile.good())
    		CPPUNIT_FAIL("File not open or eof");
    	chunked_istream chunked_infile(infile, buf_size, 0x00);

    	string out = file + ".plain";
    	fstream outfile(out.c_str(), ios::out|ios::binary);

    	char c;
    	int count = 1;
    	chunked_infile.read(&c, 1);
    	int num = chunked_infile.gcount();
    	DBG(cerr << "num: " << count++ << endl);
    	while (num > 0 && !chunked_infile.eof()) {
    		outfile.write(&c, num);
    		chunked_infile.read(&c, 1);
    		num = chunked_infile.gcount();
            DBG(cerr << "num: " << count++ << endl);
    	}

    	if (num > 0 && !chunked_infile.bad())
    	    outfile.write(&c, num);

    	outfile.flush();
    }

    void
    read_128char_data(const string &file, int buf_size)
    {
    	string in = file + ".chunked";
    	fstream infile(in.c_str(), ios::in|ios::binary);
    	if (!infile.good())
    		cerr << "File not open or eof" << endl;
    	chunked_istream chunked_infile(infile, buf_size);

    	string out = file + ".plain";
    	fstream outfile(out.c_str(), ios::out|ios::binary);

    	char str[128];
    	int count = 1;
    	chunked_infile.read(str, 128);
    	int num = chunked_infile.gcount();
    	DBG(cerr << "num: " << num << ", " << count++ << endl);
    	while (num > 0 && !chunked_infile.eof()) {
    		outfile.write(str, num);
    		chunked_infile.read(str, 128);
    		num = chunked_infile.gcount();
    		DBG(cerr << "num: " << num << ", " <<  count++ << endl);
    	}

    	if (num > 0 && !chunked_infile.bad()) {
    		outfile.write(str, num);
    	}

    	outfile.flush();
    }

    void
    read_24char_data_with_error_option(const string &file, int buf_size)
    {
    	string in = file + ".chunked";
    	fstream infile(in.c_str(), ios::in|ios::binary);
    	if (!infile.good())
    		cerr << "File not open or eof" << endl;
    	chunked_istream chunked_infile(infile, buf_size);

    	string out = file + ".plain";
    	fstream outfile(out.c_str(), ios::out|ios::binary);

    	try {
    		char str[24];
    		chunked_infile.read(str, 24);
    		int num = chunked_infile.gcount();
    		if (num > 0 && !chunked_infile.eof()) {
    			outfile.write(str, num);
    			outfile.flush();
    		}

    		chunked_infile.read(str, 24);
    		num = chunked_infile.gcount();
    		while (num > 0 && !chunked_infile.eof()) {
    			outfile.write(str, num);
    			chunked_infile.read(str, 24);
    			num = chunked_infile.gcount();
    		}

    		if (num > 0 && !chunked_infile.bad()) {
    			outfile.write(str, num);
    		}

    		outfile.flush();
    	}
    	catch (Error &e) {
    		cerr << "Error chunk found: " << e.get_error_message() << endl;
    	}
    }

    // these are the tests
    void test_write_1_read_1_small_file() {
    	single_char_write(small_file, 32);
    	single_char_read(small_file, 32);
    	string cmp = "cmp " + small_file + " " + small_file + ".plain";
    	CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_1_text_file() {
    	single_char_write(text_file, 32);
    	single_char_read(text_file, 32);
    	string cmp = "cmp " + text_file + " " + text_file + ".plain";
    	CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_1_big_file() {
    	single_char_write(big_file, 28);
    	single_char_read(big_file, 28);
    	string cmp = "cmp " + big_file + " " + big_file + ".plain";
    	CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // these are the tests
    void test_write_1_read_128_small_file() {
        single_char_write(small_file, 32);
        read_128char_data(small_file, 32);
        string cmp = "cmp " + small_file + " " + small_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_128_text_file() {
        single_char_write(text_file, 32);
        read_128char_data(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_128_big_file() {
        single_char_write(big_file, 28);
        read_128char_data(big_file, 28);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    CPPUNIT_TEST_SUITE(chunked_iostream_test);

    CPPUNIT_TEST(test_write_1_read_1_small_file);
    CPPUNIT_TEST(test_write_1_read_1_text_file);
    CPPUNIT_TEST(test_write_1_read_1_big_file);

    CPPUNIT_TEST(test_write_1_read_128_small_file);
    CPPUNIT_TEST(test_write_1_read_128_text_file);
    CPPUNIT_TEST(test_write_1_read_128_big_file);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(chunked_iostream_test);

int
main(int argc, char *argv[])
{
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
        while (i < argc) {
            test = string("chunked_iostream_test::") + argv[i++];
            if (debug)
                cerr << "Running " << test << endl;
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
