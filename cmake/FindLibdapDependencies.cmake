# Load standard check modules
include(CheckIncludeFiles)
include(CheckSymbolExists)

# --- Basic system headers ---
check_include_files("unistd.h" HAVE_UNISTD_H)
check_include_files("sys/time.h" HAVE_TM_IN_SYS_TIME)
check_include_files("pthread.h" HAVE_PTHREAD_H)
check_include_files("uuid/uuid.h" HAVE_UUID_UUID_H)
check_include_files("uuid.h" HAVE_UUID_H)
check_include_files("sys/stat.h" HAVE_SYS_STAT_H)
check_include_files("sys/types.h;sys/stat.h" HAVE_SYS_TYPES_H_AND_SYS_STAT_H)
check_include_files("string.h" HAVE_STRING_H)
check_include_files("stdlib.h" HAVE_STDLIB_H)

# --- Library headers (set CMAKE_REQUIRED_INCLUDES as needed) ---

# libxml2
if(LibXml2_FOUND)
    set(CMAKE_REQUIRED_INCLUDES ${LIBXML2_INCLUDE_DIR})
    check_include_files("libxml/xmlwriter.h" HAVE_LIBXML2)
    unset(CMAKE_REQUIRED_INCLUDES)
endif()

# curl
if(CURL_FOUND)
    set(CMAKE_REQUIRED_INCLUDES ${CURL_INCLUDE_DIRS})
    check_include_files("curl/curl.h" HAVE_CURL)
    unset(CMAKE_REQUIRED_INCLUDES)
endif()

# tirpc
if(TIRPC_INCLUDE_DIR)
    set(CMAKE_REQUIRED_INCLUDES ${TIRPC_INCLUDE_DIR})
    check_include_files("rpc/xdr.h" HAVE_TIRPC)
    unset(CMAKE_REQUIRED_INCLUDES)
endif()

# --- Function tests ---
check_symbol_exists(pthread_create pthread.h HAVE_PTHREADS)
check_symbol_exists(strftime "time.h" HAVE_STRFTIME)
check_symbol_exists(timegm "time.h" HAVE_TIMEGM)
check_symbol_exists(mktime "time.h" HAVE_MKTIME)
check_symbol_exists(atexit "stdlib.h" HAVE_ATEXIT)

# --- Platform-specific logic ---
if(APPLE)
    set(LIBDAP_PLATFORM "macOS")
    set(UUID_LIB "")  # libSystem handles uuid
elseif(UNIX)
    set(LIBDAP_PLATFORM "Linux/Unix")
    find_library(UUID_LIB uuid)
    if(UUID_LIB)
        message(STATUS "Found UUID library: ${UUID_LIB}")
    else()
        message(FATAL_ERROR "Required library 'uuid' not found on this platform")
    endif()
else()
    set(LIBDAP_PLATFORM "Unknown")
endif()
