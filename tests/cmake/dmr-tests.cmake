## DMR tests. Unlike the DAS and DDS test, these test enumerate the individual test
## one by one since various tests will use different functions. 7/10/25 jhrg
## Also, since the inputs are each listed explicitly, they are not paths that have to
## be trimmed using 'get_filename_component(fullname "${dds_filename}" NAME)'. 7/11/25 jhrg
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
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

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
	# This makes it so we can run just these tests and also makes it easy to run the
	# unit tests _before_ the integration tests with a 'check' target. See the top-leve
	# CMakeLists file. 7/8/25 jhrg
	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr")
endfunction()

#AT_CHECK([$abs_builddir/dmr-test -x -p $input -c $ce], [ignore], [stdout], [stderr])
#AT_CHECK([cat stdout stderr > tmp])
#AT_CHECK([diff -b -B $baseline tmp], [0])
#AT_XFAIL_IF([test "z$3" = "zxfail"])
function(add_dmr_parse_ce_test test_input ce test_baseline)
	# string(REGEX REPLACE "\\.xml$" "" raw "${test_baseline}")
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_parse_ce_${test_baseline}")

	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_baseline}")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			# 1) run das-test, redirect all output into a temp file
			# 2) diff that file against the baseline"
			"\"$<TARGET_FILE:dmr-test>\" -x -p \"${input}\" -c \"${ce}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)
	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr,parse-ce")
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

# Test DAP CE parse errors - ensure they don't leak the supplied
# CE text into the error message. jhrg 4/15/20
add_dmr_parse_ce_test("test_simple_1.xml" "nasty" "test_simple_1.xml.parse_ce_1")
# This string is 'd1rox<script>alert(1)</script>d55je=1' (%25 --> '%')
# That is, the % is escaped in this text: %253c --> %3c --> '<'
add_dmr_parse_ce_test("test_simple_1.xml" "d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1" "test_simple_1.xml.parse_ce_2")

add_dmr_parse_ce_test("test_simple_6.3.xml" "s.nasty" "test_simple_6.3.xml.parse_ce_1")
add_dmr_parse_ce_test("test_simple_6.3.xml" "s.d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1" "test_simple_6.3.xml.parse_ce_2")

add_dmr_parse_ce_test("vol_1_ce_12.xml" "temp[nasty]" "vol_1_ce_12.xml.parse_ce_1")
add_dmr_parse_ce_test("vol_1_ce_12.xml" "temp[d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1]" "vol_1_ce_12.xml.parse_ce_2")

# Test reversed array indices
add_dmr_parse_ce_test("test_array_4.xml" "b[2:1][2:3]" "test_array_4.xml.error.base")


## For byte_order == universal, this test removed the <Value> information
## and replaces it with nothing (which is different from the trans + ce
## tests which replace the <Value> info with 'removed checksum'). 7/12/25 jhrg
function(add_dmr_trans_test test_input byte_order)
	string(REGEX REPLACE "\\.xml$" "" raw "${test_input}")
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_trans_${raw}")
	if("${byte_order}" STREQUAL "universal")
		set(testname "${testname}_univ")
	endif()
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${byte_order}/${test_input}.trans_base")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dmr-test>\" -C -x -t \"${input}\" > \"${output}\" 2>&1; \
			if test \"${byte_order}\" = \"universal\"; then \
				sed 's@<Value>[0-9a-f][0-9a-f]*</Value>@@' \"${output}\" > \"${output}_univ\"; \
				diff -b -B \"${baseline}\" \"${output}_univ\"; \
			else \
				diff -b -B \"${baseline}\" \"${output}\"; \
			fi;"
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr;trans")
	# set_tests_properties(${testname} PROPERTIES RESOURCE_LOCKS "dmr-trans")
	# These are run serially because the dmr-test program uses a temp file to
	# store the 'binary data' amd different tests will make those binary data
	# files with the same (i.e., conflicting) names. I tied using the
	# RESOURCE_LOCKS property, but it was slower and did not stop the collisions.
	# 7/12/25 jhrg
	set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
