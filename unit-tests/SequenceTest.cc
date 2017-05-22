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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <cstring>

//#define DODS_DEBUG

#include "DDS.h"
#include "ConstraintEvaluator.h"

#include "../tests/TestTypeFactory.h"
#include "../tests/TestSequence.h"
#include "../tests/TestInt32.h"
#include "../tests/TestStr.h"

#include "GNURegex.h"
#include "GetOpt.h"
#include "debug.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

using namespace CppUnit;
using namespace std;

int test_variable_sleep_interval;

//  Note: MS VC++ won't tolerate the embedded newlines in strings, hence the \n
//  is explicit.
static const char *s_as_string =
    "BaseType \\(0x.*\\):\n\
          _name: s\n\
          _type: Sequence\n\
          _dataset: \n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0.*\n\
          d_attr: 0x.*\n\
BaseType \\(0x.*\\):\n\
          _name: i1\n\
          _type: Int32\n\
          _dataset: \n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0x.*\n\
          d_attr: 0x.*\n\
BaseType \\(0x.*\\):\n\
          _name: str1\n\
          _type: String\n\
          _dataset: \n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0x.*\n\
          d_attr: 0x.*\n\
BaseType \\(0x.*\\):\n\
          _name: i2\n\
          _type: Int32\n\
          _dataset: \n\
          _read_p: 0\n\
          _send_p: 0\n\
          _synthesized_p: 0\n\
          d_parent: 0x.*\n\
          d_attr: 0x.*\n\
\n";

static Regex s_regex(s_as_string);

namespace libdap {

class SequenceTest: public TestFixture {
private:
    DDS *dds;
    TestSequence *s, *ss, *ps, *sss, *ts, *tts;

public:
    SequenceTest()
    {
    }
    ~SequenceTest()
    {
    }

    void setUp()
    {
        // Set up a simple sequence. Used to test ctor, assigment, et cetera.
        s = new TestSequence("s");
        s->add_var_nocopy(new TestInt32("i1"));
        s->add_var_nocopy(new TestStr("str1"));
        s->add_var_nocopy(new TestInt32("i2"));
        s->set_series_values(true);

        // Set ss, a two level sequence
        ss = new TestSequence("ss");
        ss->add_var_nocopy(new TestInt32("i1"));
        ss->set_series_values(true);

        ps = new TestSequence("child_of_ss");
        ps->add_var_nocopy(new TestInt32("i2"));
        ps->set_series_values(true);

        ss->add_var_nocopy(ps);

        // Set up sss, used to test multi-level sequences
        sss = new TestSequence("sss");
        sss->add_var_nocopy(new TestInt32("i1"));

        ts = new TestSequence("child_of_sss");
        ts->add_var_nocopy(new TestStr("str1"));

        tts = new TestSequence("child_of_child_of_sss");
        tts->add_var_nocopy(new TestInt32("i2"));
        ts->add_var_nocopy(tts);

        sss->add_var_nocopy(ts);	// This has to be here because add_var_nocopy adds
        // copies of its argument.
        sss->set_series_values(true);

        TestTypeFactory ttf;
        dds = new DDS(&ttf);
        dds->add_var_nocopy(s);
        dds->add_var_nocopy(ss);
        dds->add_var_nocopy(sss);
    }

    void tearDown()
    {
        delete dds;
        dds = 0;
    }

    bool re_match(Regex &r, const char *s)
    {
        int match_position = r.match(s, strlen(s));
        DBG(cerr << "match position: " << match_position << " string length: " << (int )strlen(s) << endl);
        return match_position == (int) strlen(s);
    }

    CPPUNIT_TEST_SUITE (SequenceTest);

    CPPUNIT_TEST (ctor_test);
    CPPUNIT_TEST (assignment);
    CPPUNIT_TEST (copy_ctor);
#if 0
    CPPUNIT_TEST(test_set_leaf_sequence);
    CPPUNIT_TEST(test_set_leaf_sequence2);
    CPPUNIT_TEST(test_set_leaf_sequence3);
    CPPUNIT_TEST(intern_data_for_leaf_test);
    CPPUNIT_TEST(intern_data_test1);
    CPPUNIT_TEST(intern_data_test2);
    CPPUNIT_TEST(intern_data_test3);
#endif

