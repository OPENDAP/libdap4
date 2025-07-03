
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

# Replace:
# target_include_directories(test-types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# With the following. This enables linking locally and then installing so
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
# For example, it was installing the D4ResponseBuilder.h and ResponseBuilder.h
# headers, but those should not be installed. 6/29/25 jhrg
#
# file(GLOB TEST_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

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

add_executable(dds-test dds-test.cc)
target_link_libraries(dds-test PRIVATE test-types dapserver dap)

add_executable(expr-test expr-test.cc ResponseBuilder.cc)
target_include_directories(expr-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(expr-test PRIVATE test-types dapserver dapclient dap)

add_executable(dmr-test dmr-test.cc D4ResponseBuilder.cc)
target_include_directories(dmr-test
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/d4_ce  ${CMAKE_SOURCE_DIR}/d4_function)
target_link_libraries(dmr-test PRIVATE test-types dapserver dap)

