# unit-tests/CMakeLists.txt

add_library(test_support STATIC testFile.cc)
set_target_properties(test_support PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(test_support
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

# Some of these link only with libdap, some link with test_support, too.
set(TESTS_DAP_ONLY
		RegexTest.cc ErrorTest.cc ArrayTest.cc GridTest.cc AttrTableTest.cc
		ByteTest.cc MIMEUtilTest.cc DASTest.cc DDSTest.cc
		Int8Test.cc Int16Test.cc UInt16Test.cc Int32Test.cc
		UInt32Test.cc Int64Test.cc UInt64Test.cc Float32Test.cc
		Float64Test.cc D4BaseTypeFactoryTest.cc BaseTypeFactoryTest.cc
		DDXParserTest.cc generalUtilTest.cc parserUtilTest.cc SignalHandlerTest.cc
		ServerFunctionsListUnitTest.cc D4MarshallerTest.cc D4UnMarshallerTest.cc
		D4DimensionsTest.cc  D4EnumDefsTest.cc D4GroupTest.cc D4ParserSax2Test.cc
		D4AttributesTest.cc D4EnumTest.cc chunked_iostream_test.cc D4AsyncDocTest.cc
		DMRTest.cc DmrRoundTripTest.cc DmrToDap2Test.cc D4FilterClauseTest.cc
		IsDap4ProjectedTest.cc MarshallerFutureTest.cc
)

# BigArrayTest.cc seems to break things. jhrg 6/12/25

set(TESTS_DAPCLIENT util_mitTest.cc RCReaderTest.cc)

set(TESTS_DAPSERVER ancT.cc)

# MarshallerTest links with both dapclient and the test-types lib. For this
# one special case, add the dapclient library 'by hand.' See below. jhrg 6/12/25
set(TESTS_WITH_TEST_TYPES
		SequenceTest.cc arrayT.cc marshT.cc attrTableT.cc
		structT.cc sequenceT.cc ddsT.cc dasT.cc
		D4SequenceTest.cc
)

# This cmake function sets up the base condition, linking with libdap.
# Other code adds more libraries using additional calls to target_link_libraries()
# which is 'additive.'
function(build_test target source_files)
	add_executable(${target} ${source_files})

	target_include_directories(${target}
			PRIVATE
			${CMAKE_CURRENT_BINARY_DIR}
			${CMAKE_SOURCE_DIR}
			${CMAKE_BINARY_DIR}
			${LIBXML2_INCLUDE_DIR}
	)
	if(TIRPC_FOUND)
		target_include_directories(${target} PRIVATE ${TIRPC_INCLUDE_DIRS})
		target_link_libraries(${target} PRIVATE ${TIRPC_LIBRARIES})
	endif()

	target_link_libraries(${target}
			PRIVATE
			dap
			test_support
			CppUnit::CppUnit
	)

	add_test(NAME ${target} COMMAND ${target})
	# This enables using `ctest -L unit` to run just these tests.
	set_tests_properties(${target} PROPERTIES LABELS unit)
endfunction()

foreach(src ${TESTS_DAP_ONLY})
	get_filename_component(tgt ${src} NAME_WE)
	build_test(${tgt} ${src})
endforeach()

foreach(src ${TESTS_DAPCLIENT})
	get_filename_component(tgt ${src} NAME_WE)
	build_test(${tgt} ${src})
	target_link_libraries(${tgt} PRIVATE dapclient)
endforeach()

foreach(src ${TESTS_DAPSERVER})
	get_filename_component(tgt ${src} NAME_WE)
	build_test(${tgt} ${src})
	target_link_libraries(${tgt} PRIVATE dapserver)
endforeach()

foreach(src ${TESTS_WITH_TEST_TYPES})
	get_filename_component(tgt ${src} NAME_WE)
	build_test(${tgt} ${src})
	target_link_libraries(${tgt} PRIVATE test-types)
endforeach()

# Here's where we link up MarshallerTest
build_test(MarshallerTest MarshallerTest.cc)
target_link_libraries(MarshallerTest PRIVATE test-types dapclient)

# This test will cause Travis to time out. jhrg 6/14/25
if (BIG_ARRAY_TEST)
	build_test(BigArrayTest BigArrayTest.cc)
endif()

# Set TEST_SRC_DIR to the absolute path of the test sources
set(abs_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(abs_builddir "${CMAKE_CURRENT_BINARY_DIR}")

# Generate test_config.h from the template
configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
		${CMAKE_CURRENT_BINARY_DIR}/test_config.h
)