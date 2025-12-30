
### DAS integration & regression tests.

file(GLOB DAS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite"
		"${CMAKE_CURRENT_SOURCE_DIR}/das-testsuite/*.das")

# Accumulate all per-test baseline targets into this list. Used to make the
# 'das-baselines' target that rebuilds all of the baselines.
set(_DAS_BASELINE_TARGETS "")

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

	set(staged_baseline "${CMAKE_CURRENT_BINARY_DIR}/baselines/${das_filename}.base")
	set(baseline_tgt "baseline-${testname}")
	add_custom_target(${baseline_tgt}
			COMMAND ${CMAKE_COMMAND} -E make_directory "$<SHELL_PATH:${CMAKE_CURRENT_BINARY_DIR}/baselines>"
			COMMAND /bin/sh -c  "$<TARGET_FILE:das-test> -p < ${input} > ${output} 2>&1; \
         		${CMAKE_COMMAND} -E copy ${output} ${staged_baseline}"
			BYPRODUCTS "${staged_baseline}"
			COMMENT "Staging DAS baseline for ${das_filename} → ${staged_baseline}"
			VERBATIM
	)

	# Share the target name with the parent scope so we can build an aggregate target
	set(_DAS_BASELINE_TARGETS ${_DAS_BASELINE_TARGETS} ${baseline_tgt} PARENT_SCOPE)
endfunction()

# Register all tests (and per-test baseline targets).
# Use 'make test' or 'make check' to run these. See the top-level CMakeLists file
# for more ways to run the tests.
#
# Use 'make baseline-das_test_1' or 'cmake --build . --target baseline-das_test_1', ...
# build the baselines. Use 'ctest -R das_test_1' to run a single test.
foreach(das_file IN LISTS DAS_FILES)
	add_das_test(${das_file})
endforeach()

# Aggregate target: regenerate all DAS baselines in one shot. Use
# 'cmake --build . --target das-baselines' for that.
if(_DAS_BASELINE_TARGETS)
	add_custom_target(das-baselines DEPENDS ${_DAS_BASELINE_TARGETS})
endif()
