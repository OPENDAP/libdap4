
// Tests for Byte. This uses the TestByte class. Tests features of BaseType,
// too. 7/19/2001 jhrg

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include "TestByte.h"

class ByteTest : public TestCase {
private:
    TestByte *tb1;
    TestByte *tb2;
    TestByte *tb3;
    TestByte *tb4;

public: 
    ByteTest (string name) : TestCase (name) {}

    void setUp() {
	tb1 = new TestByte("tb1");
	tb2 = new TestByte("tb2 name with spaces");
	tb3 = new TestByte("tb3 %");
	tb4 = new TestByte("tb4 #");
    }

    void tearDown() {
	delete tb1; tb1 = 0;
	delete tb2; tb2 = 0;
	delete tb3; tb3 = 0;
	delete tb4; tb4 = 0;
    }

    void name_mangling_test() {
	assert(tb1->name() == "tb1");
	assert(tb2->name() == "tb2 name with spaces");
	assert(tb3->name() == "tb3 %");
	assert(tb4->name() == "tb4 #");
    }

    void decl_mangling_test() {
	ostrstream oss;
	tb1->print_decl(oss, "", false); oss << ends;
	assert(string(oss.str()) == "Byte tb1");

	ostrstream oss2;
	tb2->print_decl(oss2, "", false); oss2 << ends;
	assert(string(oss2.str()) == "Byte tb2%20name%20with%20spaces");

	ostrstream oss3;
	tb3->print_decl(oss3, "", false); oss3 << ends;
	assert(string(oss3.str()) == "Byte tb3%20%25");

	ostrstream oss4;
	tb4->print_decl(oss4, "", false); oss4 << ends;
	assert(string(oss4.str()) == "Byte tb4%20%23");
    }

    static Test *suite ()  {
	TestSuite *s = new TestSuite("ByteTest");
	s->addTest(new TestCaller<ByteTest>
		   ("name_mangling_test", &ByteTest::name_mangling_test));
	s->addTest(new TestCaller<ByteTest>
		   ("decl_mangling_test", &ByteTest::decl_mangling_test));

	return s;
    }
};