endfunction()

if(CMAKE_C_BYTE_ORDER STREQUAL "LITTLE_ENDIAN")
	set(word_order "little-endian")
elseif(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
	set(word_order "big-endian")
else()
	message(WARNING "Unknown byte order for C compiler")
endif()

add_dmr_trans_test(test_simple_1.xml "${word_order}")
add_dmr_trans_test(test_simple_2.xml "${word_order}")
add_dmr_trans_test(test_simple_3.xml "${word_order}")
add_dmr_trans_test(test_simple_4.xml "${word_order}")
add_dmr_trans_test(test_simple_5.xml "${word_order}")
add_dmr_trans_test(test_simple_6.xml "${word_order}")
add_dmr_trans_test(test_simple_7.xml "${word_order}")
add_dmr_trans_test(test_simple_8.xml "${word_order}")
add_dmr_trans_test(test_simple_9.xml "${word_order}")
add_dmr_trans_test(test_simple_9.1.xml "${word_order}")
add_dmr_trans_test(test_simple_10.xml "${word_order}")
add_dmr_trans_test(test_enum_grp.xml "${word_order}")

add_dmr_trans_test(test_array_1.xml "${word_order}")
add_dmr_trans_test(test_array_2.xml "${word_order}")
add_dmr_trans_test(test_array_3.xml "${word_order}")
add_dmr_trans_test(test_array_4.xml "${word_order}")
add_dmr_trans_test(test_array_5.xml "${word_order}")
add_dmr_trans_test(test_array_6.xml "${word_order}")
add_dmr_trans_test(test_array_7.xml "${word_order}")
add_dmr_trans_test(test_array_8.xml "${word_order}")
add_dmr_trans_test(test_array_10.xml "${word_order}")
add_dmr_trans_test(test_array_11.xml "${word_order}")

add_dmr_trans_test(test_array_9.xml "${word_order}")
add_dmr_trans_test(test_array_12.xml "${word_order}")
add_dmr_trans_test(test_array_13.xml "${word_order}")
add_dmr_trans_test(test_array_14.xml "${word_order}")

add_dmr_trans_test(test_simple_6.2.xml "${word_order}")
add_dmr_trans_test(test_simple_6.3.xml "${word_order}")

# Test out the 'universal' tests
add_dmr_trans_test(test_array_9.xml "universal")
add_dmr_trans_test(test_array_12.xml "universal")
add_dmr_trans_test(test_array_13.xml "universal")
add_dmr_trans_test(test_array_14.xml "universal")

add_dmr_trans_test(test_simple_6.2.xml "universal")
add_dmr_trans_test(test_simple_6.3.xml "universal")

function(add_dmr_intern_test test_input)
	string(REGEX REPLACE "\\.xml$" "" raw "${test_input}")
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_intern_${raw}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${word_order}/${test_input}.intern_base")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	# Add the CTest entry. 7/8/25 jhrg
	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dmr-test>\" -x -i \"${input}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr")
endfunction()

add_dmr_intern_test(test_simple_1.xml)
add_dmr_intern_test(test_simple_2.xml)
add_dmr_intern_test(test_simple_3.xml)
add_dmr_intern_test(test_simple_4.xml)
add_dmr_intern_test(test_simple_5.xml)
add_dmr_intern_test(test_simple_6.xml)
add_dmr_intern_test(test_simple_7.xml)
add_dmr_intern_test(test_simple_8.xml)
add_dmr_intern_test(test_simple_9.xml)
add_dmr_intern_test(test_simple_9.1.xml)
add_dmr_intern_test(test_simple_10.xml)
add_dmr_intern_test(test_enum_grp.xml)

