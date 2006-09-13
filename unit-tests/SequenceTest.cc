
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

// #define DODS_DEBUG
#include "DDS.h"
#include "ConstraintEvaluator.h"

#include "../tests/TestTypeFactory.h"
#include "../tests/TestSequence.h"
#include "../tests/TestInt32.h"
#include "../tests/TestStr.h"

#include "GNURegex.h"
#include "debug.h"

using namespace CppUnit;
using namespace std;

int test_variable_sleep_interval;

//  Note: MS VC++ won't tolerate the embedded newlines in strings, hence the \n
//  is explicit.
static const char *s_as_string = \
"BaseType \\(0x.*\\):\n\
          _name: s\n\
          _type: Sequence\n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0\n\
          d_attr: 0x.*\n\
BaseType \\(0x.*\\):\n\
          _name: i1\n\
          _type: Int32\n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0x.*\n\
          d_attr: 0x.*\n\
BaseType \\(0x.*\\):\n\
          _name: str1\n\
          _type: String\n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0x.*\n\
          d_attr: 0x.*\n\
BaseType \\(0x.*\\):\n\
          _name: i2\n\
          _type: Int32\n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0x.*\n\
          d_attr: 0x.*\n\
\n";

static Regex s_regex(s_as_string);

class SequenceTest : public TestFixture {
private:
    DDS *dds;
    TestSequence *s, *ss, *ps, *sss, *ts, *tts;

public:
    SequenceTest() {}
    ~SequenceTest() {}

    void setUp() { 
	// Set up a simple sequence. Used to test ctor, assigment, et cetera.
	s = new TestSequence("s");
	s->add_var(new TestInt32("i1"));
	s->add_var(new TestStr("str1"));
	s->add_var(new TestInt32("i2"));
        s->set_series_values(true);        

        // Set ss, a two level sequence
        ss = new TestSequence("ss");
        ss->add_var(new TestInt32("i1"));
        
        ps = new TestSequence("child_of_ss");
        ps->add_var(new TestInt32("i2"));
        
        ss->add_var(ps);
        
	// Set up sss, used to test multi-level sequences
	sss = new TestSequence("sss");
	sss->add_var(new TestInt32("i1"));
	
	ts = new TestSequence("child_of_sss");
	ts->add_var(new TestStr("str1"));
	
	tts = new TestSequence("child_of_child_of_sss");
	tts->add_var(new TestInt32("i2"));
	ts->add_var(tts);

	sss->add_var(ts);	// This has to be here because add_var adds
				// copies of its argument.
        TestTypeFactory ttf;
        dds = new DDS(&ttf);
        dds->add_var(s);
        dds->add_var(ss);
        dds->add_var(sss);
    } 

    void tearDown() { 
	delete s; s = 0;
        delete ss; ss = 0;
        delete ps; ps = 0;
	delete sss; sss = 0;
	delete ts; ts = 0;
	delete tts; tts = 0;
        delete dds; dds = 0;
    }

    bool re_match(Regex &r, const char *s) {
	int match_position = r.match(s, strlen(s));
	DBG(cerr << "match position: " << match_position 
	    << " string length: " << (int)strlen(s) << endl);
	return match_position == (int)strlen(s);
    }

    CPPUNIT_TEST_SUITE( SequenceTest );

    CPPUNIT_TEST(ctor_test);
    CPPUNIT_TEST(assignment);
    CPPUNIT_TEST(copy_ctor);
    CPPUNIT_TEST(test_set_leaf_sequence);
    CPPUNIT_TEST(test_set_leaf_sequence2);
    CPPUNIT_TEST(test_set_leaf_sequence3);
    
    CPPUNIT_TEST(transfer_data_for_leaf_test);

    CPPUNIT_TEST_SUITE_END();

    // Tests for methods
    void transfer_data_for_leaf_test() {
        ConstraintEvaluator ce;
        s->set_send_p(true);
        try {
            s->transfer_data_for_leaf("dummy", *dds, ce, true);
            BaseType *btp;
            
            // Test the first value in the first four rows
            btp = s->var_value(0, 0);
            CPPUNIT_ASSERT(dynamic_cast<Int32&>(*btp).value() == 32);
            btp = s->var_value(1, 0);
            CPPUNIT_ASSERT(dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = s->var_value(2, 0);
            CPPUNIT_ASSERT(dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = s->var_value(3, 0);
            CPPUNIT_ASSERT(dynamic_cast<Int32&>(*btp).value() == 1048576);
            DBG(s->print_val(stdout));
        }
        catch (Error &e) {
            cerr << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error in transfer_data_for_leaf_test()");
        }
    }
    
    void test_set_leaf_sequence3() {
        // Test for the rejection of a Sequence with two sequences in it.
        sss->add_var(ss);
        sss->set_send_p(true);
        try {
             sss->set_leaf_sequence(1);
             CPPUNIT_ASSERT(!"Should have thrown Error");
        }
        catch (Error &e) {
             cerr << e.get_error_message() << endl;
             CPPUNIT_ASSERT("Caught Error");
        }
    }

    void test_set_leaf_sequence2() {
        // Three level sequence
        sss->set_send_p(true);          // set send_p for whole seq
        // Now the lowest sequence is not longer to be sent. The middle sequence
        // is the lowest with fields to be sent and so should be the leaf.
        Sequence::Vars_iter i = sss->var_begin();
        Sequence *inner = dynamic_cast<Sequence*>(*++i);
        i = inner->var_begin();
        inner = dynamic_cast<Sequence*>(*++i);
        inner->set_send_p(false);       // now clear send_p for the inner most seq
        sss->set_leaf_sequence(1);

        CPPUNIT_ASSERT(!sss->is_leaf_sequence());
        
        i = sss->var_begin();
        inner = dynamic_cast<Sequence*>(*++i);
        CPPUNIT_ASSERT(inner && inner->is_leaf_sequence());

        i = inner->var_begin();
        Sequence *inner2 = dynamic_cast<Sequence*>(*++i);
        CPPUNIT_ASSERT(inner2 && !inner2->is_leaf_sequence());
    }

    void test_set_leaf_sequence() {
        // One level sequence
        s->set_send_p(true);
        s->set_leaf_sequence(1);
        CPPUNIT_ASSERT(s->is_leaf_sequence());
        
        // Two level sequence
        ss->set_send_p(true);
        ss->set_leaf_sequence(1);
        CPPUNIT_ASSERT(!ss->is_leaf_sequence());
        // add_var() _copies_ the object, so ps should not be used here.
        Sequence::Vars_iter i = ss->var_begin();
        Sequence *inner = dynamic_cast<Sequence*>(*++i);
        CPPUNIT_ASSERT(inner->type() == dods_sequence_c && inner->is_leaf_sequence());
        
        // Three level sequence
        sss->set_send_p(true);
        sss->set_leaf_sequence(1);
        CPPUNIT_ASSERT(!sss->is_leaf_sequence());
        
        i = sss->var_begin();
        inner = dynamic_cast<Sequence*>(*++i);
        CPPUNIT_ASSERT(inner && !inner->is_leaf_sequence());

        i = inner->var_begin();
        Sequence *inner2 = dynamic_cast<Sequence*>(*++i);
        CPPUNIT_ASSERT(inner2 && inner2->is_leaf_sequence());
    }
    
    void ctor_test() {
	DBG(cerr << "s: " << s->toString() << endl);
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(SequenceTest);

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return wasSuccessful ? 0 : 1;
}
