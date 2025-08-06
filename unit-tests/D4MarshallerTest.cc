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

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdint.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "D4StreamMarshaller.h"

#include "debug.h"
#include "run_tests_cppunit.h"
#include "test_config.h"

#include <sys/time.h>

static bool write_baselines = false;

#define prolog string("D4MarshallerTest::").append(__func__).append("() - ")

#if WORDS_BIGENDIAN
const static string path = (string)TEST_SRC_DIR + "/D4-marshaller/big-endian";
#else
const static string path = (string)TEST_SRC_DIR + "/D4-marshaller/little-endian";
#endif

using namespace std;
using namespace libdap;
using namespace CppUnit;

class D4MarshallerTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(D4MarshallerTest);

    CPPUNIT_TEST(test_cmp);
    CPPUNIT_TEST(test_scalars_with_checksums);
    CPPUNIT_TEST(test_scalars_no_checksums);
    CPPUNIT_TEST(test_real_scalars_no_checksums);
    CPPUNIT_TEST(test_real_scalars_with_checksums);
    CPPUNIT_TEST(test_str_no_checksums);
    CPPUNIT_TEST(test_str_with_checksums);
    CPPUNIT_TEST(test_opaque_no_checksums);
    CPPUNIT_TEST(test_opaque_with_checksums);
    CPPUNIT_TEST(test_vector_no_checksums);
    CPPUNIT_TEST(test_vector_with_checksums);
    CPPUNIT_TEST(checksum_speed_test);

    CPPUNIT_TEST_SUITE_END();

    /**
     * Compare the contents of a file with a memory buffer
     */
    bool cmp(const char *buf, unsigned long long len, string file) {

        DBG(cerr << prolog << "buf: " << ((void *)buf) << ", len: " << len << ", baseline_file:" << file << endl);
        fstream in;
        in.open(file.c_str(), fstream::binary | fstream::in);
        if (!in)
            throw Error("Could not open file: " + file);

        vector<char> fbuf(len);
        in.read(fbuf.data(), len);
        if (!in) {
            ostringstream oss("Could not read ");
            oss << len << " bytes from file.";
            throw Error(oss.str());
        }

        for (unsigned long long i = 0; i < len; ++i)
            if (buf[i] != fbuf[i]) {
                cerr << prolog << "Response differs from baseline at byte " << i << endl;
                cerr << prolog << "Baseline File[" << i << "]: 0x";
                cerr << setfill('0') << setw(2) << hex << static_cast<unsigned int>(fbuf[i]) << "; test result: 0x"
                     << static_cast<unsigned int>(buf[i]) << dec << endl;

                return false;
            }

        return true;
    }

    void write_binary_file(const char *buf, int len, string file) {
        fstream out;
        out.open(file.c_str(), fstream::binary | fstream::out);
        if (!out)
            throw Error("Could not open file: " + file);
        out.write(buf, len);
    }

    void get_time_of_day(timeval &time_val) {
        if (gettimeofday(&time_val, nullptr) != 0) {
            const char *c_err = strerror(errno);
            string errno_msg = c_err != nullptr ? c_err : "unknown error";
            string msg = prolog + "ERROR The gettimeofday() function failed. errno_msg: " + errno_msg + "\n";
            cerr << msg;
            throw Error(msg);
        }
    }
    unsigned long int get_elapsed_us(const timeval &start, const timeval &stop) const {
        return get_time_us(stop) - get_time_us(start);
    }

    unsigned long int get_time_us(const timeval &time_val) const {
        return time_val.tv_sec * 1000 * 1000 + time_val.tv_usec;
    }

