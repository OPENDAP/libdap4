
// -*- C++ -*-

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <strstream.h>
#include <string>

#include "Regex.h"

#include "Sequence.h"
#include "Int32.h"
#include "Str.h"

// #define DODS_DEBUG
#include "debug.h"

using namespace CppUnit;

static const char *s_as_string = 
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

class SequenceTest : public TestFixture {
private:
    Sequence *s, *sss, *ts, *tts;

public:
    SequenceTest() {}
    ~SequenceTest() {}

    void setUp() { 
	// Set up a simple sequence. Used to test ctor, assigment, et cetera.
	s = new Sequence("s");
	s->add_var(new Int32("i1"));
	s->add_var(new Str("str1"));
	s->add_var(new Int32("i2"));

	// Set up sss, used to test multi-level sequences
	sss = new Sequence("sss");
	sss->add_var(new Int32("i1"));
	
	ts = new Sequence("child_of_sss");
	ts->add_var(new Str("str1"));
	
	tts = new Sequence("child_of_child_of_sss");
	tts->add_var(new Int32("i2"));
	ts->add_var(tts);

	sss->add_var(ts);	// This has to be here because add_var adds
				// copies of its argument.
    } 

    void tearDown() { 
	delete s; s = 0;
	delete sss; sss = 0;
	delete ts; ts = 0;
	delete tts; tts = 0;
    }

    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }

    // Tests for methods
    void ctor_test() {
	CPPUNIT_ASSERT(re_match(s_regex, s->toString().c_str()));
    }

    void assignment() {
	Sequence ts2;

	ts2 = *s;
	DBG(cerr << "ts2: " << ts2.toString() << endl);
	CPPUNIT_ASSERT(re_match(s_regex, ts2.toString().c_str()));
    }

    void copy_ctor() {
	Sequence s2 = *s;
	CPPUNIT_ASSERT(re_match(s_regex, s2.toString().c_str()));
    }

    CPPUNIT_TEST_SUITE( SequenceTest );

    CPPUNIT_TEST(ctor_test);
    CPPUNIT_TEST(assignment);
    CPPUNIT_TEST(copy_ctor);

    CPPUNIT_TEST_SUITE_END();

};

CPPUNIT_TEST_SUITE_REGISTRATION(SequenceTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}
