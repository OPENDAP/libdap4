
### DDS integration & regression tests.

## I used file() with GLOB to grab all of the files that match *.dds in the
## dds-testsuite subdir. This is simple and eliminates the need to list every
## file, but can inadvertently include files that are not part of the test.
## Listing the files intended, one by one, is better but also tedious and means
## that new tests have to be manually added. 7/8/25 jhrg
file(GLOB DDS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite"
		"${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/*.dds")

# Accumulate all per-test baseline targets into this list. Used to make the
# 'das-baselines' target that rebuilds all of the baselines.
set(_DDS_BASELINE_TARGETS "")

## This function will take the name of a DDS file and use it as input to a DDS
## test. There are some tricks here as well. 7/8/25 jhrg
function(add_dds_test dds_filename)
	# Here the name of the dds file is morphed into something that will work
	# as a cmake name (dots are not allowed in cmake names). 7/8/25 jhrg
	get_filename_component(fullname "${dds_filename}" NAME)
	string(REGEX REPLACE "\\.dds$" "" raw "${fullname}")
	# sanitize; test.1 → test_1
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dds_${raw}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/${dds_filename}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/${dds_filename}.base")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	set(the_test   "$<TARGET_FILE:dds-test> -p < ${input} > ${output} 2>&1")

	# Add the CTest entry. Here the shell is used so that we can employ redirection.
	# The extra double quotes are 'best practice' for cmake, but really not needed here
	# because we know that $<TARGET_FILE:dds-test> and the various variables (e.g. ${input})
	# do not have spaces. The extra backslash characters make it harder to decipher
	# what is going on. 7/8/25 jhrg
	# ...so  removed them. jhrg 12/30/25
	add_test(NAME ${testname}
			COMMAND /bin/sh -c "${the_test}; diff -u -b -B ${baseline} ${output} && rm -f ${output}"
	)
	# This makes it so we can run just these tests and also makes it easy to run the
	# unit tests _before_ the integration tests with a 'check' target. See the top-level
	# CMakeLists file. 7/8/25 jhrg
	set_tests_properties(${testname} PROPERTIES LABELS "integration;dds")

	set(staged_baseline "${CMAKE_CURRENT_BINARY_DIR}/baselines/${dds_filename}.base")
	set(baseline_tgt "baseline-${testname}")
	add_custom_target(${baseline_tgt}
			COMMAND ${CMAKE_COMMAND} -E make_directory "$<SHELL_PATH:${CMAKE_CURRENT_BINARY_DIR}/baselines>"
			COMMAND /bin/sh -c  "${the_test}; ${CMAKE_COMMAND} -E copy ${output} ${staged_baseline} && rm -f ${output}"
			BYPRODUCTS "${staged_baseline}"
			COMMENT "Staging DAS baseline for ${das_filename} → ${staged_baseline}"
			VERBATIM
	)

	# Share the target name with the parent scope so we can build an aggregate target
	set(_DDS_BASELINE_TARGETS ${_DDS_BASELINE_TARGETS} ${baseline_tgt} PARENT_SCOPE)

endfunction()

## Iterate over all of the DDS filed and make a cmake/ctest for each one. 7/8/25 jhrg
foreach(dds_file IN LISTS DDS_FILES)
	add_dds_test(${dds_file})
endforeach()

# Aggregate target: regenerate all DDS baselines in one shot. Use
# 'cmake --build . --target dds-baselines' for that.
if(_DDS_BASELINE_TARGETS)
	add_custom_target(dds-baselines DEPENDS ${_DDS_BASELINE_TARGETS})
endif()
