
// Tests for the cgi util functions.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "cgi_util.h"

class cgiUtilTest : public TestCase {
private:

public: 
  cgiUtilTest (string name) : TestCase (name) {}

  void setUp() {
  }

  void tearDown() {
  }

  // Tests for methods
  void find_ancillary_file_test() {
    assert(find_ancillary_file("das-testsuite/test.1", "das", "", "") ==
	   "das-testsuite/test.1.das");
  }

  static Test *suite ()  {
    TestSuite *s = new TestSuite("cgiUtilTest");
    s->addTest(new TestCaller<cgiUtilTest>("find_ancillary_file_test",
					      &cgiUtilTest::find_ancillary_file_test));

    return s;
  }
};


