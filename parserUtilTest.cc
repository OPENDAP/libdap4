
// -*- C++ -*-
// Tests for the cgi util functions.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "parser.h"

using namespace CppUnit;

class parserUtilTest : public TestFixture {
private:

public: 
    parserUtilTest() {}
    ~parserUtilTest() {}

    void setUp() {
    }

    void tearDown() {
    }

    CPPUNIT_TEST_SUITE(parserUtilTest);

    CPPUNIT_TEST(check_byte_test);
    CPPUNIT_TEST(check_float32_test);
    CPPUNIT_TEST(check_float64_test);

    CPPUNIT_TEST_SUITE_END();

    // Tests for methods
    void check_byte_test() {
	CPPUNIT_ASSERT(check_byte("255"));
	CPPUNIT_ASSERT(!check_byte("256"));
	CPPUNIT_ASSERT(check_byte("0"));
	CPPUNIT_ASSERT(check_byte("-127"));
	CPPUNIT_ASSERT(check_byte("-128"));
	CPPUNIT_ASSERT(!check_byte("-129"));
	CPPUNIT_ASSERT(!check_byte("-32767"));
	CPPUNIT_ASSERT(!check_byte("32767"));
    }

    void check_float32_test() {
	CPPUNIT_ASSERT(check_float32("1.0"));
	CPPUNIT_ASSERT(check_float32("0.0"));
	CPPUNIT_ASSERT(check_float32("-0.0"));
	CPPUNIT_ASSERT(check_float32("+0.0"));
	CPPUNIT_ASSERT(check_float32(".0"));
	CPPUNIT_ASSERT(check_float32("3.402823466E+38F"));
	CPPUNIT_ASSERT(check_float32("-3.402823466E+38F"));
	CPPUNIT_ASSERT(check_float32("1.175494351E-38F"));
	CPPUNIT_ASSERT(check_float32("-1.175494351E-38F"));
	CPPUNIT_ASSERT(check_float32("NaN"));
	CPPUNIT_ASSERT(check_float32("nan"));
	CPPUNIT_ASSERT(check_float32("Nan"));

	CPPUNIT_ASSERT(!check_float32("3.502823466E+38F"));
	CPPUNIT_ASSERT(!check_float32("-3.502823466E+38F"));
	CPPUNIT_ASSERT(!check_float32("1.0E-38F"));
	CPPUNIT_ASSERT(!check_float32("-1.0E-38F"));

	CPPUNIT_ASSERT(!check_float32("1.7976931348623157E+308"));
	CPPUNIT_ASSERT(!check_float32("-1.7976931348623157E+308"));
	CPPUNIT_ASSERT(!check_float32("2.2250738585072014E-308"));
	CPPUNIT_ASSERT(!check_float32("-2.2250738585072014E-308"));
    }
	
    void check_float64_test() {
	CPPUNIT_ASSERT(check_float64("1.0"));
	CPPUNIT_ASSERT(check_float64("0.0"));
	CPPUNIT_ASSERT(check_float64("-0.0"));
	CPPUNIT_ASSERT(check_float64("+0.0"));
	CPPUNIT_ASSERT(check_float64(".0"));
	CPPUNIT_ASSERT(check_float64("3.402823466E+38F"));
	CPPUNIT_ASSERT(check_float64("-3.402823466E+38F"));
	CPPUNIT_ASSERT(check_float64("1.175494351E-38F"));
	CPPUNIT_ASSERT(check_float64("-1.175494351E-38F"));

	CPPUNIT_ASSERT(check_float64("1.7976931348623157E+308"));
	CPPUNIT_ASSERT(check_float64("-1.7976931348623157E+308"));
	CPPUNIT_ASSERT(check_float64("2.2250738585072014E-308"));
	CPPUNIT_ASSERT(check_float64("-2.2250738585072014E-308"));

	CPPUNIT_ASSERT(check_float64("NaN"));
	CPPUNIT_ASSERT(check_float64("nan"));
	CPPUNIT_ASSERT(check_float64("Nan"));

	CPPUNIT_ASSERT(!check_float64("255E400"));
	CPPUNIT_ASSERT(!check_float64("255E-400"));

	CPPUNIT_ASSERT(!check_float64("1.8E+308"));
	CPPUNIT_ASSERT(!check_float64("-1.8E+308"));
	CPPUNIT_ASSERT(!check_float64("2.0E-308"));
	CPPUNIT_ASSERT(!check_float64("-2.0E-308"));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(parserUtilTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}


