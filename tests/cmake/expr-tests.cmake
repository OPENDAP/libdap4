
function(expr_test test_num option input ce baseline xfail)
	set(testname "expr_test_${test_num}")
	set(input "${CMAKE_CURRENT_SOURCE_DIR}/expr-testsuite/${input}")
	set(baseline "${CMAKE_CURRENT_SOURCE_DIR}/expr-testsuite/${baseline}")
	set(output "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:expr-test>\" \"${option}\" \"${input}\" -k \"${ce}\" -f \"dummy\"> \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;expr")
	if(${option} STREQUAL "-w" OR ${option} STREQUAL "-bw")
		# until we fix HYRAX-1843 the whole-enchilada tests must be run serially. 7/17/25 jhrg
		set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
	endif()
	if("${xfail}" STREQUAL "xfail")
		set_tests_properties(${testname} PROPERTIES WILL_FAIL TRUE)
	endif()
endfunction()

## The xfail tests below should pass but have empty baseline files due to a bug
## in the tests from 2013 (?). Make new baselines! 7/15/25 jhrg

expr_test(1 "-w" "test.1" "i" "test.1.base" "pass")
expr_test(2 "-W" "test.1" "i" "test.1.base" "pass")
expr_test(3 "-w" "test.1" "i,j" "test.1a.base" "pass")
expr_test(4 "-W" "test.1" "i,j" "test.1a.base" "pass")
expr_test(5 "-w" "test.1" "i,j&i=j" "test.1b.base" "pass")
expr_test(6 "-W" "test.1" "i,j&i=j" "test.1b.base" "pass")
expr_test(7 "-w" "test.1" "i&i=j" "test.1d.base" "pass")
expr_test(8 "-W" "test.1" "i&i=j" "test.1d.base" "pass")
expr_test(9 "-w" "test.2" "s1" "test.2.base" "pass")
expr_test(10 "-W" "test.2" "s1" "test.2.base" "pass")
expr_test(11 "-w" "test.2" "s2" "test.2a.base" "pass")
expr_test(12 "-W" "test.2" "s2" "test.2a.base" "pass")
expr_test(13 "-w" "test.2" "s2,s3" "test.2b.base" "pass")
expr_test(14 "-W" "test.2" "s2,s3" "test.2b.base" "pass")
expr_test(15 "-w" "test.2" "s2[2:2:4],s3.o" "test.2c.base" "pass")
expr_test(16 "-W" "test.2" "s2[2:2:4],s3.o" "test.2c.base" "pass")

expr_test(17 "-w" "test.2" "s2[2:2:4].m" "test.2d.base" "pass")
expr_test(18 "-W" "test.2" "s2[2:2:4].m" "test.2d.base" "pass")
expr_test(19 "-w" "test.2" "s2[2:2:4].m,s2[2:2:4].l" "test.2e.base" "pass")
expr_test(20 "-W" "test.2" "s2[2:2:4].m,s2[2:2:4].l" "test.2e.base" "pass")

expr_test(21 "-w" "test.2a" "s2[2:4].m[0:4],s2[2:4].l[0:5]" "test.2f.base" "pass")
expr_test(22 "-W" "test.2a" "s2[2:4].m[0:4],s2[2:4].l[0:5]" "test.2f.base" "pass")
expr_test(23 "-w" "test.3" "i[1:10]" "test.3.base" "pass")
expr_test(24 "-W" "test.3" "i[1:10]" "test.3.base" "pass")

# Test the new code to limit reversed index values. 10:10 should pass. jhrg 2/3/11
expr_test(25 "-w" "test.3" "i[10:10]" "test.3a.base" "pass")
expr_test(26 "-W" "test.3" "i[10:10]" "test.3a.base" "pass")

# NB: \\ -> \ and \" -> " so \\\" -> \" which then becomes " inside the quoted string. 7/16/25 jhrg
expr_test(27 "-w" "test.4" "s&s=~\\\"^Silly.*\\\"" "test.4.base" "pass")
expr_test(28 "-W" "test.4" "s&s=~\\\"^Silly.*\\\"" "test.4.base" "pass")

# In changing the TestStr class so that it writes a constant value (Silly ...: 1)
# I had to hack this test to use the -b option to expr-test. EXPR_RESPONSE_P uses
# only the -w/-W options, so I dropped using that macro kluge and went right for the
# actual test macro. 12/2/13 jhrg
#

