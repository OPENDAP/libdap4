#!/bin/bash

# exit immediately if a command exits with a non-zero status
set -e  

echo "-- Checking zlib installation..."
find ../install/zlib-1.3.1/ -name "*.lib" -o -name "*.a" -o -name "zlib.h"

echo "-- Checking LibXml2 installation..."
find ../install/libxml2-2.14.5/ -name "*.lib" -o -name "*.a" -o -name "xmlwriter.h"

echo "-- Checking curl installation..."
find ../install/curl-8.15.0/ -name "*.lib" -o -name "*.a" -o -name "curl.h"

echo "-- Checking cppunit installation..."
find ../install/cppunit-1.15.1/ -name "*.lib" -o -name "*.a" -o -name "TestCase.h"

# build directory 
mkdir -p ../build/libdap4
pushd ../build
pushd libdap4

# set the prefix path to include all dependency installation directories
CMAKE_PREFIX_PATH="../install/cppunit-1.15.1;../install/libxml2-2.14.5;../install/curl-8.15.0;../install/zlib-1.3.1"

# detect platform and set appropriate library paths
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    echo "-- Detected Windows system, using .lib libraries"
    LIBXML2_LIB_PATH="../../install/libxml2-2.14.5/lib/libxml2sd.lib"
    ZLIB_ROOT_PATH="../../install/zlib-1.3.1"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "-- Detected macOS system, using .a libraries"
    LIBXML2_LIB_PATH="../../install/libxml2-2.14.5/lib/libxml2.a"
    ZLIB_ROOT_PATH="../../install/zlib-1.3.1"
else
    echo "-- Detected Linux/Unix system, using .a libraries"
    LIBXML2_LIB_PATH="../../install/libxml2-2.14.5/lib/libxml2.a"
    ZLIB_ROOT_PATH="../../install/zlib-1.3.1"
    LIBXML2_ABSOLUTE_PATH=$(realpath ../../install/libxml2-2.14.5)
fi

# find zlib library file
ZLIB_LIB_FILE=""
for lib_name in libz.a libzlib.a zlibstatic.lib zlibstaticd.lib; do
    if [ -f "$ZLIB_ROOT_PATH/lib/$lib_name" ]; then
        ZLIB_LIB_FILE="$ZLIB_ROOT_PATH/lib/$lib_name"
        echo "-- Found zlib library: $ZLIB_LIB_FILE"
        break
    fi
done

cmake ../../libdap4 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_INSTALL_PREFIX="../../install/libdap4" \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  -DLibXml2_ROOT="../install/libxml2-2.14.5" \
  -DLIBXML2_LIBRARY="$LIBXML2_LIB_PATH" \
  -DZLIB_ROOT="$ZLIB_ROOT_PATH" \
  -DZLIB_INCLUDE_DIR="$ZLIB_ROOT_PATH/include" \
  -DUSE_ASAN=OFF \
  -DBUILD_DEVELOPER=ON \
  -DCMAKE_VERBOSE_MAKEFILE=ON \
  --fresh 
  
echo "-- Press any key to build libdap4..."
read -n 1 -s

cmake --build . --config Debug --verbose
cmake --install . --config Debug

popd 
popd
