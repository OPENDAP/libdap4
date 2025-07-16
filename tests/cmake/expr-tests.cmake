
#m4_define([_EXPR_TEST], [
#		# AT_BANNER([Test $1 $2 $3])
#		AT_SETUP([expr-test $1 $2 -k $3 ($5)])
#		AT_KEYWORDS([expr])
#
#		AS_IF([test -n "$baselines" -a x$baselines = xyes],
#		[
#		AT_CHECK([$abs_builddir/expr-test $1 $2 -k $3 -f "dummy" || true], [0], [stdout], [stderr])
#		AT_CHECK([mv stdout $4.tmp])
#		],
#		[
#		AT_CHECK([$abs_builddir/expr-test $1 $2 -k $3 -f "dummy" || true], [0], [stdout], [stderr])
#		AT_CHECK([diff -b -B $4 stdout || diff -b -B $4 stderr], [0], [ignore])
#		AT_XFAIL_IF([test "$5" = "xfail"])
#		])
#
#		AT_CLEANUP
#		])
#
#m4_define([EXPR_RESPONSE_P], [
#		# AT_BANNER([EXPR response for $1, $2, $4.])
#		# Test the serialize/deserialize methods
#		_EXPR_TEST([-w], [$abs_srcdir/expr-testsuite/$1], [$2], [$abs_srcdir/expr-testsuite/$3.base], $4)
#		# Test the intern_data method (which combines serialize and deserialize)
#		_EXPR_TEST([-W], [$abs_srcdir/expr-testsuite/$1], [$2], [$abs_srcdir/expr-testsuite/$3.base], $4)
#		])