expr_test(29 "-bw" "test.e" "names.s&names.s=~\\\".*:.3\\\"" "test.ea.base" "pass")
expr_test(30 "-bW" "test.e" "names.s&names.s=~\\\".*:.3\\\"" "test.ea.base" "pass")

# since 'Silly ...:5' will never be produced, this tests what happens when the sequence
# does not contain the value. Note that this test does not have to be rewritten because
# of the change to the TestStr code. 12/2/13 jhrg
expr_test(31 "-w" "test.e" "names.s&names.s=~\\\".*: 5\\\"" "test.eb.base" "pass")
expr_test(32 "-W" "test.e" "names.s&names.s=~\\\".*: 5\\\"" "test.eb.base" "pass")

expr_test(33 "-w" "test.5" "g[0:2:4][0][0]" "test.5.base" "pass")
expr_test(34 "-W" "test.5" "g[0:2:4][0][0]" "test.5.base" "pass")
expr_test(35 "-w" "test.5" "g[0:2:4][0:2:4][0:2:4]" "test.5a.base" "pass")
expr_test(36 "-W" "test.5" "g[0:2:4][0:2:4][0:2:4]" "test.5a.base" "pass")
expr_test(37 "-w" "test.6" "i" "test.6.base" "pass")
expr_test(38 "-W" "test.6" "i" "test.6.base" "pass")
expr_test(39 "-w" "test.6" "i[1:2][2:4]" "test.6a.base" "pass")
expr_test(40 "-W" "test.6" "i[1:2][2:4]" "test.6a.base" "pass")
# Added test of '*' DAP2 syntax. jhrg 2/4/22
expr_test(41 "-w" "test.6" "i[1:2][*]" "test.6b.base" "pass")
expr_test(42 "-W" "test.6" "i[1:2][*]" "test.6b.base" "pass")

expr_test(43 "-w" "test.5" "g.val[0:1][0:1][0:1]" "test.5b.base" "pass")
expr_test(44 "-W" "test.5" "g.val[0:1][0:1][0:1]" "test.5b.base" "pass")
expr_test(45 "-w" "test.5" "g.length" "test.5c.base" "pass")
expr_test(46 "-W" "test.5" "g.length" "test.5c.base" "pass")
expr_test(47 "-w" "test.5" "g.length,g.width" "test.5d.base" "pass")
expr_test(48 "-W" "test.5" "g.length,g.width" "test.5d.base" "pass")
expr_test(49 "-w" "test.2" "j,o" "test.2g.base" "pass")
expr_test(50 "-W" "test.2" "j,o" "test.2g.base" "pass")
expr_test(51 "-w" "test.8" "\"data%23i[0:2:9][0:2]\"" "test.8.base" "pass")
expr_test(52 "-W" "test.8" "\"data%23i[0:2:9][0:2]\"" "test.8.base" "pass")
expr_test(53 "-w" "test.7" "x,y,f" "test.7.base" "pass")
expr_test(54 "-W" "test.7" "x,y,f" "test.7.base" "pass")
expr_test(55 "-w" "test.8" "\"x%23y,y\"" "test.8a.base" "pass")
expr_test(56 "-W" "test.8" "\"x%23y,y\"" "test.8a.base" "pass")
expr_test(57 "-w" "test.8" "\"data%20name,y\"" "test.8b.base" "pass")
expr_test(58 "-W" "test.8" "\"data%20name,y\"" "test.8b.base" "pass")
expr_test(59 "-w" "test.9" "\"Data-Set-2.fakeDim0[0:3],Data-Set-2.fakeDim1[0:3]\"" "test.9.base" "pass")
expr_test(60 "-W" "test.9" "\"Data-Set-2.fakeDim0[0:3],Data-Set-2.fakeDim1[0:3]\"" "test.9.base" "pass")

# These should fail - that is, there should be stuff in stderr in addition to stdout.
# The baselines appear malformed. 7/16/25 jhrg
# Baselines fixed. 7/17/25 jhrg
expr_test(61 "-w" "test.5" "g[1:4:9]" "test.5e.base" "pass")
expr_test(62 "-W" "test.5" "g[1:4:9]" "test.5e.base" "pass")
expr_test(63 "-w" "test.6" "i[1:4:9]" "test.6c.base" "pass")
expr_test(64 "-W" "test.6" "i[1:4:9]" "test.6c.base" "pass")

