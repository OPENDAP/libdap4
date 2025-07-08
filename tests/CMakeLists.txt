
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
	target_link_libraries(dds-test PRIVATE ${TIRPC_LIBRARIES})
endif()

add_executable(expr-test expr-test.cc ResponseBuilder.cc)
target_include_directories(expr-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})
target_link_libraries(expr-test PRIVATE test-types dapserver dapclient dap)
if(TIRPC_FOUND)
	target_include_directories(expr-test PRIVATE ${TIRPC_INCLUDE_DIRS})
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
