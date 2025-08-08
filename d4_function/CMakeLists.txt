cmake_minimum_required(VERSION 3.10)

# Require Flex and Bison for parsing grammars
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Options inherited from top-level CMakeLists
#option(BUILD_DEVELOPER "Developer mode" OFF)
#option(USE_ASAN "Enable Address Sanitizer" OFF)

# Set standard explicitly if required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Basic compilation flags
set(D4_FUNCTION_CXXFLAGS -Wall -Wcast-align -Wno-inconsistent-missing-override)

if (BUILD_DEVELOPER)
    list(APPEND D4_FUNCTION_CXXFLAGS -g3 -O0)
    if (USE_ASAN)
        list(APPEND D4_FUNCTION_CXXFLAGS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    endif()
else()
    list(APPEND D4_FUNCTION_CXXFLAGS -g -O2)
endif()

# Generate parser files using Bison
BISON_TARGET(d4_function_parser
        d4_function_parser.yy
        ${CMAKE_CURRENT_BINARY_DIR}/d4_function_parser.tab.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/d4_function_parser.tab.hh)

# Generate scanner files using Flex
FLEX_TARGET(d4_function_scanner
        d4_function_scanner.ll
        ${CMAKE_CURRENT_BINARY_DIR}/lex.d4_function.cc)

ADD_FLEX_BISON_DEPENDENCY(d4_function_scanner d4_function_parser)

# Group generated sources
set(GENERATED_FUNCTION_SRC
        ${BISON_d4_function_parser_OUTPUT_SOURCE}
        ${BISON_d4_function_parser_OUTPUT_HEADER}
        ${FLEX_d4_function_scanner_OUTPUTS}
)

# Library sources including manually written files
set(D4_FUNCTION_LIB_SOURCES
        ${GENERATED_FUNCTION_SRC}
        D4FunctionEvaluator.cc
        D4FunctionEvaluator.h
        D4FunctionScanner.h
)

# Define a static library
add_library(d4_function_parser OBJECT ${D4_FUNCTION_LIB_SOURCES})
set_target_properties(d4_function_parser PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Include directories (including generated files)
target_include_directories(d4_function_parser
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/d4_ce
)

target_include_directories(d4_function_parser PRIVATE ${LIBXML2_INCLUDE_DIR})

if(TIRPC_FOUND)
    target_include_directories(d4_function_parser PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(d4_function_parser PRIVATE ${TIRPC_LIBRARIES})
endif()

# Set compiler flags
target_compile_options(d4_function_parser PRIVATE ${D4_FUNCTION_CXXFLAGS})

# Mark generated files explicitly
set_source_files_properties(${GENERATED_FUNCTION_SRC}
        PROPERTIES GENERATED TRUE)

# Clean-up generated files
set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES
        "${GENERATED_FUNCTION_SRC};stack.hh;location.hh;position.hh"
)

# Install public headers if required
install(FILES D4FunctionEvaluator.h
        DESTINATION include/libdap)
