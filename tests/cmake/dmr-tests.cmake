
## DMR tests. Unlike the DAS and DDS test, these test enumerate the individual test
## one by one since various tests will use different functions. 7/10/25 jhrg

function(dmr_parse_ce_test test_number test_input ce test_baseline)
	set(testname "dmr_parse_ce_test_${test_number}")

	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${test_baseline}")
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
			"\"$<TARGET_FILE:dmr-test>\" -x -p \"${input}\" -c \"${ce}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)
	# This makes it so we can run just these tests and also makes it easy to run the
	# unit tests _before_ the integration tests with a 'check' target. See the top-leve
	# CMakeLists file. 7/8/25 jhrg
	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr;dmr-parse")
endfunction()

# DMR parse+CE integration tests, using dmr_parse_ce_test(test_number, test_input, ce, test_baseline)

dmr_parse_ce_test( 1  test_simple_1.xml            ""  test_simple_1.xml.baseline)
dmr_parse_ce_test( 2  test_simple_2.xml            ""  test_simple_2.xml.baseline)
dmr_parse_ce_test( 3  test_simple_3.xml            ""  test_simple_3.xml.baseline)

dmr_parse_ce_test( 4  test_simple_3_error_1.xml    ""  test_simple_3_error_1.xml.baseline)
dmr_parse_ce_test( 5  test_simple_3_error_2.xml    ""  test_simple_3_error_2.xml.baseline)
dmr_parse_ce_test( 6  test_simple_3_error_3.xml    ""  test_simple_3_error_3.xml.baseline)

dmr_parse_ce_test( 7  test_simple_4.xml            ""  test_simple_4.xml.baseline)
dmr_parse_ce_test( 8  test_simple_5.xml            ""  test_simple_5.xml.baseline)
dmr_parse_ce_test( 9  test_simple_6.xml            ""  test_simple_6.xml.baseline)
dmr_parse_ce_test(10  test_simple_7.xml            ""  test_simple_7.xml.baseline)
dmr_parse_ce_test(11  test_simple_8.xml            ""  test_simple_8.xml.baseline)
dmr_parse_ce_test(12  test_simple_9.xml            ""  test_simple_9.xml.baseline)
dmr_parse_ce_test(13  test_simple_9.1.xml          ""  test_simple_9.1.xml.baseline)
dmr_parse_ce_test(14  test_simple_10.xml           ""  test_simple_10.xml.baseline)
dmr_parse_ce_test(15  test_enum_grp.xml            ""  test_enum_grp.xml.baseline)

dmr_parse_ce_test(16  test_array_1.xml             ""  test_array_1.xml.baseline)
dmr_parse_ce_test(17  test_array_2.xml             ""  test_array_2.xml.baseline)
dmr_parse_ce_test(18  test_array_3.xml             ""  test_array_3.xml.baseline)
dmr_parse_ce_test(19  test_array_4.xml             ""  test_array_4.xml.baseline)
dmr_parse_ce_test(20  test_array_5.xml             ""  test_array_5.xml.baseline)
dmr_parse_ce_test(21  test_array_6.xml             ""  test_array_6.xml.baseline)
dmr_parse_ce_test(22  test_array_7.xml             ""  test_array_7.xml.baseline)
dmr_parse_ce_test(23  test_array_8.xml             ""  test_array_8.xml.baseline)
dmr_parse_ce_test(24  test_array_10.xml            ""  test_array_10.xml.baseline)
dmr_parse_ce_test(25  test_array_11.xml            ""  test_array_11.xml.baseline)

dmr_parse_ce_test(26  ignore_foreign_xml_1.xml     ""  ignore_foreign_xml_1.xml.baseline)
dmr_parse_ce_test(27  ignore_foreign_xml_2.xml     ""  ignore_foreign_xml_2.xml.baseline)
dmr_parse_ce_test(28  ignore_foreign_xml_3.xml     ""  ignore_foreign_xml_3.xml.baseline)

dmr_parse_ce_test(29  test_array_9.xml             ""  test_array_9.xml.baseline)
dmr_parse_ce_test(30  test_array_12.xml            ""  test_array_12.xml.baseline)
dmr_parse_ce_test(31  test_array_13.xml            ""  test_array_13.xml.baseline)
dmr_parse_ce_test(32  test_array_14.xml            ""  test_array_14.xml.baseline)

# Test empty Structures. jhrg 1/29/16
dmr_parse_ce_test(33  test_simple_6.2.xml          ""  test_simple_6.2.xml.baseline)
dmr_parse_ce_test(34  test_simple_6.3.xml          ""  test_simple_6.3.xml.baseline)

# Test DAP CE parse errors - ensure they don't leak the supplied
# CE text into the error message. jhrg 4/15/20
dmr_parse_ce_test(35 test_simple_1.xml nasty test_simple_1.xml.parse_ce_1)
# This string is 'd1rox<script>alert(1)</script>d55je=1' (%25 --> '%')
# That is, the % is escaped in this text: %253c --> %3c --> '<'
dmr_parse_ce_test(36 test_simple_1.xml d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1 test_simple_1.xml.parse_ce_2)

