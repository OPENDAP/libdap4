
### DAS integration & regression tests.

file(GLOB DAS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite"
		"${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/*.das")

# Helper to register one DAS‐response test
function(add_das_test das_filename)
	#	# get "test.1.das" → fullname="test.1.das"
	#	message(STATUS "testname: ${testname}")
	get_filename_component(fullname "${das_filename}" NAME)
	# strip just ".das" → raw="test.1"
	string(REGEX REPLACE "\\.das$" "" raw "${fullname}")
	# sanitize; test.1 → test_1
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "das_${raw}")
	# message(STATUS "testname: ${testname}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/${das_filename}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/${das_filename}.base")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	# Add the CTest entry. Assume das_filename has no spaces -> no need to quote the
	# variables in the shell command -> makes the command more readable
	add_test(NAME ${testname}
			COMMAND /bin/sh -c  "$<TARGET_FILE:das-test> -p < ${input} > ${output} 2>&1; \
					diff -u -b -B ${baseline} ${output} && rm -f ${output}"
	)
	set_tests_properties(${testname} PROPERTIES LABELS "integration;das")
endfunction()

foreach(das_file IN LISTS DAS_FILES)
	add_das_test(${das_file})
endforeach()