# New tests for server functions that take arrays and include array projections
# Added 10/23/13 jhrg
expr_test(65 "-w" "test.6" "scale\(i,2\)" "test.6.func1.base" "pass")
expr_test(66 "-W" "test.6" "scale\(i,2\)" "test.6.func1.base" "pass")
expr_test(67 "-w" "test.6" "scale\(i[2:4][3:6],2\)" "test.6.func2.base" "pass")
expr_test(68 "-W" "test.6" "scale\(i[2:4][3:6],2\)" "test.6.func2.base" "pass")

expr_test(69 "-w" "test.5" "scale\(i[3],2\)" "test.5.func3.base" "pass")
expr_test(70 "-W" "test.5" "scale\(i[3],2\)" "test.5.func3.base" "pass")

# This doesn't work yet 10/23/13 jhrg
expr_test(71 "-w" "test.5" "scale\(j,2\)" "test.5.func4.base" "pass")
expr_test(72 "-W" "test.5" "scale\(j,2\)" "test.5.func4.base" "pass")

# all the tests from here on fail in the cmake build but pass in the autotools
# build. 7/1625 jhrg
expr_test(73 "-bw" "test.a" "" "test.a.base" "pass")
expr_test(74 "-bW" "test.a" "" "test.a.base" "pass")
expr_test(75 "-bw" "test.a" "&i<2000" "test.aa.base" "pass")
expr_test(76 "-bW" "test.a" "&i<2000" "test.aa.base" "pass")
expr_test(77 "-bw" "test.a" "j&i>2000" "test.ab.base" "pass")
expr_test(78 "-bW" "test.a" "j&i>2000" "test.ab.base" "pass")
expr_test(79 "-bw" "test.a" "i,j&i<0" "test.ac.base" "pass")
expr_test(80 "-bW" "test.a" "i,j&i<0" "test.ac.base" "pass")
# Added 2/3/22 jhrg
expr_test(81 "-bw" "test.a" "s[0:3]" "test.ad.base" "pass")
expr_test(82 "-bW" "test.a" "s[0:3]" "test.ad.base" "pass")
expr_test(83 "-bw" "test.b" "" "test.b.base" "pass")
expr_test(84 "-bW" "test.b" "" "test.b.base" "pass")
expr_test(85 "-bw" "test.b" "i,f" "test.ba.base" "pass")
expr_test(86 "-bW" "test.b" "i,f" "test.ba.base" "pass")
expr_test(87 "-bw" "test.b" "i,f&i<2000" "test.bb.base" "pass")
expr_test(88 "-bW" "test.b" "i,f&i<2000" "test.bb.base" "pass")
expr_test(89 "-bw" "test.b" "i,f&f<0" "test.bc.base" "pass")
expr_test(90 "-bW" "test.b" "i,f&f<0" "test.bc.base" "pass")
expr_test(91 "-bw" "test.b" "i,j&i<2000" "test.bd.base" "pass")
expr_test(92 "-bW" "test.b" "i,j&i<2000" "test.bd.base" "pass")
expr_test(93 "-bw" "test.b" "&i<0" "test.be.base" "pass")
expr_test(94 "-bW" "test.b" "&i<0" "test.be.base" "pass")
expr_test(95 "-bw" "test.d" "" "test.d.base" "pass")
expr_test(96 "-bW" "test.d" "" "test.d.base" "pass")
expr_test(97 "-bw" "test.d" "i,f,a" "test.da.base" "pass")
expr_test(98 "-bW" "test.d" "i,f,a" "test.da.base" "pass")
expr_test(99 "-bw" "test.d" "i,f,a&i<2000" "test.db.base" "pass")
expr_test(100 "-bW" "test.d" "i,f,a&i<2000" "test.db.base" "pass")
expr_test(101 "-bw" "test.d" "i,f,a&f<0" "test.dc.base" "pass")
expr_test(102 "-bW" "test.d" "i,f,a&f<0" "test.dc.base" "pass")
expr_test(103 "-bw" "test.d" "i,f,a&a<10" "test.dd.base" "pass")
expr_test(104 "-bW" "test.d" "i,f,a&a<10" "test.dd.base" "pass")
expr_test(105 "-bw" "test.d" "i,f&i<2000" "test.de.base" "pass")
expr_test(106 "-bW" "test.d" "i,f&i<2000" "test.de.base" "pass")
expr_test(107 "-bw" "test.d" "i&i<2000" "test.df.base" "pass")
expr_test(108 "-bW" "test.d" "i&i<2000" "test.df.base" "pass")
expr_test(109 "-bw" "test.d" "i,f,a&i<0" "test.dg.base" "pass")
expr_test(110 "-bW" "test.d" "i,f,a&i<0" "test.dg.base" "pass")

