
// Tests for the AttrTable class.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Regex.h>

#include "TestSequence.h"
#include "TestInt32.h"
#include "TestStr.h"
#include "SequenceTest.h"
#define DODS_DEBUG
#include "debug.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SequenceTest);

using namespace CppUnit;

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

SequenceTest::SequenceTest()
{
}

SequenceTest::~SequenceTest()
{
}

void
SequenceTest::setUp() 
{ 
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

    sss->add_var(ts);		// This has to be here because add_var adds
				// copies of its argument.
} 

void
SequenceTest::tearDown() 
{ 
    delete s; s = 0;
    delete sss; sss = 0;
}

// Tests for methods
void
SequenceTest::ctor_test() 
{
    CPPUNIT_ASSERT(re_match(s_regex, s->toString().c_str()));
}

void
SequenceTest::assignment() 
{
    TestSequence ts2;

    ts2 = *s;
    DBG(cerr << "ts2: " << ts2.toString() << endl);
    CPPUNIT_ASSERT(re_match(s_regex, ts2.toString().c_str()));
}

void
SequenceTest::copy_ctor() 
{
    TestSequence s2 = *s;
    CPPUNIT_ASSERT(re_match(s_regex, s2.toString().c_str()));
}

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}