dmr_parse_ce_test(37 test_simple_6.3.xml s.nasty test_simple_6.3.xml.parse_ce_1)
dmr_parse_ce_test(38 test_simple_6.3.xml s.d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1 test_simple_6.3.xml.parse_ce_2)

dmr_parse_ce_test(39 vol_1_ce_12.xml temp[nasty] vol_1_ce_12.xml.parse_ce_1)
dmr_parse_ce_test(40 vol_1_ce_12.xml temp[d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1] vol_1_ce_12.xml.parse_ce_2)

# Test reversed array indices
dmr_parse_ce_test(41 test_array_4.xml b[2:1][2:3] test_array_4.xml.error.base)

if(CMAKE_C_BYTE_ORDER STREQUAL "LITTLE_ENDIAN")
	set(word_order "little-endian")
elseif(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
	set(word_order "big-endian")
else()
	message(WARNING "Unknown byte order for C compiler")
endif()

function(dmr_intern_test number input baseline)
	set(testname "dmr_intern_test_${number}")

	# Paths
	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${word_order}/${baseline}")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	# Add the CTest entry. 7/8/25 jhrg
	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dmr-test>\" -x -i \"${input}\" > \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr;dmr-intern")
endfunction()

dmr_intern_test( 1  test_simple_1.xml       test_simple_1.xml.intern_base)
dmr_intern_test( 2  test_simple_2.xml       test_simple_2.xml.intern_base)
dmr_intern_test( 3  test_simple_3.xml       test_simple_3.xml.intern_base)
dmr_intern_test( 4  test_simple_4.xml       test_simple_4.xml.intern_base)
dmr_intern_test( 5  test_simple_5.xml       test_simple_5.xml.intern_base)
dmr_intern_test( 6  test_simple_6.xml       test_simple_6.xml.intern_base)
dmr_intern_test( 7  test_simple_7.xml       test_simple_7.xml.intern_base)
dmr_intern_test( 8  test_simple_8.xml       test_simple_8.xml.intern_base)
dmr_intern_test( 9  test_simple_9.xml       test_simple_9.xml.intern_base)
dmr_intern_test(10  test_simple_9.1.xml     test_simple_9.1.xml.intern_base)
dmr_intern_test(11  test_simple_10.xml      test_simple_10.xml.intern_base)
dmr_intern_test(12  test_enum_grp.xml       test_enum_grp.xml.intern_base)

dmr_intern_test(13  test_array_1.xml        test_array_1.xml.intern_base)
dmr_intern_test(14  test_array_2.xml        test_array_2.xml.intern_base)
dmr_intern_test(15  test_array_3.xml        test_array_3.xml.intern_base)
dmr_intern_test(16  test_array_4.xml        test_array_4.xml.intern_base)
dmr_intern_test(17  test_array_5.xml        test_array_5.xml.intern_base)
dmr_intern_test(18  test_array_6.xml        test_array_6.xml.intern_base)
dmr_intern_test(19  test_array_7.xml        test_array_7.xml.intern_base)
dmr_intern_test(20  test_array_8.xml        test_array_8.xml.intern_base)
dmr_intern_test(21  test_array_10.xml       test_array_10.xml.intern_base)
dmr_intern_test(22  test_array_11.xml       test_array_11.xml.intern_base)

dmr_intern_test(23  test_array_9.xml        test_array_9.xml.intern_base)
dmr_intern_test(24  test_array_12.xml       test_array_12.xml.intern_base)
dmr_intern_test(25  test_array_13.xml       test_array_13.xml.intern_base)
dmr_intern_test(26  test_array_14.xml       test_array_14.xml.intern_base)

dmr_intern_test(27  test_simple_6.2.xml     test_simple_6.2.xml.intern_base)
dmr_intern_test(28  test_simple_6.3.xml     test_simple_6.3.xml.intern_base)

## For byte_order == universal, this test removed the <Value> information
## and replaces it with nothing (which is different from the trans + ce
## tests which replace the <Value> info with 'removed checksum'). 7/12/25 jhrg
## NB: Many tests use "" for either ce or func or both. 7/18/25 jhrg
function(dmr_trans_test number input ce func baseline byte_order)
	set(testname "dmr_trans_test_${number}")

	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${byte_order}/${baseline}")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	# Pure hackery. Fix the baselines someday. If this is called with a
	# CE or a function, then the universal tests use 'removed checksum" but
	# the simpler tests just used the null string. jhrg 7/18/25
	set(checksum_replacement "")
	if(NOT ce STREQUAL "" OR NOT func STREQUAL "")
		set(checksum_replacement "removed checksum")
	endif()

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dmr-test>\" -Cxt \"${input}\" -c \"${ce}\" -f \"${func}\" > \"${output}\" 2>&1; \
			if test \"${byte_order}\" = \"universal\"; then \
				sed 's@<Value>[0-9a-f][0-9a-f]*</Value>@${checksum_replacement}@' \"${output}\" > \"${output}_univ\"; \
				mv \"${output}_univ\" \"${output}\"; \
			fi; \
			diff -b -B \"${baseline}\" \"${output}\";"
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

