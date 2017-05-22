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
#include <sstream>

//#define DODS_DEBUG

#include "DMR.h"
#include "D4Group.h"
#include "D4RValue.h"
#include "D4FilterClause.h"

#include "../tests/D4TestTypeFactory.h"
#include "../tests/TestD4Sequence.h"
#include "../tests/TestInt32.h"
#include "../tests/TestStr.h"
#include "../tests/TestFloat32.h"

#include "testFile.h"
#include "test_config.h"

#include "GNURegex.h"
#include "GetOpt.h"
#include "debug.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

using namespace CppUnit;
using namespace std;

int test_variable_sleep_interval;

const static string prefix = string(TEST_SRC_DIR) + "/D4-type-testsuite/";

const static string s_txt = "TestD4Sequence_s.txt";
const static string one_clause_txt = "TestD4Sequence_one_clause.txt";
const static string two_clause_txt = "TestD4Sequence_two_clause.txt";

namespace libdap {

class D4SequenceTest: public TestFixture {
private:
    TestD4Sequence *s;

public:
    D4SequenceTest() :
        s(0)
    {
    }
    ~D4SequenceTest()
    {
    }

    void setUp()
    {
        // Set up a simple sequence. Used to test ctor, assigment, et cetera.
        s = new TestD4Sequence("s");
        s->add_var_nocopy(new TestInt32("i32"));
        s->add_var_nocopy(new TestStr("str"));
        s->add_var_nocopy(new TestFloat32("f32"));
        s->set_series_values(true);
        s->set_send_p(true);
        s->set_length(7);
    }

    void tearDown()
    {
        delete s;
        s = 0;
    }

    void ctor_test()
    {
        s->intern_data();
        CPPUNIT_ASSERT(s->length() == 7);

        ostringstream oss;
        s->output_values(oss);

        DBG(cerr << "s: " << oss.str() << endl);
        DBG(cerr << "Baseline: " << readTestBaseline(prefix + s_txt) << endl);

        CPPUNIT_ASSERT(oss.str() == readTestBaseline(prefix + s_txt));
    }

    void assignment_test()
    {
        TestD4Sequence ts = *s;
        ts.intern_data();
        CPPUNIT_ASSERT(ts.length() == 7);

        ostringstream oss;
        ts.output_values(oss);

        DBG(cerr << "ts: " << oss.str() << endl);
        //CPPUNIT_ASSERT(oss.str() == readTestBaseline(prefix + s_txt));
    }

    void copy_ctor_test()
    {
        auto_ptr<TestD4Sequence> ts(new TestD4Sequence(*s));
        ts->intern_data();

        CPPUNIT_ASSERT(ts->length() == 7);

        ostringstream oss;
        ts->output_values(oss);

        DBG(cerr << "ts: " << oss.str() << endl);
        //CPPUNIT_ASSERT(oss.str() == readTestBaseline(prefix + s_txt));
    }

    void one_clause_test()
    {
        D4RValue *arg1 = new D4RValue(s->var("i32"));
        D4RValue *arg2 = new D4RValue((long long) 1024);
        s->clauses().add_clause(new D4FilterClause(D4FilterClause::equal, arg1, arg2));

        s->intern_data();

        DBG(cerr << "one_clause_test, s->length(): " << s->length() << endl);

        ostringstream oss;
        s->output_values(oss);

        DBG(cerr << "one_clause_test, s: " << oss.str() << endl);
        CPPUNIT_ASSERT(s->length() == 1);

        CPPUNIT_ASSERT(oss.str() == readTestBaseline(prefix + one_clause_txt));
    }

    void two_clause_test()
    {
        D4RValue *arg1 = new D4RValue(s->var("i32"));
        D4RValue *arg2 = new D4RValue((long long) 1024);
        s->clauses().add_clause(new D4FilterClause(D4FilterClause::greater_equal, arg1, arg2));

        D4RValue *arg1_2 = new D4RValue(s->var("i32"));
        D4RValue *arg2_2 = new D4RValue((long long) 1048576);
        s->clauses().add_clause(new D4FilterClause(D4FilterClause::less_equal, arg1_2, arg2_2));

        s->intern_data();

        DBG(cerr << "two_clause_test, s->length(): " << s->length() << endl);

        ostringstream oss;
        s->output_values(oss);

        DBG(cerr << "two_clause_test, s: " << oss.str() << endl);

        CPPUNIT_ASSERT(s->length() == 3);

        CPPUNIT_ASSERT(oss.str() == readTestBaseline(prefix + two_clause_txt));
    }

    void two_variable_test()
    {
        D4RValue *arg1 = new D4RValue(s->var("i32"));
        D4RValue *arg2 = new D4RValue((long long) 1024);
        s->clauses().add_clause(new D4FilterClause(D4FilterClause::greater_equal, arg1, arg2));

        D4RValue *arg1_2 = new D4RValue(s->var("f32"));
        D4RValue *arg2_2 = new D4RValue((long long) 0.0);
        s->clauses().add_clause(new D4FilterClause(D4FilterClause::less, arg1_2, arg2_2));

        s->intern_data();

        DBG(cerr << "two_variable_test, s->length(): " << s->length() << endl);

        ostringstream oss;
        s->output_values(oss);

        DBG(cerr << "two_variable_test, s: " << oss.str() << endl);

        CPPUNIT_ASSERT(s->length() == 1);
        // ...just happens to be hte same baseline file at one_clause_test()
        CPPUNIT_ASSERT(oss.str() == readTestBaseline(prefix + one_clause_txt));
    }

    CPPUNIT_TEST_SUITE (D4SequenceTest);

    CPPUNIT_TEST (ctor_test);
    CPPUNIT_TEST (assignment_test);
    CPPUNIT_TEST (copy_ctor_test);

    CPPUNIT_TEST (one_clause_test);
    CPPUNIT_TEST (two_clause_test);
    CPPUNIT_TEST (two_variable_test);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4SequenceTest);

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
            cerr << "Usage: D4SequenceTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::D4SequenceTest::suite()->getTests();
            unsigned int prefix_len = libdap::D4SequenceTest::suite()->getName().append("::").length();
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
            test = libdap::D4SequenceTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
