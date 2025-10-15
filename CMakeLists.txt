cmake_minimum_required(VERSION 3.20)
project(libdap LANGUAGES C CXX)
set(LIBDAP_VERSION "3.21.1")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tells the linker to embed the final install path
# (e.g., /usr/local/lib or /custom/prefix/lib) which is
# generally less likely to break for code that does not
# use cmake. Not sure how this will work on platforms
# that use ...lib64. 6/27/25 jhrg
set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")


# Look for 'find' macros in the 'cmake' dir. jhrg 6/18/25
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CheckCXXSourceCompiles)

# Dependency packages
find_package(CppUnit REQUIRED)
find_package(LibXml2 REQUIRED)
# message(STATUS "LIBXML2_LIBRARIES: ${LIBXML2_LIBRARIES}")
# message(STATUS "LIBXML2_INCLUDE_DIR: ${LIBXML2_INCLUDE_DIR}")

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Manual TIRPC detection
find_path(TIRPC_INCLUDE_DIR rpc/rpc.h PATH_SUFFIXES tirpc)
find_library(TIRPC_LIBRARY tirpc)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TIRPC DEFAULT_MSG TIRPC_INCLUDE_DIR TIRPC_LIBRARY)
if(TIRPC_INCLUDE_DIR AND TIRPC_LIBRARY)
    set(TIRPC_FOUND TRUE)
    set(TIRPC_INCLUDE_DIRS ${TIRPC_INCLUDE_DIR})
    message(STATUS "TIRPC_INCLUDE_DIRS: ${TIRPC_INCLUDE_DIRS}")
    set(TIRPC_LIBRARIES ${TIRPC_LIBRARY})
    message(STATUS "TIRPC_LIBRARIES: ${TIRPC_LIBRARIES}")
endif()

# Perform config/feature checks
include(FindLibdapDependencies)
message(STATUS "Target platform: ${LIBDAP_PLATFORM}")

# Optional ASAN and Developer mode flags (based on CMake options)
option(USE_ASAN "Enable Address Sanitizer" OFF)
option(BUILD_DEVELOPER "Build in developer mode" OFF)
option(USE_CPP_11_REGEX "Expect and use C++11 Regex code" ON)
option(BIG_ARRAY_TEST "Run the Big Array Unit test - takes a long time" OFF)

set(COMMON_WARNINGS -Wall -Wcast-align)

if(BUILD_DEVELOPER)
    set(DEV_FLAGS -g3 -O0 ${COMMON_WARNINGS})
else()
    set(DEV_FLAGS -O2 -DNDEBUG ${COMMON_WARNINGS})
endif()

if(USE_ASAN)
    list(APPEND DEV_FLAGS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    set(ASAN_LINK_FLAGS -fsanitize=address -fsanitize=undefined)
endif()

set(LIBDAP_ROOT "${CMAKE_INSTALL_PREFIX}")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/dods-datatypes-static.h"
        "${CMAKE_CURRENT_BINARY_DIR}/dods-datatypes.h"
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/xdr-datatypes-static.h"
        "${CMAKE_CURRENT_BINARY_DIR}/xdr-datatypes.h"
)

# This is local to this project. 6/25/25/ jhrg
include(cmake/GenerateCompilerFlags.cmake)

generate_pkg_config_flags(CURL CURL_INCLUDE_DIRS CURL_LIBRARIES
        CURL_PKG_CFLAGS CURL_PKG_LIBS)

generate_pkg_config_flags(LibXml2 LIBXML2_INCLUDE_DIR LIBXML2_LIBRARIES
        XML2_PKG_CFLAGS XML2_PKG_LIBS)

generate_pkg_config_flags(TIRPC TIRPC_INCLUDE_DIRS TIRPC_LIBRARIES
        TIRPC_PKG_CFLAGS TIRPC_PKG_LIBS)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/dap-config.in.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/dap-config"
        @ONLY
)

install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/dap-config"
        DESTINATION bin)

