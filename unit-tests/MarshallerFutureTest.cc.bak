// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2025 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <sys/fcntl.h>

#include "../MarshallerFuture.h"
#include "run_tests_cppunit.h"

using namespace libdap;

class MarshallerThreadSharedPtrTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MarshallerThreadSharedPtrTest);

    CPPUNIT_TEST(test_write_to_stream);
    CPPUNIT_TEST(test_write_to_stream_shared_ptr);

    CPPUNIT_TEST(test_write_part_to_stream);
    CPPUNIT_TEST(test_write_part_to_stream_shared_ptr);

    CPPUNIT_TEST(test_write_to_file_descriptor);
    CPPUNIT_TEST(test_write_to_file_descriptor_shared_ptr);

    CPPUNIT_TEST_SUITE_END();

public:
    static void test_write_to_stream() {
        std::ostringstream oss;

        std::string test_data = "Shared pointer test";

        // This block is used to force a call to the future get() method
        {
            MarshallerThread mt;
            mt.start_thread(oss, test_data.data(), test_data.size());
            std::cerr << "Bytes written: " << mt.get_ostream_future() << "\n";
            std::cerr << "The data written: " << oss.str() << "\n";
        }

        CPPUNIT_ASSERT_EQUAL(test_data, oss.str());
    }

    static void test_write_to_stream_shared_ptr() {
        std::ostringstream oss;

        std::string test_data = "Shared pointer test";
        std::size_t len = test_data.size();
        std::shared_ptr<char> buf(new char[len], std::default_delete<char[]>());

        std::memcpy(buf.get(), test_data.data(), test_data.size());

        // This block is used to force a call to the future get() method
        {
            MarshallerThread mt;
            mt.start_thread(oss, buf, test_data.size());
            std::cerr << "Bytes written: " << mt.get_ostream_future() << "\n";
            std::cerr << "The data written: " << oss.str() << "\n";
        }

        CPPUNIT_ASSERT_EQUAL(test_data, oss.str());
    }

    static void test_write_part_to_stream() {
        std::ostringstream oss;

        std::string test_data = "1234DATA";

        {
            MarshallerThread mt;
            mt.start_thread_part(oss, test_data.data(), test_data.size());
            std::cerr << "Bytes written: " << mt.get_ostream_future() << "\n";
            std::cerr << "The data written: " << oss.str() << "\n";
        }

        CPPUNIT_ASSERT_EQUAL(std::string("DATA"), oss.str());
    }

    static void test_write_part_to_stream_shared_ptr() {
        std::ostringstream oss;

        std::string test_data = "1234DATA";
        std::size_t len = test_data.size();
        std::shared_ptr<char> buf(new char[len], std::default_delete<char[]>());
        std::memcpy(buf.get(), test_data.data(), len);

        {
            MarshallerThread mt;
            mt.start_thread_part(oss, buf, test_data.size());
            std::cerr << "Bytes written: " << mt.get_ostream_future() << "\n";
            std::cerr << "The data written: " << oss.str() << "\n";
        }

        CPPUNIT_ASSERT_EQUAL(std::string("DATA"), oss.str());
    }

    static void test_write_to_file_descriptor() {
        std::string test_data = "Shared FD test";

        const char *filename = "/tmp/marshaller_test_shared_ptr.txt";
        int fd = creat(filename, 0644);
        CPPUNIT_ASSERT(fd != -1);

        {
            MarshallerThread mt;
            mt.start_thread(fd, test_data.data(), test_data.size());
            std::cerr << "Bytes written: " << mt.get_fp_future() << "\n";
        }

        close(fd);

        std::ifstream ifs(filename);
        std::stringstream content;
        content << ifs.rdbuf();
        ifs.close();

        CPPUNIT_ASSERT_EQUAL(test_data, content.str());
    }

    static void test_write_to_file_descriptor_shared_ptr() {
        std::string test_data = "Shared FD test";
        std::size_t len = test_data.size();
        std::shared_ptr<char> buf(new char[len], std::default_delete<char[]>());
        std::memcpy(buf.get(), test_data.data(), len);

        const char *filename = "/tmp/marshaller_test_shared_ptr.txt";
        int fd = creat(filename, 0644);
        CPPUNIT_ASSERT(fd != -1);

        {
            MarshallerThread mt;
            mt.start_thread(fd, buf, test_data.size());
            std::cerr << "Bytes written: " << mt.get_fp_future() << "\n";
        }

        close(fd);

        std::ifstream ifs(filename);
        std::stringstream content;
        content << ifs.rdbuf();
        ifs.close();

        CPPUNIT_ASSERT_EQUAL(test_data, content.str());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MarshallerThreadSharedPtrTest);

int main(int argc, char *argv[]) { return run_tests<MarshallerThreadSharedPtrTest>(argc, argv) ? 0 : 1; }