expr_test(111 "-bw" "test.61" "i" "data.61a.base" "pass")
expr_test(112 "-bW" "test.61" "i" "data.61a.base" "pass")

expr_test(113 "-bw" "test.61" "i[0:2][0:2]" "data.61b.base" "pass")
expr_test(114 "-bW" "test.61" "i[0:2][0:2]" "data.61b.base" "pass")

expr_test(115 "-bw" "test.61" "i[1:2][0:2]" "data.61c.base" "pass")
expr_test(116 "-bW" "test.61" "i[1:2][0:2]" "data.61c.base" "pass")

expr_test(117 "-bw" "test.61" "i[1:2][1:2]" "data.61d.base" "pass")
expr_test(118 "-bW" "test.61" "i[1:2][1:2]" "data.61d.base" "pass")

expr_test(119 "-bw" "test.c0" "SST" "data.z1.base" "pass")
expr_test(120 "-bW" "test.c0" "SST" "data.z1.base" "pass")

expr_test(121 "-bw" "test.f" "" "test.fa.base" "pass")
expr_test(122 "-bW" "test.f" "" "test.fa.base" "pass")

expr_test(123 "-bw" "test.f" "&i<3000" "test.fb.base" "pass")
expr_test(124 "-bW" "test.f" "&i<3000" "test.fb.base" "pass")

expr_test(125 "-bw" "test.21.dds" "" "data.21.base" "pass")
expr_test(126 "-bW" "test.21.dds" "" "data.21.base" "pass")

expr_test(127 "-bw" "test.22.dds" "" "data.22.base" "pass")
expr_test(128 "-bW" "test.22.dds" "" "data.22.base" "pass")

expr_test(129 "-bw" "test.23.dds" "" "data.23.base" "pass")
expr_test(130 "-bW" "test.23.dds" "" "data.23.base" "pass")

expr_test(131 "-bw" "test.24.dds" "" "data.24.base" "pass")
expr_test(132 "-bW" "test.24.dds" "" "data.24.base" "pass")

## Empty Structures. jhrg 1/29/16
expr_test(133 "-bw" "test.25.dds" "" "data.25.base" "pass")
expr_test(134 "-bW" "test.25.dds" "" "data.25.base" "pass")

expr_test(135 "-bw" "test.26.dds" "" "data.26.base" "pass")
expr_test(136 "-bW" "test.26.dds" "" "data.26.base" "pass")

## Test error responses. None of the parsers should allow any part of
## the malformed CE into the error messages. jhrg 4/15/20
## Morphed to cmake 7/17/25 jhrg
function(expr_error_test test_num option input ce baseline xfail)
	set(testname "expr_test_${test_num}")
	set(input "${CMAKE_CURRENT_SOURCE_DIR}/expr-testsuite/${input}")
	set(baseline "${CMAKE_CURRENT_SOURCE_DIR}/expr-testsuite/${baseline}")
	set(err_output "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			"\"$<TARGET_FILE:expr-test>\" \"${option}\" \"${input}\" -k \"${ce}\" > /dev/null 2> \"${err_output}\"; \
			diff -b -B \"${baseline}\" \"${err_output}\""
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;expr-error")
	# set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
	if("${xfail}" STREQUAL "xfail")
		set_tests_properties(${testname} PROPERTIES WILL_FAIL TRUE)
	endif()
endfunction()

expr_error_test(137 "-ep"  "test.1" "d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1" "test.1.error" "pass")
expr_error_test(138 "-ep"  "test.2" "d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1" "test.2.error" "pass")
expr_error_test(139 "-ep"  "test.3" "d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1" "test.3.error" "pass")
expr_error_test(140 "-ep"  "test.5" "d1rox%253cscript%253ealert%25281%2529%253c%252fscript%253ed55je=1" "test.5.error" "pass")
expr_error_test(141 "-ep"  "test.3" "i[10:9]" "test.3b.error" "pass")
expr_error_test(142 "-ep"  "test.3" "i[4:1]" "test.3c.error" "pass")
expr_error_test(143 "-ep"  "test.5" "g[2:2:1][0][0]" "test.5a.error" "pass")
expr_error_test(144 "-ep"  "test.5" "g[2:4][0][1:0]" "test.5b.error" "pass")

expr_test(145 "-bw"  "test.a" "s[3:0]" "test.a.error.base" "pass")
expr_test(146 "-bW"  "test.a" "s[3:0]" "test.a.error.base" "pass")
