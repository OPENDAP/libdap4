// -*- mode: c++; c-basic-offset:4 -*-

//
// Created by slloyd on 3/6/26.
//

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <experimental/filesystem>
#include <cstring>

// #define DODS_DEBUG
#include "util.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace libdap;
using namespace CppUnit;
using namespace std;

class SegmentReadWriteT : public TestFixture {

    public:
        SegmentReadWriteT() = default;
        ~SegmentReadWriteT() = default;

        void setUp() override {}

        void tearDown() override {
#if 0
            dirPath = "/tmp/srwt";
            std::error_code ec; // Use an error code to prevent exceptions
            // remove_all attempts to delete the contents and the directory itself recursively
            if (fs::remove_all(dirPath, ec) == static_cast<std::uintmax_t>(-1) || ec) {
                std::cerr << "Error deleting directory " << dirPath << ": " << ec.message() << std::endl;
            } else {
                std::cout << "Successfully deleted directory: " << dirPath << std::endl;
            }
#endif
        }

        bool gen_large_test_file(uint64_t size, string filepath) {
            // use dd to gen and write an extremely large file here
            //  number should be size in bytes the file should be
            uint64_t bufferSize = 10'000'000;
            uint64_t count;
            if (size > bufferSize){
                count = size / bufferSize;
            }
            else {
                bufferSize = size;
                count = 1;
            }

            auto command = "dd if=/dev/urandom of=" + filepath + " bs=10M count=" + to_string(count);

            std::cout << "Running command: " << command << std::endl;

            // Execute the dd command
            int return_code = std::system(command.c_str());

            if (return_code == 0) {
                std::cout << "dd command executed successfully." << std::endl;
            } else {
                std::cerr << "dd command failed with return code: " << return_code << std::endl;
            }

            return return_code;
        }

        void write_buffer_to_file(char *buffer, uint64_t buffer_length, string filepath) {
            // Open the file in binary mode
            std::ofstream outfile(filepath, std::ios::out | std::ios::binary);

            if (!outfile) {
                DBG(cerr << "Error opening file" << endl);
                return;
            }

            // Write the buffer to the file
            // The write function expects a const char* and the number of bytes to write
            outfile.write(buffer, buffer_length);

            // Always close the file when done
            outfile.close();

            DBG(cerr << "Buffer written to output.bin as binary data." << endl);
        }

        void seg_read_test_0() {
            // setup test file
            uint64_t bytes = 10;
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";

            // call seg_read on file
            vector<char> buff(bytes);
            int v = 0;
            for (auto &c : buff){
                c = (char) v;
                v++;
            }
            write_buffer_to_file(buff.data(), bytes, filepath);
            vector<char> buff2(bytes);
            std::filebuf fb;
            if (fb.open(filepath, std::ios::in)) {
                std::istream is(&fb);
                segmented_read(is, buff2.data(), bytes);
                fb.close();
            }
            CPPUNIT_ASSERT_MESSAGE("buffer should be 0", buff2[0] == 0);
            CPPUNIT_ASSERT_MESSAGE("buffer should be 9", buff2[9] == 9);

#if 0
            // take seg_read data and write to file
            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            write_buffer_to_file(buff2.data(), bytes, filepath2);

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            std::cout << "Running command: " << command << std::endl;
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
#endif
        } // end seg_read_test_1()

        void seg_write_test_0() {
            // setup test file
            uint64_t bytes = 10;
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";

            // call seg_read on file
            vector<char> buff(bytes);
            int v = 0;
            for (auto &c : buff){
                c = (char) v;
                v++;
            }

            std::filebuf fb;
            if (fb.open(filepath, std::ios::out)) {
                std::ostream os(&fb);
                segmented_write(os, buff.data(), bytes);
                fb.close();
            }

            if (fb.open(filepath, std::ios::in)) {
                std::istream is(&fb);
                char c;
                is.read(&c, 1);
                CPPUNIT_ASSERT_MESSAGE("char should be 0", c == 0);
                is.seekg(9);
                is.read(&c, 1);
                CPPUNIT_ASSERT_MESSAGE("char should be 0", c == 9);
                fb.close();
            }


        } // end seg_write_test_1()

        void seg_read_test_1() {
            // setup test file
            // uint64_t bytes = std::pow(2, 32);
            uint64_t bytes = 1000;
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";
            int code = gen_large_test_file(bytes, filepath);
            if (code != 0) {
                CPPUNIT_FAIL("dd command failed with return code: " + to_string(code));
            }

            // call seg_read on file
            vector<char> buff(bytes);
            std::filebuf fb;
            if (fb.open(filepath, std::ios::in)) {
                std::istream is(&fb);
                segmented_read(is, buff.data(), bytes);
                fb.close();
            }

            // take seg_read data and write to file
            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            write_buffer_to_file(buff.data(), bytes, filepath2);

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            std::cout << "Running command: " << command << std::endl;
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
        } // end seg_read_test_1()

        CPPUNIT_TEST_SUITE(SegmentReadWriteT);

        CPPUNIT_TEST(seg_read_test_0); // seg_read test 2^32
        CPPUNIT_TEST(seg_write_test_0);
        // CPPUNIT_TEST(...);             // seg_read test 2^31
        // CPPUNIT_TEST(...);             // seg_read test (2^32)-1 byte
        // CPPUNIT_TEST(...);             // seg_write test 2^32
        // CPPUNIT_TEST(...);             // seg_write test 2^31
        // CPPUNIT_TEST(...);             // seg_write test (2^32)-1 byte1`

        CPPUNIT_TEST_SUITE_END();
    };

    CPPUNIT_TEST_SUITE_REGISTRATION(SegmentReadWriteT);

    int main(int argc, char *argv[]) { return run_tests<SegmentReadWriteT>(argc, argv) ? 0 : 1; }