
// Tests for the DODSFilter class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "DODSFilter.h"

class DODSFilterTest : public TestCase {
private:
  DODSFilter *df;

public: 
  DODSFilterTest (string name) : TestCase (name) {}

  void setUp() {
    char *argv_1[] = {"test_case", "das-testsuite/test.1"};
    df = new DODSFilter(2, argv_1);
  }

  void tearDown() {
    delete df;
  }

  // Tests for methods
  void read_ancillary_das_test() {
    // Tests using assert.
  }

  void read_ancillary_dds_test() {
    // Tests using assert.
  }

  static Test *suite ()  {
    TestSuite *s = new TestSuite("DODSFilterTest");
    s->addTest(new TestCaller<DODSFilterTest>("read_ancillary_das_test",
					      &DODSFilterTest::read_ancillary_das_test));
    s->addTest(new TestCaller<DODSFilterTest>("read_ancillary_dds_test",
					      &DODSFilterTest::read_ancillary_dds_test));
    return s;
  }
};


