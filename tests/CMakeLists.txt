
add_library(test-types STATIC
		TestArray.cc TestD4Sequence.cc TestInt32.cc TestTypeFactory.cc
		TestByte.cc TestFloat32.cc TestInt64.cc TestUInt16.cc
		TestCommon.cc TestFloat64.cc TestInt8.cc TestUInt32.cc
		TestD4Enum.cc TestFunction.cc TestSequence.cc TestUInt64.cc
		TestD4Group.cc TestGrid.cc TestStr.cc TestUrl.cc
		TestD4Opaque.cc TestInt16.cc TestStructure.cc D4TestTypeFactory.cc
		D4TestFunction.cc
)
set_target_properties(test-types PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Replace: target_include_directories(test-types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# with the following. This enables linking locally and then installing so
# library users can link to these without linking to the source tree. 6/27/25 jhrg
target_include_directories(test-types
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include/libdap/test>
)

target_include_directories(test-types PRIVATE ${LIBXML2_INCLUDE_DIR})

if(TIRPC_FOUND)
	target_include_directories(test-types PRIVATE ${TIRPC_INCLUDE_DIRS})
	target_link_libraries(test-types PRIVATE ${TIRPC_LIBRARIES})
endif()

target_link_libraries(test-types PRIVATE dap)

install(TARGETS test-types
		EXPORT libdap4Targets
		ARCHIVE DESTINATION lib
		INCLUDES DESTINATION include/libdap/test)

set(TEST_HEADERS
		TestD4Enum.h		TestGrid.h			TestStructure.h TestD4Group.h
		TestInt16.h			TestTypeFactory.h 	TestD4Opaque.h	TestInt32.h
		TestUInt16.h 		TestD4Sequence.h	TestInt64.h		TestUInt32.h
		TestArray.h			TestFloat32.h		TestInt8.h		TestUInt64.h
		TestByte.h			TestFloat64.h		TestSequence.h	TestUrl.h
		TestCommon.h		TestFunction.h		TestStr.h
		D4TestFunction.h	D4TestTypeFactory.h)

install(FILES ${TEST_HEADERS}
		DESTINATION include/libdap/test)

add_executable(das-test das-test.cc)
target_link_libraries(das-test PRIVATE test-types dapserver dap)
target_include_directories(das-test PRIVATE ${LIBXML2_INCLUDE_DIR})

add_executable(dds-test dds-test.cc)
target_link_libraries(dds-test PRIVATE test-types dapserver dap)
target_include_directories(dds-test PRIVATE ${LIBXML2_INCLUDE_DIR})
if(TIRPC_FOUND)
	target_include_directories(dds-test PRIVATE ${TIRPC_INCLUDE_DIRS})
	message(STATUS "dds-test, TIRPC_INCLUDE_DIRS: ${TIRPC_INCLUDE_DIRS}")
	target_link_libraries(dds-test PRIVATE ${TIRPC_LIBRARIES})
endif()

add_executable(expr-test expr-test.cc ResponseBuilder.cc)
target_include_directories(expr-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})
target_link_libraries(expr-test PRIVATE test-types dapserver dapclient dap)
if(TIRPC_FOUND)
	target_include_directories(expr-test PRIVATE ${TIRPC_INCLUDE_DIRS})
	message(STATUS "expr-test, TIRPC_INCLUDE_DIRS: ${TIRPC_INCLUDE_DIRS}")
	target_link_libraries(expr-test PRIVATE ${TIRPC_LIBRARIES})
endif()

add_executable(dmr-test dmr-test.cc D4ResponseBuilder.cc)
target_include_directories(dmr-test
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR}
		${CMAKE_SOURCE_DIR}/d4_ce  ${CMAKE_SOURCE_DIR}/d4_function)
target_link_libraries(dmr-test PRIVATE test-types dapserver dap)
if(TIRPC_FOUND)
	target_include_directories(dmr-test PRIVATE ${TIRPC_INCLUDE_DIRS})
	message(STATUS "dmr-test, TIRPC_INCLUDE_DIRS: ${TIRPC_INCLUDE_DIRS}")
	target_link_libraries(dmr-test PRIVATE ${TIRPC_LIBRARIES})
