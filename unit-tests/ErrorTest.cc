// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2025 OPeNDAP, Inc.
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
// Written by Gemini Flash 2.0. jhrg 5/9/25

#include "Error.h" // Include the Error class definition
#include "InternalErr.h"
#include "run_tests_cppunit.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cstdio> // For FILE*
#include <iostream>
#include <sstream>
#include <string> // Include string

using namespace std;
using namespace libdap;

class ErrorTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ErrorTest);
    CPPUNIT_TEST(test_constructor_default);
    CPPUNIT_TEST(test_constructor_code_message);
    CPPUNIT_TEST(test_constructor_message);
    CPPUNIT_TEST(test_copy_constructor);
    CPPUNIT_TEST(test_assignment_operator);
    CPPUNIT_TEST(test_assignment_self);
    CPPUNIT_TEST(test_ok);
    CPPUNIT_TEST(test_parse);
    CPPUNIT_TEST(test_parse_null_file); // Added test for null file pointer
    CPPUNIT_TEST(test_print_file);
    CPPUNIT_TEST(test_print_ostream);
    CPPUNIT_TEST(test_get_error_code);
    CPPUNIT_TEST(test_get_error_message);
    CPPUNIT_TEST_FAIL(test_set_error_code);
    CPPUNIT_TEST(test_set_error_message);
    CPPUNIT_TEST(test_get_file);
    CPPUNIT_TEST(test_set_file);
    CPPUNIT_TEST(test_get_line);
    CPPUNIT_TEST(test_set_line);
    CPPUNIT_TEST(test_what);
    CPPUNIT_TEST_SUITE_END();

public:
    void test_constructor_default();
    void test_constructor_code_message();
    void test_constructor_message();
    void test_copy_constructor();
    void test_assignment_operator();
    void test_assignment_self();
    void test_ok();
    void test_parse();
    void test_parse_null_file(); // Added test declaration
    void test_print_file();
    void test_print_ostream();
    void test_get_error_code();
    void test_get_error_message();
    void test_set_error_code();
    void test_set_error_message();
    void test_get_file();
    void test_set_file();
    void test_get_line();
    void test_set_line();
    void test_what();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorTest);

void ErrorTest::test_constructor_default() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error;
    CPPUNIT_ASSERT_EQUAL(undefined_error, error.get_error_code());
    CPPUNIT_ASSERT_EQUAL(string(""), error.get_error_message());
    CPPUNIT_ASSERT(error.OK());
}

void ErrorTest::test_constructor_code_message() {
    DBG(cerr << "Test: " << __func__ << "\n");
    ErrorCode code = no_such_file;
    string message = "No such file exists";
    string file = "test_file.cpp";
    int line = 123;
    Error error(code, message, file, line);
    CPPUNIT_ASSERT_EQUAL(code, error.get_error_code());
    CPPUNIT_ASSERT_EQUAL(message, error.get_error_message());
    CPPUNIT_ASSERT_EQUAL(file, error.get_file());
    CPPUNIT_ASSERT_EQUAL(line, error.get_line());
    CPPUNIT_ASSERT(error.OK());
}

void ErrorTest::test_constructor_message() {
    DBG(cerr << "Test: " << __func__ << "\n");
    string message = "Invalid argument";
    string file = "another_file.cpp";
    int line = 456;
    Error error(message, file, line);
    CPPUNIT_ASSERT_EQUAL(unknown_error, error.get_error_code());
    CPPUNIT_ASSERT_EQUAL(message, error.get_error_message());
    CPPUNIT_ASSERT_EQUAL(file, error.get_file());
    CPPUNIT_ASSERT_EQUAL(line, error.get_line());
    CPPUNIT_ASSERT(error.OK());
}

void ErrorTest::test_copy_constructor() {
    DBG(cerr << "Test: " << __func__ << "\n");
    ErrorCode code = malformed_expr;
    string message = "Malformed expression error";
    string file = "copy_file.cpp";
    int line = 789;
    Error error1(code, message, file, line);
    Error error2(error1);
    CPPUNIT_ASSERT_EQUAL(code, error2.get_error_code());
    CPPUNIT_ASSERT_EQUAL(message, error2.get_error_message());
    CPPUNIT_ASSERT_EQUAL(file, error2.get_file());
    CPPUNIT_ASSERT_EQUAL(line, error2.get_line());
    CPPUNIT_ASSERT(error2.OK());
}

void ErrorTest::test_assignment_operator() {
    DBG(cerr << "Test: " << __func__ << "\n");
    ErrorCode code1 = no_authorization;
    string message1 = "Not authorized";
    string file1 = "file1.cpp";
    int line1 = 1011;
    Error error1(code1, message1, file1, line1);

    ErrorCode code2 = cannot_read_file;
    string message2 = "Cannot read file";
    string file2 = "file2.cpp";
    int line2 = 1213;
    Error error2(code2, message2, file2, line2);

    error1 = error2;
    CPPUNIT_ASSERT_EQUAL(code2, error1.get_error_code());
    CPPUNIT_ASSERT_EQUAL(message2, error1.get_error_message());
    CPPUNIT_ASSERT_EQUAL(file2, error1.get_file());
    CPPUNIT_ASSERT_EQUAL(line2, error1.get_line());
    CPPUNIT_ASSERT(error1.OK());
}