add_dmr_intern_test(test_array_1.xml)
add_dmr_intern_test(test_array_2.xml)
add_dmr_intern_test(test_array_3.xml)
add_dmr_intern_test(test_array_4.xml)
add_dmr_intern_test(test_array_5.xml)
add_dmr_intern_test(test_array_6.xml)
add_dmr_intern_test(test_array_7.xml)
add_dmr_intern_test(test_array_8.xml)
add_dmr_intern_test(test_array_10.xml)
add_dmr_intern_test(test_array_11.xml)

add_dmr_intern_test(test_array_9.xml)
add_dmr_intern_test(test_array_12.xml)
add_dmr_intern_test(test_array_13.xml)
add_dmr_intern_test(test_array_14.xml)

add_dmr_intern_test(test_simple_6.2.xml)
add_dmr_intern_test(test_simple_6.3.xml)

# For these tests, use the baseline filename to form the test name
# since those are unique while the inputs are used multiple times.
# 7/11/25 jhrg
function(add_dmr_trans_ce_test test_input ce test_baseline byte_order)
	string(REGEX REPLACE "\\.xml$" "" raw "${test_input}")
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_trans_${raw}_${ce}")
	if("${byte_order}" STREQUAL "universal")
		set(testname "${testname}_univ")
	endif()
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${byte_order}/${test_baseline}")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	#message(STATUS "CE: ${ce}")

	add_test(NAME ${testname}
			COMMAND /bin/sh -c
			"$<TARGET_FILE:dmr-test> -C -x -t ${input} -c \'${ce}\' > ${output} 2>&1; \
			if test \"${byte_order}\" = \"universal\"; then \
				sed 's@<Value>[0-9a-f][0-9a-f]*</Value>@removed checksum@' \"${output}\" > \"${output}_univ\"; \
				diff -b -B \"${baseline}\" \"${output}_univ\"; \
			else \
				diff -b -B \"${baseline}\" \"${output}\"; \
			fi;"
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr;ce")
	set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
endfunction()

if("${word_order}" STREQUAL "little-endian")
	add_dmr_trans_ce_test("test_array_3.1.dmr" "row;x" "test_array_3.1.dmr.1.trans_base" ${word_order})
	add_dmr_trans_ce_test("test_array_3.1.dmr" "row=[2:3];x" "test_array_3.1.dmr.2.trans_base" ${word_order})
	add_dmr_trans_ce_test("test_array_3.1.dmr" "row=[2:3];x[0:1]" "test_array_3.1.dmr.3.trans_base" ${word_order})
	add_dmr_trans_ce_test("test_array_3.1.dmr" "x[0:1]" "test_array_3.1.dmr.4.trans_base" ${word_order})
	add_dmr_trans_ce_test("test_array_3.1.dmr" "x" "test_array_3.1.dmr.5.trans_base" ${word_order})
endif()

# Test various facets of the CE parser and evaluation engine

add_dmr_trans_ce_test("test_array_4.xml" "a" "test_array_4.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_4.xml" "a[][] " "test_array_4.xml.1.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_4.xml" "/row=[0:1];/col=[3];a" "test_array_4.xml.3.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_4.xml" "/row=[0:1];/col=[3];a[][] " "test_array_4.xml.4.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_4.xml" "/row=[0:1];/col=[3];a[][];b[0][];c[0:][0:] " "test_array_4.xml.5.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_4.xml" "x[][] " "/test_array_4.xml.6.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_4.xml" "/row=[0:1];x[][] " "test_array_4.xml.7.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_4.xml" "c[2:][2:] " "test_array_4.xml.8.trans_base" ${word_order})

add_dmr_trans_ce_test("test_simple_6.xml" "s" "test_simple_6.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_6.xml" "s.i1" "test_simple_6.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_6.xml" "s.s" "test_simple_6.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_6.1.xml" "s.inner.i2" "test_simple_6.1.xml.1.trans_base" ${word_order})

add_dmr_trans_ce_test("test_simple_6.xml" "s{i1}" "test_simple_6.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_6.xml" "s{s}" "test_simple_6.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_6.1.xml" "s{inner.i2}" "test_simple_6.1.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_6.1.xml" "s{inner{i2}}" "test_simple_6.1.xml.1.trans_base" ${word_order})

# test_array_6 holds a 2D array of Structure
add_dmr_trans_ce_test("test_array_6.xml" "a" "test_array_6.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.xml" "a[][] " "test_array_6.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.xml" "/row=[0:1];a[][] " "test_array_6.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.xml" "/row=[0:1];a[][1:2] " "test_array_6.xml.3.trans_base" ${word_order})

# test_array holds a Structure that has a 2D array for a field
add_dmr_trans_ce_test("test_array_6.2.xml" "a" "test_array_6.2.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.2.xml" "a{i;j}" "test_array_6.2.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.2.xml" "a.i" "test_array_6.2.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.2.xml" "a{i}" "test_array_6.2.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.2.xml" "a.i[0][1:2] " "test_array_6.2.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.2.xml" "a{i[0][1:2]} " "test_array_6.2.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.2.xml" "/row=[0:1];a.i[][1:2] " "test_array_6.2.xml.4.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.2.xml" "/row=[0:1];a{i[][1:2]} " "test_array_6.2.xml.4.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_6.2.xml" "a.j" "test_array_6.2.xml.5.trans_base" ${word_order})

# test_array_6.1 is an array of Structure that holds an array and a scalar
add_dmr_trans_ce_test("test_array_6.1.xml" "a" "test_array_6.1.xml.1.trans_base" ${word_order})

# slice the structure but not the field
add_dmr_trans_ce_test("test_array_6.1.xml" "/row=[1:2];a[][0] " "test_array_6.1.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.1.xml" "/row=[1:2];a[][0]{i;j} " "test_array_6.1.xml.2.trans_base" ${word_order})
# Do we really need the FQN?
add_dmr_trans_ce_test("test_array_6.1.xml" "row=[1:2];a[][0]{i;j} " "test_array_6.1.xml.2.trans_base" ${word_order})

# slice the field but not the structure
add_dmr_trans_ce_test("test_array_6.1.xml" "a{i[1:2][1:3];j} " "test_array_6.1.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_6.1.xml" "a[][]{i[1:2][1:3];j} " "test_array_6.1.xml.3.trans_base" ${word_order})

# slice both the structure and the array it contains using both the shared dim and
# a local slice for each
add_dmr_trans_ce_test("test_array_6.1.xml" "/row=[1];a[][0]{i[][0:1]} " "test_array_6.1.xml.4.trans_base" ${word_order})

# Test sequences and arrays of sequences
add_dmr_trans_ce_test("test_simple_7.xml" "s" "test_simple_7.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_7.xml" "s{i1;s}" "test_simple_7.xml.1.trans_base" ${word_order})

add_dmr_trans_ce_test("test_simple_7.xml" "s.i1" "test_simple_7.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_simple_7.xml" "s{i1}" "test_simple_7.xml.2.trans_base" ${word_order})

# Modified to use elided CRC - we have tested the CRC elsewhere and in the BES
# we remove the CRC from both the output and the baselines to avoid making baselines
# for both big- and little-endian machines. jhrg 1/22/24
add_dmr_trans_ce_test("test_simple_8.xml" "outer" "test_simple_8.xml.1.trans_base" "universal")
add_dmr_trans_ce_test("test_simple_8.xml" "outer.s.s" "test_simple_8.xml.2.trans_base" "universal")
add_dmr_trans_ce_test("test_simple_8.xml" "outer{s{s}}" "test_simple_8.xml.2.trans_base" "universal")

add_dmr_trans_ce_test("test_array_7.xml" "s" "test_array_7.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.xml" "s{i1;s}" "test_array_7.xml.1.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.xml" "s.i1" "test_array_7.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.xml" "s{i1}" "test_array_7.xml.2.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.xml" "s[1] " "test_array_7.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.xml" "s[1]{i1;s}" "test_array_7.xml.3.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.xml" "s[1]{i1}" "test_array_7.xml.4.trans_base" ${word_order})

# test_array_8 has a 2D Sequence that uses a shared dim
add_dmr_trans_ce_test("test_array_8.xml" "/col=[1:2];s[1][]{i1}" "test_array_8.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_8.xml" "col=[1:2];s[1][]{i1}" "test_array_8.xml.1.trans_base" ${word_order})

# test_array_7.1 holds a sequence that has an array for one of its fields
add_dmr_trans_ce_test("test_array_7.1.xml" "" "test_array_7.1.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "s" "test_array_7.1.xml.1.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.1.xml" "s.i1" "test_array_7.1.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "s.i1[][] " "test_array_7.1.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "s{i1}" "test_array_7.1.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "s{i1[][]} " "test_array_7.1.xml.2.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.1.xml" "s.i1[0][0] " "test_array_7.1.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "s{i1[0][0]} " "test_array_7.1.xml.3.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.1.xml" "s.i1[0:2][1:2] " "test_array_7.1.xml.4.trans_base" ${word_order})

# Should this CE be supported? jhrg 12/23/13
# add_dmr_trans_ce_test("test_array_7.1.xml" "/row=[2:3];/col=[2:3] " "test_array_7.1.xml.5.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "/row=[2:3];/col=[2:3];s " "test_array_7.1.xml.5.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.1.xml" "/row=[2:3];/col=[2:3];s.i1 " "test_array_7.1.xml.6.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "/row=[2:3];/col=[2:3];s.i1[][] " "test_array_7.1.xml.6.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "/row=[2:3];/col=[2:3];s{i1} " "test_array_7.1.xml.6.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.1.xml" "/row=[2:3];/col=[2:3];s{i1[][]} " "test_array_7.1.xml.6.trans_base" ${word_order})

# test_array_7.2 is a sequence array that holds an array as one of its fields
add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s[]{i1}" "test_array_7.2.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s[]{i1[][]}" "test_array_7.2.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s{i1[][]}" "test_array_7.2.xml.1.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s[]{i1[0][]}" "test_array_7.2.xml.2.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s{i1[0][]}" "test_array_7.2.xml.2.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s[0]{i1}" "test_array_7.2.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s[0]{i1[][]}" "test_array_7.2.xml.3.trans_base" ${word_order})

add_dmr_trans_ce_test("test_array_7.2.xml" "/col=[1:2];s[0]{i1[0][]}" "test_array_7.2.xml.4.trans_base" ${word_order})

# Test projections that only name groups
add_dmr_trans_ce_test("vol_1_ce_2.xml" "/inst2" "vol_1_ce_2.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("vol_1_ce_2.xml" "inst2" "vol_1_ce_2.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("vol_1_ce_2.xml" "/inst2/Point" "vol_1_ce_2.xml.2.trans_base" ${word_order})

add_dmr_trans_ce_test("vol_1_ce_13.xml" "/inst2" "vol_1_ce_13.xml.1.trans_base" ${word_order})
add_dmr_trans_ce_test("vol_1_ce_13.xml" "/inst2/inst3" "vol_1_ce_13.xml.2.trans_base" ${word_order})

add_dmr_trans_ce_test("vol_1_ce_13.xml" "/attr_only_global" "vol_1_ce_13.xml.3.trans_base" ${word_order})
add_dmr_trans_ce_test("vol_1_ce_13.xml" "/inst2/attr_only" "vol_1_ce_13.xml.4.trans_base" ${word_order})

#AT_CHECK([$abs_builddir/dmr-test -x -t $input -f "$fe" -c "$ce" || true], [], [stdout], [stderr])
#AT_CHECK([diff -b -B $baseline stdout || diff -b -B $baseline stderr], [], [ignore],[],[])
function(add_dmr_trans_func_test test_input func ce test_baseline byte_order)
	string(REGEX REPLACE "\\.xml$" "" raw "${test_input}")
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_trans_${func}_${ce}_${raw}")
	if("${byte_order}" STREQUAL "universal")
		set(testname "${testname}_univ")
	endif()
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${byte_order}/${test_baseline}")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dmr-test>\" -C -x -t \"${input}\" -f \"${func}\" -c \"${ce}\" > \"${output}\" 2>&1; \
			if test \"${byte_order}\" = \"universal\"; then \
				sed 's@<Value>[0-9a-f][0-9a-f]*</Value>@removed checksum@' \"${output}\" > \"${output}_univ\"; \
				diff -b -B \"${baseline}\" \"${output}_univ\"; \
			else \
				diff -b -B \"${baseline}\" \"${output}\"; \
			fi;"
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr;func")
	set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
endfunction()

# Test the function parser and evaluator. The function 'scale' is defined
# for both DAP2 and DAP4 in D4TestFunction.cc/h
add_dmr_trans_func_test("test_array_1.xml" "scale(x,1)" "" "test_array_1.xml.1.func_base" "${word_order}")
add_dmr_trans_func_test("test_array_1.xml" "scale(x,10)" "" "test_array_1.xml.2.func_base" "${word_order}")
add_dmr_trans_func_test("test_array_1.xml" "scale(x,-10)" "" "test_array_1.xml.3.func_base" "${word_order}")
add_dmr_trans_func_test("test_array_1.xml" "scale(x,0.001)" "" "test_array_1.xml.4.func_base" "${word_order}")
add_dmr_trans_func_test("test_array_1.xml" "scale(x,-0.001)" "" "test_array_1.xml.5.func_base" "${word_order}")

# Test the largest signed int64 value (it will be stored in a D4RValue that
# holds a Int64 variable).
add_dmr_trans_func_test("test_array_1.xml" "scale(x,0x7fffffffffffffff)" "" "test_array_1.xml.6.func_base" "${word_order}")
# This only fits in a unsigned long long (DAP4's UInt64)
add_dmr_trans_func_test("test_array_1.xml" "scale(x,0x8fffffffffffffff)" "" "test_array_1.xml.7.func_base" "${word_order}")

# test_array_5 has 64 bit ints in c and d; a and b are Int8 and UInt8 types
# all of these test arrays that use named dimensions
add_dmr_trans_func_test("test_array_5.xml" "scale(a,0.001)" "" "test_array_5.xml.1.func_base" "${word_order}")
add_dmr_trans_func_test("test_array_5.xml" "scale(b,0.001)" "" "test_array_5.xml.2.func_base" "${word_order}")

add_dmr_trans_func_test("test_array_5.xml" "scale(c,0.001)" "" "test_array_5.xml.3.func_base" "${word_order}")
add_dmr_trans_func_test("test_array_5.xml" "scale(d,0.001)" "" "test_array_5.xml.4.func_base" "${word_order}")

# Use the vol_1_ce_* datasets for tests

# Test using variables for source values and functional composition
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(u,10)" "" "vol_1_ce_1.xml.1.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(u,v)" "" "vol_1_ce_1.xml.2.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(scale(u,10),0.01)" "" "vol_1_ce_1.xml.3.func_base" "${word_order}")

# Test name parsing for Structure members
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(Point.x,10)" "" "vol_1_ce_1.xml.4.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(Point.x,Point.y)" "" "vol_1_ce_1.xml.5.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(scale(Point.x,10),0.01)" "" "vol_1_ce_1.xml.6.func_base" "${word_order}")

# Test the 'array constant' special form (we need a dataset only because the parser needs a DMR to run)
# In cmake, '\' is the escape character and '\\' is used for a single backslash. 7/14/25 jhrg
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$Byte(20:1,2,3,4),10)" "" "vol_1_ce_1.xml.7.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$Int8(20:10,11,12,-9),10)" "" "vol_1_ce_1.xml.8.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$UInt16(20:1,2,3,4),10)" "" "vol_1_ce_1.xml.9.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$Int16(20:1,2,3,-4),10)" "" "vol_1_ce_1.xml.10.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$UInt32(20:1,2,3,4),10)" "" "vol_1_ce_1.xml.11.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$Int32(20:1,2,3,-4),10)" "" "vol_1_ce_1.xml.12.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$UInt64(20:1,2,3,0xffffffffffffffff),1)" "" "vol_1_ce_1.xml.13.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$Int64(20:1,2,3,0x7fffffffffffffff),1)" "" "vol_1_ce_1.xml.14.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$Float32(20:1,2,3,4.55),10)" "" "vol_1_ce_1.xml.15.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_1.xml" "scale(\\$Float64(20:1,2,3,4.55),10)" "" "vol_1_ce_1.xml.16.func_base" "${word_order}")