endif()

# find_package(Python3 COMPONENTS Interpreter REQUIRED)

#set(DAS_HELPER_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/run_das_response_test.py")
#set(DAS_HELPER_BIN  "${CMAKE_CURRENT_BINARY_DIR}/run_das_response_test.py")
#configure_file(
#		${DAS_HELPER_SRC}  ${DAS_HELPER_BIN}
#		COPYONLY
#)

file(GLOB DAS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite"
	"${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/*.das")

# message(STATUS "Found these DAS tests: ${DAS_FILES}")
# message(STATUS "Looked in: ${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite")

# Helper to register one DAS‐response test
function(add_das_test das_filename)
	#	# get "test.1.das" → fullname="test.1.das"
	#	message(STATUS "testname: ${testname}")
	get_filename_component(fullname "${das_filename}" NAME)
	# strip just ".das" → raw="test.1"
	string(REGEX REPLACE "\\.das$" "" raw "${fullname}")
	# sanitize; test.1 → test_1
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "${raw}")
	# message(STATUS "testname: ${testname}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/${das_filename}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/${das_filename}.base")
	set(output     "${CMAKE_CURRENT_SOURCE_DIR}/${testname}.out")

	# Add the CTest entry
	add_test(NAME das_${testname}
			COMMAND /bin/sh "-c"
			# 1) run das-test, redirect all output into a temp file
			# 2) diff that file against the baseline"
			"\"$<TARGET_FILE:das-test>\" -p < \"${input}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)
	set_tests_properties(das_${testname} PROPERTIES LABELS integration)
endfunction()

#function(add_das_response_test das_filename)
#	# get "test.1.das" → fullname="test.1.das"
#	message(STATUS "testname: ${testname}")
#	get_filename_component(fullname "${das_filename}" NAME)
#	# strip just ".das" → raw="test.1"
#	string(REGEX REPLACE "\\.das$" "" raw "${fullname}")
#	# sanitize → test_1
#	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "${raw}")
#	message(STATUS "testname: ${testname}")
#	# 3) set up paths
#	set(input    "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/${das_filename}")
#	set(baseline "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/${das_filename}.base")
#	set(diffout  "${CMAKE_CURRENT_BINARY_DIR}/${testname}.diff")
#
#	# 4) register the test
#	add_test(
#			NAME das_response_${testname}
#			COMMAND
#			${Python3_EXECUTABLE}
#			"${DAS_HELPER_BIN}"
#			$<TARGET_FILE:das-test>
#			"${input}"
#			"${baseline}"
#			"${diffout}"
#	)
#	set_tests_properties(das_response_${testname} PROPERTIES LABELS integration)
#endfunction()

foreach(dfile IN LISTS DAS_FILES)
	add_das_test(${dfile})
endforeach()

### DDS tests start here.

## I used file() with GLOB to grab all of the files that match *.dds in the
## dds-testsuite subdir. This is simple and eliminates the need to list every
## file, but can inadvertently include files that are not part of the test.
## Listing the files intended, one by one, is better but also tedious and means
## that new tests have to be manually added. 7/8/25 jhrg
file(GLOB DDS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite"
		"${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/*.dds")

## This function will take the name of a DDS file and use it as input to a DDS
## test. There are some tricks here as well. 7/8/25 jhrg
function(add_dds_test dds_filename)
	# Here the name of the dds file is morphed into something that will work
	# as a cmake name (dots are not allowed in cmake names). 7/8/25 jhrg
	#	message(STATUS "testname: ${testname}")
	get_filename_component(fullname "${dds_filename}" NAME)
	# strip just ".das" → raw="test.1"
	string(REGEX REPLACE "\\.dds$" "" raw "${fullname}")
	# sanitize; test.1 → test_1
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "${raw}")
	# message(STATUS "testname: ${testname}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/${dds_filename}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/${dds_filename}.base")
	set(output     "${CMAKE_CURRENT_SOURCE_DIR}/${testname}.out")

	# Add the CTest entry. Here the shell is used so that we can employ redirection.
	# The extra double quotes are 'best practice' for cmake, but really not needed here
	# because we know that $<TARGET_FILE:dds-test> and the various variables (e.g. ${input})
	# do not have spaces. The extra backslash characters make it harder to decipher
	# what is going on. 7/8/25 jhrg
	add_test(NAME dds_${testname}
			COMMAND /bin/sh "-c"
			# 1) run das-test, redirect all output into a temp file
			# 2) diff that file against the baseline"
			"\"$<TARGET_FILE:dds-test>\" -p < \"${input}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)
	# This makes it so we cna run just these tests and also makes it easy to run the
	# unit tests _before_ the integration tests with a 'check' target. See the top-leve
	# CMakeLists file. 7/8/25 jhrg
	set_tests_properties(dds_${testname} PROPERTIES LABELS integration)
endfunction()

## Iterate over all of the DDS filed and make a cmake/ctest for each one. 7/8/25 jhrg
foreach(dfile IN LISTS DDS_FILES)
	add_dds_test(${dfile})
endforeach()

## DMR tests. Unlike the DAS and DDS test, these test enumerate the individual test
## one by one since various tests will use different functions. 7/10/25 jhrg

## This function will take the name of a DMR file and use it as input to a DMR
## test. There are some tricks here as well. 7/8/25 jhrg
function(add_dmr_parse_test test_input)
	# Here the name of the DMR file is morphed into something that will work
	# as a cmake name (dots are not allowed in cmake names). 7/8/25 jhrg
	# get_filename_component(fullname "${dmr_filename}" NAME)
	# strip just "test.1.xml" → raw="test.1"
	string(REGEX REPLACE "\\.xml$" "" raw "${test_input}")
	# sanitize; test.1 → test_1
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_parse_${raw}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}.baseline")
	set(output     "${CMAKE_CURRENT_SOURCE_DIR}/${testname}.out")

	# Add the CTest entry. Here the shell is used so that we can employ redirection.
	# The extra double quotes are 'best practice' for cmake, but really not needed here
	# because we know that $<TARGET_FILE:dmr-test> and the various variables (e.g. ${input})
	# do not have spaces. The extra backslash characters make it harder to decipher
	# what is going on. 7/8/25 jhrg
	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			# 1) run das-test, redirect all output into a temp file
			# 2) diff that file against the baseline"
			"\"$<TARGET_FILE:dmr-test>\" -x -p \"${input}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)
	# This makes it so we cna run just these tests and also makes it easy to run the
	# unit tests _before_ the integration tests with a 'check' target. See the top-leve
	# CMakeLists file. 7/8/25 jhrg
	set_tests_properties(${testname} PROPERTIES LABELS integration)
