// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sstream>

#include "Byte.h"
#include "Int8.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"

#include "Float32.h"
#include "Float64.h"

#include "Str.h"
#include "Url.h"
//#include "Array.h"
#include "Structure.h"

#include "D4RValue.h"
#include "D4FilterClause.h"
#include "DMR.h"    // We need this because D4FilterClause::value needs it (sort of).

#include "GetOpt.h"
#include "util.h"
#include "debug.h"

//#include "testFile.h"
//#include "test_config.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) {x;} } while(false)

using namespace CppUnit;
using namespace std;

namespace libdap {

class D4FilterClauseTest: public TestFixture {
    // Build a DMR and build several D4RValue objects that reference its variables.
    // Then build several D4RValue objects that hold constants

private:
    Byte *byte;
    Float32 *f32;
    Str *str;
    Url *url;

    DMR dmr;

public:
    D4FilterClauseTest() :
        byte(0), f32(0), str(0), url(0)
    {
    }
    ~D4FilterClauseTest()
    {
    }

    void setUp()
    {
        byte = new Byte("byte");
        byte->set_value(17);

        f32 = new Float32("f32");
        f32->set_value(3.1415);

        str = new Str("str");
        str->set_value("Einstein");

        url = new Url("url");
        url->set_value("https://github.com/opendap");
    }

    void tearDown()
    {
        delete byte;
        delete str;
    }

    // FilterClauseList tests further down...

    void Byte_and_long_long_test()
    {
        D4RValue *arg1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));
        CPPUNIT_ASSERT(less->value(dmr));

        D4RValue *arg2_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2_2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> greater(new D4FilterClause(D4FilterClause::greater, arg2_1, arg2_2));
        CPPUNIT_ASSERT(!greater->value(dmr));

