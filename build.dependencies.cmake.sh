#!/bin/bash

echo "==========================================="
echo "Building all libdap4 dependencies"
echo "==========================================="

# Exit on any error
set -e

# Make sure we're in the libdap4-3.21.1 directory
if [ ! -f "README.cmake.dependencies.md" ]; then
    echo "Error: This script must be run from the libdap4-3.21.1 directory"
    echo "Current directory: $(pwd)"
    exit 1
fi

# Make all scripts executable
echo "-- Making build scripts executable..."
chmod +x build.zlib.cmake.sh
chmod +x build.libxml2.cmake.sh
chmod +x build.curl.cmake.sh
chmod +x build.cppunit.cmake.sh
chmod +x build.libdap.cmake.sh

echo ""
echo "==========================================="
echo "Step 1/5: Building zlib"
echo "==========================================="
./build.zlib.cmake.sh

echo ""
echo "==========================================="
echo "Step 2/5: Building libxml2"
echo "==========================================="
./build.libxml2.cmake.sh

echo ""
echo "==========================================="
echo "Step 3/5: Building curl"
echo "==========================================="
./build.curl.cmake.sh

echo ""
echo "==========================================="
echo "Step 4/5: Building cppunit"
echo "==========================================="
./build.cppunit.cmake.sh

echo ""
echo "==========================================="
echo "Step 5/5: Building libdap4"
echo "==========================================="
./build.libdap.cmake.sh

echo ""
echo "==========================================="
echo "Build complete!"
echo "==========================================="
echo "All dependencies and libdap4 have been built successfully."
echo ""
echo "Installation directories:"
echo "  - zlib:     ../install/zlib-1.3.1/"
echo "  - libxml2:  ../install/libxml2-2.14.5/"
echo "  - curl:     ../install/curl-8.15.0/"
echo "  - cppunit:  ../install/cppunit-1.15.1/"
echo "  - libdap4:  ../install/libdap4-3.21.1/"
echo ""
echo "To use libdap4 in your projects, set:"
echo "  CMAKE_PREFIX_PATH=../install/libdap4-3.21.1"