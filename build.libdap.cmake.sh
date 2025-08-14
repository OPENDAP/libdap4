#!/bin/bash

echo "-- Checking dependency installations..."

echo "-- Checking zlib installation..."
find ../install/zlib-1.3.1/ -name "*.lib" -o -name "*.a" -o -name "zlib.h"

echo "-- Checking LibXml2 installation..."
find ../install/libxml2-2.14.5/ -name "*.lib" -o -name "*.a" -o -name "xmlwriter.h"

echo "-- Checking curl installation..."
find ../install/curl-8.15.0/ -name "*.lib" -o -name "*.a" -o -name "curl.h"

echo "-- Checking cppunit installation..."
find ../install/cppunit-1.15.1/ -name "*.lib" -o -name "*.a" -o -name "TestCase.h"

# build directory 
rm -rf ../build/libdap4
mkdir -p ../build/libdap4
pushd ../build
pushd libdap4

# set the prefix path to include all dependency installation directories
CMAKE_PREFIX_PATH="../install/cppunit-1.15.1;../install/libxml2-2.14.5;../install/curl-8.15.0;../install/zlib-1.3.1"

# detect platform and set appropriate library paths
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    # Windows
    echo "-- Detected Windows system, using .lib libraries"
    LIBXML2_LIB_PATH="../install/libxml2-2.14.5/lib/libxml2sd.lib"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    echo "-- Detected macOS system, using .a libraries"
    LIBXML2_LIB_PATH="../install/libxml2-2.14.5/lib/libxml2.a"
else
    # Linux and other Unix-like systems
    echo "-- Detected Linux/Unix system, using .a libraries"
    LIBXML2_LIB_PATH="../install/libxml2-2.14.5/lib/libxml2.a"
fi

cmake ../../libdap4 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX="../../install/libdap4" \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  -DLibXml2_ROOT="../install/libxml2-2.14.5" \
  -DLIBXML2_LIBRARY="$LIBXML2_LIB_PATH" \
  -DUSE_ASAN=ON \
  -DBUILD_DEVELOPER=ON \
  --fresh
  
echo "Press any key to continue..."
read -n 1 -s

cmake --build . --config Debug --verbose

echo "Press any key to continue..."
read -n 1 -s

cmake --install . --config Debug
popd 
popd