    CPPUNIT_TEST_SUITE_END();

#if 0
    // Tests for methods
    void intern_data_test1() {
        ConstraintEvaluator ce;
        s->set_send_p(true);
        s->set_leaf_sequence();
        try {
            s->intern_data(ce, *dds);

            // Test the first value in the first four rows
            BaseType *btp = s->var_value(0, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32);
            btp = s->var_value(1, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = s->var_value(2, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = s->var_value(3, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1048576);
            DBG(s->print_val(stdout));
        }
        catch (Error &e) {
            cerr << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error in transfer_data_for_leaf_test1()");
        }
    }

    void intern_data_test2() {
        ConstraintEvaluator ce;
        ss->set_send_p(true);
        ss->set_leaf_sequence();
        try {
            ss->intern_data(ce, *dds);
            DBG(ss->print_val(stdout));

            // Test the first value in the first four rows
            BaseType *btp = ss->var_value(0, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32);
            btp = ss->var_value(1, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = ss->var_value(2, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = ss->var_value(3, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1048576);

            // Look at some values in the inner sequence
            Sequence *sp = dynamic_cast<Sequence*>(ss->var_value(0, 1));
            CPPUNIT_ASSERT(sp);
            btp = sp->var_value(0, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32);
            btp = sp->var_value(1, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = sp->var_value(2, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = sp->var_value(3, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1048576);

            sp = dynamic_cast<Sequence*>(ss->var_value(3, 1));
            CPPUNIT_ASSERT(sp);
            btp = sp->var_value(0, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32);
            btp = sp->var_value(1, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = sp->var_value(2, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = sp->var_value(3, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1048576);
        }
        catch (Error &e) {
            cerr << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error in transfer_data_test2()");
        }
    }

    void intern_data_test3() {
        ConstraintEvaluator ce;
        sss->set_send_p(true);
        sss->set_leaf_sequence();
        try {
            sss->intern_data(ce, *dds);
            DBG(sss->print_val_by_rows(stdout, "", true, true));
            // Test the first value in the first four rows
            BaseType *btp = sss->var_value(0, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32);
            btp = sss->var_value(1, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = sss->var_value(2, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = sss->var_value(3, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1048576);

            // Look at some values in the inner-most sequence (skip the middle
            // sequence since I don't have a value() accessor for that yet.
            Sequence *sp = dynamic_cast<Sequence*>(sss->var_value(0, 1));
            CPPUNIT_ASSERT(sp);
            Sequence *ssp = dynamic_cast<Sequence*>(sp->var_value(0, 1));
            CPPUNIT_ASSERT(ssp);

            btp = ssp->var_value(0, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32);
            btp = ssp->var_value(1, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = ssp->var_value(2, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = ssp->var_value(3, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1048576);

            sp = dynamic_cast<Sequence*>(sss->var_value(3, 1));
            CPPUNIT_ASSERT(sp);
            ssp = dynamic_cast<Sequence*>(sp->var_value(3, 1));
            CPPUNIT_ASSERT(ssp);

            btp = ssp->var_value(0, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32);
            btp = ssp->var_value(1, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1024);
            btp = ssp->var_value(2, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 32768);
            btp = ssp->var_value(3, 0);
            CPPUNIT_ASSERT(btp && dynamic_cast<Int32&>(*btp).value() == 1048576);
        }
        catch (Error &e) {
            cerr << e.get_error_message() << endl;
            CPPUNIT_ASSERT(!"Error in transfer_data_test3()");
        }
    }

    void intern_data_for_leaf_test() {
        ConstraintEvaluator ce;
        s->set_send_p(true);
        try {
            Sequence::sequence_values_stack_t sequence_values_stack;
            sequence_values_stack.push(&s->d_values);
            s->intern_data_for_leaf(*dds, ce, sequence_values_stack);

            // Test the first value in the first four rows
            BaseType *btp = s->var_value(0, 0);
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
        sss->add_var_nocopy(ss);
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
        sss->set_send_p(true);// set send_p for whole seq
        // Now the lowest sequence is not longer to be sent. The middle sequence
        // is the lowest with fields to be sent and so should be the leaf.
        Sequence::Vars_iter i = sss->var_begin();
        Sequence *inner = dynamic_cast<Sequence*>(*++i);
        i = inner->var_begin();
        inner = dynamic_cast<Sequence*>(*++i);
        inner->set_send_p(false);// now clear send_p for the inner most seq
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
        // add_var_nocopy() _copies_ the object, so ps should not be used here.
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
#endif

    void assignment()
    {
        Sequence ts2 = *s;
        DBG(cerr << "ts2: " << ts2.toString() << endl);
        CPPUNIT_ASSERT(re_match(s_regex, ts2.toString().c_str()));
    }

    void ctor_test()
    {
        DBG(cerr << "s: " << s->toString() << endl);
        CPPUNIT_ASSERT(re_match(s_regex, s->toString().c_str()));
    }

    void copy_ctor()
    {
        Sequence s2 = *s;
        CPPUNIT_ASSERT(re_match(s_regex, s2.toString().c_str()));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (SequenceTest);

}

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;
    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: SequenceTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::SequenceTest::suite()->getTests();
            unsigned int prefix_len = libdap::SequenceTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            break;
        }
        default:
            break;
        }

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::SequenceTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
