
// -*- C++ -*-

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "DAS.h"

using namespace CppUnit;

class DASTest: public TestFixture {
private:
    DAS *das, *das2;

public:
    DASTest() {}
    ~DASTest() {}

    void setUp() { 
	das = new DAS();
	das2 = new DAS();
    } 

    void tearDown() { 
	delete das; das = 0;
	delete das2; das2 = 0;
    }

    CPPUNIT_TEST_SUITE( DASTest );

    CPPUNIT_TEST(error_values_test);
    CPPUNIT_TEST(symbol_name_test);

    CPPUNIT_TEST_SUITE_END();

    void error_values_test() {
	try {
	    das->parse("das-testsuite/bad_value_test.1");
	    das->print(stderr);
	}
	catch (Error &e) {
	    e.display_message();
	}
    }

    void symbol_name_test() {
	das->parse("das-testsuite/test.33");
	CPPUNIT_ASSERT(das->get_table("var1")->get_attr("y#z", 0) == "15");

	string s = das->get_table("var1.component1.inner component")->get_attr("tag");
	CPPUNIT_ASSERT(s == "\"xyz123\"");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DASTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}
