// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"
#include "run_tests_cppunit.h"

using namespace CppUnit;
using namespace libdap;
using namespace std;

class D4StreamRoundTripTest : public TestFixture {
    CPPUNIT_TEST_SUITE(D4StreamRoundTripTest);
    CPPUNIT_TEST(test_round_trip_scalars_and_vectors);
    CPPUNIT_TEST(test_truncated_stream_throws);
    CPPUNIT_TEST_SUITE_END();

public:
    void test_round_trip_scalars_and_vectors() {
        stringstream ss(ios::in | ios::out | ios::binary);

        const dods_int32 i32_min = std::numeric_limits<dods_int32>::min();
        const dods_int64 i64_max = std::numeric_limits<dods_int64>::max();
        const dods_uint64 u64_max = std::numeric_limits<dods_uint64>::max();
        const dods_float32 f32_inf = std::numeric_limits<dods_float32>::infinity();
        const dods_float64 f64_nan = std::numeric_limits<dods_float64>::quiet_NaN();

        {
            D4StreamMarshaller marshaller(ss, true, false);
            marshaller.put_int32(i32_min);
            marshaller.put_int64(i64_max);
            marshaller.put_uint64(u64_max);
            marshaller.put_float32(f32_inf);
            marshaller.put_float64(f64_nan);
            marshaller.put_str("hello");

            dods_float32 f32_vals[3] = {1.5f, -2.0f, 0.0f};
            marshaller.put_vector_float32(reinterpret_cast<char *>(f32_vals), 3);

            dods_float64 f64_vals[2] = {3.25, -4.5};
            marshaller.put_vector_float64(reinterpret_cast<char *>(f64_vals), 2);
        }

        ss.seekg(0, ios::beg);

        D4StreamUnMarshaller unmarshaller(ss, false);

        dods_int32 i32_out;
        dods_int64 i64_out;
        dods_uint64 u64_out;
        dods_float32 f32_out;
        dods_float64 f64_out;
        string s_out;

        unmarshaller.get_int32(i32_out);
        unmarshaller.get_int64(i64_out);
        unmarshaller.get_uint64(u64_out);
        unmarshaller.get_float32(f32_out);
        unmarshaller.get_float64(f64_out);
        unmarshaller.get_str(s_out);

        CPPUNIT_ASSERT_EQUAL(i32_min, i32_out);
        CPPUNIT_ASSERT_EQUAL(i64_max, i64_out);
        CPPUNIT_ASSERT_EQUAL(u64_max, u64_out);
        CPPUNIT_ASSERT(std::isinf(f32_out) && f32_out > 0);
        CPPUNIT_ASSERT(std::isnan(f64_out));
        CPPUNIT_ASSERT_EQUAL(string("hello"), s_out);

        dods_float32 f32_out_vals[3] = {0.0f, 0.0f, 0.0f};
        unmarshaller.get_vector_float32(reinterpret_cast<char *>(f32_out_vals), 3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5, f32_out_vals[0], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-2.0, f32_out_vals[1], 1e-6);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, f32_out_vals[2], 1e-6);

        dods_float64 f64_out_vals[2] = {0.0, 0.0};
        unmarshaller.get_vector_float64(reinterpret_cast<char *>(f64_out_vals), 2);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(3.25, f64_out_vals[0], 1e-12);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-4.5, f64_out_vals[1], 1e-12);
    }

    void test_truncated_stream_throws() {
        string data("\x01\x02\x03", 3);
        stringstream ss(data, ios::in | ios::binary);
        D4StreamUnMarshaller unmarshaller(ss, false);

        dods_int32 i32_out = 0;
        CPPUNIT_ASSERT_THROW(unmarshaller.get_int32(i32_out), std::istream::failure);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(D4StreamRoundTripTest);

int main(int argc, char *argv[]) { return run_tests<D4StreamRoundTripTest>(argc, argv) ? 0 : 1; }