        D4RValue *arg3_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg3_2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> equal(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));
        CPPUNIT_ASSERT(!equal->value(dmr));

        D4RValue *arg4_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg4_2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> not_equal(new D4FilterClause(D4FilterClause::not_equal, arg4_1, arg4_2));
        CPPUNIT_ASSERT(not_equal->value(dmr));
    }

    // This version uses the D4FilterClause::value() and not value(DMR&) method
    void Byte_and_long_long_test_2()
    {
        D4RValue *arg1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));
        CPPUNIT_ASSERT(less->value());

        D4RValue *arg2_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2_2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> greater(new D4FilterClause(D4FilterClause::greater, arg2_1, arg2_2));
        CPPUNIT_ASSERT(!greater->value());

        D4RValue *arg3_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg3_2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> equal(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));
        CPPUNIT_ASSERT(!equal->value());

        D4RValue *arg4_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg4_2 = new D4RValue((long long) 21);

        auto_ptr<D4FilterClause> not_equal(new D4FilterClause(D4FilterClause::not_equal, arg4_1, arg4_2));
        CPPUNIT_ASSERT(not_equal->value());
    }

    void Byte_and_double_test()
    {
        D4RValue *arg1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2 = new D4RValue((double) 21.0);

        auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));
        CPPUNIT_ASSERT(less->value(dmr));

        D4RValue *arg2_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2_2 = new D4RValue((double) 21);

        auto_ptr<D4FilterClause> greater(new D4FilterClause(D4FilterClause::greater, arg2_1, arg2_2));
        CPPUNIT_ASSERT(!greater->value(dmr));

        D4RValue *arg3_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg3_2 = new D4RValue((double) 21);

        auto_ptr<D4FilterClause> equal(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));
        CPPUNIT_ASSERT(!equal->value(dmr));

        D4RValue *arg4_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg4_2 = new D4RValue((double) 21);

        auto_ptr<D4FilterClause> not_equal(new D4FilterClause(D4FilterClause::not_equal, arg4_1, arg4_2));
        CPPUNIT_ASSERT(not_equal->value(dmr));
    }

    /** @defgroup type_conv Tests for type promotion
     * @{
     */
    void Byte_and_int_test()
    {
        D4RValue *arg1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2 = new D4RValue((unsigned long long) (21));

        auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));
        CPPUNIT_ASSERT(less->value());

    }

    void Byte_and_float_test()
    {
        D4RValue *arg1 = new D4RValue(byte);    // holds 17
        D4RValue *arg2 = new D4RValue((float) 21.0);

        auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));
        CPPUNIT_ASSERT(less->value());

    }
    /** @} */

    // this uses a mix of value() and value(dmr), just for cover both cases
    void Str_and_str_test()
    {
        D4RValue *arg1 = new D4RValue(str);
        D4RValue *arg2 = new D4RValue(string("Tesla"));

        auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));
        CPPUNIT_ASSERT(less->value());

        D4RValue *arg2_1 = new D4RValue(str);
        D4RValue *arg2_2 = new D4RValue("Tesla");

        auto_ptr<D4FilterClause> greater(new D4FilterClause(D4FilterClause::greater, arg2_1, arg2_2));
        CPPUNIT_ASSERT(!greater->value());

        D4RValue *arg3_1 = new D4RValue(str);
        D4RValue *arg3_2 = new D4RValue("Tesla");

        auto_ptr<D4FilterClause> equal(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));
        CPPUNIT_ASSERT(!equal->value(dmr));

        D4RValue *arg4_1 = new D4RValue(str);
        D4RValue *arg4_2 = new D4RValue("Tesla");

        auto_ptr<D4FilterClause> not_equal(new D4FilterClause(D4FilterClause::not_equal, arg4_1, arg4_2));
        CPPUNIT_ASSERT(not_equal->value(dmr));
    }

    void Str_and_match_test()
    {
        D4RValue *arg1 = new D4RValue(str);
        D4RValue *arg2 = new D4RValue(string("E.*n"));

        auto_ptr<D4FilterClause> match(new D4FilterClause(D4FilterClause::match, arg1, arg2));
        CPPUNIT_ASSERT(match->value());
    }

    void Str_and_number_error_test()
    {
        D4RValue *arg1 = new D4RValue(str);
        D4RValue *arg2 = new D4RValue((long long) 21);

        try {
            auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));
            // The Filter Clause instance is built OK, but the value() method
            // will balk at this comparison. jhrg 4/21/16
            DBG(cerr << "built filter clause instance" << endl);

            less->value();
            CPPUNIT_FAIL("Expected error");
        }
        catch (Error &e) {
            DBG(cerr << "Caught error: " + e.get_error_message() << endl);
            CPPUNIT_ASSERT("Caught error");
        }
    }

    void Byte_and_string_error_test()
    {
        D4RValue *arg1 = new D4RValue(byte);
        D4RValue *arg2 = new D4RValue("Tesla");

        try {
            auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));

            CPPUNIT_ASSERT(less->value());
            CPPUNIT_FAIL("Expected error");
        }
        catch (Error &e) {
            DBG(cerr << "Caught error: " + e.get_error_message() << endl);
            CPPUNIT_ASSERT("Caught error");
        }
    }

    void Structure_and_string_error_test()
    {
        auto_ptr<Structure> s(new Structure("s"));
        s->add_var(byte); // copy the object
        D4RValue *arg1 = new D4RValue(s.get()); // BaseType*s are not free'd by D4RValue
        D4RValue *arg2 = new D4RValue("Tesla");

        try {
            auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));

            CPPUNIT_ASSERT(less->value());
            CPPUNIT_FAIL("Expected error");
        }
        catch (Error &e) {
            DBG(cerr << "Caught error: " + e.get_error_message() << endl);
            CPPUNIT_ASSERT("Caught error");
        }
    }

    // There's no way this will get past the parser, but ...
    void Byte_and_Structure_error_test()
    {
        auto_ptr<Structure> s(new Structure("s"));
        s->add_var(str); // copy the object
        D4RValue *arg1 = new D4RValue(byte);
        D4RValue *arg2 = new D4RValue(s.get());

        try {
            auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));

            CPPUNIT_ASSERT(less->value());
            CPPUNIT_FAIL("Expected error");
        }
        catch (Error &e) {
            DBG(cerr << "Caught error: " + e.get_error_message() << endl);
            CPPUNIT_ASSERT("Caught error");
        }
    }

    // There's no way this will get past the parser, but ...
    void Str_and_Structure_error_test()
    {
        auto_ptr<Structure> s(new Structure("s"));
        s->add_var(str); // copy the object
        D4RValue *arg1 = new D4RValue(str);
        D4RValue *arg2 = new D4RValue(s.get());

        try {
            auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg1, arg2));

            CPPUNIT_ASSERT(less->value());
            CPPUNIT_FAIL("Expected error");
        }
        catch (Error &e) {
            DBG(cerr << "Caught error: " + e.get_error_message() << endl);
            CPPUNIT_ASSERT("Caught error");
        }
    }

    // test Url and Float32
    void grab_bag_test()
    {
        D4RValue *arg2_1 = new D4RValue(f32);
        D4RValue *arg2_2 = new D4RValue(17.0);

        auto_ptr<D4FilterClause> less(new D4FilterClause(D4FilterClause::less, arg2_1, arg2_2));
        CPPUNIT_ASSERT(less->value());

        D4RValue *arg3_1 = new D4RValue(url);
        D4RValue *arg3_2 = new D4RValue("https://github.com/opendap");

        auto_ptr<D4FilterClause> equal(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));
        CPPUNIT_ASSERT(equal->value(dmr));

        D4RValue *arg4_1 = new D4RValue(url);
        D4RValue *arg4_2 = new D4RValue("https://.*dap$");

        auto_ptr<D4FilterClause> not_equal(new D4FilterClause(D4FilterClause::not_equal, arg4_1, arg4_2));
        CPPUNIT_ASSERT(not_equal->value(dmr));

        D4RValue *arg5_1 = new D4RValue(url);
        D4RValue *arg5_2 = new D4RValue("https://.*dap$");

        auto_ptr<D4FilterClause> match(new D4FilterClause(D4FilterClause::match, arg5_1, arg5_2));
        CPPUNIT_ASSERT(match->value(dmr));
    }

    void float_test()
    {
        D4RValue *arg2_1 = new D4RValue(f32);
        D4RValue *arg2_2 = new D4RValue(3.1415);

        auto_ptr<D4FilterClause> clause(new D4FilterClause(D4FilterClause::equal, arg2_1, arg2_2));
        CPPUNIT_ASSERT(clause->value());
    }

    void float_test_2()
    {
        D4RValue *arg2_1 = new D4RValue(f32);
        D4RValue *arg2_2 = new D4RValue(3.1415);

        auto_ptr<D4FilterClause> clause(new D4FilterClause(D4FilterClause::greater_equal, arg2_1, arg2_2));
        CPPUNIT_ASSERT(clause->value());
    }

    void float_test_3()
    {
        D4RValue *arg2_1 = new D4RValue(f32);
        D4RValue *arg2_2 = new D4RValue(3.1415);

        auto_ptr<D4FilterClause> clause(new D4FilterClause(D4FilterClause::less_equal, arg2_1, arg2_2));
        CPPUNIT_ASSERT(clause->value());
    }

    void int_test()
    {
        auto_ptr<Int8> i8(new Int8(""));
        i8->set_value(17);
        D4RValue *arg2_1 = new D4RValue(i8.get());
        D4RValue *arg2_2 = new D4RValue((long long) 17);

        auto_ptr<D4FilterClause> clause(new D4FilterClause(D4FilterClause::equal, arg2_1, arg2_2));
        CPPUNIT_ASSERT(clause->value());
    }

    void true_clauses_test()
    {
        // Testing this as a pointer since that's how it will be stored in D4Sequence
        auto_ptr<D4FilterClauseList> clauses(new D4FilterClauseList());

        D4RValue *arg1_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg1_2 = new D4RValue((double) 21.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::less, arg1_1, arg1_2));

        D4RValue *arg2_1 = new D4RValue(f32);       // holds pi
        D4RValue *arg2_2 = new D4RValue(17.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::less, arg2_1, arg2_2));

        D4RValue *arg3_1 = new D4RValue(url);
        D4RValue *arg3_2 = new D4RValue("https://github.com/opendap");
        clauses->add_clause(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));

        D4RValue *arg4_1 = new D4RValue(url);
        D4RValue *arg4_2 = new D4RValue("https://.*dap$");
        clauses->add_clause(new D4FilterClause(D4FilterClause::match, arg4_1, arg4_2));

        CPPUNIT_ASSERT(clauses->size() == 4);
        CPPUNIT_ASSERT(clauses->value(dmr));
        CPPUNIT_ASSERT(clauses->value());
    }

    // This should return true
    void no_clauses_test()
    {
        auto_ptr<D4FilterClauseList> clauses(new D4FilterClauseList());

        CPPUNIT_ASSERT(clauses->size() == 0);
        CPPUNIT_ASSERT(clauses->value(dmr));
        CPPUNIT_ASSERT(clauses->value());
    }

    void false_clauses_test()
    {
        auto_ptr<D4FilterClauseList> clauses(new D4FilterClauseList());

        D4RValue *arg1_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg1_2 = new D4RValue((double) 21.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::less, arg1_1, arg1_2));

        // This clause will fail
        D4RValue *arg2_1 = new D4RValue(f32);       // holds pi
        D4RValue *arg2_2 = new D4RValue(17.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::greater, arg2_1, arg2_2));

        D4RValue *arg3_1 = new D4RValue(url);
        D4RValue *arg3_2 = new D4RValue("https://github.com/opendap");
        clauses->add_clause(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));

        D4RValue *arg4_1 = new D4RValue(url);
        D4RValue *arg4_2 = new D4RValue("https://.*dap$");
        clauses->add_clause(new D4FilterClause(D4FilterClause::match, arg4_1, arg4_2));

        CPPUNIT_ASSERT(clauses->size() == 4);
        CPPUNIT_ASSERT(clauses->value(dmr) == false);
        CPPUNIT_ASSERT(clauses->value() == false);
    }

    void evaluation_order_test()
    {
        auto_ptr<D4FilterClauseList> clauses(new D4FilterClauseList());

        D4RValue *arg1_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg1_2 = new D4RValue((double) 21.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::less, arg1_1, arg1_2));

        // This clause will fail and we should not get to the next clause, which will
        // throw and exception.
        D4RValue *arg2_1 = new D4RValue(f32);       // holds pi
        D4RValue *arg2_2 = new D4RValue(17.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::greater, arg2_1, arg2_2));

        D4RValue *arg3_1 = new D4RValue(url);
        D4RValue *arg3_2 = new D4RValue(17.0); // Error - mismatched types
        clauses->add_clause(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));

        D4RValue *arg4_1 = new D4RValue(url);
        D4RValue *arg4_2 = new D4RValue("https://.*dap$");
        clauses->add_clause(new D4FilterClause(D4FilterClause::match, arg4_1, arg4_2));

        try {
            CPPUNIT_ASSERT(clauses->size() == 4);
            CPPUNIT_ASSERT(clauses->value(dmr) == false);
            CPPUNIT_ASSERT(clauses->value() == false);
        }
        catch (Error &e) {
            DBG(cerr << "Caught error: " + e.get_error_message() << endl);
            CPPUNIT_FAIL("Exception, but the thrid clause should not have been evaluated");

        }
    }

    void evaluation_order_test_2()
    {
        auto_ptr<D4FilterClauseList> clauses(new D4FilterClauseList());

        D4RValue *arg1_1 = new D4RValue(byte);    // holds 17
        D4RValue *arg1_2 = new D4RValue((double) 21.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::less, arg1_1, arg1_2));

        // This clause will *pass* and we *should* get to the next clause, which will
        // throw and exception.
        D4RValue *arg2_1 = new D4RValue(f32);       // holds pi
        D4RValue *arg2_2 = new D4RValue(17.0);
        clauses->add_clause(new D4FilterClause(D4FilterClause::less, arg2_1, arg2_2));

        D4RValue *arg3_1 = new D4RValue(url);
        D4RValue *arg3_2 = new D4RValue(17.0); // Error - mismatched types
        clauses->add_clause(new D4FilterClause(D4FilterClause::equal, arg3_1, arg3_2));

        D4RValue *arg4_1 = new D4RValue(url);
        D4RValue *arg4_2 = new D4RValue("https://.*dap$");
        clauses->add_clause(new D4FilterClause(D4FilterClause::match, arg4_1, arg4_2));

        try {
            CPPUNIT_ASSERT(clauses->size() == 4);
            CPPUNIT_ASSERT(clauses->value(dmr));

            CPPUNIT_FAIL("Expected the third clause to throw an exception.");
        }
        catch (Error &e) {
            DBG(cerr << "Caught error: " + e.get_error_message() << endl);
            CPPUNIT_ASSERT("Expected exception found.");
        }
    }

    CPPUNIT_TEST_SUITE (D4FilterClauseTest);

    CPPUNIT_TEST (Byte_and_long_long_test);
    CPPUNIT_TEST (Byte_and_long_long_test_2);
    CPPUNIT_TEST (Byte_and_double_test);
    // These float --> double, etc.
    CPPUNIT_TEST (Byte_and_int_test);
    CPPUNIT_TEST (Byte_and_float_test);

    CPPUNIT_TEST (Str_and_str_test);
    CPPUNIT_TEST (Str_and_match_test);
    CPPUNIT_TEST (Str_and_number_error_test);
    CPPUNIT_TEST (Byte_and_string_error_test);
    CPPUNIT_TEST (Structure_and_string_error_test);
    CPPUNIT_TEST (Byte_and_Structure_error_test);
    CPPUNIT_TEST (Str_and_Structure_error_test);

    CPPUNIT_TEST (grab_bag_test);
    CPPUNIT_TEST (float_test);
    CPPUNIT_TEST (float_test_2);
    CPPUNIT_TEST (float_test_3);
    CPPUNIT_TEST (int_test);

    // FilterClauseList tests
    CPPUNIT_TEST (true_clauses_test);
    CPPUNIT_TEST (no_clauses_test);
    CPPUNIT_TEST (false_clauses_test);
    CPPUNIT_TEST (evaluation_order_test);
    CPPUNIT_TEST (evaluation_order_test_2);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4FilterClauseTest);

} // namepsace libdap

int main(int argc, char*argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        case 'h': {     // help - show test names
            cerr << "Usage: D4FilterClauseTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::D4FilterClauseTest::suite()->getTests();
            unsigned int prefix_len = libdap::D4FilterClauseTest::suite()->getName().append("::").length();
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
            test = libdap::D4FilterClauseTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}