# DMR translation tests → dmr_trans_test(number, input, ce, func, baseline, byte_order)

dmr_trans_test( 1  test_simple_1.xml   "" ""  test_simple_1.xml.trans_base   ${word_order})
dmr_trans_test( 2  test_simple_2.xml   "" ""  test_simple_2.xml.trans_base   ${word_order})
dmr_trans_test( 3  test_simple_3.xml   "" ""  test_simple_3.xml.trans_base   ${word_order})
dmr_trans_test( 4  test_simple_4.xml   "" ""  test_simple_4.xml.trans_base   ${word_order})
dmr_trans_test( 5  test_simple_5.xml   "" ""  test_simple_5.xml.trans_base   ${word_order})
dmr_trans_test( 6  test_simple_6.xml   "" ""  test_simple_6.xml.trans_base   ${word_order})
dmr_trans_test( 7  test_simple_7.xml   "" ""  test_simple_7.xml.trans_base   ${word_order})
dmr_trans_test( 8  test_simple_8.xml   "" ""  test_simple_8.xml.trans_base   ${word_order})
dmr_trans_test( 9  test_simple_9.xml   "" ""  test_simple_9.xml.trans_base   ${word_order})
dmr_trans_test(10  test_simple_9.1.xml "" ""  test_simple_9.1.xml.trans_base ${word_order})
dmr_trans_test(11  test_simple_10.xml  "" ""  test_simple_10.xml.trans_base  ${word_order})
dmr_trans_test(12  test_enum_grp.xml   "" ""  test_enum_grp.xml.trans_base   ${word_order})

dmr_trans_test(13  test_array_1.xml    "" ""  test_array_1.xml.trans_base    ${word_order})
dmr_trans_test(14  test_array_2.xml    "" ""  test_array_2.xml.trans_base    ${word_order})
dmr_trans_test(15  test_array_3.xml    "" ""  test_array_3.xml.trans_base    ${word_order})
dmr_trans_test(16  test_array_4.xml    "" ""  test_array_4.xml.trans_base    ${word_order})
dmr_trans_test(17  test_array_5.xml    "" ""  test_array_5.xml.trans_base    ${word_order})
dmr_trans_test(18  test_array_6.xml    "" ""  test_array_6.xml.trans_base    ${word_order})
dmr_trans_test(19  test_array_7.xml    "" ""  test_array_7.xml.trans_base    ${word_order})
dmr_trans_test(20  test_array_8.xml    "" ""  test_array_8.xml.trans_base    ${word_order})
dmr_trans_test(21  test_array_10.xml   "" ""  test_array_10.xml.trans_base   ${word_order})
dmr_trans_test(22  test_array_11.xml   "" ""  test_array_11.xml.trans_base   ${word_order})

dmr_trans_test(23  test_array_9.xml    "" ""  test_array_9.xml.trans_base    ${word_order})
dmr_trans_test(24  test_array_12.xml   "" ""  test_array_12.xml.trans_base   ${word_order})
dmr_trans_test(25  test_array_13.xml   "" ""  test_array_13.xml.trans_base   ${word_order})
dmr_trans_test(26  test_array_14.xml   "" ""  test_array_14.xml.trans_base   ${word_order})

dmr_trans_test(27  test_simple_6.2.xml "" ""  test_simple_6.2.xml.trans_base ${word_order})
dmr_trans_test(28  test_simple_6.3.xml "" ""  test_simple_6.3.xml.trans_base ${word_order})

# “Universal” runs
dmr_trans_test(29  test_array_9.xml    "" ""  test_array_9.xml.trans_base    "universal" )
dmr_trans_test(30  test_array_12.xml   "" ""  test_array_12.xml.trans_base   "universal" )
dmr_trans_test(31  test_array_13.xml   "" ""  test_array_13.xml.trans_base   "universal" )
dmr_trans_test(32  test_array_14.xml   "" ""  test_array_14.xml.trans_base   "universal" )
dmr_trans_test(33  test_simple_6.2.xml "" ""  test_simple_6.2.xml.trans_base "universal" )
dmr_trans_test(34  test_simple_6.3.xml "" ""  test_simple_6.3.xml.trans_base "universal" )

# Wrapping strings in "" is often not needed in cmake.
#
# I removed them for some of the CEs, but it seems somewhat pointless. Still,
# it's good to know. Any function argument with a ';' needs to be double-quoted.
# jhrg 7/19/25

