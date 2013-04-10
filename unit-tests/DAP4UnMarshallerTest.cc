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

//#define DODS_DEBUG2 1

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

#include "DAP4StreamUnMarshaller.h"

#include "debug.h"

using namespace std;
using namespace libdap;

class DAP4UnMarshallerTest: public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE( DAP4UnMarshallerTest );

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
    DAP4UnMarshallerTest() {
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
            DAP4StreamUnMarshaller dsm(in, is_host_big_endian());

            dods_byte b;
            dsm.get_byte(b);
            CPPUNIT_ASSERT(b == 17);
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "47ed733b8d10be225eceba344d533586");

            dods_int16 i1;
            dsm.get_int16(i1);
            CPPUNIT_ASSERT(i1 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "e2b50a7929c9fa04c82d9793b9fb710f");

            dods_int32 i2;
            dsm.get_int32(i2);
            CPPUNIT_ASSERT(i2 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "9ef06d55906276f617c008036beaba81");

            dods_int64 i3;
            dsm.get_int64(i3);
            CPPUNIT_ASSERT(i3 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "843fd2acf107350d495cae589a37913c");

            dods_uint16 ui1;
            dsm.get_uint16(ui1);
            CPPUNIT_ASSERT(ui1 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "e2b50a7929c9fa04c82d9793b9fb710f");

            dods_uint32 ui2;
            dsm.get_uint32(ui2);
            CPPUNIT_ASSERT(ui2 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "9ef06d55906276f617c008036beaba81");

            dods_uint64 ui3;
            dsm.get_uint64(ui3);
            CPPUNIT_ASSERT(ui3 == 17);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "843fd2acf107350d495cae589a37913c");
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
            DAP4StreamUnMarshaller dsm(in, is_host_big_endian());

            dods_float32 r1;
            dsm.get_float32(r1);
            CPPUNIT_ASSERT(r1 == 17.0);
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "104e50b0d8d66fcc1ed3ff3f43b71018");

            dods_float64 r2;
            dsm.get_float64(r2);
            CPPUNIT_ASSERT(r2 == 17.0);
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "77a53e5b4de7c90741dcbe24b827e866");
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
            DAP4StreamUnMarshaller dsm(in, is_host_big_endian());

            string s;
            dsm.get_str(s);
            CPPUNIT_ASSERT(s == "This is a test string with 40 characters");
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "263c8416bf984a01145f31b9328e6e8b");

            string u;
            dsm.get_url(u);
            CPPUNIT_ASSERT(u == "http://www.opendap.org/lame/unit/test");
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "e761355247b5b64889492373e1758107");
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
            DAP4StreamUnMarshaller dsm(in, is_host_big_endian());

            // Test both get_opaque calls; this one that expects the caller
            // to allocate memory.
            vector<unsigned char> buf(32768);
            dsm.get_opaque(reinterpret_cast<char*>(&buf[0]), 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf[i] == i % (1 << 7));
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "4188b46ae4fcbaeee1b2cb74850a7c65");

            char *buf2;
            unsigned int len;
            dsm.get_opaque(&buf2, len);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf2[i] == i % (1 << 7));
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "4188b46ae4fcbaeee1b2cb74850a7c65");

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
            DAP4StreamUnMarshaller dsm(in, is_host_big_endian());

            vector<unsigned char> buf1(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf1[0]), 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf1[i] == i % (1 << 7));
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "4188b46ae4fcbaeee1b2cb74850a7c65");

            vector<dods_int32> buf2(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf2[0]), 32768, sizeof(dods_int32), dods_int32_c);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf2[i] == i % (1 << 9));
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "bde002389f9caa5c590d315ce1b6e34e");

            vector<dods_float64> buf3(32768);
            dsm.get_vector(reinterpret_cast<char*>(&buf3[0]), 32768, sizeof(dods_float64), dods_float64_c);
            for (int i = 0; i < 32768; ++i) {
                if (buf3[i] != i % (1 << 9))
                    cerr << "buf3[" << i << "]: " << buf3[i] << endl;
                CPPUNIT_ASSERT(buf3[i] == i % (1 << 9));
            }
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "f249b4d23ba0fd2afa290fe374a2556b");
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
            DAP4StreamUnMarshaller dsm(in, is_host_big_endian());

            // Reuse the same pointer for all of the data...
            char *buf;
            unsigned int len;
            dsm.get_varying_vector(&buf, len);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(buf[i] == i % (1 << 7));
            string ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "4188b46ae4fcbaeee1b2cb74850a7c65");
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
            CPPUNIT_ASSERT(ck == "bde002389f9caa5c590d315ce1b6e34e");
            delete i_buf;

            dods_float64 *f_buf;
            dsm.get_varying_vector(reinterpret_cast<char**>(&f_buf), len, sizeof(dods_float64), dods_float64_c);
            CPPUNIT_ASSERT(len == 32768);
            for (int i = 0; i < 32768; ++i)
                CPPUNIT_ASSERT(f_buf[i] == i % (1 << 9));
            ck = dsm.get_checksum(dsm.get_checksum());
            DBG2(cerr << "ck: " << ck << endl);
            CPPUNIT_ASSERT(ck == "f249b4d23ba0fd2afa290fe374a2556b");
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

CPPUNIT_TEST_SUITE_REGISTRATION( DAP4UnMarshallerTest ) ;

int main(int, char **)
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest(registry.makeTest());
    runner.setOutputter(CppUnit::CompilerOutputter::defaultOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run("", false);
    return wasSuccessful ? 0 : 1;
}

