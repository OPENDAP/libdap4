

// -*- C++ -*-

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "DDS.h"

using namespace CppUnit;

class DDSTest : public TestFixture {
private:
    DDS *dds1, *dds2;

public: 
    DDSTest() {}
    ~DDSTest() {}

    void setUp() {
	dds1 = new DDS("test1");
	dds2 = new DDS("test2");
    }

    void tearDown() {
	delete dds1; dds1 = 0;
	delete dds2; dds2 = 0;
    }

    CPPUNIT_TEST_SUITE( DDSTest );

    CPPUNIT_TEST(symbol_name_test);

    CPPUNIT_TEST_SUITE_END();

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
};

CPPUNIT_TEST_SUITE_REGISTRATION(DDSTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}



