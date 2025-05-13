

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h> // For a simple test runner main()

#include "Error.h"
#include "run_tests_cppunit.h"

#include <iostream> // For potential debug output or if CompilerOutputter uses it
#include <string>
#include <utility> // For std::move (though constructor handles it in Error class)

// (Includes like cstdlib for EXIT_SUCCESS/FAILURE, iostream for cout/cerr should be at the top of the file)
// Make sure these includes are present at the top of your single test file:
#include <algorithm> // For std::equal to compare char buffers
#include <cstdio>    // For FILE*, tmpnam, fopen, fclose, fgets, etc.
#include <sstream>   // For std::ostringstream
#include <vector>    // For reading file content easily

// --- Forward declarations or includes for libdap internals needed by tests ---
// This is highly dependent on your actual libdap structure.
// You MUST ensure these are properly declared/defined and linked.

namespace libdap {
// Assuming InternalErr is defined elsewhere, e.g., in error.h or its own header.
// If not, a minimal declaration might be needed for the test to compile,
// but the actual definition must be linked.
class InternalErr;

// Assuming err_messages is declared (e.g., extern const char* const err_messages[];)
// and defined in one of your libdap source files.
extern const char *const err_messages[];

// Assuming parser-related functions and types are declared:
// struct parser_arg { /* ... actual members ... */
//     void* m_object; // Typically the 'Error*' object
//     bool m_status;  // Parser success status
// public:
//     parser_arg(void* obj) : m_object(obj), m_status(true) {}
//     bool status() const { return m_status; }
//     void set_status(bool s) { m_status = s; }
//     Error* get_error_object() { return static_cast<Error*>(m_object); }
// }; // This is a GUESS - replace with actual parser_arg definition
//
// extern void *Error_buffer(FILE *fp);
// extern void Error_switch_to_buffer(void *buffer);
// extern int Errorparse(parser_arg *arg); // Or whatever the signature is
// extern void Error_delete_buffer(void *buffer);
} // namespace libdap

// Define the new Test Fixture: ErrorTest
class ErrorTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ErrorTest);
    CPPUNIT_TEST(test_ok_valid_empty);
    CPPUNIT_TEST(test_ok_valid_message_and_code);
    CPPUNIT_TEST(test_ok_invalid_empty_message_with_code);
    CPPUNIT_TEST(test_ok_invalid_message_with_undefined_code);
    CPPUNIT_TEST(test_get_error_code);
    CPPUNIT_TEST(test_set_error_code_empty_message_in_range);
    CPPUNIT_TEST(test_set_error_code_empty_message_undefined);
    CPPUNIT_TEST(test_set_error_code_empty_message_out_of_range);
    CPPUNIT_TEST(test_set_error_code_with_existing_message);
    CPPUNIT_TEST(test_get_error_message);
    CPPUNIT_TEST(test_set_error_message);
    CPPUNIT_TEST(test_set_error_message_empty_and_ok_status);
    CPPUNIT_TEST(test_print_to_file);
    CPPUNIT_TEST(test_print_to_file_quoted_message);
    CPPUNIT_TEST(test_print_to_ostream);
    CPPUNIT_TEST(test_print_to_ostream_quoted_message);
    CPPUNIT_TEST(test_parse_valid_input);
    CPPUNIT_TEST(test_parse_null_file_pointer);
    CPPUNIT_TEST(test_parse_malformed_input); // Basic malformed test
    // Add more CPPUNIT_TEST lines here if more fine-grained tests for parse are created
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

protected:
    // Test methods for OK()
    void test_ok_valid_empty();
    void test_ok_valid_message_and_code();
    void test_ok_invalid_empty_message_with_code();
    void test_ok_invalid_message_with_undefined_code();

    // Test methods for get_error_code() / set_error_code()
    void test_get_error_code();
    void test_set_error_code_empty_message_in_range();
    void test_set_error_code_empty_message_undefined();
    void test_set_error_code_empty_message_out_of_range();
    void test_set_error_code_with_existing_message();

    // Test methods for get_error_message() / set_error_message()
    void test_get_error_message();
    void test_set_error_message();
    void test_set_error_message_empty_and_ok_status();

    // Test methods for print()
    void test_print_to_file();
    void test_print_to_file_quoted_message();
    void test_print_to_ostream();
    void test_print_to_ostream_quoted_message();

    // Test methods for parse()
    void test_parse_valid_input();
    void test_parse_null_file_pointer();
    void test_parse_malformed_input();