if(${word_order} STREQUAL "little-endian")
	dmr_trans_test(35  test_array_3.1.dmr      "row;x"               ""  test_array_3.1.dmr.1.trans_base  ${word_order})
	dmr_trans_test(36  test_array_3.1.dmr      "row=[2:3];x"         ""  test_array_3.1.dmr.2.trans_base  ${word_order})
	dmr_trans_test(37  test_array_3.1.dmr      "row=[2:3];x[0:1]"    ""  test_array_3.1.dmr.3.trans_base  ${word_order})
	dmr_trans_test(38  test_array_3.1.dmr      x[0:1]              ""  test_array_3.1.dmr.4.trans_base  ${word_order})
	dmr_trans_test(39  test_array_3.1.dmr      x                   ""  test_array_3.1.dmr.5.trans_base  ${word_order})
endif()

# Test various facets of the CE parser and evaluation engine
dmr_trans_test(40  test_array_4.xml         "a"                   ""  test_array_4.xml.1.trans_base    ${word_order})
dmr_trans_test(41  test_array_4.xml         "a[][] "              ""  test_array_4.xml.1.trans_base    ${word_order})
dmr_trans_test(42  test_array_4.xml         "/row=[0:1];/col=[3];a" "" test_array_4.xml.3.trans_base  ${word_order})
dmr_trans_test(43  test_array_4.xml         "/row=[0:1];/col=[3];a[][] " "" test_array_4.xml.4.trans_base ${word_order})
dmr_trans_test(44  test_array_4.xml         "/row=[0:1];/col=[3];a[][];b[0][];c[0:][0:] " "" test_array_4.xml.5.trans_base ${word_order})
dmr_trans_test(45  test_array_4.xml         "x[][] "             ""  test_array_4.xml.6.trans_base    ${word_order})
dmr_trans_test(46  test_array_4.xml         "/row=[0:1];x[][] "  ""  test_array_4.xml.7.trans_base    ${word_order})
dmr_trans_test(47  test_array_4.xml         "c[2:][2:] "         ""  test_array_4.xml.8.trans_base    ${word_order})

dmr_trans_test(48  test_simple_6.xml        "s"                   ""  test_simple_6.xml.1.trans_base   ${word_order})
dmr_trans_test(49  test_simple_6.xml        "s.i1"                ""  test_simple_6.xml.2.trans_base   ${word_order})
dmr_trans_test(50  test_simple_6.xml        "s.s"                 ""  test_simple_6.xml.3.trans_base   ${word_order})
dmr_trans_test(51  test_simple_6.1.xml      "s.inner.i2"          ""  test_simple_6.1.xml.1.trans_base ${word_order})

dmr_trans_test(52  test_simple_6.xml        s{i1}               ""  test_simple_6.xml.2.trans_base   ${word_order})
dmr_trans_test(53  test_simple_6.xml        "s{s}"                ""  test_simple_6.xml.3.trans_base   ${word_order})
dmr_trans_test(54  test_simple_6.1.xml      "s{inner.i2}"         ""  test_simple_6.1.xml.1.trans_base ${word_order})
dmr_trans_test(55  test_simple_6.1.xml      "s{inner{i2}}"        ""  test_simple_6.1.xml.1.trans_base ${word_order})

# test_array_6
dmr_trans_test(56  test_array_6.xml         "a"                   ""  test_array_6.xml.1.trans_base    ${word_order})
dmr_trans_test(57  test_array_6.xml         "a[][] "              ""  test_array_6.xml.1.trans_base    ${word_order})
dmr_trans_test(58  test_array_6.xml         "/row=[0:1];a[][] "   ""  test_array_6.xml.2.trans_base    ${word_order})
dmr_trans_test(59  test_array_6.xml         "/row=[0:1];a[][1:2] " ""  test_array_6.xml.3.trans_base   ${word_order})

# test_array_6.2 (Structure with nested CE)
dmr_trans_test(60  test_array_6.2.xml       "a"                   ""  test_array_6.2.xml.1.trans_base  ${word_order})
dmr_trans_test(61  test_array_6.2.xml       "a{i;j}"              ""  test_array_6.2.xml.1.trans_base  ${word_order})
dmr_trans_test(62  test_array_6.2.xml       "a.i"                 ""  test_array_6.2.xml.2.trans_base  ${word_order})
dmr_trans_test(63  test_array_6.2.xml       "a{i}"                ""  test_array_6.2.xml.2.trans_base  ${word_order})
dmr_trans_test(64  test_array_6.2.xml       "a.i[0][1:2] "        ""  test_array_6.2.xml.3.trans_base  ${word_order})
dmr_trans_test(65  test_array_6.2.xml       "a{i[0][1:2]} "       ""  test_array_6.2.xml.3.trans_base  ${word_order})
dmr_trans_test(66  test_array_6.2.xml       "/row=[0:1];a.i[][1:2] " "" test_array_6.2.xml.4.trans_base ${word_order})
dmr_trans_test(67  test_array_6.2.xml       "/row=[0:1];a{i[][1:2]} " "" test_array_6.2.xml.4.trans_base ${word_order})
dmr_trans_test(68  test_array_6.2.xml       "a.j"                 ""  test_array_6.2.xml.5.trans_base  ${word_order})

