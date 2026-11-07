// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstring>
#include <sstream>
#include <string>

#include "MarshallerThread.h"
#include "run_tests_cppunit.h"

using namespace CppUnit;
using namespace libdap;
using namespace std;

class MarshallerThreadTest : public TestFixture {
    CPPUNIT_TEST_SUITE(MarshallerThreadTest);
    CPPUNIT_TEST(test_write_thread_to_stream);
    CPPUNIT_TEST_SUITE_END();

public:
    void test_write_thread_to_stream() {
        MarshallerThread mt;
        stringstream out(ios::in | ios::out | ios::binary);

        const string payload = "threaded-write";
        char *buf = new char[payload.size()];
        memcpy(buf, payload.data(), payload.size());

        mt.write_async(out, buf, payload.size());

        // Wait for the write to finish.
        mt.wait();

        CPPUNIT_ASSERT_EQUAL(payload, out.str());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MarshallerThreadTest);

int main(int argc, char *argv[]) { return run_tests<MarshallerThreadTest>(argc, argv) ? 0 : 1; }
