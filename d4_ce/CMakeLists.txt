cmake_minimum_required(VERSION 3.10)

# Flex and Bison are required for parsing grammar
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Define options similarly as your top-level (inherited)
#option(BUILD_DEVELOPER "Developer mode" OFF)
#option(USE_ASAN "Enable Address Sanitizer" OFF)

# Set compiler flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Basic CXX flags
set(D4_CE_CXXFLAGS -Wall -Wcast-align -Wno-inconsistent-missing-override)

if (BUILD_DEVELOPER)
    list(APPEND D4_CE_CXXFLAGS -g3 -O0)
    if (USE_ASAN)
        list(APPEND D4_CE_CXXFLAGS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    endif()
else()
    list(APPEND D4_CE_CXXFLAGS -g -O2)
endif()

# Parser generation with Bison
BISON_TARGET(d4_ce_parser
        d4_ce_parser.yy
        ${CMAKE_CURRENT_BINARY_DIR}/d4_ce_parser.tab.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/d4_ce_parser.tab.hh)

# Scanner generation with Flex
FLEX_TARGET(d4_ce_scanner
        d4_ce_scanner.ll
        ${CMAKE_CURRENT_BINARY_DIR}/lex.d4_ce.cc)

ADD_FLEX_BISON_DEPENDENCY(d4_ce_scanner d4_ce_parser)

# Generated files explicitly
set(GENERATED_PARSER_SRC
        ${BISON_d4_ce_parser_OUTPUT_SOURCE}
        ${BISON_d4_ce_parser_OUTPUT_HEADER}
        ${FLEX_d4_ce_scanner_OUTPUTS}
)

# Library sources
set(D4_CE_LIB_SOURCES
        ${GENERATED_PARSER_SRC}
        D4ConstraintEvaluator.cc
        D4ConstraintEvaluator.h
        D4CEScanner.h
)

# Define library (static, as 'noinst_LTLIBRARIES' implies not installed)
add_library(d4_ce_parser OBJECT ${D4_CE_LIB_SOURCES})
set_target_properties(d4_ce_parser PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Include generated headers
target_include_directories(d4_ce_parser
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

target_include_directories(d4_ce_parser PRIVATE ${LIBXML2_INCLUDE_DIR})

if(TIRPC_FOUND)
    target_include_directories(d4_ce_parser PRIVATE ${TIRPC_INCLUDE_DIRS})
    target_link_libraries(d4_ce_parser PRIVATE ${TIRPC_LIBRARIES})
endif()

# Apply compiler flags
target_compile_options(d4_ce_parser PRIVATE ${D4_CE_CXXFLAGS})

# Ensure grammar files build before other sources
set_source_files_properties(${GENERATED_PARSER_SRC}
        PROPERTIES GENERATED TRUE)

# Custom cleanup
set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES
        "${GENERATED_PARSER_SRC};stack.hh;location.hh;position.hh"
)

# Optionally install public headers if required (as in original automake)
install(FILES D4ConstraintEvaluator.h
        DESTINATION include/libdap)