function(expr_test test_num option input ce baseline xfail)
	set(testname "expr_test_${test_num}")
	set(input "${CMAKE_CURRENT_SOURCE_DIR}/expr-testsuite/${input}")
	set(baseline "${CMAKE_CURRENT_SOURCE_DIR}/expr-testsuite/${baseline}.base")
	set(output "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	add_test(NAME ${testname}
			COMMAND /bin/sh "-c"
			# 1) run das-test, redirect all output into a temp file
			# 2) diff that file against the baseline"
			"\"$<TARGET_FILE:expr-test>\" \"${option}\" \"${input}\" -k \"${ce}\" -f \"dummy\"> \"${output}\" 2>&1; \
			diff -b -B \"${baseline}\" \"${output}\""
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;expr")
	# Not needed, but here if we use this as a templet for other tests. 7/15//25 jhrg
	# set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
	if("${xfail}" STREQUAL "xfail")
		set_tests_properties(${testname} PROPERTIES WILL_FAIL TRUE)
	endif()
endfunction()

## The xfail tests below should pass but have empty baseline files due to a bug
## in the tests from 2013 (?). Make new baselines! 7/15/25 jhrg

expr_test(1 "-w" "test.1" "i" "test.1" "pass")
expr_test(2 "-W" "test.1" "i" "test.1" "pass")
expr_test(3 "-w" "test.1" "i,j" "test.1a" "pass")
expr_test(4 "-W" "test.1" "i,j" "test.1a" "pass")
expr_test(5 "-w" "test.1" "i,j&i=j" "test.1b" "pass")
expr_test(6 "-W" "test.1" "i,j&i=j" "test.1b" "pass")
expr_test(7 "-w" "test.1" "i&i=j" "test.1d" "pass")
expr_test(8 "-W" "test.1" "i&i=j" "test.1d" "pass")
expr_test(9 "-w" "test.2" "s1" "test.2" "pass")
expr_test(10 "-W" "test.2" "s1" "test.2" "pass")
expr_test(11 "-w" "test.2" "s2" "test.2a" "pass")
expr_test(12 "-W" "test.2" "s2" "test.2a" "pass")
expr_test(13 "-w" "test.2" "s2,s3" "test.2b" "pass")
expr_test(14 "-W" "test.2" "s2,s3" "test.2b" "pass")
expr_test(15 "-w" "test.2" "s2[2:2:4],s3.o" "test.2c" "pass")
expr_test(16 "-W" "test.2" "s2[2:2:4],s3.o" "test.2c" "pass")

expr_test(17 "-w" "test.2" "s2[2:2:4].m" "test.2d" "pass")
expr_test(18 "-W" "test.2" "s2[2:2:4].m" "test.2d" "pass")
expr_test(19 "-w" "test.2" "s2[2:2:4].m,s2[2:2:4].l" "test.2e" "pass")
expr_test(20 "-W" "test.2" "s2[2:2:4].m,s2[2:2:4].l" "test.2e" "pass")

expr_test(21 "-w" "test.2a" "s2[2:4].m[0:4],s2[2:4].l[0:5]" "test.2f" "pass")
expr_test(22 "-W" "test.2a" "s2[2:4].m[0:4],s2[2:4].l[0:5]" "test.2f" "pass")
expr_test(23 "-w" "test.3" "i[1:10]" "test.3" "pass")
expr_test(24 "-W" "test.3" "i[1:10]" "test.3" "pass")

# Test the new code to limit reversed index values. 10:10 should pass. jhrg 2/3/11
expr_test(25 "-w" "test.3" "i[10:10]" "test.3a" "pass")
expr_test(26 "-W" "test.3" "i[10:10]" "test.3a" "pass")

expr_test(27 "-w" "test.4" "s&s=~\"^Silly.*\"" "test.4" "xfail")
expr_test(28 "-W" "test.4" "s&s=~\"^Silly.*\"" "test.4" "xfail")

# In changing the TestStr class so that it writes a constant value (Silly ...: 1)
# I had to hack this test to use the -b option to expr-test. EXPR_RESPONSE_P uses
# only the -w/-W options, so I dropped using that macro kluge and went right for the
# actual test macro. 12/2/13 jhrg
#

expr_test(29 "\"-b -w\"" "test.e" "names.s&names.s=~\".*:.3\"" "test.ea" "xfail") # these fail because the CE quoting is broken.
expr_test(30 "\"-b -W\"" "test.e" "names.s&names.s=~\".*:.3\"" "test.ea" "xfail")

# since 'Silly ...:5' will never be produced, this tests what happens when the sequence
# does not contain the value. Note that this test does not have to be rewritten because
# of the change to the TestStr code. 12/2/13 jhrg
expr_test(31 "-w" "test.e" "names.s&names.s=~\".*: 5\"" "test.eb" "xfail") # these will also fail, see above
expr_test(32 "-W" "test.e" "names.s&names.s=~\".*: 5\"" "test.eb" "xfail")

expr_test(33 "-w" "test.5" "g[0:2:4][0][0]" "test.5" "pass")
expr_test(34 "-W" "test.5" "g[0:2:4][0][0]" "test.5" "pass")
expr_test(35 "-w" "test.5" "g[0:2:4][0:2:4][0:2:4]" "test.5a" "pass")
expr_test(36 "-W" "test.5" "g[0:2:4][0:2:4][0:2:4]" "test.5a" "pass")
expr_test(37 "-w" "test.6" "i" "test.6" "pass")
expr_test(38 "-W" "test.6" "i" "test.6" "pass")
expr_test(39 "-w" "test.6" "i[1:2][2:4]" "test.6a" "pass")
expr_test(40 "-W" "test.6" "i[1:2][2:4]" "test.6a" "pass")
# Added test of '*' DAP2 syntax. jhrg 2/4/22
expr_test(41 "-w" "test.6" "i[1:2][*]" "test.6b" "pass")
expr_test(42 "-W" "test.6" "i[1:2][*]" "test.6b" "pass")

expr_test(43 "-w" "test.5" "g.val[0:1][0:1][0:1]" "test.5b" "pass")
expr_test(44 "-W" "test.5" "g.val[0:1][0:1][0:1]" "test.5b" "pass")
expr_test(45 "-w" "test.5" "g.length" "test.5c" "pass")
expr_test(46 "-W" "test.5" "g.length" "test.5c" "pass")
expr_test(47 "-w" "test.5" "g.length,g.width" "test.5d" "pass")
expr_test(48 "-W" "test.5" "g.length,g.width" "test.5d" "pass")
expr_test(49 "-w" "test.2" "j,o" "test.2g" "pass")
expr_test(50 "-W" "test.2" "j,o" "test.2g" "pass")
expr_test(51 "-w" "test.8" "\"data%23i[0:2:9][0:2]\"" "test.8" "pass")
expr_test(52 "-W" "test.8" "\"data%23i[0:2:9][0:2]\"" "test.8" "pass")
expr_test(53 "-w" "test.7" "x,y,f" "test.7" "pass")
expr_test(54 "-W" "test.7" "x,y,f" "test.7" "pass")
expr_test(55 "-w" "test.8" "\"x%23y,y\"" "test.8a" "pass")
expr_test(56 "-W" "test.8" "\"x%23y,y\"" "test.8a" "pass")
expr_test(57 "-w" "test.8" "\"data%20name,y\"" "test.8b" "pass")
expr_test(58 "-W" "test.8" "\"data%20name,y\"" "test.8b" "pass")
expr_test(59 "-w" "test.9" "\"Data-Set-2.fakeDim0[0:3],Data-Set-2.fakeDim1[0:3]\"" "test.9" "pass")
expr_test(60 "-W" "test.9" "\"Data-Set-2.fakeDim0[0:3],Data-Set-2.fakeDim1[0:3]\"" "test.9" "pass")

expr_test(61 "-w" "test.5" "g[1:4:9]" "test.5e" "pass")
expr_test(62 "-W" "test.5" "g[1:4:9]" "test.5e" "pass")
expr_test(63 "-w" "test.6" "i[1:4:9]" "test.6c" "pass")
expr_test(64 "-W" "test.6" "i[1:4:9]" "test.6c" "pass")

# New tests for server functions that take arrays and include array projections
# Added 10/23/13 jhrg
expr_test(65 "-w" "test.6" "scale\(i,2\)" "test.6.func1" "pass")
expr_test(66 "-W" "test.6" "scale\(i,2\)" "test.6.func1" "pass")
expr_test(67 "-w" "test.6" "scale\(i[2:4][3:6],2\)" "test.6.func2" "pass")
expr_test(68 "-W" "test.6" "scale\(i[2:4][3:6],2\)" "test.6.func2" "pass")

expr_test(69 "-w" "test.5" "scale\(i[3],2\)" "test.5.func3" "pass")
expr_test(70 "-W" "test.5" "scale\(i[3],2\)" "test.5.func3" "pass")

# This doesn't work yet 10/23/13 jhrg
expr_test(71 "-w" "test.5" "scale\(j,2\)" "test.5.func4" "pass")
expr_test(72 "-W" "test.5" "scale\(j,2\)" "test.5.func4" "pass")

expr_test(73 "-w" "test.a" "-b" "test.a" "pass")
expr_test(74 "-W" "test.a" "-b" "test.a" "pass")
expr_test(75 "-w" "test.a" "\"&i<2000\" -b" "test.aa" "pass")
expr_test(76 "-W" "test.a" "\"&i<2000\" -b" "test.aa" "pass")
expr_test(77 "-w" "test.a" "\"j&i>2000\" -b" "test.ab" "pass")
expr_test(78 "-W" "test.a" "\"j&i>2000\" -b" "test.ab" "pass")
expr_test(79 "-w" "test.a" "\"i,j&i<0\" -b" "test.ac" "pass")
expr_test(80 "-W" "test.a" "\"i,j&i<0\" -b" "test.ac" "pass")
# Added 2/3/22 jhrg
expr_test(81 "-w" "test.a" "[s[0:3] -b" "test.ad" "pass")
expr_test(82 "-W" "test.a" "[s[0:3] -b" "test.ad" "pass")
expr_test(83 "-w" "test.b" "-b" "test.b" "pass")
expr_test(84 "-W" "test.b" "-b" "test.b" "pass")
expr_test(85 "-w" "test.b" "\"i,f\" -b" "test.ba" "pass")
expr_test(86 "-W" "test.b" "\"i,f\" -b" "test.ba" "pass")
expr_test(87 "-w" "test.b" "\"i,f&i<2000\" -b" "test.bb" "pass")
expr_test(88 "-W" "test.b" "\"i,f&i<2000\" -b" "test.bb" "pass")
expr_test(89 "-w" "test.b" "\"i,f&f<0\" -b" "test.bc" "pass")
expr_test(90 "-W" "test.b" "\"i,f&f<0\" -b" "test.bc" "pass")
expr_test(91 "-w" "test.b" "\"i,j&i<2000\" -b" "test.bd" "pass")
expr_test(92 "-W" "test.b" "\"i,j&i<2000\" -b" "test.bd" "pass")
expr_test(93 "-w" "test.b" "\"&i<0\" -b" "test.be" "pass")
expr_test(94 "-W" "test.b" "\"&i<0\" -b" "test.be" "pass")
expr_test(95 "-w" "test.d" "-b" "test.d" "pass")
expr_test(96 "-W" "test.d" "-b" "test.d" "pass")
expr_test(97 "-w" "test.d" "\"i,f,a\" -b" "test.da" "pass")
expr_test(98 "-W" "test.d" "\"i,f,a\" -b" "test.da" "pass")
expr_test(99 "-w" "test.d" "\"i,f,a&i<2000\" -b" "test.db" "pass")
expr_test(100 "-W" "test.d" "\"i,f,a&i<2000\" -b" "test.db" "pass")
expr_test(101 "-w" "test.d" "\"i,f,a&f<0\" -b" "test.dc" "pass")
expr_test(102 "-W" "test.d" "\"i,f,a&f<0\" -b" "test.dc" "pass")
expr_test(103 "-w" "test.d" "\"i,f,a&a<10\" -b" "test.dd" "pass")
expr_test(104 "-W" "test.d" "\"i,f,a&a<10\" -b" "test.dd" "pass")
expr_test(105 "-w" "test.d" "\"i,f&i<2000\" -b" "test.de" "pass")
expr_test(106 "-W" "test.d" "\"i,f&i<2000\" -b" "test.de" "pass")
expr_test(107 "-w" "test.d" "\"i&i<2000\" -b" "test.df" "pass")
expr_test(108 "-W" "test.d" "\"i&i<2000\" -b" "test.df" "pass")
expr_test(109 "-w" "test.d" "\"i,f,a&i<0\" -b" "test.dg" "pass")
expr_test(110 "-W" "test.d" "\"i,f,a&i<0\" -b" "test.dg" "pass")
