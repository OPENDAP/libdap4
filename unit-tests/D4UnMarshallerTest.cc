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
#include <iomanip>
#include <cstring>

#include "D4StreamUnMarshaller.h"

#include "Type.h"

#include "GetOpt.h"
#include "debug.h"
#include "test_config.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

#if WORDS_BIGENDIAN
const static string path = string(TEST_SRC_DIR) + "/D4-marshaller/big-endian";
#else
const static string path = string(TEST_SRC_DIR) + "/D4-marshaller/little-endian";
#endif

using namespace std;
using namespace libdap;
using namespace CppUnit;

class D4UnMarshallerTest: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE (D4UnMarshallerTest);

    CPPUNIT_TEST (test_scalars);
    CPPUNIT_TEST (test_real_scalars);
    CPPUNIT_TEST (test_str);
    CPPUNIT_TEST (test_opaque);
    CPPUNIT_TEST (test_vector);

    CPPUNIT_TEST_SUITE_END( );

    static inline bool is_host_big_endian()
    {
#ifdef COMPUTE_ENDIAN_AT_RUNTIME

        dods_int16 i = 0x0100;
        char *c = reinterpret_cast<char*>(&i);
        return *c;

#else

#ifdef __BIG_ENDIAN__
        return true;
#else
        return false;
#endif

#endif
    }

public:
    D4UnMarshallerTest()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    void test_scalars()
    {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            string file = path + "/test_scalars_1_bin.dat";
            DBG(cerr << "file: " << file << endl);
            in.open(file.c_str(), fstream::binary | fstream::in);

            // Don't use is_host_big_endian() because these tests should
            // never 'twiddle bytes' They are always testing little to little
            // of big to big
            D4StreamUnMarshaller dsm(in, 0 /*is_host_big_endian()*/);

            dods_byte b;
            dsm.get_byte(b);
            CPPUNIT_ASSERT(b == 17);
            string ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            // Same checksum for both big- and little-endian
            CPPUNIT_ASSERT(ck == "b8b2cf7f");

            dods_int16 i1;
            dsm.get_int16(i1);
            CPPUNIT_ASSERT(i1 == 17);
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            // little-endian || big-endian checksum values
            CPPUNIT_ASSERT(ck == "120031ef" || ck == "2b69320d");

            dods_int32 i2;
            dsm.get_int32(i2);
            CPPUNIT_ASSERT(i2 == 17);
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "c9e1efe6" || ck == "4bf4ffee");

            dods_int64 i3;
            dsm.get_int64(i3);
            CPPUNIT_ASSERT(i3 == 17);
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d533eedc" || ck == "0f92ff9b");

            dods_uint16 ui1;
            dsm.get_uint16(ui1);
            CPPUNIT_ASSERT(ui1 == 17);
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "120031ef" || ck == "2b69320d");

            dods_uint32 ui2;
            dsm.get_uint32(ui2);
            CPPUNIT_ASSERT(ui2 == 17);
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "c9e1efe6" || ck == "4bf4ffee");

            dods_uint64 ui3;
            dsm.get_uint64(ui3);
            CPPUNIT_ASSERT(ui3 == 17);
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d533eedc" || ck == "0f92ff9b");
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
        catch (istream::failure &e) {
            cerr << "File error: " << e.what() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_real_scalars()
    {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            string file = path + "/test_scalars_2_bin.dat";
            in.open(file.c_str(), fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, 0);

            dods_float32 r1;
            dsm.get_float32(r1);
            CPPUNIT_ASSERT(r1 == 17.0);
            string ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d3c5bc59" || ck == "edcaaa7c");

            dods_float64 r2;
            dsm.get_float64(r2);
            CPPUNIT_ASSERT(r2 == 17.0);
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d5a3994b" || ck == "42abb362");
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
        catch (istream::failure &e) {
            cerr << "File error: " << e.what() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_str()
    {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            string file = path + "/test_scalars_3_bin.dat";
            in.open(file.c_str(), fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, 0);

            string s;
            dsm.get_str(s);
            CPPUNIT_ASSERT(s == "This is a test string with 40 characters");
            string ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "af117544");

            string u;
            dsm.get_url(u);
            CPPUNIT_ASSERT(u == "http://www.opendap.org/lame/unit/test");
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "41e10081");
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
        catch (istream::failure &e) {
            cerr << "File error: " << e.what() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_opaque()
    {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            string file = path + "/test_opaque_1_bin.dat";
            in.open(file.c_str(), fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, 0);

            char *buf2;
            int64_t len;
            dsm.get_opaque_dap4(&buf2, len);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf2[i] == i % (1 << 7));
            string ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "199ad7f5");

            delete buf2;
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
        catch (istream::failure &e) {
            cerr << "File error: " << e.what() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_vector()
    {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            string file = path + "/test_vector_1_bin.dat";
            in.open(file.c_str(), fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, 0);

            vector<unsigned char> buf1(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf1[0]), 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf1[i] == i % (1 << 7));
            string ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "199ad7f5" || ck == "199ad7f5");

            vector<dods_int32> buf2(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf2[0]), 32768, sizeof(dods_int32));
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf2[i] == i % (1 << 9));
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "5c1bf29f" || ck == "8efd2d3d");

            vector<dods_float64> buf3(32768);
            dsm.get_vector_float64(reinterpret_cast<char*>(&buf3[0]), 32768);
            for (int i = 0; i < 32768; ++i) {
                if (buf3[i] != i % (1 << 9)) cerr << "buf3[" << i << "]: " << buf3[i] << endl;
                CPPUNIT_ASSERT(buf3[i] == i % (1 << 9));
            }
            ck = dsm.get_checksum_str();
            DBG(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "aafc2a91" || ck == "7bdf9931");
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
        catch (istream::failure &e) {
            cerr << "File error: " << e.what() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION (D4UnMarshallerTest);

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = true;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: D4UnMarshallerTest has the following tests:" << endl;
            const std::vector<Test*> &tests = D4UnMarshallerTest::suite()->getTests();
            unsigned int prefix_len = D4UnMarshallerTest::suite()->getName().append("::").length();
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
            test = D4UnMarshallerTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
