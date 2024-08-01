// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2024 OPeNDAP, Inc.
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
//
// Created by James Gallagher on 2/23/24.
//

#include <iostream>

#include "Byte.h"
#include "D4BaseTypeFactory.h"
#include "D4Group.h"
#include "DMR.h"

#include "unit-tests/run_tests_cppunit.h"

#include "D4ConstraintEvaluator.h"

using namespace CppUnit;
using namespace std;

namespace libdap {

class MockDMR : public DMR {
public:
    D4BaseTypeFactory d_factory;
    MockDMR() : DMR(&d_factory) {
        root()->add_var_nocopy(new Byte("x"));
        root()->add_var_nocopy(new Byte("y"));
    }

    ~MockDMR() override = default;
};

class D4ConstraintEvaluatorTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(D4ConstraintEvaluatorTest);

    CPPUNIT_TEST(test_parse_valid_expression);
    CPPUNIT_TEST(test_parse_empty_expression);
    CPPUNIT_TEST(test_parse_invalid_expression);

    CPPUNIT_TEST(test_throw_not_found_prevent_xxs);
    CPPUNIT_TEST(test_throw_not_found_no_prevent_xxs);
    CPPUNIT_TEST(test_throw_not_array_prevent_xxs);
    CPPUNIT_TEST(test_throw_not_array_no_prevent_xxs);

    CPPUNIT_TEST_SUITE_END();

public:
    void test_parse_valid_expression() {
        DBG(cerr << "test_parse_valid_expression" << endl);
        std::string validExpr = "x;y";
        MockDMR dmr;
        D4ConstraintEvaluator evaluator(&dmr);
        CPPUNIT_ASSERT(evaluator.parse(validExpr));
    }

    void test_parse_empty_expression() {
        DBG(cerr << "test_parse_empty_expression" << endl);
        std::string emptyExpr = "";
        MockDMR dmr;
        D4ConstraintEvaluator evaluator(&dmr);
        CPPUNIT_ASSERT(evaluator.parse(emptyExpr));
    }

    void test_parse_invalid_expression() {
        DBG(cerr << "test_parse_invalid_expression" << endl);
        std::string invalidExpr = "x$y";
        MockDMR dmr;
        D4ConstraintEvaluator evaluator(&dmr);
        CPPUNIT_ASSERT_THROW(evaluator.parse(invalidExpr), Error);
    }

    void test_throw_not_found_prevent_xxs() {
        // Test throw_not_found with PREVENT_XXS_VIA_CE enabled
        CPPUNIT_ASSERT_THROW(D4ConstraintEvaluator::throw_not_found("", ""), Error);
    }

    void test_throw_not_found_no_prevent_xxs() {
        // Test throw_not_found with PREVENT_XXS_VIA_CE disabled
#ifndef PREVENT_XXS_VIA_CE
        // Expected behavior only applies if PREVENT_XXS_VIA_CE is not defined
        try {
            D4ConstraintEvaluator::throw_not_found("id", "ident");
            CPPUNIT_FAIL("Expected throw_not_found to throw an exception");
        } catch (const Error &e) {
            // Verify specific error message and details (if applicable)
            CPPUNIT_ASSERT_EQUAL(e.get_error_code(), no_such_variable);
            // Additional assertions for message details based on implementation
        }
#endif
    }

    void test_throw_not_array_prevent_xxs() {
        // Test throw_not_array with PREVENT_XXS_VIA_CE enabled
        CPPUNIT_ASSERT_THROW(D4ConstraintEvaluator::throw_not_array("", ""), Error);
    }

    void test_throw_not_array_no_prevent_xxs() {
        // Test throw_not_array with PREVENT_XXS_VIA_CE disabled
#ifndef PREVENT_XXS_VIA_CE
        // Expected behavior only applies if PREVENT_XXS_VIA_CE is not defined
        try {
            D4ConstraintEvaluator::throw_not_array("id", "ident");
            CPPUNIT_FAIL("Expected throw_not_array to throw an exception");
        } catch (const Error &e) {
            // Verify specific error message and details (if applicable)
            CPPUNIT_ASSERT_EQUAL(e.get_error_code(), no_such_variable);
            // Additional assertions for message details based on implementation
        }
#endif
    }
};

} // namespace libdap

CPPUNIT_TEST_SUITE_REGISTRATION(libdap::D4ConstraintEvaluatorTest);

int main(int argc, char *argv[]) { return run_tests<libdap::D4ConstraintEvaluatorTest>(argc, argv) ? 0 : 1; }
