#!/bin/bash

# check if ../ext/cppunit-1.15.1 directory exists, if not try different approaches based on platform
if [ ! -d "../ext/cppunit-1.15.1" ]; then
    echo "cppunit-1.15.1 not found in ../ext/"
    
    # Check platform - use system packages for Linux and macOS, build from source for Windows
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "Detected Linux system - checking for system cppunit package..."
        
        # Check if system has cppunit development packages
        if pkg-config --exists cppunit 2>/dev/null; then
            echo "Found system cppunit package, creating symlink..."
            mkdir -p ../ext
            mkdir -p ../ext/cppunit-1.15.1
            
            # Create a minimal structure pointing to system cppunit
            CPPUNIT_INCLUDE=$(pkg-config --variable=includedir cppunit)
            CPPUNIT_LIB=$(pkg-config --variable=libdir cppunit)
            
            echo "System cppunit found at:"
            echo "  Include: $CPPUNIT_INCLUDE"
            echo "  Lib: $CPPUNIT_LIB"
            echo "Skipping build - will use system version"
            
            # Create marker file
            touch ../ext/cppunit-1.15.1/.system_package
        else
            echo "ERROR: No system cppunit found. Please install it:"
            echo "  Ubuntu/Debian: sudo apt-get install libcppunit-dev"
            echo "  CentOS/RHEL: sudo yum install cppunit-devel"
            echo "  Fedora: sudo dnf install cppunit-devel"
            echo "Then run this script again."
            exit 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "Detected macOS system - checking for Homebrew cppunit..."
        
        # Check for Homebrew installation of cppunit
        if command -v brew >/dev/null 2>&1 && brew list cppunit >/dev/null 2>&1; then
            echo "Found Homebrew cppunit package, creating symlink..."
            mkdir -p ../ext
            mkdir -p ../ext/cppunit-1.15.1
            
            # Get Homebrew prefix and cppunit paths
            BREW_PREFIX=$(brew --prefix)
            CPPUNIT_INCLUDE="$BREW_PREFIX/include"
            CPPUNIT_LIB="$BREW_PREFIX/lib"
            
            echo "Homebrew cppunit found at:"
            echo "  Include: $CPPUNIT_INCLUDE"
            echo "  Lib: $CPPUNIT_LIB"
            echo "Skipping build - will use Homebrew version"
            
            # Create marker file
            touch ../ext/cppunit-1.15.1/.system_package
        else
            echo "ERROR: No Homebrew cppunit found. Please install it:"
            if ! command -v brew >/dev/null 2>&1; then
                echo "  First install Homebrew: https://brew.sh"
            fi
            echo "  Then install cppunit: brew install cppunit"
            echo "Then run this script again."
            exit 1
        fi
    else
        # Windows or other platforms - build from source
        echo "Detected Windows/other system - building from source..."
        mkdir -p ../ext
        pushd ../ext
        
        git -c advice.detachedHead=false clone --depth 1 https://anongit.freedesktop.org/git/libreoffice/cppunit.git cppunit-1.15.1
        if [ $? -ne 0 ]; then
            echo "Failed to clone cppunit repository"
            exit 1
        fi
        popd
    fi
else
    echo "cppunit-1.15.1 already exists in ../ext/"
fi

# Check if using system package (Linux and macOS)
if [ -f "../ext/cppunit-1.15.1/.system_package" ]; then
    echo "Using system cppunit package - no build needed"
    
    # Create install directory structure for compatibility
    mkdir -p ../install/cppunit-1.15.1/include
    mkdir -p ../install/cppunit-1.15.1/lib
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Linux: use pkg-config
        CPPUNIT_INCLUDE=$(pkg-config --variable=includedir cppunit)
        CPPUNIT_LIB=$(pkg-config --variable=libdir cppunit)
    else
        # macOS: use Homebrew paths
        BREW_PREFIX=$(brew --prefix)
        CPPUNIT_INCLUDE="$BREW_PREFIX/include"
        CPPUNIT_LIB="$BREW_PREFIX/lib"
    fi
    
    # Create symlinks to system installation
    if [ -d "$CPPUNIT_INCLUDE/cppunit" ]; then
        ln -sf "$CPPUNIT_INCLUDE/cppunit" ../install/cppunit-1.15.1/include/cppunit
    fi
    
    # Find the library file
    if [ -f "$CPPUNIT_LIB/libcppunit.so" ]; then
        ln -sf "$CPPUNIT_LIB/libcppunit.so" ../install/cppunit-1.15.1/lib/libcppunit.so
    elif [ -f "$CPPUNIT_LIB/libcppunit.a" ]; then
        ln -sf "$CPPUNIT_LIB/libcppunit.a" ../install/cppunit-1.15.1/lib/libcppunit.a
    elif [ -f "$CPPUNIT_LIB/libcppunit.dylib" ]; then
        ln -sf "$CPPUNIT_LIB/libcppunit.dylib" ../install/cppunit-1.15.1/lib/libcppunit.dylib
    fi
    
    echo "System cppunit setup complete"
    exit 0
