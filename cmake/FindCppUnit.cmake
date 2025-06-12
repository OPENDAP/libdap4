# cmake/FindCppUnit.cmake

find_path(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
		HINTS /usr/local/include
)

find_library(CPPUNIT_LIBRARY cppunit
		HINTS /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CppUnit DEFAULT_MSG
		CPPUNIT_LIBRARY CPPUNIT_INCLUDE_DIR
)

if(CPPUNIT_FOUND)
	set(CppUnit_INCLUDE_DIRS ${CPPUNIT_INCLUDE_DIR})
	set(CppUnit_LIBRARIES ${CPPUNIT_LIBRARY})

	add_library(CppUnit::CppUnit UNKNOWN IMPORTED)
	set_target_properties(CppUnit::CppUnit PROPERTIES
			IMPORTED_LOCATION "${CPPUNIT_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${CPPUNIT_INCLUDE_DIR}"
	)
endif()
