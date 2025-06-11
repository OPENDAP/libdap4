cmake_minimum_required(VERSION 3.10)

# Options consistent with top-level configuration
option(BUILD_DEVELOPER "Developer mode" OFF)
option(USE_ASAN "Enable Address Sanitizer" OFF)

# Basic CXX compilation settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Base CXX flags (assuming COMPILER_IS_GCC)
set(HTTP_DAP_CXXFLAGS -Wall -Wcast-align)

if (BUILD_DEVELOPER)
    list(APPEND HTTP_DAP_CXXFLAGS -g3 -O0)
    if (USE_ASAN)
        list(APPEND HTTP_DAP_CXXFLAGS -fsanitize=address -fno-omit-frame-pointer)
    endif()
else()
    list(APPEND HTTP_DAP_CXXFLAGS -g -O2)
endif()

# Define sources explicitly
set(HTTP_DAP_SOURCES
        HTTPCache.cc
        HTTPCache.h
        HTTPCacheDisconnectedMode.h
        HTTPCacheResponse.h
        HTTPCacheTable.cc
        HTTPCacheTable.h
        HTTPConnect.cc
        HTTPConnect.h
        HTTPResponse.h
        ResponseTooBigErr.cc
        ResponseTooBigErr.h
)

# Create static library (noinst_LTLIBRARIES implies not installed directly)
add_library(http_dap STATIC ${HTTP_DAP_SOURCES})

# Include directories (matching AM_CPPFLAGS)
target_include_directories(http_dap
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/GNU
)

# Set compilation flags
target_compile_options(http_dap PRIVATE ${HTTP_DAP_CXXFLAGS})

# Install public headers explicitly (matching pkginclude_HEADERS)
install(FILES
        HTTPConnect.h
        HTTPCache.h
        HTTPCacheDisconnectedMode.h
        DESTINATION include/libdap
)

# Custom cleanup for coverage files, etc.
set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "*.gcda;*.gcno;*.gcov"
)