# test_array_6.1 (Sequence-of-Structure CE)
dmr_trans_test(69  test_array_6.1.xml       "a"                   ""  test_array_6.1.xml.1.trans_base  ${word_order})
dmr_trans_test(70  test_array_6.1.xml       "/row=[1:2];a[][0] "  ""  test_array_6.1.xml.2.trans_base  ${word_order})
dmr_trans_test(71  test_array_6.1.xml       "/row=[1:2];a[][0]{i;j} " "" test_array_6.1.xml.2.trans_base ${word_order})
dmr_trans_test(72  test_array_6.1.xml       "row=[1:2];a[][0]{i;j} " "" test_array_6.1.xml.2.trans_base ${word_order})

# sequences/arrays
dmr_trans_test(73  test_simple_7.xml       "s"                   ""  test_simple_7.xml.1.trans_base  ${word_order})
dmr_trans_test(74  test_simple_7.xml       "s{i1;s}"             ""  test_simple_7.xml.1.trans_base  ${word_order})
dmr_trans_test(75  test_simple_7.xml       "s.i1"                ""  test_simple_7.xml.2.trans_base  ${word_order})
dmr_trans_test(76  test_simple_7.xml       "s{i1}"               ""  test_simple_7.xml.2.trans_base  ${word_order})

# universal (elided CRC)
dmr_trans_test(77  test_simple_8.xml       "outer"               ""  test_simple_8.xml.1.trans_base  "universal")
dmr_trans_test(78  test_simple_8.xml       "outer.s.s"           ""  test_simple_8.xml.2.trans_base  "universal")
dmr_trans_test(79  test_simple_8.xml       "outer{s{s}}"         ""  test_simple_8.xml.2.trans_base  "universal")

# back to little-endian patterns
dmr_trans_test(80  test_array_7.xml        "s"                   ""  test_array_7.xml.1.trans_base  ${word_order})
dmr_trans_test(81  test_array_7.xml        "s{i1;s}"             ""  test_array_7.xml.1.trans_base  ${word_order})
dmr_trans_test(82  test_array_7.xml        "s.i1"                ""  test_array_7.xml.2.trans_base  ${word_order})
dmr_trans_test(83  test_array_7.xml        "s{i1}"               ""  test_array_7.xml.2.trans_base  ${word_order})
dmr_trans_test(84  test_array_7.xml        "s[1] "               ""  test_array_7.xml.3.trans_base  ${word_order})
dmr_trans_test(85  test_array_7.xml        "s[1]{i1;s}"          ""  test_array_7.xml.3.trans_base  ${word_order})
dmr_trans_test(86  test_array_7.xml        "s[1]{i1}"            ""  test_array_7.xml.4.trans_base  ${word_order})
dmr_trans_test(87  test_array_8.xml        "/col=[1:2];s[1][]{i1}" "" test_array_8.xml.1.trans_base  ${word_order})
dmr_trans_test(88  test_array_8.xml        "col=[1:2];s[1][]{i1}" "" test_array_8.xml.1.trans_base  ${word_order})

