
// Tests for the DataDDS class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "DataDDS.h"

class DataDDSTest : public TestCase {
private:
    DataDDS *dds1, *dds2, *dds3, *dds4, *dds5, *dds6;

public: 
    DataDDSTest (string name) : TestCase (name) {}

    void setUp() {
	dds1 = new DataDDS("test1", "ff-dods/3.2.1");
	dds2 = new DataDDS("test2", "ff-dods/3.2.1 (Highland)");
	dds3 = new DataDDS("test3", "FF-dods/3.2.1");
	dds4 = new DataDDS("test4", "ff-dods/3.2");
	dds5 = new DataDDS("test5", "dods/3.2.1");
	dds6 = new DataDDS("test6", "DODS/3.2.1");
    }

    void tearDown() {
	delete dds1; dds1 = 0;
	delete dds2; dds2 = 0;
	delete dds3; dds3 = 0;
	delete dds4; dds4 = 0;
	delete dds5; dds5 = 0;
	delete dds6; dds6 = 0;
    }

    void version_number_test() {
	assert(dds1->get_version_major() == 3 && dds1->get_version_minor() == 2);
	assert(dds2->get_version_major() == 3 && dds2->get_version_minor() == 2);
	assert(dds3->get_version_major() == 3 && dds3->get_version_minor() == 2);
	assert(dds4->get_version_major() == 3 && dds4->get_version_minor() == 2);
	assert(dds5->get_version_major() == 3 && dds5->get_version_minor() == 2);
	assert(dds6->get_version_major() == 3 && dds6->get_version_minor() == 2);
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("DataDDSTest");
	s->addTest(new TestCaller<DataDDSTest>
		   ("version_number_test",
		    &DataDDSTest::version_number_test));

	return s;
    }
};