configure_file(
        ${CMAKE_SOURCE_DIR}/libdap.pc.in
        ${CMAKE_BINARY_DIR}/libdap.pc
        @ONLY
)

install(FILES ${CMAKE_BINARY_DIR}/libdap.pc DESTINATION lib/pkgconfig)

### I am not sure we need this anymore - copied from the autotools build.
### This is very old, we never build on Solaris anymore. jhrg 6/10/25
# Check if we're on Solaris (typically passed in by the toolchain)
# These are best handled via:
if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
    add_definitions(-DSOLARIS)
endif()

# If you want to define _REENTRANT manually:
add_definitions(-D_REENTRANT)

### End old Solaris stuff

### Set the SO/dylib versions

# How to set these SO variables:
# No interfaces changed, only implementations (good): ==> Increment REVISION.
# Interfaces added, none removed (good): ==> Increment CURRENT,
# increment AGE, set REVISION to 0.
# Interfaces removed or changed (BAD, breaks upward compatibility):
# ==> Increment CURRENT, set AGE and REVISION to 0.

SET(DAPLIB_CURRENT 30)
SET(DAPLIB_REVISION 0)
SET(DAPLIB_AGE 3)

SET(LIBDAP_SO_VERSION "${DAPLIB_CURRENT}:${DAPLIB_REVISION}:${DAPLIB_AGE}")

SET(CLIENTLIB_CURRENT 8)
SET(CLIENTLIB_REVISION 0)
SET(CLIENTLIB_AGE 2)

SET(CLIENTLIB_SO_VERSION "${CLIENTLIB_CURRENT}:${CLIENTLIB_REVISION}:${CLIENTLIB_AGE}")

SET(SERVERLIB_CURRENT 14)
SET(SERVERLIB_REVISION 0)
SET(SERVERLIB_AGE 7)

SET(SERVERLIB_SO_VERSION "${SERVERLIB_CURRENT}:${SERVERLIB_REVISION}:${SERVERLIB_AGE}")

# Drop the 'gl' library for now. jhrg 6/8/25

# add_subdirectory(gl)
add_subdirectory(d4_ce)
add_subdirectory(d4_function)
add_subdirectory(http_dap)

# Include directories (assuming top-level dirs)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/GNU
        ${CMAKE_CURRENT_SOURCE_DIR}/http_dap

        # Make the built header(s) findable
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Define sources
set(GNU_SRC GNU/GetOpt.cc GNU/GNURegex.cc)

set(DAP_SRC
        AttrTable.cc DAS.cc DDS.cc DataDDS.cc DDXParserSAX2.cc
        BaseType.cc Byte.cc Int32.cc Float64.cc Str.cc Url.cc
        Vector.cc Array.cc Structure.cc Sequence.cc Grid.cc UInt32.cc
        Int16.cc UInt16.cc Float32.cc Constructor.cc
        BaseTypeFactory.cc SignalHandler.cc Error.cc InternalErr.cc
        util.cc xdrutil_ppc.c parser-util.cc escaping.cc
        Clause.cc RValue.cc ConstraintEvaluator.cc DapIndent.cc
        XDRUtils.cc XDRFileMarshaller.cc XDRStreamMarshaller.cc
        XDRFileUnMarshaller.cc XDRStreamUnMarshaller.cc mime_util.cc
        Keywords2.cc XMLWriter.cc ServerFunctionsList.cc ServerFunction.cc
        DapXmlNamespaces.cc MarshallerThread.cc
)

set(DAP4_ONLY_SRC
        D4StreamMarshaller.cc D4StreamUnMarshaller.cc Int64.cc UInt64.cc Int8.cc
        D4ParserSax2.cc D4BaseTypeFactory.cc D4Dimensions.cc D4EnumDefs.cc D4Group.cc
        DMR.cc D4Attributes.cc D4Enum.cc chunked_ostream.cc chunked_istream.cc
        D4Sequence.cc D4Maps.cc D4Opaque.cc D4AsyncUtil.cc D4RValue.cc D4FilterClause.cc
)

set(CLIENT_SRC RCReader.cc Connect.cc D4Connect.cc util_mit.cc)

