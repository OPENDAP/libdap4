
// Tests for the cgi util functions.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "parser.h"

class parserUtilTest : public TestCase {
private:

public: 
  parserUtilTest (string name) : TestCase (name) {}

  void setUp() {
  }

  void tearDown() {
  }

  // Tests for methods
  void check_byte_test() {
      assert(check_byte("255"));
      assert(!check_byte("256"));
      assert(check_byte("0"));
      assert(check_byte("-127"));
      assert(check_byte("-128"));
      assert(!check_byte("-129"));
      assert(!check_byte("-32767"));
      assert(!check_byte("32767"));
  }

    void check_float32_test() {
	assert(check_float32("1.0"));
	assert(check_float32("0.0"));
	assert(check_float32("-0.0"));
	assert(check_float32("+0.0"));
	assert(check_float32(".0"));
	assert(check_float32("3.402823466E+38F"));
	assert(check_float32("-3.402823466E+38F"));
	assert(check_float32("1.175494351E-38F"));
	assert(check_float32("-1.175494351E-38F"));
	assert(check_float32("NaN"));
	assert(check_float32("nan"));
	assert(check_float32("Nan"));

	assert(!check_float32("3.502823466E+38F"));
	assert(!check_float32("-3.502823466E+38F"));
	assert(!check_float32("1.0E-38F"));
	assert(!check_float32("-1.0E-38F"));

	assert(!check_float32("1.7976931348623157E+308"));
	assert(!check_float32("-1.7976931348623157E+308"));
	assert(!check_float32("2.2250738585072014E-308"));
	assert(!check_float32("-2.2250738585072014E-308"));
    }
	
    void check_float64_test() {
	assert(check_float64("1.0"));
	assert(check_float64("0.0"));
	assert(check_float64("-0.0"));
	assert(check_float64("+0.0"));
	assert(check_float64(".0"));
	assert(check_float64("3.402823466E+38F"));
	assert(check_float64("-3.402823466E+38F"));
	assert(check_float64("1.175494351E-38F"));
	assert(check_float64("-1.175494351E-38F"));

	assert(check_float64("1.7976931348623157E+308"));
	assert(check_float64("-1.7976931348623157E+308"));
	assert(check_float64("2.2250738585072014E-308"));
	assert(check_float64("-2.2250738585072014E-308"));

	assert(check_float64("NaN"));
	assert(check_float64("nan"));
	assert(check_float64("Nan"));

	assert(!check_float64("255E400"));
	assert(!check_float64("255E-400"));

	assert(!check_float64("1.8E+308"));
	assert(!check_float64("-1.8E+308"));
	assert(!check_float64("2.0E-308"));
	assert(!check_float64("-2.0E-308"));
    }

  static Test *suite ()  {
    TestSuite *s = new TestSuite("parserUtilTest");

    s->addTest(new TestCaller<parserUtilTest>
	       ("check_byte_test",&parserUtilTest::check_byte_test));
    s->addTest(new TestCaller<parserUtilTest>
	       ("check_float32_test",&parserUtilTest::check_float32_test));
    s->addTest(new TestCaller<parserUtilTest>
	       ("check_float64_test",&parserUtilTest::check_float64_test));

    return s;
  }
};


