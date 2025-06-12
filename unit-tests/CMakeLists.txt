# unit-tests/CMakeLists.txt

add_library(test_support STATIC
		testFile.cc
)

target_include_directories(test_support
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

# List all your test sources (each one contains main())
## Not tested
#AISDatabaseParserTest.cc
#AISMergeTest.cc
#AISResourcesTest.cc

set(TEST_SOURCES
		ancT.cc
		arrayT.cc
		ArrayTest.cc
		attrTableT.cc
		AttrTableTest.cc
		BaseTypeFactoryTest.cc
		BigArrayTest.cc
		ByteTest.cc
		chunked_iostream_test.cc
		D4AsyncDocTest.cc
		D4AttributesTest.cc
		D4BaseTypeFactoryTest.cc
		D4DimensionsTest.cc
		D4EnumDefsTest.cc
		D4EnumTest.cc
		D4FilterClauseTest.cc
		D4GroupTest.cc
		D4MarshallerTest.cc
		D4ParserSax2Test.cc
		D4SequenceTest.cc
		D4UnMarshallerTest.cc
		dasT.cc
		DASTest.cc
		ddsT.cc
		DDSTest.cc
		DDXParserTest.cc
		DmrRoundTripTest.cc
		DMRTest.cc
		DmrToDap2Test.cc
		DODSFilterTest.cc
		ErrorTest.cc
		fdiostreamTest.cc
		Float32Test.cc
		Float64Test.cc
		generalUtilTest.cc
		GridTest.cc
		Int16Test.cc
		Int32Test.cc
		Int64Test.cc
		Int8Test.cc
		IsDap4ProjectedTest.cc
		MarshallerFutureTest.cc
		MarshallerTest.cc
		marshT.cc
		MIMEUtilTest.cc
		parserUtilTest.cc
		RCReaderTest.cc
		RegexTest.cc
		ResponseBuilderTest.cc
		ResponseCacheTest.cc
		sequenceT.cc
		SequenceTest.cc
		ServerFunctionsListUnitTest.cc
		SignalHandlerTest.cc
		structT.cc
		testFile.cc
		ThreadTest.cc
		UInt16Test.cc
		UInt32Test.cc
		UInt64Test.cc
		util_mitTest.cc
)

# Loop through each file and build as its own executable
foreach(test_file ${TEST_SOURCES})
	# Get the base name (e.g., TestAttrTable from TestAttrTable.cc)
	get_filename_component(test_name ${test_file} NAME_WE)

	# Define the executable
	add_executable(${test_name} ${test_file})

	# Link to libdap and CppUnit
	target_link_libraries(${test_name}
			PRIVATE
			dap
			test_types
			CppUnit::CppUnit
			test_support
	)

	# Include the top-level for headers and config_dap.h
	target_include_directories(${test_name}
			PRIVATE
			${CMAKE_CURRENT_BINARY_DIR}
			${CMAKE_SOURCE_DIR}
			${CMAKE_BINARY_DIR}
	)

	# Register with CTest
	add_test(NAME ${test_name} COMMAND ${test_name})
	# This enables using `ctest -L unit` to run just these tests.
	set_tests_properties(${test_name} PROPERTIES LABELS "unit")
endforeach()

# Set TEST_SRC_DIR to the absolute path of the test sources
set(abs_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "TEST_SRC_DIR = ${abs_srcdir}")

# Generate test_config.h from the template
configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
		${CMAKE_CURRENT_BINARY_DIR}/test_config.h
)