// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>

#include "BaseTypeFactory.h"
#include "ConstraintEvaluator.h"
#include "DDS.h"
#include "Int32.h"
#include "Error.h"

#include "run_tests_cppunit.h"

using namespace CppUnit;
using namespace libdap;
using namespace std;

class ConstraintEvaluatorTest : public TestFixture {
    CPPUNIT_TEST_SUITE(ConstraintEvaluatorTest);
    CPPUNIT_TEST(test_simple_and_expression);
    CPPUNIT_TEST(test_invalid_expression_throws);
    CPPUNIT_TEST_SUITE_END();

public:
    void test_simple_and_expression() {
        BaseTypeFactory factory;
        DDS dds(&factory);

        Int32 *a = new Int32("a");
        a->set_value(5);
        dds.add_var_nocopy(a);

        ConstraintEvaluator eval;
        eval.parse_constraint("a&a>3", dds);
        CPPUNIT_ASSERT(eval.eval_selection(dds, ""));

        ConstraintEvaluator eval1;
        eval1.parse_constraint("a&a>3&a<10", dds);
        CPPUNIT_ASSERT(eval1.eval_selection(dds, ""));

        ConstraintEvaluator eval2;
        eval2.parse_constraint("&a>7&a<10", dds);
        CPPUNIT_ASSERT(!eval2.eval_selection(dds, ""));
    }

    void test_invalid_expression_throws() {
        BaseTypeFactory factory;
        DDS dds(&factory);

        Int32 *a = new Int32("a");
        a->set_value(5);
        dds.add_var_nocopy(a);

        ConstraintEvaluator eval;
        CPPUNIT_ASSERT_THROW(eval.parse_constraint("a>>3", dds), Error);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ConstraintEvaluatorTest);

int main(int argc, char *argv[]) { return run_tests<ConstraintEvaluatorTest>(argc, argv) ? 0 : 1; }
