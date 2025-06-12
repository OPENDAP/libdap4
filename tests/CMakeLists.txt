
add_library(test-types STATIC
		TestArray.cc TestD4Sequence.cc TestInt32.cc TestTypeFactory.cc
		TestByte.cc TestFloat32.cc TestInt64.cc TestUInt16.cc
		TestCommon.cc TestFloat64.cc TestInt8.cc TestUInt32.cc
		TestD4Enum.cc TestFunction.cc TestSequence.cc TestUInt64.cc
		TestD4Group.cc TestGrid.cc TestStr.cc TestUrl.cc
		TestD4Opaque.cc TestInt16.cc TestStructure.cc
)

target_include_directories(test-types
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test-types PRIVATE dap)