fi

# Only proceed with CMakeLists.txt creation for Windows (not Linux/macOS)
if [[ "$OSTYPE" != "linux-gnu"* ]] && [[ "$OSTYPE" != "darwin"* ]]; then

# check if CMakeLists.txt exists in cppunit, if not create it
if [ ! -f "../ext/cppunit-1.15.1/CMakeLists.txt" ]; then
    echo "Creating CMakeLists.txt for cppunit-1.15.1..."
    cat > "../ext/cppunit-1.15.1/CMakeLists.txt" << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(cppunit VERSION 1.15.1)

# Options
option(CPPUNIT_BUILD_SHARED_LIBS "Build shared libraries" ON)
option(CPPUNIT_BUILD_TESTS "Build tests" OFF)

# Set library type
if(CPPUNIT_BUILD_SHARED_LIBS)
    set(CPPUNIT_LIBRARY_TYPE SHARED)
else()
    set(CPPUNIT_LIBRARY_TYPE STATIC)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

# Configure header - create config directory if needed
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/cppunit)

# Create config-auto.h (the missing header file)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/include/cppunit/config-auto.h
"#ifndef CPPUNIT_CONFIG_AUTO_H
#define CPPUNIT_CONFIG_AUTO_H

#include <typeinfo>

/* Define to 1 if you have the <dlfcn.h> header file. */
#define CPPUNIT_HAVE_DLFCN_H 1

/* define if library exports are declared as dll export under win32 */
#ifdef _WIN32
#define CPPUNIT_BUILD_DLL 1
#endif

/* Define to 1 if you have the `finite' function. */
#define CPPUNIT_HAVE_FINITE 1

/* define if the library defines strstream */
#define CPPUNIT_HAVE_CLASS_STRSTREAM 0

/* Define if you have the GNU dld library. */
/* #undef CPPUNIT_HAVE_DLD */

/* Define to 1 if you have the `dlerror' function. */
#define CPPUNIT_HAVE_DLERROR 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define CPPUNIT_HAVE_DLFCN_H 1

/* Define to 1 if you have the `dlopen' function. */
#define CPPUNIT_HAVE_DLOPEN 1

/* define if the library defines std::isfinite */
#define CPPUNIT_HAVE_ISFINITE 1

/* Define if you have the libdl library or equivalent. */
#define CPPUNIT_HAVE_LIBDL 1

/* Define to 1 if you have the `m' library (-lm). */
#define CPPUNIT_HAVE_LIBM 1

/* Define if you have the shl_load function. */
/* #undef CPPUNIT_HAVE_SHL_LOAD */

/* define if the library defines sstream */
#define CPPUNIT_HAVE_SSTREAM 1

/* Define to 1 if you have the <strstream> header file. */
#define CPPUNIT_HAVE_STRSTREAM 0

/* Name of package */
#define CPPUNIT_PACKAGE \"cppunit\"

/* Define to the address where bug reports for this package should be sent. */
#define CPPUNIT_PACKAGE_BUGREPORT \"\"

/* Define to the full name of this package. */
#define CPPUNIT_PACKAGE_NAME \"cppunit\"

/* Define to the full name and version of this package. */
#define CPPUNIT_PACKAGE_STRING \"cppunit 1.15.1\"

/* Define to the one symbol short name of this package. */
#define CPPUNIT_PACKAGE_TARNAME \"cppunit\"

/* Define to the version of this package. */
#define CPPUNIT_PACKAGE_VERSION \"1.15.1\"

/* Version number of package */
#define CPPUNIT_VERSION \"1.15.1\"

/* Define to 1 if you have the ANSI C header files. */
#define CPPUNIT_STDC_HEADERS 1

#endif /* CPPUNIT_CONFIG_AUTO_H */
")

# Create config.h.cmake
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.cmake
"#ifndef CPPUNIT_CONFIG_H
#define CPPUNIT_CONFIG_H

#include \"cppunit/config-auto.h\"

