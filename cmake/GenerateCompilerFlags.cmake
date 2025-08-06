
# These cmake functions take variables set using find_pacakge, etc., and
# reformat them so they can be easily used by the dap-config script and
# pkg-config *.pc scripts. 6/25/25 jhrg

function(generate_include_flags input_list_var output_var)
	set(result "")
	foreach(path IN LISTS ${input_list_var})
		list(APPEND result "-I${path}")
	endforeach()
	list(REMOVE_DUPLICATES result)
	string(JOIN " " joined_result ${result})
	set(${output_var} "${joined_result}" PARENT_SCOPE)
endfunction()

function(generate_link_flags input_libs_var ldflags_out_var ldlibs_out_var)
	set(ldflags "")
	set(ldlibs "")

	foreach(lib IN LISTS ${input_libs_var})
		get_filename_component(lib_dir "${lib}" DIRECTORY)
		get_filename_component(lib_name "${lib}" NAME_WE)
		string(REPLACE "lib" "" lib_stripped "${lib_name}")

		list(APPEND ldflags "-L${lib_dir}")
		list(APPEND ldlibs "-l${lib_stripped}")
	endforeach()

	list(REMOVE_DUPLICATES ldflags)
	list(REMOVE_DUPLICATES ldlibs)

	string(JOIN " " joined_ldflags ${ldflags})
	string(JOIN " " joined_ldlibs ${ldlibs})

	set(${ldflags_out_var} "${joined_ldflags}" PARENT_SCOPE)
	set(${ldlibs_out_var} "${joined_ldlibs}" PARENT_SCOPE)
endfunction()

function(generate_pkg_config_flags prefix incs libs out_cflags out_libs)
	generate_include_flags(${incs} _incs)
	generate_link_flags(${libs} _ldflags _ldlibs)

	set(${out_cflags} "${_incs}" PARENT_SCOPE)
	set(${out_libs} "${_ldflags} ${_ldlibs}" PARENT_SCOPE)
endfunction()
