#!/bin/bash

# check if ../ext/zlib-1.3.1 directory exists, if not clone it
if [ ! -d "../ext/zlib-1.3.1" ]; then
    echo "zlib-1.3.1 not found in ../ext/, cloning from GitHub..."
    mkdir -p ../ext
    pushd ../ext
    git -c advice.detachedHead=false clone --branch v1.3.1 --depth 1 https://github.com/madler/zlib.git zlib-1.3.1
    if [ $? -ne 0 ]; then
        echo "Failed to clone zlib repository"
        exit 1
    fi
    popd
else
    echo "zlib-1.3.1 already exists in ../ext/"
fi

# build zlib
mkdir -p ../build/zlib-1.3.1
pushd ../build
pushd zlib-1.3.1
cmake ../../ext/zlib-1.3.1 \
  -DCMAKE_INSTALL_PREFIX=../../install/zlib-1.3.1 \
  -DBUILD_SHARED_LIBS=OFF 
cmake --build . --config Debug --parallel
cmake --install . --config Debug
popd 
popd