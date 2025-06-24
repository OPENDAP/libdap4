# d4_ce/unit-tests/CMakeLists.txt

add_executable(D4ConstraintEvaluatorTest D4ConstraintEvaluatorTest.cc)

target_include_directories(D4ConstraintEvaluatorTest
		PRIVATE
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/d4_ce
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_BINARY_DIR}
		${LIBXML2_INCLUDE_DIR}
)

target_link_libraries(D4ConstraintEvaluatorTest
		PRIVATE
		dap
		d4_ce_parser
		CppUnit::CppUnit
)

add_test(NAME D4ConstraintEvaluatorTest COMMAND D4ConstraintEvaluatorTest)
# This enables using `ctest -L unit` to run just these tests.
set_tests_properties(${test_name} PROPERTIES LABELS "unit")

# Set TEST_SRC_DIR to the absolute path of the test sources
set(abs_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "TEST_SRC_DIR = ${abs_srcdir}")

# Generate test_config.h from the template
configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
		${CMAKE_CURRENT_BINARY_DIR}/test_config.h
)