# Test a sequence of function calls and the application of a Constraint to
# a function result
add_dmr_trans_func_test("vol_1_ce_10.xml" "scale(lat,10)" "" "vol_1_ce_10.xml.1.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_10.xml" "scale(lat,10);scale(lon,10)" "" "vol_1_ce_10.xml.2.func_base" "${word_order}")
add_dmr_trans_func_test("vol_1_ce_10.xml" "scale(lat,10);scale(lon,10)" "lat[10:11][10:11];lon[10:11][10:11]" "vol_1_ce_10.xml.3.func_base" "${word_order}")

#DMR_TRANS_SERIES_CE
#AT_CHECK([$abs_builddir/dmr-test -x -e -t $input -c "$ce" | sed 's@<Value>[[0-9a-f]][[0-9a-f]]*</Value>@@' ], [], [stdout], [])
#AT_CHECK([diff -b -B $baseline stdout], [], [ignore],[],[])
#AT_XFAIL_IF([test "X$4" = "Xxfail"])

## Two things different about this set of tests: they assume baselines that are independent
## of word order and they use CEs that have operators (!, <=, ...). Making a test name substituting
## those chars with '_' doesn't make unique test names. But, for this we can use the baseline
## names. Also, some of these tests are expected to fail. 7/14/25 jhrg
function(add_dmr_trans_series_test test_input ce test_baseline xfail)
	string(REGEX REPLACE "\\.xml$" "" raw "${test_input}")
	string(REGEX REPLACE "[^A-Za-z0-9_]" "_" testname "dmr_series_${ce}_${test_baseline}")

	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/universal/${test_baseline}")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dmr-test>\" -C -x -e -t \"${input}\" -c \"${ce}\" > \"${output}\" 2>&1; \
			sed 's@<Value>[0-9a-f][0-9a-f]*</Value>@@' \"${output}\" > \"${output}_univ\"; \
			mv \"${output}_univ\" \"${output}\"; \
			diff -b -B \"${baseline}\" \"${output}\""
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr;series")
	set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
	if("${xfail}" STREQUAL "xfail")
		set_tests_properties(${testname} PROPERTIES WILL_FAIL TRUE)
	endif()