#define CPPUNIT_HAVE_SSTREAM 1
#define CPPUNIT_HAVE_STRSTREAM 0
#define CPPUNIT_HAVE_CLASS_STRSTREAM 0
#define CPPUNIT_HAVE_FINITE 1
#define CPPUNIT_HAVE_DLFCN_H 1

#ifdef _WIN32
#  define CPPUNIT_DLL_BUILD
#endif

#endif
")

# Configure header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/include/cppunit/config.h
)

# Source files - exclude Windows-specific files on non-Windows platforms
file(GLOB_RECURSE CPPUNIT_SOURCES
    "src/cppunit/*.cpp"
)

# Remove Windows-specific files on non-Windows platforms
if(NOT WIN32)
    list(FILTER CPPUNIT_SOURCES EXCLUDE REGEX ".*DllMain\\.cpp$")
    list(FILTER CPPUNIT_SOURCES EXCLUDE REGEX ".*Win32.*")
    list(FILTER CPPUNIT_SOURCES EXCLUDE REGEX ".*windows.*")
endif()

# Create the library - use 'cppunit' name (lowercase) to match FindCppUnit expectations
add_library(cppunit ${CPPUNIT_LIBRARY_TYPE} ${CPPUNIT_SOURCES})

# Set compiler flags to handle compatibility issues
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(cppunit PRIVATE 
        -fpermissive 
        -Wno-error
        -Wno-deprecated-declarations
        -std=c++14
        -DCPPUNIT_NO_TESTPLUGIN
        -ftemplate-backtrace-limit=0
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_options(cppunit PRIVATE 
        -Wno-error
        -Wno-deprecated-declarations
        -std=c++14
        -Wno-unqualified-std-cast-call
        -Wno-deprecated-builtins
        -D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES=1
    )
endif()

# Add preprocessor definitions to help with compatibility
target_compile_definitions(cppunit PRIVATE
    CPPUNIT_NO_TESTPLUGIN=1
    _GLIBCXX_USE_CXX11_ABI=0
    _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES=1
)

# Set C++ standard to C++14 which has better compatibility
set_target_properties(cppunit PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "cppunit"
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories for the target
target_include_directories(cppunit
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Install targets
install(TARGETS cppunit
    EXPORT cppunitTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY include/cppunit
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Install generated config headers
install(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/include/cppunit/config.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/cppunit/config-auto.h
    DESTINATION include/cppunit
)

# Create a traditional FindCppUnit.cmake compatible setup
# Install a cppunit-config.cmake file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cppunit-config.cmake
"# CppUnit Config File
get_filename_component(CPPUNIT_CMAKE_DIR \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)
set(CPPUNIT_INCLUDE_DIRS \"\${CPPUNIT_CMAKE_DIR}/../../include\")
set(CPPUNIT_INCLUDE_DIR \"\${CPPUNIT_INCLUDE_DIRS}\")
set(CPPUNIT_LIBRARIES cppunit)
set(CPPUNIT_LIBRARY cppunit)
set(CPPUNIT_FOUND TRUE)

if(NOT TARGET cppunit)
    include(\"\${CPPUNIT_CMAKE_DIR}/cppunitTargets.cmake\")
endif()
")

# Export targets
install(EXPORT cppunitTargets
    FILE cppunitTargets.cmake
    NAMESPACE cppunit::
    DESTINATION lib/cmake/cppunit
)

# Install config file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cppunit-config.cmake
    DESTINATION lib/cmake/cppunit
)

# Also install pkg-config file for traditional builds
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cppunit.pc
"prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: cppunit
Description: C++ unit testing framework
Version: ${PROJECT_VERSION}
Libs: -L\${libdir} -lcppunit
Cflags: -I\${includedir}
")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cppunit.pc
    DESTINATION lib/pkgconfig
)
EOF
else
    echo "CMakeLists.txt already exists in cppunit-1.15.1"
fi

fi  # End of Windows platforms check

# build cppunit (only for Windows, not Linux/macOS)
if [[ "$OSTYPE" != "linux-gnu"* ]] && [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Building cppunit from source..."
    mkdir -p ../build/cppunit-1.15.1
    pushd ../build
    pushd cppunit-1.15.1
    cmake ../../ext/cppunit-1.15.1 \
      -DCMAKE_INSTALL_PREFIX=../../install/cppunit-1.15.1 \
      -DCPPUNIT_BUILD_SHARED_LIBS=OFF \
      --fresh
    cmake --build . --config Debug --parallel
    cmake --install . --config Debug
    popd 
    popd
else
    echo "Skipping cppunit build - using system package"
fi