endfunction()

add_dmr_parse_test(test_simple_1.xml)
add_dmr_parse_test(test_simple_2.xml)
add_dmr_parse_test(test_simple_3.xml)

add_dmr_parse_test(test_simple_3_error_1.xml)
add_dmr_parse_test(test_simple_3_error_2.xml)
add_dmr_parse_test(test_simple_3_error_3.xml)

add_dmr_parse_test(test_simple_4.xml)
add_dmr_parse_test(test_simple_5.xml)
add_dmr_parse_test(test_simple_6.xml)
add_dmr_parse_test(test_simple_7.xml)
add_dmr_parse_test(test_simple_8.xml)
add_dmr_parse_test(test_simple_9.xml)
add_dmr_parse_test(test_simple_9.1.xml)
add_dmr_parse_test(test_simple_10.xml)
add_dmr_parse_test(test_enum_grp.xml)

add_dmr_parse_test(test_array_1.xml)
add_dmr_parse_test(test_array_2.xml)
add_dmr_parse_test(test_array_3.xml)
add_dmr_parse_test(test_array_4.xml)
add_dmr_parse_test(test_array_5.xml)
add_dmr_parse_test(test_array_6.xml)
add_dmr_parse_test(test_array_7.xml)
add_dmr_parse_test(test_array_8.xml)
add_dmr_parse_test(test_array_10.xml)
add_dmr_parse_test(test_array_11.xml)

