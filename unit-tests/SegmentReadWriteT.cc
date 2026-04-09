// -*- mode: c++; c-basic-offset:4 -*-

//
// Created by slloyd on 3/6/26.
//

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cstring>
#include <sys/stat.h> // POSIX header

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
        } else {
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
        auto command =
            "dd if=/dev/urandom of=" + filepath + " bs=" + to_string(size) + "c iflag=fullblock count=1 status=none";

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

    vector<char> read_file_to_buffer(string filepath) {
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
        for (auto &c : buff) {
            c = (char)v;
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
        for (auto &c : buff) {
            c = (char)v;
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

    CPPUNIT_TEST_SUITE(SegmentReadWriteT);

    // tests
    CPPUNIT_TEST(seg_read_test_0);
    CPPUNIT_TEST(seg_write_test_0);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SegmentReadWriteT);

int main(int argc, char *argv[]) { return run_tests<SegmentReadWriteT>(argc, argv) ? 0 : 1; }
