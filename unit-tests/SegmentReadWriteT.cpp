// -*- mode: c++; c-basic-offset:4 -*-

//
// Created by slloyd on 3/6/26.
//

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

// #define DODS_DEBUG
#include "util.h"

#include "run_tests_cppunit.h"
#include "test_config.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

class SegmentReadWriteT : public TestFixture {
    util utils;
};

public:
    SegmentReadWriteT : utils(util::instance()){} //<< probably won't work, revisit

    void setUp() override {}

    void teardown() override {}

    bool gen_large_test_file(uint64_t number, string filepath){
        //use dd to gen and write an extremely large file here
        // number should be size in bytes the file should be
        int bufferSize = 10000000;
        uint64_t count = number / bufferSize;

        const char* command = "dd if=/dev/urandom of="+ filepath +" bs=10M count="+ count;

        std::cout << "Running command: " << command << std::endl;

        // Execute the dd command
        int return_code = std::system(command);

        if (return_code == 0) {
            std::cout << "dd command executed successfully." << std::endl;
        } else {
            std::cerr << "dd command failed with return code: " << return_code << std::endl;
        }

        return return_code;
    }

    void file_cleanup(string filepath){
        // delete the large file that was generated
    }

    void write_buffer_to_file(char buffer, string filepath){
        std::streamsize buffer_length = std::strlen(buffer);

        // Open the file in binary mode
        std::ofstream outfile(filepath, std::ios::out | std::ios::binary);

        if (!outfile) {
            std::cerr << "Error opening file" << std::endl;
            return 1;
        }

        // Write the buffer to the file
        // The write function expects a const char* and the number of bytes to write
        outfile.write(buffer, buffer_length);

        // Always close the file when done
        outfile.close();

        std::cout << "Buffer written to output.bin as binary data." << std::endl;

        return 0;
    }

    seg_read_test_1(){
        // setup test file
        uint64_t bytes = std::pow(2,32);
        string filepath = "/tmp/segReadWriteTest1.bin";
        int code = gen_large_test_file(bytes, filepath);
        if (code != 0){
            CPPUNIT_FAIL("dd command failed with return code: "+ to_string(code));
        }

        // call seg_read on file
        char* buff; // <---- suspicious char buffer
        std::filebuf fb;
        if (fb.open (filepath,std::ios::in))
        {
            std::istream is(&fb);
            segmented_read(is, buff, bytes); // <---- James, this is the fct in question
            fb.close();
        }

        // take seg_read data and write to file
        string filepath2 = "/tmp/segReadWriteTest2.bin";
        write_buffer_to_file(buff);

        // assert (compare files)
        const char* command = "cmp "+ filepath +" "+ filepath2;
        std::cout << "Running command: " << command << std::endl;
        int return_code = std::system(command);
        CPPUNIT_ASSERT(return_code == 0);
    } // end seg_read_test_1()

    CPPUNIT_TEST_SUITE(SegmentReadWriteT);

    CPPUNIT_TEST(seg_read_test_1); //seg_read test 2^32
    CPPUNIT_TEST(...); //seg_read test 2^31
    CPPUNIT_TEST(...); //seg_read test (2^32)-1 byte
    CPPUNIT_TEST(...); //seg_write test 2^32
    CPPUNIT_TEST(...); //seg_write test 2^31
    CPPUNIT_TEST(...); //seg_write test (2^32)-1 byte1`

    CPPUNIT_TEST_SUITE_END();

}