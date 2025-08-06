
### DDS integration & regression tests.

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
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dds_${raw}")
	# message(STATUS "testname: ${testname}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/${dds_filename}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dds-testsuite/${dds_filename}.base")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	# Add the CTest entry. Here the shell is used so that we can employ redirection.
	# The extra double quotes are 'best practice' for cmake, but really not needed here
	# because we know that $<TARGET_FILE:dds-test> and the various variables (e.g. ${input})
	# do not have spaces. The extra backslash characters make it harder to decipher
	# what is going on. 7/8/25 jhrg
	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dds-test>\" -p < \"${input}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)
	# This makes it so we can run just these tests and also makes it easy to run the
	# unit tests _before_ the integration tests with a 'check' target. See the top-level
	# CMakeLists file. 7/8/25 jhrg
	set_tests_properties(${testname} PROPERTIES LABELS "integration;dds")
endfunction()

## Iterate over all of the DDS filed and make a cmake/ctest for each one. 7/8/25 jhrg
foreach(dfile IN LISTS DDS_FILES)
	add_dds_test(${dfile})
endforeach()
