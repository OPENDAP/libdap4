cmake_minimum_required(VERSION 3.10)
project(libdap LANGUAGES C CXX)
set(LIBDAP_VERSION "3.21.1")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Find required external packages
find_package(LibXml2 REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

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

add_library(parsers STATIC ${GENERATED_PARSER_SRC})

# Ensure grammar files build before other sources
set_source_files_properties(${GENERATED_PARSER_SRC}
        PROPERTIES GENERATED TRUE)

# Custom cleanup
set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES
        "${GENERATED_PARSER_SRC};stack.hh;location.hh;position.hh"
)

# Define libraries
# add_library(parsers STATIC ${FLEX_SRC} ${BISON_SRC})
target_include_directories(parsers PRIVATE ${LIBXML2_INCLUDE_DIR})
target_include_directories(parsers PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(parsers PRIVATE ${LIBXML2_LIBRARIES})

add_library(dap SHARED ${DAP_SRC} ${GNU_SRC} ${DAP4_ONLY_SRC})
target_include_directories(dap PRIVATE ${LIBXML2_INCLUDE_DIR})
target_include_directories(dap PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(dap PRIVATE parsers ${LIBXML2_LIBRARIES} Threads::Threads)

set_target_properties(dap PROPERTIES VERSION ${LIBDAP_VERSION}) # SOVERSION ${LIBDAP_SO_VERSION})

add_library(dapclient SHARED ${CLIENT_SRC})
target_include_directories(dapclient PRIVATE ${CURL_INCLUDE_DIRS})
target_include_directories(dapclient PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(dapclient PRIVATE dap http_dap ${CURL_LIBRARIES} Threads::Threads)

set_target_properties(dapclient PROPERTIES VERSION ${LIBDAP_VERSION}) # SOVERSION ${CLIENTLIB_SO_VERSION})

add_library(dapserver SHARED ${SERVER_SRC})
target_link_libraries(dapserver PRIVATE dap)
target_include_directories(dapserver PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(dapserver PROPERTIES VERSION ${LIBDAP_VERSION}) # SOVERSION ${SERVERLIB_SO_VERSION})

# Custom compilation flags and definitions

# Define executables
add_executable(getdap getdap.cc)
target_link_libraries(getdap PRIVATE dapclient dap)
target_include_directories(getdap PRIVATE ${CURL_INCLUDE_DIRS})

add_executable(getdap4 getdap4.cc)
target_link_libraries(getdap4 PRIVATE dapclient dap)
target_include_directories(getdap4 PRIVATE ${CURL_INCLUDE_DIRS})

### make the config.h header

include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckCXXSourceCompiles)

# System headers
check_include_files("unistd.h" HAVE_UNISTD_H)
# Check if <sys/time.h> defines struct tm
# (This is a proxy for TM_IN_SYS_TIME)
check_include_files("sys/time.h" HAVE_TM_IN_SYS_TIME)
check_include_files("pthread.h" HAVE_PTHREAD_H)
check_include_files("uuid/uuid.h" HAVE_UUID_UUID_H)
check_include_files("uuid.h" HAVE_UUID_H)
check_include_files("sys/stat.h" HAVE_SYS_STAT_H)
check_include_files("sys/types.h;sys/stat.h" HAVE_SYS_TYPES_H_AND_SYS_STAT_H)
check_include_files("string.h" HAVE_STRING_H)
check_include_files("stdlib.h" HAVE_STDLIB_H)

# Library headers
check_include_files("libxml/parser.h" HAVE_LIBXML2)
check_include_files("curl/curl.h" HAVE_CURL)
check_include_files("tirpc/rpc/rpc.h" HAVE_TIRPC)

# Functions
check_symbol_exists(pthread_create pthread.h HAVE_PTHREADS)
check_symbol_exists(strftime "time.h" HAVE_STRFTIME)
check_symbol_exists(timegm "time.h" HAVE_TIMEGM)
check_symbol_exists(mktime "time.h" HAVE_MKTIME)
check_symbol_exists(atexit "stdlib.h" HAVE_ATEXIT)

# Optional ASAN and Developer mode flags (based on CMake options)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(BUILD_DEVELOPER "Build in developer mode" OFF)

set(LIBDAP_ROOT "${CMAKE_INSTALL_PREFIX}")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

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

if(BUILD_DEVELOPER)
    target_compile_options(dap PRIVATE -g3 -O0 -Wall -Wcast-align)
else()
    target_compile_options(dap PRIVATE -O2 -DNDEBUG -Wall -Wcast-align)
endif()

if(USE_ASAN)
    target_compile_options(dap PRIVATE -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    target_link_options(dap PRIVATE -fsanitize=address -fsanitize=undefined)
endif()

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
        util.h escaping.h parser.h debug.h dods-limits.h dods-datatypes.h Type.h util_mit.h
        expr.h Clause.h RValue.h ConstraintEvaluator.h ce_parser.h DapIndent.h DapObj.h
        XDRFileMarshaller.h Marshaller.h UnMarshaller.h XDRFileUnMarshaller.h
        XDRStreamMarshaller.h XDRUtils.h xdr-datatypes.h mime_util.h cgi_util.h
        XDRStreamUnMarshaller.h Keywords2.h XMLWriter.h ServerFunctionsList.h
        ServerFunction.h media_types.h DapXmlNamespaces.h parser-util.h MarshallerThread.h)

set(DAP4_ONLY_HDR D4StreamMarshaller.h D4StreamUnMarshaller.h Int64.h UInt64.h Int8.h
        D4ParserSax2.h D4BaseTypeFactory.h D4Maps.h D4Dimensions.h D4EnumDefs.h D4Group.h
        DMR.h D4Attributes.h D4AttributeType.h D4Enum.h chunked_stream.h chunked_ostream.h
        chunked_istream.h D4Sequence.h crc.h D4Opaque.h D4AsyncUtil.h D4Function.h D4RValue.h
        D4FilterClause.h)

set(CLIENT_HDR RCReader.h Connect.h Resource.h D4Connect.h Response.h
        StdinResponse.h SignalHandlerRegisteredErr.h)

set(SERVER_HDR DODSFilter.h AlarmHandler.h EventHandler.h Ancillary.h)

install(FILES ${DAP_HDR} ${DAP4_ONLY_HDR} ${CLIENT_HDR} ${SERVER_HDR}
        DESTINATION include/dap)
