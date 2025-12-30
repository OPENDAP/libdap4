
function(getdap_test test_num option url baseline xfail)
	set(testname "getdap_test_${test_num}")
	set(baseline "${CMAKE_CURRENT_SOURCE_DIR}/getdap-testsuite/${baseline}")
	set(output "${CMAKE_CURRENT_BINARY_DIR}/${testname}.out")

	set(the_test   "$<TARGET_FILE:getdap> ${option} ${url} > ${output} 2>&1")

	add_test(NAME ${testname}
			COMMAND /bin/sh -c "${the_test}; diff -b -B ${baseline} ${output} && rm -f ${output}"
	)

	set_tests_properties(${testname} PROPERTIES LABELS "integration;getdap")
	# Not needed, but here if we use this as a templet for other tests. 7/15//25 jhrg
	# set_tests_properties(${testname} PROPERTIES RUN_SERIAL TRUE)
	if("${xfail}" STREQUAL "xfail")
		set_tests_properties(${testname} PROPERTIES WILL_FAIL TRUE)
	endif()

endfunction()

getdap_test(1 "-d" "http://test.opendap.org/dap/data/nc/fnoc1.nc" "fnoc1.nc.dds" "pass")
getdap_test(2 "-a" "http://test.opendap.org/dap/data/nc/fnoc1.nc" "fnoc1.nc.das" "pass")
