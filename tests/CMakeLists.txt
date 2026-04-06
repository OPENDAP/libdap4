
find_program(AUTOCONF_EXECUTABLE autoconf REQUIRED)
find_program(AUTOM4TE_EXECUTABLE autom4te REQUIRED)

add_library(test-types STATIC
		TestArray.cc TestD4Sequence.cc TestInt32.cc TestTypeFactory.cc
		TestByte.cc TestFloat32.cc TestInt64.cc TestUInt16.cc
		TestFloat64.cc TestInt8.cc TestUInt32.cc
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

# This is an alternative to the set(VAR ...) and install(FILES ...) pattern
# I used in the top-level CMakeLists file. It will install every header here
# which makes it more robust when files change/move but also easier to install
# something that should be private. 6/27/25 jhrg
file(GLOB TEST_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# TODO Remove dead code
# set(TEST_HEADERS
# 		TestD4Enum.h		TestGrid.h			TestStructure.h TestD4Group.h
# 		TestInt16.h			TestTypeFactory.h 	TestD4Opaque.h	TestInt32.h
# 		TestUInt16.h 		TestD4Sequence.h	TestInt64.h		TestUInt32.h
# 		TestArray.h			TestFloat32.h		TestInt8.h		TestUInt64.h
# 		TestByte.h			TestFloat64.h		TestSequence.h	TestUrl.h
# 		TestCommon.h		TestFunction.h		TestStr.h
# 		D4TestFunction.h	D4TestTypeFactory.h)

# install(FILES ${TEST_HEADERS}
# 		DESTINATION include/libdap/test)

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
	# message(STATUS "dmr-test, TIRPC_INCLUDE_DIRS: ${TIRPC_INCLUDE_DIRS}")
	target_link_libraries(dmr-test PRIVATE ${TIRPC_LIBRARIES})
endif()

set(TESTS_AUTOCONF_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac")
set(TESTS_CONFIGURE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/configure")
set(TESTS_ATCONFIG "${CMAKE_CURRENT_BINARY_DIR}/atconfig")
set(TESTS_ATLOCAL "${CMAKE_CURRENT_BINARY_DIR}/atlocal")
set(TESTS_PACKAGE_M4 "${CMAKE_CURRENT_BINARY_DIR}/package.m4")

add_custom_command(
		OUTPUT "${TESTS_CONFIGURE_SCRIPT}"
		COMMAND "${AUTOCONF_EXECUTABLE}" "--output=${TESTS_CONFIGURE_SCRIPT}" "${TESTS_AUTOCONF_INPUT}"
		DEPENDS "${TESTS_AUTOCONF_INPUT}"
		COMMENT "Generating tests/configure from ${TESTS_AUTOCONF_INPUT}"
		VERBATIM
)

add_custom_command(
		OUTPUT "${TESTS_ATCONFIG}" "${TESTS_ATLOCAL}" "${TESTS_PACKAGE_M4}"
		COMMAND "${TESTS_CONFIGURE_SCRIPT}" "--srcdir=${CMAKE_CURRENT_SOURCE_DIR}"
		DEPENDS
			"${TESTS_CONFIGURE_SCRIPT}"
			"${CMAKE_CURRENT_SOURCE_DIR}/atlocal.in"
			"${CMAKE_CURRENT_SOURCE_DIR}/package.m4.in"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Configuring autotest support files in ${CMAKE_CURRENT_BINARY_DIR}"
		VERBATIM
)

set(AUTOTEST_SUITES
		DASTest
		DDSTest
		EXPRTest
		DMRTest
		getdapTest
)

set(AUTOTEST_GENERATED_SCRIPTS "")

foreach(autotest_suite IN LISTS AUTOTEST_SUITES)
	set(autotest_input "${CMAKE_CURRENT_SOURCE_DIR}/${autotest_suite}.at")
	set(autotest_output "${CMAKE_CURRENT_BINARY_DIR}/${autotest_suite}")

	add_custom_command(
			OUTPUT "${autotest_output}"
			COMMAND
				"${AUTOM4TE_EXECUTABLE}"
				"--language=autotest"
				"-I" "${CMAKE_CURRENT_SOURCE_DIR}"
				"-I" "${CMAKE_CURRENT_BINARY_DIR}"
				"-o" "${autotest_output}.tmp"
				"${autotest_input}"
			COMMAND "${CMAKE_COMMAND}" -E rename "${autotest_output}.tmp" "${autotest_output}"
			DEPENDS
				"${autotest_input}"
				"${TESTS_ATCONFIG}"
				"${TESTS_ATLOCAL}"
				"${TESTS_PACKAGE_M4}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
			COMMENT "Generating autotest driver ${autotest_suite}"
			VERBATIM
	)

	list(APPEND AUTOTEST_GENERATED_SCRIPTS "${autotest_output}")
endforeach()

add_custom_target(tests ALL
		DEPENDS
			${AUTOTEST_GENERATED_SCRIPTS}
			das-test
			dds-test
			expr-test
			dmr-test
			getdap
)

function(add_autotest_suite_test suite_name suite_label)
	set(suite_script "${CMAKE_CURRENT_BINARY_DIR}/${suite_name}")

	add_test(
			NAME ${suite_name}
			COMMAND /bin/sh -c "eval \"exec \\\"\$1\\\" \$TESTSUITEFLAGS\"" _ "${suite_script}"
	)

	# Keep the suite wrappers serial at the CTest layer and rely on
	# TESTSUITEFLAGS (for example, --jobs=N) for any intra-suite parallelism.
	set_tests_properties(${suite_name} PROPERTIES
			LABELS "integration;${suite_label}"
			RUN_SERIAL TRUE
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
endfunction()

add_autotest_suite_test(DASTest das)
add_autotest_suite_test(DDSTest dds)
add_autotest_suite_test(EXPRTest expr)
add_autotest_suite_test(DMRTest dmr)
add_autotest_suite_test(getdapTest getdap)
