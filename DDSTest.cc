
// Tests for the DataDDS class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "DDS.h"

class DDSTest : public TestCase {
private:
    DDS *dds1, *dds2;

public: 
    DDSTest (string name) : TestCase (name) {}

    void setUp() {
	dds1 = new DDS("test1");
	dds2 = new DDS("test2");
    }

    void tearDown() {
	delete dds1; dds1 = 0;
	delete dds2; dds2 = 0;
    }

    void symbol_name_test() {
	// read a DDS.
	dds1->parse("dds-testsuite/test.18");
	assert(dds1->var("oddTemp"));
	
	dds2->parse("dds-testsuite/test.19b");
	assert(dds2->var("b#c"));
	assert(!dds2->var("b%23c"));
	assert(dds2->var("huh.Image#data"));
	assert(dds2->var("c d"));
	assert(!dds2->var("c%20d"));
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("DDSTest");
	s->addTest(new TestCaller<DDSTest>
		   ("symbol_name_test", &DDSTest::symbol_name_test));

	return s;
    }
};




