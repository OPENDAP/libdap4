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
#include <cppunit/extensions/HelperMacros.h>
#include <cstdlib>

#include <exception> // std::exception
#include <fstream>
#include <iostream>
#include <string>

#include "chunked_istream.h"
#include "chunked_ostream.h"

#include "debug.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

#undef DBG
#define DBG(x)                                                                                                         \
    do {                                                                                                               \
        if (debug)                                                                                                     \
            (x);                                                                                                       \
    } while (false)

#define prolog string("chunked_iostream_test::").append(__func__).append("() - ")
#define TIMING_TEST 0

const string path = (string)TEST_SRC_DIR + "/chunked-io";

using namespace std;
using namespace CppUnit;
using namespace libdap;

string the_test_text = "Stephen could remember an evening when he had sat there in the warm,\n"
                       "deepening twilight, watching the sea; it had barely a ruffle on its surface,\n"
                       "and yet the Sophie picked up enough moving air with her topgallants\n"
                       "to draw a long straight whispering furrow across the water, a line\n"
                       "brilliant with unearthly phosphorescence, visible for quarter of a mile behind her.\n"
                       "Days and nights of unbelievable purity. Nights when the steady Ionian breeze\n"
                       "rounded the square mainsail – not a brace to be touched, watch relieving watch –\n"
                       "and he and Jack on deck, sawing away, sawing away, lost in their music,\n"
                       "until the falling dew untuned their strings. And days when the perfection\n"
                       "of dawn was so great, the emptiness so entire, that men were almost afraid to speak.\n";

/**
 * The intent is to test writing to and reading from a chunked iostream,
 * using various combinations of chunk/buffer sizes and character red/write
 * sizes. There are three write functions and three read functions and
 * all combinations are tested.
 */
class chunked_iostream_test : public TestFixture {
private:
    // This should be big enough to do meaningful timing tests
    string big_file, big_file_2, big_file_3;
    // This should be smaller than a single buffer
    string small_file;
    // A modest sized text file - makes looking at the results easier
    string text_file;

public:
    chunked_iostream_test() = default;
    ~chunked_iostream_test() override = default;

    void setUp() override {
        DBG(cerr << "\n");

        big_file = path + "/test_big_binary_file.bin";
        big_file_2 = path + "/test_big_binary_file_2.bin";
        big_file_3 = path + "/test_big_binary_file_3.bin"; // not used yet

        small_file = path + "/test_small_text_file.txt";
        text_file = path + "/test_text_file.txt";
    }

    void tearDown() override {}