public:
    D4MarshallerTest() {}

    void setUp() { DBG(cerr << "\n"); }

    void tearDown() {}

    void test_cmp() {
        char buf[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        DBG(cerr << prolog << "Path: " << path << endl);
        CPPUNIT_ASSERT(cmp(buf, 16, path + "/test_cmp.dat"));
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void checksum_speed_test() {

        // long element_count = 2100000007;
        long element_count = 2000000;
        long elapsed_with;
        long elapsed_without;

        DBG(cerr << endl);
        DBG(cerr << prolog << "Timing checksums vs no checksums " << endl);
        DBG(cerr << prolog << "Testing vectors with " << element_count << " elements." << endl);

        {
            timeval start{};
            timeval stop{};
            string baseline{};

            get_time_of_day(start);
            test_vector(true, element_count, baseline);
            get_time_of_day(stop);
            elapsed_with = get_elapsed_us(start, stop);
            DBG(cerr << prolog << "With Checksums: " << std::setw(10) << elapsed_with << " µs" << endl);
        }

        {
            timeval start{};
            timeval stop{};
            string baseline{};

            get_time_of_day(start);
            test_vector(false, element_count, baseline);
            get_time_of_day(stop);
            elapsed_without = get_elapsed_us(start, stop);
            DBG(cerr << prolog << "  No Checksums: " << std::setw(10) << elapsed_without << " µs" << endl);
        }
        double ratio = 100 * (double)elapsed_with / (double)elapsed_without;
        DBG(cerr << prolog << "             Ratio: " << ratio << endl);
    }
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void test_scalars_with_checksums() { test_scalars(true, path + "/test_scalars_1_bin.dat"); }

    void test_scalars_no_checksums() { test_scalars(false, path + "/test_scalars_1_bin_no_checksums.dat"); }

    void test_scalars(const bool checksums, const string &baseline_file) {
        DBG(cerr << prolog << "checksums: " << (checksums ? "true" : "false") << ",  baseline_file: " << baseline_file
                 << endl);
        ostringstream oss;
        // computes checksums and writes data
        try {
            D4StreamMarshaller dsm(oss, true, checksums);

            dsm.reset_checksum();

            dsm.put_byte(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_byte() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }

            dsm.put_int16(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_int16() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            dsm.put_int32(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_int32() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            dsm.put_int64(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_int64() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            dsm.put_uint16(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_uint16() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            dsm.put_uint32(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_uint32() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            dsm.put_uint64(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_uint64() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), baseline_file);
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), baseline_file));
        } catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void test_real_scalars_no_checksums() { test_real_scalars(false, path + "/test_scalars_2_no_checksums_bin.dat"); }

    void test_real_scalars_with_checksums() { test_real_scalars(true, path + "/test_scalars_2_bin.dat"); }

    void test_real_scalars(const bool checksums, const string &baseline_file) {
        DBG(cerr << prolog << "checksums: " << (checksums ? "true" : "false") << ",  baseline_file: " << baseline_file
                 << endl);
        ostringstream oss;
        // computes checksums and writes data
        try {
            D4StreamMarshaller dsm(oss, true, checksums);

            dsm.reset_checksum();

            dsm.put_float32(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_float32() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }

            dsm.put_float64(17);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_float64() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }

            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), baseline_file);
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), baseline_file));
        } catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void test_str_no_checksums() { test_str(false, path + "/test_scalars_3_no_checksums_bin.dat"); }

    void test_str_with_checksums() { test_str(true, path + "/test_scalars_3_bin.dat"); }

    void test_str(const bool checksums, const string &baseline_file) {
        DBG(cerr << prolog << "checksums: " << (checksums ? "true" : "false") << ",  baseline_file: " << baseline_file
                 << endl);
        ostringstream oss;
        try {
            D4StreamMarshaller dsm(oss, true, checksums);

            dsm.reset_checksum();

            dsm.put_str("This is a test string with 40 characters");
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_str() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }

            // 37 chars --> 0x25 --> 0x25 as a 128-bit varint
            dsm.put_url("http://www.opendap.org/lame/unit/test");
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_url() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            // True these are not really scalars...
            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), baseline_file);
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), baseline_file));
        } catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void test_opaque_no_checksums() { test_opaque(false, path + "/test_opaque_1_no_checksums_bin.dat"); }
    void test_opaque_with_checksums() { test_opaque(true, path + "/test_opaque_1_bin.dat"); }
    void test_opaque(const bool checksums, const string &baseline_file) {
        DBG(cerr << prolog << "checksums: " << (checksums ? "true" : "false") << ",  baseline_file: " << baseline_file
                 << endl);
        ostringstream oss;
        try {
            D4StreamMarshaller dsm(oss, true, checksums);
            vector<unsigned char> buf(32768);
            for (int i = 0; i < 32768; ++i)
                buf[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_opaque_dap4(reinterpret_cast<char *>(buf.data()), 32768);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_opaque_dap4() checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }
            if (write_baselines)
                write_binary_file(oss.str().data(), oss.str().length(), baseline_file);
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), baseline_file));
        } catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void test_vector_no_checksums() { test_vector(false, 32768, path + "/test_vector_1_no_checksums_bin.dat"); }

    void test_vector_with_checksums() { test_vector(true, 32768, path + "/test_vector_1_bin.dat"); }

    void test_vector(const bool checksums, const long num_elements, const string &baseline_file) {
        DBG(cerr << prolog << "checksums: " << (checksums ? "true" : "false") << ",  baseline_file: " << baseline_file
                 << endl);
        ostringstream oss;
        try {
            D4StreamMarshaller dsm(oss, true, checksums);
            vector<unsigned char> buf1(num_elements);
            for (int i = 0; i < num_elements; ++i)
                buf1[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_vector(reinterpret_cast<char *>(buf1.data()), num_elements);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_vector(unsigned char) checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }

            vector<dods_int32> buf2(num_elements);
            for (int i = 0; i < num_elements; ++i)
                buf2[i] = i % (1 << 9);

            dsm.put_vector(reinterpret_cast<char *>(buf2.data()), num_elements, sizeof(dods_int32));
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_vector(int32) checksum: " << dsm.get_checksum() << endl);
                dsm.reset_checksum();
            }

            vector<dods_float64> buf3(num_elements);
            for (int i = 0; i < num_elements; ++i)
                buf3[i] = i % (1 << 9);

            dsm.put_vector_float64(reinterpret_cast<char *>(buf3.data()), num_elements);
            if (checksums) {
                dsm.put_checksum();
                DBG(cerr << prolog << "dsm.put_vector_float64() checksum: " << dsm.get_checksum() << endl);
            }

            if (!baseline_file.empty()) {

                if (write_baselines) {
                    write_binary_file(oss.str().data(), oss.str().length(), baseline_file);
                }

                const auto data = oss.str();
                const auto str_len = data.length();
                CPPUNIT_ASSERT(cmp(data.c_str(), str_len, baseline_file));
            }
        } catch (Error &e) {
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

            dsm.put_varying_vector(reinterpret_cast<char*>(buf1.data()), 32768);
            dsm.put_checksum();
            DBG(cerr << prolog << "test_varying_vector: first checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            vector<dods_int32> buf2(32768);
            for (int i = 0; i < 32768; ++i)
            buf2[i] = i % (1 << 9);

            dsm.put_varying_vector(reinterpret_cast<char*>(buf2.data()), 32768, sizeof(dods_int32));
            dsm.put_checksum();
            DBG(cerr << prolog << "second checksum: " << dsm.get_checksum() << endl);
            dsm.reset_checksum();

            vector<dods_float64> buf3(32768);
            for (int i = 0; i < 32768; ++i)
            buf3[i] = i % (1 << 9);

            dsm.put_varying_vector(reinterpret_cast<char*>(buf3.data()), 32768, sizeof(dods_float64), dods_float64_c);
            dsm.put_checksum();
            DBG(cerr << prolog << "third checksum: " << dsm.get_checksum() << endl);

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

CPPUNIT_TEST_SUITE_REGISTRATION(D4MarshallerTest);

int main(int argc, char *argv[]) { return run_tests<D4MarshallerTest>(argc, argv) ? 0 : 1; }
