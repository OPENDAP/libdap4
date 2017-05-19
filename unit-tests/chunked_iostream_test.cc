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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

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

const string path = (string) TEST_SRC_DIR + "/chunked-io";

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
    string big_file, big_file_2, big_file_3;
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
        big_file = path + "/test_big_binary_file.bin";
        big_file_2 = path + "/test_big_binary_file_2.bin";
        big_file_3 = path + "/test_big_binary_file_3.bin"; // not used yet

        small_file = path + "/test_small_text_file.txt";
        text_file = path + "/test_text_file.txt";
    }

    void tearDown()
    {
    }

    void single_char_write(const string &file, int buf_size)
    {
        fstream infile(file.c_str(), ios::in | ios::binary);
        DBG(cerr << "infile: " << file << endl);
        if (!infile.good()) CPPUNIT_FAIL("File not open or eof");

        string out = file + ".chunked";
        fstream outfile(out.c_str(), ios::out | ios::binary);

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

    void write_128char_data(const string &file, int buf_size)
    {
        fstream infile(file.c_str(), ios::in | ios::binary);
        if (!infile.good()) CPPUNIT_FAIL("File not open or eof");

        string out = file + ".chunked";
        fstream outfile(out.c_str(), ios::out | ios::binary);

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

    // This will not work with the small text file. This code assume that
    // the file to be written has at least 24 bytes for the first chunk,
    // which is deliberately sent using flush before the buffer is full and
    // then has at least 48 more bytes (but ideally 49, because this code
    // tries to send an End chunk with one or more bytes as opposed to
    // sending the last data chunk with fewer than buf_size and then sending a
    // zero length END chunk).
    void write_24char_data_with_error_option(const string &file, int buf_size, bool error = false)
    {
        fstream infile(file.c_str(), ios::in | ios::binary);
        if (!infile.good()) CPPUNIT_FAIL("File not open or eof");

        string out = file + ".chunked";
        fstream outfile(out.c_str(), ios::out | ios::binary);

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
            if (error) throw Error("Testing error transmission");

            infile.read(str, 24);
            num = infile.gcount();
            while (num == 24 && !infile.eof()) {
                chunked_outfile.write(str, num);
                infile.read(str, 24);
                num = infile.gcount();
            }

            if (num > 0 && !infile.bad()) {
                chunked_outfile.write(str, num);
            }

            // flush() calls sync() which forces a DATA chunk to be sent, regardless of
            // the amount of data in the buffer. When the stream is destroyed, end_chunk()
            // is sent with the remain chars, so removing flush() here ensures that we test
            // a non-empty END chunk.
            // chunked_outfile.flush();
        }
        catch (Error &e) {
            chunked_outfile.write_err_chunk(e.get_error_message());
        }
    }

    void single_char_read(const string &file, int buf_size)
    {
        string in = file + ".chunked";
        fstream infile(in.c_str(), ios::in | ios::binary);
        if (!infile.good()) CPPUNIT_FAIL("File not open or eof");
#if BYTE_ORDER_PREFIX
        chunked_istream chunked_infile(infile, buf_size, 0x00);
#else
        chunked_istream chunked_infile(infile, buf_size);
#endif
        string out = file + ".plain";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        char c;
        int count = 1;
        chunked_infile.read(&c, 1);
        int num = chunked_infile.gcount();
        DBG(cerr << "num: " << num << ", " << count++ << endl);
        while (num > 0 && !chunked_infile.eof()) {
            outfile.write(&c, num);
            chunked_infile.read(&c, 1);
            num = chunked_infile.gcount();
            DBG(cerr << "num: " << num << ", " << count++ << ", eof: " << chunked_infile.eof() << endl);
        }

        DBG(cerr << "eof is :" << chunked_infile.eof() << ", num: " << num << endl);

        if (num > 0 && !chunked_infile.bad()) outfile.write(&c, num);

        outfile.flush();
    }

    void read_128char_data(const string &file, int buf_size)
    {
        string in = file + ".chunked";
        fstream infile(in.c_str(), ios::in | ios::binary);
        if (!infile.good()) cerr << "File not open or eof" << endl;
        chunked_istream chunked_infile(infile, buf_size);

        string out = file + ".plain";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        char str[128];
        int count = 1;
        chunked_infile.read(str, 128);
        int num = chunked_infile.gcount();
        DBG(cerr << "num: " << num << ", " << count++ << endl);
        while (num > 0 && !chunked_infile.eof()) {
            outfile.write(str, num);
            chunked_infile.read(str, 128);
            num = chunked_infile.gcount();
            DBG(cerr << "num: " << num << ", " << count++ << ", eof: " << chunked_infile.eof() << endl);
        }

        if (num > 0 && !chunked_infile.bad()) {
            outfile.write(str, num);
        }

        outfile.flush();
    }

    void read_24char_data_with_error_option(const string &file, int buf_size)
    {
        string in = file + ".chunked";
        fstream infile(in.c_str(), ios::in | ios::binary);
        if (!infile.good()) cerr << "File not open or eof" << endl;
        chunked_istream chunked_infile(infile, buf_size);

        string out = file + ".plain";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        try {
#if 0
            chunked_infile.read(str, 24);
            int num = chunked_infile.gcount();
            if (num > 0 && !chunked_infile.eof()) {
                outfile.write(str, num);
                outfile.flush();
            }
#endif
            char str[24];
            chunked_infile.read(str, 24);
            int num = chunked_infile.gcount();
            while (num > 0 && !chunked_infile.eof()) {
                outfile.write(str, num);
                chunked_infile.read(str, 24);
                num = chunked_infile.gcount();
            }

            // The chunked_istream uses a chunked_inbuf and that signals error
            // using EOF. The error message is stored in the buffer and can be
            // detected and accessed using the error() error_message() methods
            // that both the buffer and istream classes have.
            if (chunked_infile.error()) throw Error("Found an error in the stream");

            if (num > 0 && !chunked_infile.bad()) {
                outfile.write(str, num);
            }

            outfile.flush();
        }
        catch (Error &e) {
            DBG(cerr << "Error chunk found: " << e.get_error_message() << endl);
            throw;
        }
    }

    // these are the tests
    void test_write_1_read_1_small_file()
    {
        single_char_write(small_file, 32);
        single_char_read(small_file, 32);
        string cmp = "cmp " + small_file + " " + small_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_1_text_file()
    {
        single_char_write(text_file, 32);
        single_char_read(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_1_big_file()
    {
        single_char_write(big_file, 28);
        single_char_read(big_file, 28);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }
    void test_write_1_read_1_big_file_2()
    {
        single_char_write(big_file_2, 28);
        single_char_read(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // these are the tests
    void test_write_1_read_128_small_file()
    {
        single_char_write(small_file, 32);
        read_128char_data(small_file, 32);
        string cmp = "cmp " + small_file + " " + small_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_128_text_file()
    {
        single_char_write(text_file, 32);
        read_128char_data(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_128_big_file()
    {
        single_char_write(big_file, 28);
        read_128char_data(big_file, 28);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_1_read_128_big_file_2()
    {
        single_char_write(big_file_2, 28);
        read_128char_data(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // 24 char write units

    void test_write_24_read_1_text_file()
    {
        write_24char_data_with_error_option(text_file, 32);
        single_char_read(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_1_big_file()
    {
        write_24char_data_with_error_option(big_file, 1024);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file, 1024);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_1_big_file_2()
    {
        write_24char_data_with_error_option(big_file_2, 1024);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file_2, 1024);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_128_text_file()
    {
        write_24char_data_with_error_option(text_file, 32);
        read_128char_data(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_128_big_file()
    {
        write_24char_data_with_error_option(big_file, 28);
        DBG(cerr << "Wrote the file" << endl);

        read_128char_data(big_file, 28);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_128_big_file_2()
    {
        write_24char_data_with_error_option(big_file_2, 28);
        read_128char_data(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // 128 char writes
    void test_write_128_read_1_text_file()
    {
        write_128char_data(text_file, 32);
        single_char_read(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_1_big_file()
    {
        write_128char_data(big_file, 32);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file, 32);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_1_big_file_2()
    {
        write_128char_data(big_file_2, 32);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file_2, 1024);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_128_text_file()
    {
        write_128char_data(text_file, 32);
        read_128char_data(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_128_big_file()
    {
        write_128char_data(big_file, 28);
        DBG(cerr << "Wrote the file" << endl);

        read_128char_data(big_file, 28);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_128_big_file_2()
    {
        write_128char_data(big_file_2, 28);
        read_128char_data(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // Send an error

    void test_write_24_read_24_big_file_2_error()
    {
        write_24char_data_with_error_option(big_file_2, 2048, true /*error*/);
        try {
            read_24char_data_with_error_option(big_file_2, 2048);
            CPPUNIT_FAIL("Should have caught an error message");
        }
        catch (Error &e) {
            CPPUNIT_ASSERT(!e.get_error_message().empty());
        }
    }

    CPPUNIT_TEST_SUITE (chunked_iostream_test);

    CPPUNIT_TEST (test_write_1_read_1_small_file);
    CPPUNIT_TEST (test_write_1_read_1_text_file);
    CPPUNIT_TEST (test_write_1_read_1_big_file);
    CPPUNIT_TEST (test_write_1_read_1_big_file_2);

    CPPUNIT_TEST (test_write_1_read_128_small_file);
    CPPUNIT_TEST (test_write_1_read_128_text_file);
    CPPUNIT_TEST (test_write_1_read_128_big_file);
    CPPUNIT_TEST (test_write_1_read_128_big_file_2);

    CPPUNIT_TEST (test_write_24_read_1_text_file);
    CPPUNIT_TEST (test_write_24_read_1_big_file);
    CPPUNIT_TEST (test_write_24_read_1_big_file_2);

    CPPUNIT_TEST (test_write_24_read_128_text_file);
    CPPUNIT_TEST (test_write_24_read_128_big_file);
    CPPUNIT_TEST (test_write_24_read_128_big_file_2);

    CPPUNIT_TEST (test_write_128_read_1_text_file);
    CPPUNIT_TEST (test_write_128_read_1_big_file);
    CPPUNIT_TEST (test_write_128_read_1_big_file_2);

    CPPUNIT_TEST (test_write_128_read_128_text_file);
    CPPUNIT_TEST (test_write_128_read_128_big_file);
    CPPUNIT_TEST (test_write_128_read_128_big_file_2);

    CPPUNIT_TEST (test_write_24_read_24_big_file_2_error);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (chunked_iostream_test);

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;

        case 'h': {     // help - show test names
            cerr << "Usage: chunked_iostream_test has the following tests:" << endl;
            const std::vector<Test*> &tests = chunked_iostream_test::suite()->getTests();
            unsigned int prefix_len = chunked_iostream_test::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            break;
        }

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
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = chunked_iostream_test::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