# sequence‐array of structures CE
dmr_trans_test(89  test_array_7.1.xml      ""                    ""  test_array_7.1.xml.1.trans_base ${word_order})
dmr_trans_test(90  test_array_7.1.xml      "s"                   ""  test_array_7.1.xml.1.trans_base ${word_order})
dmr_trans_test(91  test_array_7.1.xml      "s.i1"                ""  test_array_7.1.xml.2.trans_base ${word_order})
dmr_trans_test(92  test_array_7.1.xml      "s.i1[][] "          ""  test_array_7.1.xml.2.trans_base ${word_order})
dmr_trans_test(93  test_array_7.1.xml      "s{i1}"              ""  test_array_7.1.xml.2.trans_base ${word_order})
dmr_trans_test(94  test_array_7.1.xml      "s{i1[][]} "        ""  test_array_7.1.xml.2.trans_base ${word_order})
dmr_trans_test(95  test_array_7.1.xml      "s.i1[0][0] "       ""  test_array_7.1.xml.3.trans_base ${word_order})
dmr_trans_test(96  test_array_7.1.xml      "s{i1[0][0]} "      ""  test_array_7.1.xml.3.trans_base ${word_order})
dmr_trans_test(97  test_array_7.1.xml      "s.i1[0:2][1:2] "   ""  test_array_7.1.xml.4.trans_base ${word_order})
dmr_trans_test(98  test_array_7.1.xml      "/row=[2:3];/col=[2:3];s " "" test_array_7.1.xml.5.trans_base ${word_order})
dmr_trans_test(99  test_array_7.1.xml      "/row=[2:3];/col=[2:3];s.i1 " "" test_array_7.1.xml.6.trans_base ${word_order})
dmr_trans_test(100 test_array_7.1.xml      "/row=[2:3];/col=[2:3];s.i1[][] " "" test_array_7.1.xml.6.trans_base ${word_order})
dmr_trans_test(101 test_array_7.1.xml      "/row=[2:3];/col=[2:3];s{i1} " "" test_array_7.1.xml.6.trans_base ${word_order})
dmr_trans_test(102 test_array_7.2.xml      "/col=[1:2];s[]{i1}" "" test_array_7.2.xml.1.trans_base ${word_order})
dmr_trans_test(103 test_array_7.2.xml      "/col=[1:2];s[]{i1[][]}" "" test_array_7.2.xml.1.trans_base ${word_order})
dmr_trans_test(104 test_array_7.2.xml      "/col=[1:2];s{i1[][]}" "" test_array_7.2.xml.1.trans_base ${word_order})
dmr_trans_test(105 test_array_7.2.xml      "/col=[1:2];s[]{i1[0][]}" "" test_array_7.2.xml.2.trans_base ${word_order})
dmr_trans_test(106 test_array_7.2.xml      "/col=[1:2];s{i1[0][]}" "" test_array_7.2.xml.2.trans_base ${word_order})
dmr_trans_test(107 test_array_7.2.xml      "/col=[1:2];s[0]{i1}" "" test_array_7.2.xml.3.trans_base ${word_order})
dmr_trans_test(108 test_array_7.2.xml      "/col=[1:2];s[0]{i1[][]}" "" test_array_7.2.xml.3.trans_base ${word_order})
dmr_trans_test(109 test_array_7.2.xml      "/col=[1:2];s[0]{i1[0][]}" "" test_array_7.2.xml.4.trans_base ${word_order})

# Test projections
dmr_trans_test(110 vol_1_ce_2.xml         "/inst2"             ""  vol_1_ce_2.xml.1.trans_base      ${word_order})
dmr_trans_test(111 vol_1_ce_2.xml         "inst2"              ""  vol_1_ce_2.xml.1.trans_base      ${word_order})
dmr_trans_test(112 vol_1_ce_2.xml         "/inst2/Point"       ""  vol_1_ce_2.xml.2.trans_base      ${word_order})

dmr_trans_test(113 vol_1_ce_13.xml        "/inst2"             ""  vol_1_ce_13.xml.1.trans_base     ${word_order})
dmr_trans_test(114 vol_1_ce_13.xml        "/inst2/inst3"       ""  vol_1_ce_13.xml.2.trans_base     ${word_order})

dmr_trans_test(115 vol_1_ce_13.xml        "/attr_only_global"  ""  vol_1_ce_13.xml.3.trans_base     ${word_order})
dmr_trans_test(116 vol_1_ce_13.xml        "/inst2/attr_only"   ""  vol_1_ce_13.xml.4.trans_base     ${word_order})

# DMR function‐CE tests → use dmr_trans_test(number, input, ce, func, baseline, byte_order)
dmr_trans_test(117 test_array_1.xml "" "scale(x,1)"                             test_array_1.xml.1.func_base      ${word_order})
dmr_trans_test(118 test_array_1.xml "" "scale(x,10)"                            test_array_1.xml.2.func_base      ${word_order})
dmr_trans_test(119 test_array_1.xml "" "scale(x,-10)"                           test_array_1.xml.3.func_base      ${word_order})
dmr_trans_test(120 test_array_1.xml "" "scale(x,0.001)"                         test_array_1.xml.4.func_base      ${word_order})
dmr_trans_test(121 test_array_1.xml "" "scale(x,-0.001)"                        test_array_1.xml.5.func_base      ${word_order})

dmr_trans_test(122 test_array_1.xml "" "scale(x,0x7fffffffffffffff)"            test_array_1.xml.6.func_base      ${word_order})
dmr_trans_test(123 test_array_1.xml "" "scale(x,0x8fffffffffffffff)"            test_array_1.xml.7.func_base      ${word_order})

dmr_trans_test(124 test_array_5.xml "" "scale(a,0.001)"                         test_array_5.xml.1.func_base      ${word_order})
dmr_trans_test(125 test_array_5.xml "" "scale(b,0.001)"                         test_array_5.xml.2.func_base      ${word_order})
dmr_trans_test(126 test_array_5.xml "" "scale(c,0.001)"                         test_array_5.xml.3.func_base      ${word_order})
dmr_trans_test(127 test_array_5.xml "" "scale(d,0.001)"                         test_array_5.xml.4.func_base      ${word_order})

