// -*- mode: c++; c-basic-offset:4 -*-

//
// Created by slloyd on 3/6/26.
//

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h> // POSIX header
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

        void setUp() override {
            string path_str = "/tmp/srwt";
            // Permissions: read/write/execute for owner (0700)
            // You can use S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH for 0755
            mode_t mode = 0700;

            if (mkdir(path_str.c_str(), mode) == 0) {
                DBG(cerr << "Directory '" << path_str << "' created successfully." << endl);
            }
            else{
                DBG(cerr << "Unable to create " << path_str << endl);
            }
        } // end setUp()

        void tearDown() override {
            auto command = "rm -rf /tmp/srwt";

            int return_code = system(command);
            if (return_code == 0) {
                DBG(cerr << "rm command executed successfully." << endl);
            } else {
                DBG(cerr << "rm command failed with return code: " << return_code << endl);
            }
        } // end tearDown()

        bool gen_large_test_file(uint64_t size, string filepath) {
            // use dd to gen and write an extremely large file here
            auto command = "dd if=/dev/urandom of=" + filepath + " bs=" + to_string(size) + "c iflag=fullblock count=1 status=none";

            DBG(cerr << "Running command: " << command << endl);

            // Execute the dd command
            int return_code = system(command.c_str());

            if (return_code == 0) {
                DBG(cerr << "dd command executed successfully." << endl);
            } else {
                DBG(cerr << "dd command failed with return code: " << return_code << endl);
            }

            return return_code;
        } // end gen_large_test_file(...)

        void write_buffer_to_file(char *buffer, uint64_t buffer_length, string filepath) {
            // Open the file in binary mode
            std::ofstream outfile(filepath, std::ios::out | std::ios::binary);

            if (!outfile) {
                DBG(cerr << "Error opening file" << endl);
                throw std::system_error(errno, generic_category(), "Failed to open " + filepath);
            }

            // Write the buffer to the file
            // The write function expects a const char* and the number of bytes to write
            outfile.write(buffer, buffer_length);

            // Always close the file when done
            outfile.close();

            DBG(cerr << "Buffer written to output.bin as binary data." << endl);
        } // end write_buffer_to_file(...)

        vector<char> read_file_to_buffer(string filepath){
            ifstream file(filepath, ios::binary); // Open in binary mode for raw bytes
            if (!file) {
                DBG(cerr << "Error: Could not open the file " << filepath << endl);
                return {}; // Return an empty vector on failure
            }

            // Read the entire file content into the vector using iterators
            vector<char> file_contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

            return file_contents;

        } // end read_file_to_buffer(...)

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
        } // end seg_read_test_0()

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


        } // end seg_write_test_0()

        void seg_read_test_1() {
            // setup test file
            uint64_t bytes = std::pow(2, 31);
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
            DBG(cerr << "Running command: " << command << endl);
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
        } // end seg_read_test_1()

        void seg_read_test_2() {
            // setup test file
            uint64_t bytes = std::pow(2, 31) - 1;
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
            DBG(cerr << "Running command: " << command << endl);
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
        } // end seg_read_test_2()

        void seg_read_test_3() {
            // setup test file
            uint64_t bytes = std::pow(2, 31) + 1;
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
            DBG(cerr << "Running command: " << command << endl);
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
        } // end seg_read_test_3()

        void seg_write_test_1() {
            uint64_t bytes = std::pow(2, 31);
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";
            int code = gen_large_test_file(bytes, filepath);
            if (code != 0) {
                CPPUNIT_FAIL("dd command failed with return code: " + to_string(code));
            }

            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            vector<char> buffer = read_file_to_buffer(filepath);
            filebuf fb;
            if (fb.open(filepath2, ios::out)) {
                ostream os(&fb);
                segmented_write(os, buffer.data(), bytes);
                fb.close();
            }

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            DBG(cerr << "Running command: " << command << endl);
            int return_code = system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);

        } // end seg_write_test_1()

        void seg_write_test_2() {
            uint64_t bytes = std::pow(2, 31) - 1;
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";
            int code = gen_large_test_file(bytes, filepath);
            if (code != 0) {
                CPPUNIT_FAIL("dd command failed with return code: " + to_string(code));
            }

            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            vector<char> buffer = read_file_to_buffer(filepath);
            filebuf fb;
            if (fb.open(filepath2, ios::out)) {
                ostream os(&fb);
                segmented_write(os, buffer.data(), bytes);
                fb.close();
            }

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            DBG(cerr << "Running command: " << command << endl);
            int return_code = system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);

        } // end seg_write_test_2()

        void seg_write_test_3() {
            uint64_t bytes = std::pow(2, 31) + 1;
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";
            int code = gen_large_test_file(bytes, filepath);
            if (code != 0) {
                CPPUNIT_FAIL("dd command failed with return code: " + to_string(code));
            }

            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            vector<char> buffer = read_file_to_buffer(filepath);
            filebuf fb;
            if (fb.open(filepath2, ios::out)) {
                ostream os(&fb);
                segmented_write(os, buffer.data(), bytes);
                fb.close();
            }

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            DBG(cerr << "Running command: " << command << endl);
            int return_code = system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);

        } // end seg_write_test_3()

        void seg_read_test_4() {
            // setup test file
            uint64_t bytes = std::pow(2, 32);
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
            DBG(cerr << "Running command: " << command << endl);
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
        } // end seg_read_test_4()

        void seg_read_test_5() {
            // setup test file
            uint64_t bytes = std::pow(2, 32) - 1;
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
            DBG(cerr << "Running command: " << command << endl);
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
        } // end seg_read_test_5()

        void seg_read_test_6() {
            // setup test file
            uint64_t bytes = std::pow(2, 32) + 1;
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
            DBG(cerr << "Running command: " << command << endl);
            int return_code = std::system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);
        } // end seg_read_test_6()

        void seg_write_test_4() {
            uint64_t bytes = std::pow(2, 32);
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";
            int code = gen_large_test_file(bytes, filepath);
            if (code != 0) {
                CPPUNIT_FAIL("dd command failed with return code: " + to_string(code));
            }

            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            vector<char> buffer = read_file_to_buffer(filepath);
            filebuf fb;
            if (fb.open(filepath2, ios::out)) {
                ostream os(&fb);
                segmented_write(os, buffer.data(), bytes);
                fb.close();
            }

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            DBG(cerr << "Running command: " << command << endl);
            int return_code = system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);

        } // end seg_write_test_4()

        void seg_write_test_5() {
            uint64_t bytes = std::pow(2, 32) - 1;
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";
            int code = gen_large_test_file(bytes, filepath);
            if (code != 0) {
                CPPUNIT_FAIL("dd command failed with return code: " + to_string(code));
            }

            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            vector<char> buffer = read_file_to_buffer(filepath);
            filebuf fb;
            if (fb.open(filepath2, ios::out)) {
                ostream os(&fb);
                segmented_write(os, buffer.data(), bytes);
                fb.close();
            }

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            DBG(cerr << "Running command: " << command << endl);
            int return_code = system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);

        } // end seg_write_test_5()

        void seg_write_test_6() {
            uint64_t bytes = std::pow(2, 32) + 1;
            string filepath = "/tmp/srwt/segReadWriteTest1.bin";
            int code = gen_large_test_file(bytes, filepath);
            if (code != 0) {
                CPPUNIT_FAIL("dd command failed with return code: " + to_string(code));
            }

            string filepath2 = "/tmp/srwt/segReadWriteTest2.bin";
            vector<char> buffer = read_file_to_buffer(filepath);
            filebuf fb;
            if (fb.open(filepath2, ios::out)) {
                ostream os(&fb);
                segmented_write(os, buffer.data(), bytes);
                fb.close();
            }

            // assert (compare files)
            auto command = "cmp " + filepath + " " + filepath2;
            DBG(cerr << "Running command: " << command << endl);
            int return_code = system(command.c_str());
            CPPUNIT_ASSERT(return_code == 0);

        } // end seg_write_test_6()

        CPPUNIT_TEST_SUITE(SegmentReadWriteT);

        // Baby tests
        CPPUNIT_TEST(seg_read_test_0);
        CPPUNIT_TEST(seg_write_test_0);

        // 2GB tests
        CPPUNIT_TEST(seg_read_test_1);    // seg_read test 2^31
        CPPUNIT_TEST(seg_read_test_2);    // seg_read test 2^31 - 1
        CPPUNIT_TEST(seg_read_test_3);    // seg_read test 2^31 + 1

        CPPUNIT_TEST(seg_write_test_1);   // seg_write test 2^31
        CPPUNIT_TEST(seg_write_test_2);   // seg_write test 2^31 - 1
        CPPUNIT_TEST(seg_write_test_3);   // seg_write test 2^31 + 1

        // 4GB tests
        CPPUNIT_TEST(seg_read_test_4);    // seg_read test 2^32
        CPPUNIT_TEST(seg_read_test_5);    // seg_read test 2^32 - 1
        CPPUNIT_TEST(seg_read_test_6);    // seg_read test 2^32 + 1

        CPPUNIT_TEST(seg_write_test_4);   // seg_write test 2^32
        CPPUNIT_TEST(seg_write_test_5);   // seg_write test 2^32 - 1
        CPPUNIT_TEST(seg_write_test_6);   // seg_write test 2^32 + 1

        CPPUNIT_TEST_SUITE_END();
    };

    CPPUNIT_TEST_SUITE_REGISTRATION(SegmentReadWriteT);

    int main(int argc, char *argv[]) { return run_tests<SegmentReadWriteT>(argc, argv) ? 0 : 1; }