set(SERVER_SRC DODSFilter.cc Ancillary.cc)

### Grammars

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(DDSParser dds.yy ${CMAKE_CURRENT_BINARY_DIR}/dds.tab.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/dds.tab.hh)
# Using an explicit path for the output doesn't work. This, however, does put the output
# in the binary directory. The error is that the macro calls flex with no space between
# the -o option and the destination path. jhrg 6/9/25
FLEX_TARGET(DDSScanner dds.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.dds.cc)

set_source_files_properties(${FLEX_DDSScanner_OUTPUTS} PROPERTIES GENERATED TRUE)
# Example cmake debugging. jhrg 6/9/25
# message(STATUS "FLEX_DDSScanner_OUTPUTS = ${FLEX_DDSScanner_OUTPUTS}")
ADD_FLEX_BISON_DEPENDENCY(DDSScanner DDSParser)

BISON_TARGET(DASParser das.yy ${CMAKE_CURRENT_BINARY_DIR}/das.tab.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/das.tab.hh)

FLEX_TARGET(DASScanner das.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.das.cc)

ADD_FLEX_BISON_DEPENDENCY(DASScanner DASParser)

BISON_TARGET(CEParser ce_expr.yy ${CMAKE_CURRENT_BINARY_DIR}/ce_expr.tab.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ce_expr.tab.hh)

FLEX_TARGET(CEScanner ce_expr.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.ce_expr.cc)

ADD_FLEX_BISON_DEPENDENCY(CEScanner CEParser)

BISON_TARGET(ErrorParser Error.yy ${CMAKE_CURRENT_BINARY_DIR}/Error.tab.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Error.tab.hh)

FLEX_TARGET(ErrorScanner Error.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.Error.cc)

ADD_FLEX_BISON_DEPENDENCY(ErrorScanner ErrorParser)

set(GENERATED_PARSER_SRC
        ${BISON_DDSParser_OUTPUT_SOURCE}
        ${BISON_DDSParser_OUTPUT_HEADER}
        ${FLEX_DDSScanner_OUTPUTS}
        ${BISON_DASParser_OUTPUT_SOURCE}
        ${BISON_DASParser_OUTPUT_HEADER}
        ${FLEX_DASScanner_OUTPUTS}
        ${BISON_CEParser_OUTPUT_SOURCE}
        ${BISON_CEParser_OUTPUT_HEADER}
        ${FLEX_CEScanner_OUTPUTS}
        ${BISON_ErrorParser_OUTPUT_SOURCE}
        ${BISON_ErrorParser_OUTPUT_HEADER}
        ${FLEX_ErrorScanner_OUTPUTS}
)

# Ensure grammar files build before other sources
set_source_files_properties(${GENERATED_PARSER_SRC}
        PROPERTIES GENERATED TRUE)

# Custom cleanup
set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES
        "${GENERATED_PARSER_SRC};stack.hh;location.hh;position.hh"
)