void ErrorTest::test_assignment_self() {
    DBG(cerr << "Test: " << __func__ << "\n");
    ErrorCode code = no_such_variable;
    string message = "No such variable exists";
    string file = "self_assign_file.cpp";
    int line = 2345;
    Error error(code, message, file, line);

    error = error; // Self-assignment
    CPPUNIT_ASSERT_EQUAL(code, error.get_error_code());
    CPPUNIT_ASSERT_EQUAL(message, error.get_error_message());
    CPPUNIT_ASSERT_EQUAL(file, error.get_file());
    CPPUNIT_ASSERT_EQUAL(line, error.get_line());
    CPPUNIT_ASSERT(error.OK());
}

void ErrorTest::test_ok() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error1;
    CPPUNIT_ASSERT(error1.OK());

    Error error2(internal_error, "Internal server error");
    CPPUNIT_ASSERT(error2.OK());

    Error error3("Just a message");
    CPPUNIT_ASSERT(error3.OK());
}

void ErrorTest::test_parse() {
    DBG(cerr << "Test: " << __func__ << "\n");
    // Create a temporary file and write an error message into it
    string error_string = R"(Error { code = 1003; message = "No such file"; };)";
    FILE *fp = tmpfile();
    if (!fp) {
        CPPUNIT_FAIL(string("Failed to create temporary file: ") + strerror(errno));
    }
    fputs(error_string.c_str(), fp);
    rewind(fp);

    Error error;
    bool result = error.parse(fp);
    CPPUNIT_ASSERT(result);
    CPPUNIT_ASSERT_EQUAL(no_such_file, error.get_error_code());
    CPPUNIT_ASSERT_EQUAL(string(R"("No such file")"), error.get_error_message());

    fclose(fp);
}

void ErrorTest::test_parse_null_file() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error;
    CPPUNIT_ASSERT_THROW(error.parse(nullptr), InternalErr);
}

void ErrorTest::test_print_file() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error(no_such_variable, "No such variable", "print_file.cpp", 1415);
    FILE *fp = tmpfile();
    if (!fp) {
        CPPUNIT_FAIL(string("Failed to create temporary file: ") + strerror(errno));
    }
    error.print(fp);
    rewind(fp);

    char buffer[1024];
    string output;
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        output += buffer;
    }
    CPPUNIT_ASSERT(output.find("code = 1004") != string::npos);
    CPPUNIT_ASSERT(output.find("message = \"No such variable\"") != string::npos);
    fclose(fp);
}

void ErrorTest::test_print_ostream() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error(malformed_expr, "Malformed expression", "ostream_file.cpp", 1617);
    ostringstream oss;
    error.print(oss);
    string output = oss.str();
    CPPUNIT_ASSERT(output.find("code = 1005") != string::npos);
    CPPUNIT_ASSERT(output.find("message = \"Malformed expression\"") != string::npos);
}

void ErrorTest::test_get_error_code() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error1;
    CPPUNIT_ASSERT_EQUAL(undefined_error, error1.get_error_code());

    Error error2(no_authorization, "No authorization");
    CPPUNIT_ASSERT_EQUAL(no_authorization, error2.get_error_code());
}

void ErrorTest::test_get_error_message() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error1;
    CPPUNIT_ASSERT_EQUAL(string(""), error1.get_error_message());

    Error error2(cannot_read_file, "Cannot read file");
    CPPUNIT_ASSERT_EQUAL(string("Cannot read file"), error2.get_error_message());
}

void ErrorTest::test_set_error_code() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error;
    error.set_error_code(not_implemented);
    CPPUNIT_ASSERT_EQUAL(not_implemented, error.get_error_code());
    CPPUNIT_ASSERT_EQUAL(string("Not Implemented"), error.get_error_message());

    error.set_error_code(12345); // An invalid code.
    CPPUNIT_ASSERT_EQUAL(12345, error.get_error_code());
    CPPUNIT_ASSERT_EQUAL(string("Not Implemented"), error.get_error_message()); // Message should not change
}

void ErrorTest::test_set_error_message() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error;
    error.set_error_message("A new error message");
    CPPUNIT_ASSERT_THROW_MESSAGE("Expected an InternalErr exception", error.get_error_message(), libdap::InternalErr);
    error.set_error_code(unknown_error);
    error.set_error_message("A new error message");
    CPPUNIT_ASSERT_EQUAL(string("A new error message"), error.get_error_message());
}

void ErrorTest::test_get_file() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error("An error", "test_get_file.cpp", 1999);
    CPPUNIT_ASSERT_EQUAL(string("test_get_file.cpp"), error.get_file());
}

void ErrorTest::test_set_file() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error;
    error.set_file("new_file_name.cpp");
    CPPUNIT_ASSERT_EQUAL(string("new_file_name.cpp"), error.get_file());
}

void ErrorTest::test_get_line() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error("An error", "test_get_line.cpp", 2001);
    CPPUNIT_ASSERT_EQUAL(2001, error.get_line());
}

void ErrorTest::test_set_line() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error;
    error.set_line(2002);
    CPPUNIT_ASSERT_EQUAL(2002, error.get_line());
}

void ErrorTest::test_what() {
    DBG(cerr << "Test: " << __func__ << "\n");
    Error error(dummy_message, "Dummy message for what() test");
    CPPUNIT_ASSERT_EQUAL(string("Dummy message for what() test"), string(error.what()));
}

int main(int argc, char *argv[]) { return run_tests<ErrorTest>(argc, argv) ? 0 : 1; }
