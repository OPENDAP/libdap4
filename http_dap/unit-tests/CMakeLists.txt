# d4_ce/unit-tests/CMakeLists.txt

set(HTTP_DAP_TESTS HTTPCacheTest.cc HTTPConnectTest.cc HTTPThreadsConnectTest.cc)

function(build_test target source_files)
	add_executable(${target} ${source_files} remove_directory.cc)

	target_include_directories(${target}
			PRIVATE
			${CMAKE_SOURCE_DIR}
			${CMAKE_SOURCE_DIR}/http_dap
			${CMAKE_CURRENT_BINARY_DIR}
			${CMAKE_BINARY_DIR}
	)

	target_link_libraries(${target}
			PRIVATE
			dap
			dapclient
			CppUnit::CppUnit
	)

	add_test(NAME ${target} COMMAND ${target})
	# This enables using `ctest -L unit` to run just these tests.
	set_tests_properties(${test_name} PROPERTIES LABELS "unit")
endfunction()

foreach(src ${HTTP_DAP_TESTS})
	get_filename_component(tgt ${src} NAME_WE)
	build_test(${tgt} ${src})
endforeach()

# Set TEST_SRC_DIR to the absolute path of the test sources
set(abs_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(abs_builddir "${CMAKE_CURRENT_BINARY_DIR}")

# message(STATUS "TEST_SRC_DIR = ${abs_srcdir}")

# Generate test_config.h from the template
configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in
		${CMAKE_CURRENT_BINARY_DIR}/test_config.h
)