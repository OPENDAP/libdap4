#!/bin/bash

# check if ../ext/libxml2-2.14.5 directory exists, if not clone it
if [ ! -d "../ext/libxml2-2.14.5" ]; then
    echo "libxml2-2.14.5 not found in ../ext/, cloning from GitHub..."
    mkdir -p ../ext
    pushd ../ext
    git -c advice.detachedHead=false clone --branch v2.14.5 --depth 1 https://github.com/GNOME/libxml2.git libxml2-2.14.5
    if [ $? -ne 0 ]; then
        echo "Failed to clone libxml2 repository"
        exit 1
    fi
    popd
else
    echo "libxml2-2.14.5 already exists in ../ext/"
fi

# build libxml2
mkdir -p ../build/libxml2-2.14.5
pushd ../build
pushd libxml2-2.14.5
cmake ../../ext/libxml2-2.14.5 \
  -DCMAKE_INSTALL_PREFIX=../../install/libxml2-2.14.5 \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_BUILD_TYPE=Debug \
  -DLIBXML2_WITH_ICONV=OFF \
  -DLIBXML2_WITH_LZMA=OFF \
  -DLIBXML2_WITH_PYTHON=OFF \
  -DLIBXML2_WITH_ZLIB=ON \
  -DZLIB_ROOT=../../install/zlib-1.3.1 \
  --fresh
cmake --build . --config Debug --parallel
cmake --install . --config Debug
popd 
popd