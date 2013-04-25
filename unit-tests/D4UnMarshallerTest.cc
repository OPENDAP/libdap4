
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

//#define DODS_DEBUG2 1

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

#include "D4StreamUnMarshaller.h"

#include "GetOpt.h"
#include "debug.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

using namespace std;
using namespace libdap;

class D4UnMarshallerTest: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE( D4UnMarshallerTest );

    CPPUNIT_TEST(test_scalars);
    CPPUNIT_TEST(test_real_scalars);
    CPPUNIT_TEST(test_str);
    CPPUNIT_TEST(test_opaque);
    CPPUNIT_TEST(test_vector);
    CPPUNIT_TEST(test_varying_vector);

    CPPUNIT_TEST_SUITE_END( );

    static inline bool is_host_big_endian()
    {
#ifdef COMPUTE_ENDIAN_AT_RUNTIME

        dods_int16 i = 0x0100;
        char *c = reinterpret_cast<char*>(&i);
        return *c;

#else

#ifdef WORDS_BIGENDIAN
        return true;
#else
        return false;
#endif

#endif
    }

public:
    D4UnMarshallerTest() {
    }

    void setUp() {
    }

    void tearDown() {
    }

    void test_scalars()
    {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            in.open("test_scalars_1_bin.dat", fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, is_host_big_endian());

            dods_byte b;
            dsm.get_byte(b);
            CPPUNIT_ASSERT(b == 17);
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "b8b2cf7f");

            dods_int16 i1;
            dsm.get_int16(i1);
            CPPUNIT_ASSERT(i1 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "120031ef");

            dods_int32 i2;
            dsm.get_int32(i2);
            CPPUNIT_ASSERT(i2 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "c9e1efe6");

            dods_int64 i3;
            dsm.get_int64(i3);
            CPPUNIT_ASSERT(i3 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d533eedc");

            dods_uint16 ui1;
            dsm.get_uint16(ui1);
            CPPUNIT_ASSERT(ui1 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "120031ef");

            dods_uint32 ui2;
            dsm.get_uint32(ui2);
            CPPUNIT_ASSERT(ui2 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "c9e1efe6");

            dods_uint64 ui3;
            dsm.get_uint64(ui3);
            CPPUNIT_ASSERT(ui3 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d533eedc");
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
            in.open("test_scalars_2_bin.dat", fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, is_host_big_endian());

            dods_float32 r1;
            dsm.get_float32(r1);
            CPPUNIT_ASSERT(r1 == 17.0);
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d3c5bc59");

            dods_float64 r2;
            dsm.get_float64(r2);
            CPPUNIT_ASSERT(r2 == 17.0);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "d5a3994b");
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

    void test_str() {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            in.open("test_scalars_3_bin.dat", fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, is_host_big_endian());

            string s;
            dsm.get_str(s);
            CPPUNIT_ASSERT(s == "This is a test string with 40 characters");
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "af117544");

            string u;
            dsm.get_url(u);
            CPPUNIT_ASSERT(u == "http://www.opendap.org/lame/unit/test");
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
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

    void test_opaque() {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            in.open("test_opaque_1_bin.dat", fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, is_host_big_endian());

            // Test both get_opaque calls; this one that expects the caller
            // to allocate memory.
            vector<unsigned char> buf(32768);
            dsm.get_opaque(reinterpret_cast<char*>(&buf[0]), 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf[i] == i % (1 << 7));
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "199ad7f5");

            char *buf2;
            unsigned int len;
            dsm.get_opaque(&buf2, len);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf2[i] == i % (1 << 7));
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
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

    void test_vector() {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            in.open("test_vector_1_bin.dat", fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, is_host_big_endian());

            vector<unsigned char> buf1(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf1[0]), 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf1[i] == i % (1 << 7));
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "199ad7f5");

            vector<dods_int32> buf2(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf2[0]), 32768, sizeof(dods_int32), dods_int32_c);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf2[i] == i % (1 << 9));
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "5c1bf29f");

            vector<dods_float64> buf3(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf3[0]), 32768, sizeof(dods_float64), dods_float64_c);
            for (int i = 0; i < 32768; ++i) {
                if (buf3[i] != i % (1 << 9))
                    cerr << "buf3[" << i << "]: " << buf3[i] << endl;
                CPPUNIT_ASSERT(buf3[i] == i % (1 << 9));
            }
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "aafc2a91");
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

    void test_varying_vector() {
        fstream in;
        in.exceptions(ostream::failbit | ostream::badbit);

        // computes checksums and writes data
        try {
            in.open("test_vector_2_bin.dat", fstream::binary | fstream::in);
            D4StreamUnMarshaller dsm(in, is_host_big_endian());

            // Reuse the same pointer for all of the data...
            char *buf;
            unsigned int len;
            dsm.get_varying_vector(&buf, len);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf[i] == i % (1 << 7));
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "199ad7f5");
            delete buf;

            dods_int32 *i_buf;
            dsm.get_varying_vector(reinterpret_cast<char**>(&i_buf), len, sizeof(dods_int32), dods_int32_c);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i) {
                if (i_buf[i] != i % (1 << 9))
                    cerr << "i_buf[" << i << "]: " << i_buf[i] << endl;
                CPPUNIT_ASSERT(i_buf[i] == i % (1 << 9));
            }
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "5c1bf29f");
            delete i_buf;

            dods_float64 *f_buf;
            dsm.get_varying_vector(reinterpret_cast<char**>(&f_buf), len, sizeof(dods_float64), dods_float64_c);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(f_buf[i] == i % (1 << 9));
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "aafc2a91");
            delete f_buf;
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

CPPUNIT_TEST_SUITE_REGISTRATION( D4UnMarshallerTest ) ;

int main(int argc, char*argv[]) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    runner.setOutputter(CppUnit::CompilerOutputter::defaultOutputter(&runner.result(), std::cerr));

    GetOpt getopt(argc, argv, "d");
    char option_char;

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = true;  // debug is a static global
            break;
        default:
            break;
        }

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        while (i < argc) {
            test = string("DAP4MarshallerTest::") + argv[i++];

            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