private:
    libdap::Error *test_error;
    // Helper for file operations
    std::string read_file_content(const char *filename);
    char temp_filename[L_tmpnam + 1]; // For temporary file name
};

// Register the new suite
CPPUNIT_TEST_SUITE_REGISTRATION(ErrorTest);

// Anonymous namespace for helper constants for testing
namespace {
// 'unknown_error' is a #define but Gemini used libdap::unknown_error. Wrong. jhrg 5/9/25
constexpr libdap::ErrorCode TEST_CODE_LHS = unknown_error;
const std::string TEST_MSG_LHS = "Initial LHS error message for testing.";
const std::string TEST_FILE_LHS = "lhs_source_file.cpp";
constexpr int TEST_LINE_LHS = 75;

// see above for this #define also. fixed jhrg 5/9/25
constexpr libdap::ErrorCode TEST_CODE_RHS = no_such_file;
const std::string TEST_MSG_RHS = "Source RHS error with distinct content.";
const std::string TEST_FILE_RHS = "rhs_source_file.cc";
constexpr int TEST_LINE_RHS = 150;
} // namespace

// Define the Test Fixture: ErrorAssignmentTest
class ErrorAssignmentTest : public CppUnit::TestFixture {
    // Declare the suite and the tests
    CPPUNIT_TEST_SUITE(ErrorAssignmentTest);
    CPPUNIT_TEST(test_self_assignment);
    CPPUNIT_TEST(test_assignment_to_different_object);
    CPPUNIT_TEST(test_assignment_preserves_ok_status);
    CPPUNIT_TEST_SUITE_END();

public:
    // Constructor/Destructor for the test class (if needed, usually not for basic fixtures)
    ErrorAssignmentTest() = default;
    ~ErrorAssignmentTest() override = default;

    // setUp and tearDown methods, called before/after each test
    void setUp() override;
    void tearDown() override;

protected:
    // The test methods
    void test_self_assignment();
    void test_assignment_to_different_object();
    void test_assignment_preserves_ok_status();

private:
    // Member variables for the test fixture (our Error objects)
    // I added '= nullptr' because ctor didn't initialize. jhrg 5/9/25
    libdap::Error *error_lhs = nullptr; // Left-hand side for assignment
    libdap::Error *error_rhs = nullptr; // Right-hand side for assignment
};

// Register the suite with the factory.
// This line makes the test discoverable by CppUnit test runners.
CPPUNIT_TEST_SUITE_REGISTRATION(ErrorAssignmentTest);

// --- Method Implementations for ErrorAssignmentTest ---

void ErrorAssignmentTest::setUp() {
    // Initialize objects before each test using the detailed constructor
    error_lhs = new libdap::Error(TEST_CODE_LHS, TEST_MSG_LHS, TEST_FILE_LHS, TEST_LINE_LHS);
    error_rhs = new libdap::Error(TEST_CODE_RHS, TEST_MSG_RHS, TEST_FILE_RHS, TEST_LINE_RHS);

    // Ensure our test objects are valid from the start, as per Error class's internal asserts
    CPPUNIT_ASSERT_MESSAGE("Setup: error_lhs should be OK", error_lhs->OK());
    CPPUNIT_ASSERT_MESSAGE("Setup: error_rhs should be OK", error_rhs->OK());
}

void ErrorAssignmentTest::tearDown() {
    delete error_lhs;
    error_lhs = nullptr;
    delete error_rhs;
    error_rhs = nullptr;
}

void ErrorAssignmentTest::test_self_assignment() {
    CPPUNIT_ASSERT_MESSAGE("Pre-condition: LHS should be OK before self-assignment", error_lhs->OK());

    // Arrange: Store original values to ensure they don't change.
    libdap::ErrorCode original_code = error_lhs->get_error_code();
    std::string original_message = error_lhs->get_error_message();
    std::string original_file = error_lhs->get_file();
    int original_line = error_lhs->get_line();

// Act: Assign the object to itself.
// Suppress compiler warnings for intentional self-assignment.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif
    *error_lhs = *error_lhs;
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

    // Assert: Check that the object's state remains unchanged.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Self-assignment should not change error code", original_code,
                                 error_lhs->get_error_code());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Self-assignment should not change error message", original_message,
                                 error_lhs->get_error_message());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Self-assignment should not change file name", original_file, error_lhs->get_file());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Self-assignment should not change line number", original_line, error_lhs->get_line());

    // Assert: Check the post-condition from your operator's assert.
    CPPUNIT_ASSERT_MESSAGE("Post-condition: LHS should still be OK after self-assignment", error_lhs->OK());
}

