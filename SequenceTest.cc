
// Tests for the AttrTable class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include <Regex.h>

#include "TestSequence.h"
#include "TestInt32.h"
#include "TestStr.h"

const static char *s_as_string = 
"BaseType (0x.*):
          _name: s
          _type: 13
          _read_p: 0
          _send_p: 0
          _synthesized_p: 0
          d_parent: (nil)
BaseType (0x.*):
          _name: i1
          _type: 4
          _read_p: 0
          _send_p: 0
          _synthesized_p: 0
          d_parent: 0x.*
BaseType (0x.*):
          _name: str1
          _type: 8
          _read_p: 0
          _send_p: 0
          _synthesized_p: 0
          d_parent: 0x.*
BaseType (0x.*):
          _name: i2
          _type: 4
          _read_p: 0
          _send_p: 0
          _synthesized_p: 0
          d_parent: 0x.*

";

static Regex s_regex(s_as_string);

class SequenceTest:public TestCase {
private:
    TestSequence *s;
    TestSequence *sss;		// Multi-level Sequence
#if 0
    TestSequence *gss;		// Tricky... Struct { Seq { Seq { ...
#endif

    bool re_match(Regex &r, const char *s) {
#if 0
	cerr << "strlen(s): " << (int)strlen(s) << endl;
	cerr << "r.match(s): " << r.match(s, strlen(s)) << endl;
#endif
	return r.match(s, strlen(s)) == (int)strlen(s);
    }

public:
    SequenceTest(string name):TestCase(name) {
    } 

    void setUp() { 
	// Set up a simple sequence. Used to test ctor, assigment, et cetera.
	s = new TestSequence("s");
	s->add_var(new TestInt32("i1"));
	s->add_var(new TestStr("str1"));
	s->add_var(new TestInt32("i2"));

	// Set up sss, used to test multi-level sequences
	sss = new TestSequence("sss");
	sss->add_var(new TestInt32("i1"));
	
	TestSequence *ts = new TestSequence("child_of_sss");
	ts->add_var(new TestStr("str1"));
	
	TestSequence *tts = new TestSequence("child_of_child_of_sss");
	tts->add_var(new TestInt32("i2"));
	ts->add_var(tts);

	sss->add_var(ts);
    } 

    void tearDown() { 
	delete s; s = 0;
	delete sss; sss = 0;
    }

    // Tests for methods
    void ctor_test() {
#if 0
	cerr << s->toString() << endl;
	cerr << sss->toString() << endl;
#endif
	assert(re_match(s_regex, s->toString().c_str()));
    }

    void assignment() {
	TestSequence ts2 = *s;

	ts2 = *s;
	assert(re_match(s_regex, ts2.toString().c_str()));
    }

    void copy_ctor() {
	TestSequence s2 = *s;
	assert(re_match(s_regex, s2.toString().c_str()));
    }

    void test_super_sequence() {
	assert(sss->is_super_sequence());
	Sequence *ts = dynamic_cast<Sequence *>(sss->var("child_of_sss"));
	assert(ts->is_super_sequence());
	ts = dynamic_cast<Sequence *>(ts->var("child_of_child_of_sss"));
	assert(!ts->is_super_sequence());
    }

    static Test *suite() { 
	TestSuite *ts = new TestSuite("SequenceTest");
	ts->addTest(new TestCaller <SequenceTest> 
		    ("ctor_test", &SequenceTest::ctor_test));
	ts->addTest(new TestCaller <SequenceTest> 
		    ("copy_ctor", &SequenceTest::copy_ctor));
	ts->addTest(new TestCaller <SequenceTest> 
		    ("assignment", &SequenceTest::assignment));
	ts->addTest(new TestCaller<SequenceTest>
		    ("test_super_sequence",
		     &SequenceTest::test_super_sequence));
	return ts;
    }
};