endfunction()

## These tests are all 'universal' tests (i.e., they do not need different baselines
## for different word order machines). 7/14/25 jhrg.
add_dmr_trans_series_test("test_simple_7.xml" "s" "test_simple_7.xml.f.trans_base" "")

add_dmr_trans_series_test("test_simple_7.xml" "s|i1==1024" "test_simple_7.xml.f1.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|i1!=1024" "test_simple_7.xml.f2.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|i1<=1024" "test_simple_7.xml.f3.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|i1<1024" "test_simple_7.xml.f4.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|i1<=1024" "test_simple_7.xml.f5.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|i1>1024" "test_simple_7.xml.f6.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|i1>=1024" "test_simple_7.xml.f7.trans_base" "")

add_dmr_trans_series_test("test_simple_7.xml" "s|1024<i1" "test_simple_7.xml.f8.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|1024<=i1<=32768" "test_simple_7.xml.f9.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|i1>=1024.0" "test_simple_7.xml.fa.trans_base" "")

## \\\" --> \\ is a literal slash and \" is a literal double quote. 7/14/25 jhrg
add_dmr_trans_series_test("test_simple_7.xml" "s|s==\\\"Silly test string: 2\\\"" "test_simple_7.xml.fs1.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|s!=\\\"Silly test string: 2\\\"" "test_simple_7.xml.fs2.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|s<\\\"Silly test string: 2\\\"" "test_simple_7.xml.fs3.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|s<=\\\"Silly test string: 2\\\"" "test_simple_7.xml.fs4.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|s>\\\"Silly test string: 2\\\"" "test_simple_7.xml.fs5.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|s>=\\\"Silly test string: 2\\\"" "test_simple_7.xml.fs6.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s|s~=\\\".*2\\\"" "test_simple_7.xml.fs7.trans_base" "")