void ErrorAssignmentTest::test_assignment_to_different_object() {
    CPPUNIT_ASSERT_MESSAGE("Pre-condition: LHS should be OK before assignment", error_lhs->OK());
    CPPUNIT_ASSERT_MESSAGE("Pre-condition: RHS should be OK before assignment", error_rhs->OK());

    // Act: Assign error_rhs to error_lhs.
    *error_lhs = *error_rhs;

    // Assert: Check that error_lhs now has the values of error_rhs.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Error code should be copied from RHS to LHS", error_rhs->get_error_code(),
                                 error_lhs->get_error_code());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Error message should be copied from RHS to LHS", error_rhs->get_error_message(),
                                 error_lhs->get_error_message());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("File name should be copied from RHS to LHS", error_rhs->get_file(),
                                 error_lhs->get_file());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Line number should be copied from RHS to LHS", error_rhs->get_line(),
                                 error_lhs->get_line());

    // Assert: Check that the error_rhs (RHS) was not modified.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("RHS error code should remain unchanged after assignment", TEST_CODE_RHS,
                                 error_rhs->get_error_code());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("RHS error message should remain unchanged after assignment", TEST_MSG_RHS,
                                 error_rhs->get_error_message());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("RHS file name should remain unchanged after assignment", TEST_FILE_RHS,
                                 error_rhs->get_file());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("RHS line number should remain unchanged after assignment", TEST_LINE_RHS,
                                 error_rhs->get_line());

    // Assert: Check the post-conditions for OK() status.
    CPPUNIT_ASSERT_MESSAGE("Post-condition: LHS should be OK after assignment", error_lhs->OK());
    CPPUNIT_ASSERT_MESSAGE("Post-condition: RHS should still be OK after assignment", error_rhs->OK());
}

void ErrorAssignmentTest::test_assignment_preserves_ok_status() {
    // This test reinforces that if an object is OK, it remains OK after assignment,
    // and if an assignment happens from an OK object, the target becomes OK.
    // This is largely covered by the internal asserts in Error::operator=
    // and the checks in other tests, but an explicit test case emphasizes this behavior.

    // Assuming error_lhs and error_rhs are OK from setUp.
    *error_lhs = *error_rhs;
    CPPUNIT_ASSERT_MESSAGE("LHS should be OK after assigning an OK RHS", error_lhs->OK());

    // If there was a defined way to make an Error object "not OK" through its public interface
    // while still being a valid state to test assignment *to*, we could add:
    // libdap::Error initially_not_ok_error; // if Error() or a setter could lead to !OK()
    // if (!initially_not_ok_error.OK()) { // hypothetical
    //     initially_not_ok_error = *error_rhs; // error_rhs is OK
    //     CPPUNIT_ASSERT_MESSAGE("LHS (initially not OK) should become OK after assignment from an OK RHS",
    //                            initially_not_ok_error.OK());
    // }
    // However, the asserts in `Error::operator=` suggest that an Error object is always expected to be OK.
}

int main(int argc, char *argv[]) { return run_tests<ErrorAssignmentTest>(argc, argv) ? 0 : 1; }

// Optional: A main function to run the tests if you want this file to be self-executable.
// Your build system might handle test execution differently.
/*
int main(int argc, char* argv[]) {
    // Create the event manager and test controller
    CppUnit::TestResult controller;

    // Add a listener that collects test result
    CppUnit::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that prints dots as tests run
    CppUnit::BriefTestProgressListener progress;
    controller.addListener(&progress);

    // Add the top suite to the test runner
    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    std::cout << "Running ErrorAssignmentTest..." << std::endl;
    runner.run(controller);

    // Print test results
    CppUnit::CompilerOutputter outputter(&result, std::cerr);
    outputter.write();

    return result.wasSuccessful() ? 0 : 1;
}
*/