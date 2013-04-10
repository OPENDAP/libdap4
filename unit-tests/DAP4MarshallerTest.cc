#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdint.h>

// #define DODS_DEBUG 1

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

#include "DAP4StreamMarshaller.h"

#include "debug.h"

using namespace std;
using namespace libdap;

class DAP4MarshallerTest: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE( DAP4MarshallerTest );

    CPPUNIT_TEST(test_cmp);
    CPPUNIT_TEST(test_scalars);
    CPPUNIT_TEST(test_real_scalars);
    CPPUNIT_TEST(test_str);
    CPPUNIT_TEST(test_opaque);
    CPPUNIT_TEST(test_vector);
    CPPUNIT_TEST(test_varying_vector);

    CPPUNIT_TEST_SUITE_END( );

    /**
     * Compare the contents of a file with a memory buffer
     */
    bool cmp(const char *buf, unsigned int len, string file) {
        fstream in;
        in.open(file.c_str(), fstream::binary | fstream::in);
        if (!in) {
            cerr << "Could not open file: " << file << endl;
            return false;
        }

        vector<char> fbuf(len);
        in.read(&fbuf[0], len);
        if (!in) {
            cerr << "Could not read " << len << " bytes from file." << endl;
            return false;
        }

        for (unsigned int i = 0; i < len; ++i)
            if (*buf++ != fbuf[i]) {
                cerr << "Response differs from baseline at byte " << i << endl;
                cerr << "Expected: " << setfill('0') << setw(2) << hex
                        << (unsigned int)fbuf[i] << "; got: "
                        << (unsigned int)buf[i] << dec << endl;
                return false;
            }

        return true;
    }

    void write_binary_file(const char *buf, int len, string file) {
        fstream out;
        out.open(file.c_str(), fstream::binary | fstream::out);
        if (!out) {
            cerr << "Could not open file: " << file << endl;
            return;
        }

        out.write(buf, len);
    }

public:
    DAP4MarshallerTest() {
    }

    void setUp() {
    }

    void tearDown() {
    }

    void test_cmp() {
        char buf[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
        CPPUNIT_ASSERT(cmp(buf, 16, "test_cmp.dat"));
    }

    void test_scalars() {
        ostringstream oss;
        // computes checksums and writes data
        try {
        DAP4StreamMarshaller dsm(oss);

        dsm.reset_checksum();

        dsm.put_byte(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        dsm.put_int16(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        dsm.put_int32(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        dsm.put_int64(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        dsm.put_uint16(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        dsm.put_uint32(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        dsm.put_uint64(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        //write_binary_file(oss.str().data(), oss.str().length(), "test_scalars_1_bin.dat");
        CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), "test_scalars_1_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_real_scalars() {
        ostringstream oss;
        // computes checksums and writes data
        try {
        DAP4StreamMarshaller dsm(oss);

        dsm.reset_checksum();

        dsm.put_float32(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        dsm.put_float64(17);
        dsm.put_checksum();
        dsm.reset_checksum();

        //write_binary_file(oss.str().data(), oss.str().length(), "test_scalars_2_bin.dat");
        CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), "test_scalars_2_bin.dat"));
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_str() {
        ostringstream oss;
        try {
            DAP4StreamMarshaller dsm(oss);

            dsm.reset_checksum();

            dsm.put_str("This is a test string with 40 characters");
            dsm.put_checksum();
            dsm.reset_checksum();

            // 37 chars --> 0x25 --> 0x25 as a 128-bit varint
            dsm.put_url("http://www.opendap.org/lame/unit/test");
            dsm.put_checksum();
            dsm.reset_checksum();

            // True these are not really scalars...
            //write_binary_file(oss.str().data(), oss.str().length(), "test_scalars_3_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), "test_scalars_3_bin.dat"));
       }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_opaque() {
        ostringstream oss;
        try {
            DAP4StreamMarshaller dsm(oss);
            vector<unsigned char> buf(32768);
            for (int i = 0; i < 32768; ++i)
                buf[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_opaque(reinterpret_cast<char*>(&buf[0]), 32768);
            dsm.put_checksum();
            dsm.reset_checksum();

            dsm.put_opaque(reinterpret_cast<char*>(&buf[0]), 32768);
            dsm.put_checksum();

            //write_binary_file(oss.str().data(), oss.str().length(), "test_opaque_1_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), "test_opaque_1_bin.dat"));
       }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_vector() {
        ostringstream oss;
        try {
            DAP4StreamMarshaller dsm(oss);
            vector<unsigned char> buf1(32768);
            for (int i = 0; i < 32768; ++i)
                buf1[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_vector(reinterpret_cast<char*>(&buf1[0]), 32768);
            dsm.put_checksum();
            dsm.reset_checksum();

            vector<dods_int32> buf2(32768);
            for (int i = 0; i < 32768; ++i)
                buf2[i] = i % (1 << 9);

            dsm.put_vector(reinterpret_cast<char*>(&buf2[0]), 32768, sizeof(dods_int32), dods_int32_c);
            dsm.put_checksum();
            dsm.reset_checksum();

            vector<dods_float64> buf3(32768);
            for (int i = 0; i < 32768; ++i)
                buf3[i] = i % (1 << 9);

            dsm.put_vector(reinterpret_cast<char*>(&buf3[0]), 32768, sizeof(dods_float64), dods_float64_c);
            dsm.put_checksum();

            //write_binary_file(oss.str().data(), oss.str().length(), "test_vector_1_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), "test_vector_1_bin.dat"));
       }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

    void test_varying_vector() {
        ostringstream oss;
        try {
            DAP4StreamMarshaller dsm(oss);
            vector<unsigned char> buf1(32768);
            for (int i = 0; i < 32768; ++i)
                buf1[i] = i % (1 << 7);

            dsm.reset_checksum();

            dsm.put_varying_vector(reinterpret_cast<char*>(&buf1[0]), 32768);
            dsm.put_checksum();
            dsm.reset_checksum();

            vector<dods_int32> buf2(32768);
            for (int i = 0; i < 32768; ++i)
                buf2[i] = i % (1 << 9);

            dsm.put_varying_vector(reinterpret_cast<char*>(&buf2[0]), 32768, sizeof(dods_int32), dods_int32_c);
            dsm.put_checksum();
            dsm.reset_checksum();

            vector<dods_float64> buf3(32768);
            for (int i = 0; i < 32768; ++i)
                buf3[i] = i % (1 << 9);

            dsm.put_varying_vector(reinterpret_cast<char*>(&buf3[0]), 32768, sizeof(dods_float64), dods_float64_c);
            dsm.put_checksum();

            //write_binary_file(oss.str().data(), oss.str().length(), "test_vector_2_bin.dat");
            CPPUNIT_ASSERT(cmp(oss.str().data(), oss.str().length(), "test_vector_2_bin.dat"));
       }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            CPPUNIT_FAIL("Caught an exception.");
        }
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( DAP4MarshallerTest ) ;

int main(int, char **)
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest(registry.makeTest());
    runner.setOutputter(CppUnit::CompilerOutputter::defaultOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run("", false);
    return wasSuccessful ? 0 : 1;
}