    static void single_char_write(const string &file, int buf_size) {
        fstream infile(file.c_str(), ios::in | ios::binary);
        DBG(cerr << "infile: " << file << endl);
        if (!infile.good())
            CPPUNIT_FAIL("File not open or eof");

        string out = file + ".chunked";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        chunked_ostream chunked_outfile(outfile, buf_size);

        char c;
        infile.read(&c, 1);
        auto num = infile.gcount();
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

    static void write_128char_data(const string &file, int buf_size) {
        fstream infile(file.c_str(), ios::in | ios::binary);
        if (!infile.good())
            CPPUNIT_FAIL("File not open or eof");

        string out = file + ".chunked";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        chunked_ostream chunked_outfile(outfile, buf_size);

        char str[128];
        infile.read(str, 128);
        auto num = infile.gcount();
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

    static void write_9000char_data(const string &file, int buf_size) {
        fstream infile(file.c_str(), ios::in | ios::binary);
        if (!infile.good())
            CPPUNIT_FAIL("File not open or eof");

        string out = file + ".chunked";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        chunked_ostream chunked_outfile(outfile, buf_size);

        char str[9000];
        infile.read(str, 9000);
        auto num = infile.gcount();
        while (num > 0 && !infile.eof()) {
            chunked_outfile.write(str, num);
            infile.read(str, 9000);
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
    static void write_24char_data_with_error_option(const string &file, int buf_size, bool error = false) {
        fstream infile(file.c_str(), ios::in | ios::binary);
        if (!infile.good())
            CPPUNIT_FAIL("File not open or eof");

        string out = file + ".chunked";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        chunked_ostream chunked_outfile(outfile, buf_size);

        try {
            char str[24];
            infile.read(str, 24);
            auto num = infile.gcount();
            if (num > 0 && !infile.eof()) {
                chunked_outfile.write(str, num);
                chunked_outfile.flush();
            }

            infile.read(str, 24);
            num = infile.gcount();
            if (num > 0 && !infile.eof())
                chunked_outfile.write(str, num);

            // Send an error chunk; the 24 bytes read here are lost...
            if (error)
                throw Error("Testing error transmission");

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
        } catch (Error &e) {
            chunked_outfile.write_err_chunk(e.get_error_message());
        }
    }

    static void single_char_read(const string &file, int buf_size) {
        string in = file + ".chunked";
        fstream infile(in.c_str(), ios::in | ios::binary);
        if (!infile.good())
            CPPUNIT_FAIL("File not open or eof");
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
        auto num = chunked_infile.gcount();
        DBG(cerr << "num: " << num << ", " << count++ << endl);
        while (num > 0 && !chunked_infile.eof()) {
            outfile.write(&c, num);
            chunked_infile.read(&c, 1);
            num = chunked_infile.gcount();
            DBG(cerr << "num: " << num << ", " << count++ << ", eof: " << chunked_infile.eof() << endl);
        }

        DBG(cerr << "eof is :" << chunked_infile.eof() << ", num: " << num << endl);

        if (num > 0 && !chunked_infile.bad())
            outfile.write(&c, num);

        outfile.flush();
    }

    static void read_128char_data(const string &file, int buf_size) {
        string in = file + ".chunked";
        fstream infile(in.c_str(), ios::in | ios::binary);
        if (!infile.good())
            cerr << "File not open or eof" << endl;
        chunked_istream chunked_infile(infile, buf_size);

        string out = file + ".plain";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        char str[128];
        int count = 1;
        chunked_infile.read(str, 128);
        auto num = chunked_infile.gcount();
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

    static void read_5000char_data(const string &file, int buf_size) {
        string in = file + ".chunked";
        fstream infile(in.c_str(), ios::in | ios::binary);
        if (!infile.good())
            cerr << "File not open or eof" << endl;
        chunked_istream chunked_infile(infile, buf_size);

        string out = file + ".plain";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        char str[5000];
        int count = 1;
        chunked_infile.read(str, 5000);
        auto num = chunked_infile.gcount();
        DBG(cerr << "num: " << num << ", " << count++ << endl);
        while (num > 0 && !chunked_infile.eof()) {
            outfile.write(str, num);
            chunked_infile.read(str, 5000);
            num = chunked_infile.gcount();
            DBG(cerr << "num: " << num << ", " << count++ << ", eof: " << chunked_infile.eof() << endl);
        }

        if (num > 0 && !chunked_infile.bad()) {
            outfile.write(str, num);
        }

        outfile.flush();
    }

    static void read_24char_data_with_error_option(const string &file, int buf_size) {
        string in = file + ".chunked";
        fstream infile(in.c_str(), ios::in | ios::binary);
        if (!infile.good())
            cerr << "File not open or eof" << endl;
        chunked_istream chunked_infile(infile, buf_size);

        string out = file + ".plain";
        fstream outfile(out.c_str(), ios::out | ios::binary);

        try {
            char str[24];
            chunked_infile.read(str, 24);
            auto num = chunked_infile.gcount();
            while (num > 0 && !chunked_infile.eof()) {
                outfile.write(str, num);
                chunked_infile.read(str, 24);
                num = chunked_infile.gcount();
            }

            // The chunked_istream uses a chunked_inbuf and that signals error
            // using EOF. The error message is stored in the buffer and can be
            // detected and accessed using the error() error_message() methods
            // that both the buffer and istream classes have.
            if (chunked_infile.error())
                throw Error("Found an error in the stream");

            if (num > 0 && !chunked_infile.bad()) {
                outfile.write(str, num);
            }

            outfile.flush();
        } catch (Error &e) {
            DBG(cerr << "Error chunk found: " << e.get_error_message() << endl);
            throw;
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
    void test_write_1_read_1_big_file_2() {
        single_char_write(big_file_2, 28);
        single_char_read(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
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

    void test_write_1_read_128_big_file_2() {
        single_char_write(big_file_2, 28);
        read_128char_data(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // 24 char write units

    void test_write_24_read_1_text_file() {
        write_24char_data_with_error_option(text_file, 32);
        single_char_read(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_1_big_file() {
        write_24char_data_with_error_option(big_file, 1024);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file, 1024);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_1_big_file_2() {
        write_24char_data_with_error_option(big_file_2, 1024);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file_2, 1024);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_128_text_file() {
        write_24char_data_with_error_option(text_file, 32);
        read_128char_data(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_128_big_file() {
        write_24char_data_with_error_option(big_file, 28);
        DBG(cerr << "Wrote the file" << endl);

        read_128char_data(big_file, 28);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_24_read_128_big_file_2() {
        write_24char_data_with_error_option(big_file_2, 28);
        read_128char_data(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // 128 char writes
    void test_write_128_read_1_text_file() {
        write_128char_data(text_file, 32);
        single_char_read(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_1_big_file() {
        write_128char_data(big_file, 32);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file, 32);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_1_big_file_2() {
        write_128char_data(big_file_2, 32);
        DBG(cerr << "Wrote the file" << endl);

        single_char_read(big_file_2, 1024);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_128_text_file() {
        write_128char_data(text_file, 32);
        read_128char_data(text_file, 32);
        string cmp = "cmp " + text_file + " " + text_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_128_big_file() {
        write_128char_data(big_file, 28);
        DBG(cerr << "Wrote the file" << endl);

        read_128char_data(big_file, 28);
        string cmp = "cmp " + big_file + " " + big_file + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_128_read_128_big_file_2() {
        write_128char_data(big_file_2, 28);
        read_128char_data(big_file_2, 28);
        string cmp = "cmp " + big_file_2 + " " + big_file_2 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    void test_write_9000_read_5000_big_file_3() {
        write_9000char_data(big_file_3, 4000);
        read_5000char_data(big_file_3, 3096);
        string cmp = "cmp " + big_file_3 + " " + big_file_3 + ".plain";
        CPPUNIT_ASSERT(system(cmp.c_str()) == 0);
    }

    // Send an error

    void test_write_24_read_24_big_file_2_error() {
        write_24char_data_with_error_option(big_file_2, 2048, true /*error*/);
        try {
            read_24char_data_with_error_option(big_file_2, 2048);
            CPPUNIT_FAIL("Should have caught an error message");
        } catch (Error &e) {
            CPPUNIT_ASSERT(!e.get_error_message().empty());
        }
    }

    static string tf(bool val) { return val ? "true" : "false"; }

    string check_stream(const chunked_ostream &os) {
        stringstream msg;
        if (!os.good()) {
            msg << prolog << "chunked_outfile::good(): " << tf(os.good()) << ").\n";
            msg << prolog << "chunked_outfile::eof():  " << tf(os.eof()) << ").\n";
            msg << prolog << "chunked_outfile::fail(): " << tf(os.fail()) << ").\n";
            msg << prolog << "chunked_outfile::bad():  " << tf(os.bad()) << ").\n";
            msg << prolog << "file: " << __FILE__ << " line: " << __LINE__ << "\n";
        }
        return msg.str();
    }

    bool write_chunked_file(string &out_file, const uint64_t target_size) {
        string error_msg;
        fstream outfile(out_file.c_str(), ios::out | ios::binary);
        chunked_ostream chunked_outfile(outfile, the_test_text.size());
        error_msg = check_stream(chunked_outfile);
        if (error_msg.empty()) {
            the_test_text.size();
            uint64_t position = 0;
            uint64_t remaining;
            std::streamsize outnum;
            while (position < target_size) {
                remaining = target_size - position;
                if (the_test_text.size() < remaining) {
                    outnum = (std::streamsize)the_test_text.size();
                } else {
                    outnum = (std::streamsize)remaining;
                }
                chunked_outfile.write(the_test_text.c_str(), outnum);
                error_msg = check_stream(chunked_outfile);
                if (!error_msg.empty()) {
                    cerr << error_msg;
                    return false;
                }
                position += outnum;
            }
        }
        return true;
    }

    static uint64_t read_chunked_file(const string &ifile, const string &ofile, unsigned int bufsize) {
        fstream infile(ifile.c_str(), ios::in | ios::binary);
        if (!infile.good())
            cerr << "ERROR Failed to open or encountered eof for: " << ifile << "\n";
        chunked_istream chunked_infile(infile, bufsize);

        fstream outfile(ofile.c_str(), ios::out | ios::binary);
        if (!outfile.good())
            cerr << "ERROR Failed to open or encountered eof for: " << ofile << "\n";

        char str[8096];
        // int count = 1;
        chunked_infile.read(str, 8096);
        auto num = chunked_infile.gcount();

        uint64_t sz = 0;
        while (num > 0 && !chunked_infile.eof()) {
            outfile.write(str, num);
            sz += num;
            chunked_infile.read(str, 8096);
            num = chunked_infile.gcount();
        }
        if (num > 0 && !chunked_infile.bad()) {
            outfile.write(str, num);
            sz += num;
        }
        return sz;
    }

    /**
     * 1GB = 1073741824 bytes
     * 2GB = 2147483648 bytes
     * 3GB = 3221225472 bytes
     * 4GB = 4294967296 bytes
     * 5GB = 5368709120 bytes
     */
    void write_then_read_large_chunked_file() {
        DBG(cerr << "\n");

        string chunked_filename = path + "/large-text-file.chunked";
        DBG(cerr << prolog << "    chunked_filename: " << chunked_filename << "\n");

        uint64_t target_size = 1073741824ULL * 5; // 1073741824 == 1GB
        DBG(cerr << prolog << "         target_size: " << target_size << " bytes\n");

        bool success = write_chunked_file(chunked_filename, target_size);
        CPPUNIT_ASSERT(success == true);

        string plain_file_out = path + "/large-text-file.plain";
        DBG(cerr << prolog << "      plain_file_out: " << plain_file_out << "\n");
        auto size = read_chunked_file(chunked_filename, plain_file_out, 8096);
        DBG(cerr << prolog << " read_chunked_file(): " << size << " bytes\n");

        CPPUNIT_ASSERT(size == target_size);

        cerr << "Remember to delete the large files made by the write_then_read_large_chunked_file() test.\n";
    }

    CPPUNIT_TEST_SUITE(chunked_iostream_test);

#if TIMING_TEST
    CPPUNIT_TEST(write_then_read_large_chunked_file);
#endif

    CPPUNIT_TEST(test_write_1_read_1_small_file);
    CPPUNIT_TEST(test_write_1_read_1_text_file);
    CPPUNIT_TEST(test_write_1_read_1_big_file);
    CPPUNIT_TEST(test_write_1_read_1_big_file_2);

    CPPUNIT_TEST(test_write_1_read_128_small_file);
    CPPUNIT_TEST(test_write_1_read_128_text_file);
    CPPUNIT_TEST(test_write_1_read_128_big_file);
    CPPUNIT_TEST(test_write_1_read_128_big_file_2);

    CPPUNIT_TEST(test_write_24_read_1_text_file);
    CPPUNIT_TEST(test_write_24_read_1_big_file);
    CPPUNIT_TEST(test_write_24_read_1_big_file_2);

    CPPUNIT_TEST(test_write_24_read_128_text_file);
    CPPUNIT_TEST(test_write_24_read_128_big_file);
    CPPUNIT_TEST(test_write_24_read_128_big_file_2);

    CPPUNIT_TEST(test_write_128_read_1_text_file);
    CPPUNIT_TEST(test_write_128_read_1_big_file);
    CPPUNIT_TEST(test_write_128_read_1_big_file_2);

    CPPUNIT_TEST(test_write_128_read_128_text_file);
    CPPUNIT_TEST(test_write_128_read_128_big_file);
    CPPUNIT_TEST(test_write_128_read_128_big_file_2);

    CPPUNIT_TEST(test_write_24_read_24_big_file_2_error);

    CPPUNIT_TEST(test_write_9000_read_5000_big_file_3);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(chunked_iostream_test);

int main(int argc, char *argv[]) { return run_tests<chunked_iostream_test>(argc, argv) ? 0 : 1; }
