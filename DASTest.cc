
// Tests for the AttrTable class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "DAS.h"

class DASTest:public TestCase {
private:
    DAS *das, *das2;

public:
    DASTest(string name):TestCase(name) {
    } 

    void setUp() { 
	das = new DAS();
	das2 = new DAS();
    } 

    void tearDown() { 
	delete das; das = 0;
	delete das2; das2 = 0;
    }

    void error_values_test() {
	try {
	    das->parse("das-testsuite/bad_value_test.1");
	    das->print(cerr);
	}
	catch (Error &e) {
	    e.display_message();
	}
    }

    void symbol_name_test() {
	das->parse("das-testsuite/test.33");
	assert(das->get_table("var1")->get_attr("y#z", 0) == "15");

	string s = das->get_table("var1.component1.inner component")->get_attr("tag");
	assert(s == "\"xyz123\"");
    }

    static Test *suite() { 
	TestSuite *s = new TestSuite("DASTest");

	s->addTest(new TestCaller < DASTest >
		   ("symbol_name_test", &DASTest::symbol_name_test));

	return s;
    }
};
