
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
	assert(unhexstring("5b") == "[");
	assert(unhexstring("5d") == "]");
	assert(unhexstring("20") == " ");
	assert(unhexstring("25") == "%");
	assert(unhexstring("5B") == "[");
	assert(unhexstring("5D") == "]");
    }

    void id2www_test() {
	assert(id2www("this") == "this");
	assert(id2www("This is a test") == "This%20is%20a%20test");
	assert(id2www("This.is") == "This.is");
	assert(id2www("This-is") == "This-is");
	assert(id2www("This_is") == "This_is");
	assert(id2www("This/is") == "This%2fis");
	assert(id2www("This%is") == "This%25is");
    }

    void www2id_test() {
	assert(www2id("This_is_a_test") == "This_is_a_test");
	assert(www2id("This is a test") == "This is a test");
	assert(www2id("%5b") == "[");
	assert(www2id("%5d") == "]");
	assert(www2id("u%5b0%5d") == "u[0]");
	assert(www2id("WVC%20Lat") == "WVC Lat");
	assert(www2id("Grid.Data%20Fields[20][20]") 
	       == "Grid.Data Fields[20][20]");

	assert(www2id("Grid.Data%3aFields[20][20]") 
	       == "Grid.Data:Fields[20][20]");

	assert(www2id("Grid%3aData%20Fields%5b20%5d[20]", "%", "%20") 
	       == "Grid:Data%20Fields[20][20]");
    }

    // This is the code in expr.lex that removes emclosing double quotes and
    // %20 sequences from a string. I copied this here because that actual
    // function uses globals and would be hard to test. 7/11/2001 jhrg
    string *store_str(char *text) {
	string *s = new string(www2id(string(text)));

	if (*s->begin() == '\"' && *(s->end()-1) == '\"') {
	    s->erase(s->begin());
	    s->erase(s->end()-1);
	}

	return s;
    }

    void ce_string_parse_test() {
	assert(*store_str("testing") == "testing");
	assert(*store_str("\"testing\"") == "testing");
	assert(*store_str("\"test%20ing\"") == "test ing");
	assert(*store_str("test%20ing") == "test ing");
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
		   ("www2id_test", &generalUtilTest::www2id_test));

	s->addTest(new TestCaller<generalUtilTest>
		   ("id2www_test", &generalUtilTest::id2www_test));

	s->addTest(new TestCaller<generalUtilTest>
		   ("ce_string_parse_test", 
		    &generalUtilTest::ce_string_parse_test));

	return s;
    }
};