# Define libraries
add_library(parsers OBJECT ${GENERATED_PARSER_SRC})
target_include_directories(parsers PRIVATE ${LIBXML2_INCLUDE_DIR})
# target_include_directories(parsers PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(parsers PRIVATE ${LIBXML2_LIBRARIES})
if(TIRPC_FOUND)
    target_include_directories(parsers PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(parsers PRIVATE ${TIRPC_LIBRARIES})
endif()
set_target_properties(parsers PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(dap SHARED ${DAP_SRC} ${GNU_SRC} ${DAP4_ONLY_SRC})
target_sources(dap PRIVATE
        $<TARGET_OBJECTS:parsers>
        $<TARGET_OBJECTS:d4_ce_parser>
        $<TARGET_OBJECTS:d4_function_parser>
)
target_include_directories(dap PRIVATE ${LIBXML2_INCLUDE_DIR})
# target_include_directories(dap PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
if(TIRPC_FOUND)
    target_include_directories(dap PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(dap PRIVATE ${TIRPC_LIBRARIES})
endif()
# target_link_libraries(dap PRIVATE parsers d4_ce_parser d4_function_parser)
target_link_libraries(dap PRIVATE ${LIBXML2_LIBRARIES} Threads::Threads)
set_target_properties(dap PROPERTIES VERSION ${LIBDAP_VERSION}) # SOVERSION ${LIBDAP_SO_VERSION})

# message(STATUS "INCLUDE PATHS for dap:")
get_target_property(DAP_INCLUDES dap INCLUDE_DIRECTORIES)
# message(STATUS "  ${DAP_INCLUDES}")

add_library(dapclient SHARED ${CLIENT_SRC})
target_sources(dapclient PRIVATE
        $<TARGET_OBJECTS:http_dap>
)
target_include_directories(dapclient PRIVATE ${CURL_INCLUDE_DIRS})
target_include_directories(dapclient PRIVATE ${LIBXML2_INCLUDE_DIR})
# target_include_directories(dapclient PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
if(TIRPC_FOUND)
    target_include_directories(dapclient PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(dapclient PRIVATE ${TIRPC_LIBRARIES})
endif()
target_link_libraries(dapclient PRIVATE dap)
target_link_libraries(dapclient PRIVATE ${CURL_LIBRARIES} Threads::Threads)

set_target_properties(dapclient PROPERTIES VERSION ${LIBDAP_VERSION}) # SOVERSION ${CLIENTLIB_SO_VERSION})

# message(STATUS "INCLUDE PATHS for dapclient:")
get_target_property(DAPCLIENT_INCLUDES dapclient INCLUDE_DIRECTORIES)
# message(STATUS "  ${DAPCLIENT_INCLUDES}")

add_library(dapserver SHARED ${SERVER_SRC})
target_link_libraries(dapserver PRIVATE dap ${UUID_LIB})
target_include_directories(dapserver PRIVATE ${LIBXML2_INCLUDE_DIR})
# target_include_directories(dapserver PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
if(TIRPC_FOUND)
    target_include_directories(dapserver PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(dapserver PRIVATE ${TIRPC_LIBRARIES})
endif()
set_target_properties(dapserver PROPERTIES VERSION ${LIBDAP_VERSION}) # SOVERSION ${SERVERLIB_SO_VERSION})

# message(STATUS "INCLUDE PATHS for dapserver:")
get_target_property(DAPSERVER_INCLUDES dapserver INCLUDE_DIRECTORIES)
# message(STATUS "  ${DAPSERVER_INCLUDES}")

# Custom compilation flags and definitions

# Define executables
add_executable(getdap getdap.cc)
target_link_libraries(getdap PRIVATE dapclient dap)
target_include_directories(getdap PRIVATE ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})
if(TIRPC_FOUND)
    target_include_directories(getdap PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(getdap PRIVATE ${TIRPC_LIBRARIES})
endif()

add_executable(getdap4 getdap4.cc)
target_link_libraries(getdap4 PRIVATE dapclient dap)
target_include_directories(getdap4 PRIVATE ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})
if(TIRPC_FOUND)
    target_include_directories(getdap4 PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(getdap4 PRIVATE ${TIRPC_LIBRARIES})
endif()

foreach(target dap dapclient dapserver getdap getdap4)
    target_compile_options(${target} PRIVATE ${DEV_FLAGS})
    if(USE_ASAN)
        target_link_options(${target} PRIVATE ${ASAN_LINK_FLAGS})
    endif()
endforeach()

# Install rules (adjust paths as needed)
install(TARGETS dap dapclient dapserver getdap getdap4
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

# Headers installation (pkginclude_HEADERS from Makefile.am)
set(DAP_HDR AttrTable.h DAS.h DDS.h DataDDS.h DDXParserSAX2.h
        DDXExceptions.h BaseType.h Byte.h Int32.h Float64.h Str.h Url.h Vector.h Array.h
        Constructor.h Structure.h Sequence.h Grid.h UInt32.h Int16.h UInt16.h Float32.h
        BaseTypeFactory.h ObjectType.h EncodingType.h SignalHandler.h Error.h InternalErr.h
        util.h escaping.h parser.h debug.h dods-limits.h Type.h util_mit.h
        expr.h Clause.h RValue.h ConstraintEvaluator.h ce_parser.h DapIndent.h DapObj.h
        XDRFileMarshaller.h Marshaller.h UnMarshaller.h XDRFileUnMarshaller.h
        XDRStreamMarshaller.h XDRUtils.h mime_util.h cgi_util.h
        XDRStreamUnMarshaller.h Keywords2.h XMLWriter.h ServerFunctionsList.h
        ServerFunction.h media_types.h DapXmlNamespaces.h parser-util.h MarshallerThread.h)

set(DAP_GENERATED_HDR ${CMAKE_BINARY_DIR}/xdr-datatypes.h  ${CMAKE_BINARY_DIR}/dods-datatypes.h)

set(DAP4_ONLY_HDR D4StreamMarshaller.h D4StreamUnMarshaller.h Int64.h UInt64.h Int8.h
        D4ParserSax2.h D4BaseTypeFactory.h D4Maps.h D4Dimensions.h D4EnumDefs.h D4Group.h
        DMR.h D4Attributes.h D4AttributeType.h D4Enum.h chunked_stream.h chunked_ostream.h
        chunked_istream.h D4Sequence.h crc.h D4Opaque.h D4AsyncUtil.h D4Function.h D4RValue.h
        D4FilterClause.h)

set(CLIENT_HDR RCReader.h Connect.h Resource.h D4Connect.h Response.h
        StdinResponse.h SignalHandlerRegisteredErr.h)

set(SERVER_HDR DODSFilter.h AlarmHandler.h EventHandler.h Ancillary.h)

install(FILES ${DAP_HDR} ${DAP_GENERATED_HDR} ${DAP4_ONLY_HDR} ${CLIENT_HDR} ${SERVER_HDR}
        DESTINATION include/libdap)

enable_testing()
add_subdirectory(tests)
add_subdirectory(unit-tests)
add_subdirectory(d4_ce/unit-tests)
add_subdirectory(http_dap/unit-tests)

# a target that runs only the unit tests:
add_custom_target(unit-test
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L unit
        DEPENDS unit-tests)            # or whatever your unit‐test target is

# a target that runs only the integration tests:
add_custom_target(integration-test
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L integration
        DEPENDS tests)                 # or your integration‐test deps

# a “check” target to do unit → integration in order:
add_custom_target(check
        DEPENDS unit-test integration-test
)

# Custom cleanup for coverage files, etc.
set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "*.gcda;*.gcno;*.gcov"
)

# This supports the libdap4Config.cmake file. 6/25/25 jhrg
# With this, people can use:
#find_package(libdap4 CONFIG REQUIRED)
#target_link_libraries(mytool PRIVATE libdap4::dapclient)
#
# Make sure the install prefix is in CMAKE_PREFIX_PATH or CMAKE_INSTALL_PREFIX/lib/cmake/libdap4

# Set install location for config files
set(LIBDAP4_CONFIG_INSTALL_DIR lib/cmake/libdap4)

# Export only public-facing targets
install(TARGETS dap dapclient dapserver
        EXPORT libdap4Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/libdap)

# Install the generated Config.cmake and export file
include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${CMAKE_SOURCE_DIR}/cmake/libdap4Config.cmake.in"
        "${CMAKE_BINARY_DIR}/libdap4Config.cmake"
        INSTALL_DESTINATION ${LIBDAP4_CONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/libdap4ConfigVersion.cmake"
        VERSION ${LIBDAP_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        "${CMAKE_BINARY_DIR}/libdap4Config.cmake"
        "${CMAKE_BINARY_DIR}/libdap4ConfigVersion.cmake"
        DESTINATION ${LIBDAP4_CONFIG_INSTALL_DIR}
)

install(EXPORT libdap4Targets
        NAMESPACE libdap4::
        DESTINATION ${LIBDAP4_CONFIG_INSTALL_DIR})