# Test filtering a sequence that has only one field projected, including filtering on the values
# of a filed not projected.
add_dmr_trans_series_test("test_simple_7.xml" "s{i1}|i1<32768" "test_simple_7.xml.g1.trans_base" "")
add_dmr_trans_series_test("test_simple_7.xml" "s{i1}|s<=\\\"Silly test string: 2\\\"" "test_simple_7.xml.g1.trans_base" "")

# A nested sequence with floats in the outer sequence and the int, string combination in the inner
add_dmr_trans_series_test("test_simple_8.1.xml" "outer" "test_simple_8.1.xml.f1.trans_base" "")
add_dmr_trans_series_test("test_simple_8.1.xml" "outer{x;y}" "test_simple_8.1.xml.f2.trans_base" "")
add_dmr_trans_series_test("test_simple_8.1.xml" "outer{x;y;inner}" "test_simple_8.1.xml.f3.trans_base" "")
add_dmr_trans_series_test("test_simple_8.1.xml" "outer{x;y;inner|i1<1000}" "test_simple_8.1.xml.f4.trans_base" "")
add_dmr_trans_series_test("test_simple_8.1.xml" "outer{x;y;inner|i1<1000}|x<0.0" "test_simple_8.1.xml.f5.trans_base" "")

# These tests are regression tests for bug Hyrax-267. Spaces in variables names
# broke the DAP4 CE parser