dmr_trans_test(128 vol_1_ce_1.xml  "" "scale(u,10)"                             vol_1_ce_1.xml.1.func_base        ${word_order})
dmr_trans_test(129 vol_1_ce_1.xml  "" "scale(u,v)"                              vol_1_ce_1.xml.2.func_base        ${word_order})
dmr_trans_test(130 vol_1_ce_1.xml  "" "scale(scale(u,10),0.01)"                vol_1_ce_1.xml.3.func_base        ${word_order})

dmr_trans_test(131 vol_1_ce_1.xml  "" "scale(Point.x,10)"                      vol_1_ce_1.xml.4.func_base        ${word_order})
dmr_trans_test(132 vol_1_ce_1.xml  "" "scale(Point.x,Point.y)"                 vol_1_ce_1.xml.5.func_base        ${word_order})
dmr_trans_test(133 vol_1_ce_1.xml  "" "scale(scale(Point.x,10),0.01)"          vol_1_ce_1.xml.6.func_base        ${word_order})

dmr_trans_test(134 vol_1_ce_1.xml  "" "scale(\\$Byte(20:1,2,3,4),10)"           vol_1_ce_1.xml.7.func_base        ${word_order})
dmr_trans_test(135 vol_1_ce_1.xml  "" "scale(\\$Int8(20:10,11,12,-9),10)"       vol_1_ce_1.xml.8.func_base        ${word_order})
dmr_trans_test(136 vol_1_ce_1.xml  "" "scale(\\$UInt16(20:1,2,3,4),10)"         vol_1_ce_1.xml.9.func_base        ${word_order})
dmr_trans_test(137 vol_1_ce_1.xml  "" "scale(\\$Int16(20:1,2,3,-4),10)"         vol_1_ce_1.xml.10.func_base       ${word_order})
dmr_trans_test(138 vol_1_ce_1.xml  "" "scale(\\$UInt32(20:1,2,3,4),10)"         vol_1_ce_1.xml.11.func_base       ${word_order})
dmr_trans_test(139 vol_1_ce_1.xml  "" "scale(\\$Int32(20:1,2,3,-4),10)"         vol_1_ce_1.xml.12.func_base       ${word_order})
dmr_trans_test(140 vol_1_ce_1.xml  "" "scale(\\$UInt64(20:1,2,3,0xffffffffffffffff),1)" vol_1_ce_1.xml.13.func_base ${word_order})
dmr_trans_test(141 vol_1_ce_1.xml  "" "scale(\\$Int64(20:1,2,3,0x7fffffffffffffff),1)"   vol_1_ce_1.xml.14.func_base ${word_order})
dmr_trans_test(142 vol_1_ce_1.xml  "" "scale(\\$Float32(20:1,2,3,4.55),10)"      vol_1_ce_1.xml.15.func_base       ${word_order})
dmr_trans_test(143 vol_1_ce_1.xml  "" "scale(\\$Float64(20:1,2,3,4.55),10)"      vol_1_ce_1.xml.16.func_base       ${word_order})

dmr_trans_test(144 vol_1_ce_10.xml "" "scale(lat,10)"                           vol_1_ce_10.xml.1.func_base       ${word_order})
dmr_trans_test(145 vol_1_ce_10.xml "" "scale(lat,10);scale(lon,10)"            vol_1_ce_10.xml.2.func_base       ${word_order})
dmr_trans_test(146 vol_1_ce_10.xml "lat[10:11][10:11];lon[10:11][10:11]" "scale(lat,10);scale(lon,10)" vol_1_ce_10.xml.3.func_base ${word_order})

