
// Tests for the DataDDS class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "DDS.h"

class DDSTest : public TestCase {
private:
    DDS *dds1;

public: 
    DDSTest (string name) : TestCase (name) {}

    void setUp() {
	dds1 = new DDS("test1", "ff-dods/3.2.1");
    }

    void tearDown() {
	delete dds1; dds1 = 0;
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("DDSTest");
	s->addTest(new TestCaller<DDSTest>
		   ("version_number_test",
		    &DataDDSTest::version_number_test));

	return s;
    }
};