# These mostly fail because there's a second bug where the variables in a group are
# not printing values.
add_dmr_trans_series_test("names_with_spaces.dmr" "/u" "names_with_spaces.dmr.1.trans_base" "")
add_dmr_trans_series_test("names_with_spaces.dmr" "/inst2/u" "names_with_spaces.dmr.2.trans_base" "xfail")

add_dmr_trans_series_test("names_with_spaces.dmr" "/inst2/Point.x" "names_with_spaces.dmr.3.trans_base" "xfail")
add_dmr_trans_series_test("names_with_spaces2.dmr" "/inst2/\"Point Break\".x" "names_with_spaces2.dmr.1.trans_base" "xfail")
add_dmr_trans_series_test("names_with_spaces2.dmr" "/inst2/Point%20Break.x" "names_with_spaces2.dmr.1.trans_base" "xfail")
add_dmr_trans_series_test("names_with_spaces2.dmr" "/inst2/\"Point%20Break\".x" "names_with_spaces2.dmr.1.trans_base" "xfail")

add_dmr_trans_series_test("names_with_spaces3.dmr" "/inst2/\"New Group\"/x" "names_with_spaces3.dmr.1.trans_base" "xfail")
add_dmr_trans_series_test("names_with_spaces3.dmr" "/inst2/New%20Group/x" "names_with_spaces3.dmr.1.trans_base" "xfail")