## Two things different about this set of tests: they assume baselines that are independent
## of word order and they use CEs that have operators (!, <=, ...). Making a test name substituting
## those chars with '_' doesn't make unique test names. But, for this we can use the baseline
## names. Also, some of these tests are expected to fail. 7/14/25 jhrg
function(dmr_series_test number input ce baseline xfail)
	set(testname "dmr_series_test_${number}")

	set(input      "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/${input}")
	set(baseline   "${CMAKE_CURRENT_SOURCE_DIR}/dmr-testsuite/universal/${baseline}")
	set(output     "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:dmr-test>\" -C -x -e -t \"${input}\" -c \"${ce}\" > \"${output}\" 2>&1; \
			sed 's@<Value>[0-9a-f][0-9a-f]*</Value>@@' \"${output}\" > \"${output}_univ\"; \
			mv \"${output}_univ\" \"${output}\"; \
			diff -b -B \"${baseline}\" \"${output}\""
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;dmr;dmr-series")
	set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
	if("${xfail}" STREQUAL "xfail")
		set_tests_properties(${testname} PROPERTIES WILL_FAIL TRUE)
	endif()
endfunction()

## These tests are all 'universal' tests (i.e., they do not need different baselines
## for different word order machines). 7/14/25 jhrg.
dmr_series_test(147 test_simple_7.xml "s" test_simple_7.xml.f.trans_base "pass")

dmr_series_test(148 test_simple_7.xml "s|i1==1024" test_simple_7.xml.f1.trans_base "pass")
dmr_series_test(149 test_simple_7.xml "s|i1!=1024" test_simple_7.xml.f2.trans_base "pass")
dmr_series_test(150 test_simple_7.xml "s|i1<=1024" test_simple_7.xml.f3.trans_base "pass")
dmr_series_test(151 test_simple_7.xml "s|i1<1024" test_simple_7.xml.f4.trans_base "pass")
dmr_series_test(152 test_simple_7.xml "s|i1<=1024" test_simple_7.xml.f5.trans_base "pass")
dmr_series_test(153 test_simple_7.xml "s|i1>1024" test_simple_7.xml.f6.trans_base "pass")
dmr_series_test(154 test_simple_7.xml "s|i1>=1024" test_simple_7.xml.f7.trans_base "pass")

dmr_series_test(155 test_simple_7.xml "s|1024<i1" test_simple_7.xml.f8.trans_base "pass")
dmr_series_test(156 test_simple_7.xml "s|1024<=i1<=32768" test_simple_7.xml.f9.trans_base "pass")
dmr_series_test(157 test_simple_7.xml "s|i1>=1024.0" test_simple_7.xml.fa.trans_base "pass")

## \\\" --> \\ is a literal slash and \" is a literal double quote. 7/14/25 jhrg
dmr_series_test(158 test_simple_7.xml "s|s==\\\"Silly test string: 2\\\"" test_simple_7.xml.fs1.trans_base "pass")
dmr_series_test(159 test_simple_7.xml "s|s!=\\\"Silly test string: 2\\\"" test_simple_7.xml.fs2.trans_base "pass")
dmr_series_test(160 test_simple_7.xml "s|s<\\\"Silly test string: 2\\\"" test_simple_7.xml.fs3.trans_base "pass")
dmr_series_test(161 test_simple_7.xml "s|s<=\\\"Silly test string: 2\\\"" test_simple_7.xml.fs4.trans_base "pass")
dmr_series_test(162 test_simple_7.xml "s|s>\\\"Silly test string: 2\\\"" test_simple_7.xml.fs5.trans_base "pass")
dmr_series_test(163 test_simple_7.xml "s|s>=\\\"Silly test string: 2\\\"" test_simple_7.xml.fs6.trans_base "pass")
dmr_series_test(164 test_simple_7.xml "s|s~=\\\".*2\\\"" test_simple_7.xml.fs7.trans_base "pass")

# Test filtering a sequence that has only one field projected, including filtering on the values
# of a filed not projected.
dmr_series_test(165 test_simple_7.xml "s{i1}|i1<32768" test_simple_7.xml.g1.trans_base "pass")
dmr_series_test(166 test_simple_7.xml "s{i1}|s<=\\\"Silly test string: 2\\\"" test_simple_7.xml.g1.trans_base "pass")

# A nested sequence with floats in the outer sequence and the int, string combination in the inner
dmr_series_test(167 test_simple_8.1.xml "outer" test_simple_8.1.xml.f1.trans_base "pass")
dmr_series_test(168 test_simple_8.1.xml "outer{x;y}" test_simple_8.1.xml.f2.trans_base "pass")
dmr_series_test(169 test_simple_8.1.xml "outer{x;y;inner}" test_simple_8.1.xml.f3.trans_base "pass")
dmr_series_test(170 test_simple_8.1.xml "outer{x;y;inner|i1<1000}" test_simple_8.1.xml.f4.trans_base "pass")
dmr_series_test(171 test_simple_8.1.xml "outer{x;y;inner|i1<1000}|x<0.0" test_simple_8.1.xml.f5.trans_base "pass")

# These tests are regression tests for bug Hyrax-267. Spaces in variables names
# broke the DAP4 CE parser

# These mostly fail because there's a second bug where the variables in a group are
# not printing values.
dmr_series_test(172 names_with_spaces.dmr "/u" names_with_spaces.dmr.1.trans_base "pass")
dmr_series_test(173 names_with_spaces.dmr "/inst2/u" names_with_spaces.dmr.2.trans_base "xfail")

dmr_series_test(174 names_with_spaces.dmr "/inst2/Point.x" names_with_spaces.dmr.3.trans_base "xfail")
dmr_series_test(175 names_with_spaces2.dmr "/inst2/\"Point Break\".x" names_with_spaces2.dmr.1.trans_base "xfail")
dmr_series_test(176 names_with_spaces2.dmr "/inst2/Point%20Break.x" names_with_spaces2.dmr.1.trans_base "xfail")
dmr_series_test(177 names_with_spaces2.dmr "/inst2/\"Point%20Break\".x" names_with_spaces2.dmr.1.trans_base "xfail")

dmr_series_test(178 names_with_spaces3.dmr "/inst2/\"New Group\"/x" names_with_spaces3.dmr.1.trans_base "xfail")
dmr_series_test(179 names_with_spaces3.dmr "/inst2/New%20Group/x" names_with_spaces3.dmr.1.trans_base "xfail")
