
// Tests for the util functions in util.cc and escaping.cc

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "util.h"
#include "escaping.h"

string hexstring(unsigned char val); // originally declared static
string unhexstring(string s);

class generalUtilTest : public TestCase {
private:

public: 
    generalUtilTest (string name) : TestCase (name) {}

    void setUp() {
    }

    void tearDown() {
    }

    // Tests for methods
    void path_to_filename_test() {
	assert(path_to_filename("/this/is/the/end/my.friend") == "my.friend");
	assert(path_to_filename("this.dat") == "this.dat");
	assert(path_to_filename("/this.dat") == "this.dat");
	assert(path_to_filename("/this.dat/") == "");
    }

    void hexstring_test() {
	assert(hexstring('[') == "5b");
	assert(hexstring(']') == "5d");
	assert(hexstring(' ') == "20");
	assert(hexstring('%') == "25");
    }

    void unhexstring_test() {
#if 0
	cerr << "unhexstring(\"5b\"): " << unhexstring("5b") << endl;
#endif
	assert(unhexstring("5b") == "[");
	assert(unhexstring("5d") == "]");
	assert(unhexstring("20") == " ");
	assert(unhexstring("25") == "%");
	assert(unhexstring("5B") == "[");
	assert(unhexstring("5D") == "]");
    }

    void dods2id_test() {
	assert(dods2id("This_is_a_test") == "This_is_a_test");
	assert(dods2id("This is a test") == "This is a test");
#if 0
	cerr << "dods2id(\"%5B\") :" << dods2id("%5B") << endl;
#endif
	assert(dods2id("%5b") == "[");
	assert(dods2id("%5d") == "]");
	assert(dods2id("u%5b0%5d") == "u[0]");
	assert(dods2id("WVC%20Lat") == "WVC Lat");
	assert(dods2id("Grid.Data%20Fields[20][20]") 
	       == "Grid.Data Fields[20][20]");
	cerr << "dods2id(\"%3a\") :" << dods2id("%3a") << endl;
	assert(dods2id("Grid.Data%3aFields[20][20]") 
	       == "Grid.Data:Fields[20][20]");
	cerr << "dods2id(\"%3a%20%3a\") :" 
	     << dods2id("%3a%20%3a", "%[0-7][0-9a-fA-F]", "%20") << endl;
	assert(dods2id("Grid%3aData%20Fields%5b20%5d[20]",
		       "%[0-7][0-9a-fA-F]", "%20") 
	       == "Grid:Data%20Fields[20][20]");
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("generalUtilTest");

	s->addTest(new TestCaller<generalUtilTest>
		   ("path_to_filename_test",
		    &generalUtilTest::path_to_filename_test));

	s->addTest(new TestCaller<generalUtilTest>
		   ("hexstring_test", &generalUtilTest::hexstring_test));

	s->addTest(new TestCaller<generalUtilTest>
		   ("unhexstring_test", &generalUtilTest::unhexstring_test));

	s->addTest(new TestCaller<generalUtilTest>
		   ("dods2id_test", &generalUtilTest::dods2id_test));

	return s;
    }
};


