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

#include "config.h"

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

#include "D4StreamMarshaller.h"

#include "GetOpt.h"
#include "debug.h"
#include "test_config.h"

static bool debug = false;
static bool write_baselines = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

#if WORDS_BIGENDIAN
const static string path = (string)TEST_SRC_DIR + "/D4-marshaller/big-endian";
#else
const static string path = (string) TEST_SRC_DIR + "/D4-marshaller/little-endian";
#endif

using namespace std;
using namespace libdap;
using namespace CppUnit;

class D4MarshallerTest: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE (D4MarshallerTest);

    CPPUNIT_TEST (test_cmp);
    CPPUNIT_TEST (test_scalars);
    CPPUNIT_TEST (test_real_scalars);
    CPPUNIT_TEST (test_str);
    CPPUNIT_TEST (test_opaque);
    CPPUNIT_TEST (test_vector);

    CPPUNIT_TEST_SUITE_END( );

    /**
     * Compare the contents of a file with a memory buffer
     */
    bool cmp(const char *buf, unsigned int len, string file)
    {
        fstream in;
        in.open(file.c_str(), fstream::binary | fstream::in);
        if (!in) throw Error("Could not open file: " + file);

        vector<char> fbuf(len);
        in.read(&fbuf[0], len);
        if (!in) {
            ostringstream oss("Could not read ");
            oss << len << " bytes from file.";
            throw Error(oss.str());
        }

        for (unsigned int i = 0; i < len; ++i)
            if (buf[i] != fbuf[i]) {
                DBG(cerr << "Response differs from baseline at byte " << i << endl);
                DBG(
                    cerr << "Expected: " << setfill('0') << setw(2) << hex << (unsigned int )fbuf[i] << "; got: "
                        << (unsigned int )buf[i] << dec << endl);
                return false;
            }

        return true;
    }

    void write_binary_file(const char *buf, int len, string file)
    {
        fstream out;
        out.open(file.c_str(), fstream::binary | fstream::out);
        if (!out) throw Error("Could not open file: " + file);
        out.write(buf, len);
    }

public:
    D4MarshallerTest()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    void test_cmp()
    {
        char buf[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
        DBG(cerr << "Path: " << path << endl);
        CPPUNIT_ASSERT(cmp(buf, 16, path + "/test_cmp.dat"));
    }

    void test_scalars()
    {
        ostringstream oss;
        // computes checksums and writes data
        try {
            D4StreamMarshaller dsm(oss);

            dsm.reset_checksum();

            dsm.put_byte(17);
            dsm.put_checksum();
            DBG(cerr << "test_scalars: checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            dsm.put_int16(17);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            dsm.put_int32(17);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            dsm.put_int64(17);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            dsm.put_uint16(17);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            dsm.put_uint32(17);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            dsm.put_uint64(17);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), path + "/test_scalars_1_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), path + "/test_scalars_1_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_real_scalars()
    {
        ostringstream oss;
        // computes checksums and writes data
        try {
            D4StreamMarshaller dsm(oss);

            dsm.reset_checksum();

            dsm.put_float32(17);
            dsm.put_checksum();
            DBG(cerr << "test_real_scalars: checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            dsm.put_float64(17);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), path + "/test_scalars_2_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), path + "/test_scalars_2_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_str()
    {
        ostringstream oss;
        try {
            D4StreamMarshaller dsm(oss);

            dsm.reset_checksum();

            dsm.put_str("This is a test string with 40 characters");
            dsm.put_checksum();
            DBG(cerr << "test_str: checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            // 37 chars --> 0x25 --> 0x25 as a 128-bit varint
            dsm.put_url("http://www.opendap.org/lame/unit/test");
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            // True these are not really scalars...
            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), path + "/test_scalars_3_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), path + "/test_scalars_3_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_opaque()
    {
        ostringstream oss;
        try {
            D4StreamMarshaller dsm(oss);
            vector<unsigned char> buf(32768);
            for (int i = 0; i < 32768; ++i)
                buf[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_opaque_dap4(reinterpret_cast<char*>(&buf[0]), 32768);
            dsm.put_checksum();
            DBG(cerr << "test_opaque: checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), path + "/test_opaque_1_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), path + "/test_opaque_1_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_vector()
    {
        ostringstream oss;
        try {
            D4StreamMarshaller dsm(oss);
            vector<unsigned char> buf1(32768);
            for (int i = 0; i < 32768; ++i)
                buf1[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_vector(reinterpret_cast<char*>(&buf1[0]), 32768);
            dsm.put_checksum();
            DBG(cerr << "test_vector: checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            vector<dods_int32> buf2(32768);
            for (int i = 0; i < 32768; ++i)
                buf2[i] = i % (1 << 9);

            dsm.put_vector(reinterpret_cast<char*>(&buf2[0]), 32768, sizeof(dods_int32));
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            vector<dods_float64> buf3(32768);
            for (int i = 0; i < 32768; ++i)
                buf3[i] = i % (1 << 9);

            dsm.put_vector_float64(reinterpret_cast<char*>(&buf3[0]), 32768);
            dsm.put_checksum();
            DBG(cerr << "checksum: " << dsm.get_checksum() << endl);

            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), path + "/test_vector_1_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), path + "/test_vector_1_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }
#if 0
    void test_varying_vector() {
        ostringstream oss;
        try {
            D4StreamMarshaller dsm(oss);
            vector<unsigned char> buf1(32768);
            for (int i = 0; i < 32768; ++i)
            buf1[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_varying_vector(reinterpret_cast<char*>(&buf1[0]), 32768);
            dsm.put_checksum();
            DBG(cerr << "test_varying_vector: first checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            vector<dods_int32> buf2(32768);
            for (int i = 0; i < 32768; ++i)
            buf2[i] = i % (1 << 9);

            dsm.put_varying_vector(reinterpret_cast<char*>(&buf2[0]), 32768, sizeof(dods_int32));
            dsm.put_checksum();
            DBG(cerr << "second checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            vector<dods_float64> buf3(32768);
            for (int i = 0; i < 32768; ++i)
            buf3[i] = i % (1 << 9);

            dsm.put_varying_vector(reinterpret_cast<char*>(&buf3[0]), 32768, sizeof(dods_float64), dods_float64_c);
            dsm.put_checksum();
            DBG(cerr << "third checksum: " << dsm.get_checksum() << endl);

            if (write_baselines) write_binary_file(oss.str().data(), oss.str().length(), path + "/test_vector_2_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), path + "/test_vector_2_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }
#endif
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4MarshallerTest);

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dwh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = true;  // debug is a static global
            break;
        case 'w':
            write_baselines = true;
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: D4MarshallerTest has the following tests:" << endl;
            const std::vector<Test*> &tests = D4MarshallerTest::suite()->getTests();
            unsigned int prefix_len = D4MarshallerTest::suite()->getName().append("::").length();
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
            test = D4MarshallerTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