add_dmr_parse_test(ignore_foreign_xml_1.xml)
add_dmr_parse_test(ignore_foreign_xml_2.xml)
add_dmr_parse_test(ignore_foreign_xml_3.xml)

add_dmr_parse_test(test_array_9.xml)
add_dmr_parse_test(test_array_12.xml)
add_dmr_parse_test(test_array_13.xml)
add_dmr_parse_test(test_array_14.xml)

# Test empty Structures. jhrg 1/29/16
add_dmr_parse_test(test_simple_6.2.xml)
add_dmr_parse_test(test_simple_6.3.xml)

if(CMAKE_C_BYTE_ORDER STREQUAL "LITTLE_ENDIAN")
	set(word_order "little-endian")
elseif(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
	set(word_order "big-endian")
else()
	message(WARNING "Unknown byte order for C compiler")
endif()

function(add_dmr_trans_test test_input)
	# Here the name of the DMR file is morphed into something that will work
	# as a cmake name (dots are not allowed in cmake names). 7/8/25 jhrg
	# get_filename_component(fullname "${test_input}" NAME)
	# strip just "test.1.xml" → raw="test.1"
	string(REGEX REPLACE "\\.xml$" "" raw "${test_input}")
	# sanitize; test.1 → test_1
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_trans_${raw}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	# From the autotest DMRTest.at: dmr-testsuite/$WORD_ORDER/$1.trans_base
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${word_order}/${test_input}.trans_base")
	set(output     "${CMAKE_CURRENT_SOURCE_DIR}/${testname}.out")

	# Add the CTest entry. Here the shell is used so that we can employ redirection.
	# The extra double quotes are 'best practice' for cmake, but really not needed here
	# because we know that $<TARGET_FILE:dmr-test> and the various variables (e.g. ${input})
	# do not have spaces. The extra backslash characters make it harder to decipher
	# what is going on. 7/8/25 jhrg

	# From the autotest DMRTest.at:
	# AT_CHECK([$abs_builddir/dmr-test -x -t $input || true], [], [stdout], [stderr])
	# AT_CHECK([diff -b -B $baseline stdout || diff -b -B $baseline stderr], [], [ignore],[],[])

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			# 1) run das-test, redirect all output into a temp file
			# 2) diff that file against the baseline"
			"\"$<TARGET_FILE:dmr-test>\" -x -t \"${input}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)
	# This makes it so we cna run just these tests and also makes it easy to run the
	# unit tests _before_ the integration tests with a 'check' target. See the top-leve
	# CMakeLists file. 7/8/25 jhrg
	set_tests_properties(${testname} PROPERTIES LABELS integration)
endfunction()

add_dmr_trans_test(test_simple_1.xml)
add_dmr_trans_test(test_simple_2.xml)
add_dmr_trans_test(test_simple_3.xml)
add_dmr_trans_test(test_simple_4.xml)
add_dmr_trans_test(test_simple_5.xml)
add_dmr_trans_test(test_simple_6.xml)
add_dmr_trans_test(test_simple_7.xml)
add_dmr_trans_test(test_simple_8.xml)
add_dmr_trans_test(test_simple_9.xml)
add_dmr_trans_test(test_simple_9.1.xml)
add_dmr_trans_test(test_simple_10.xml)
add_dmr_trans_test(test_enum_grp.xml)

add_dmr_trans_test(test_array_1.xml)
add_dmr_trans_test(test_array_2.xml)
add_dmr_trans_test(test_array_3.xml)
add_dmr_trans_test(test_array_4.xml)
add_dmr_trans_test(test_array_5.xml)
add_dmr_trans_test(test_array_6.xml)
add_dmr_trans_test(test_array_7.xml)
add_dmr_trans_test(test_array_8.xml)
add_dmr_trans_test(test_array_10.xml)
add_dmr_trans_test(test_array_11.xml)

add_dmr_trans_test(test_array_9.xml)
add_dmr_trans_test(test_array_12.xml)
add_dmr_trans_test(test_array_13.xml)
add_dmr_trans_test(test_array_14.xml)

add_dmr_trans_test(test_simple_6.2.xml)
add_dmr_trans_test(test_simple_6.3.xml)
