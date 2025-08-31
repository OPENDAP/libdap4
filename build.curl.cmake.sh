#!/bin/bash

# check if ../ext/curl-8.15.0 directory exists, if not clone it
if [ ! -d "../ext/curl-8.15.0" ]; then
    echo "curl-8.15.0 not found in ../ext/, cloning from GitHub..."
    mkdir -p ../ext
    pushd ../ext
    git -c advice.detachedHead=false clone --branch curl-8_15_0 --depth 1 https://github.com/curl/curl.git curl-8.15.0
    if [ $? -ne 0 ]; then
        echo "Failed to clone curl repository"
        exit 1
    fi
    popd
else
    echo "curl-8.15.0 already exists in ../ext/"
fi

# build curl
mkdir -p ../build/curl-8.15.0
pushd ../build
pushd curl-8.15.0
cmake ../../ext/curl-8.15.0 \
  -DCMAKE_INSTALL_PREFIX=../../install/curl-8.15.0 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCURL_USE_LIBPSL=OFF \
  -DCURL_USE_OPENSSL=ON \
  -DZLIB_ROOT=../../install/zlib-1.3.1 \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_CURL_EXE=OFF \
  -DBUILD_TESTING=OFF \
  --fresh
cmake --build . --config Debug --parallel
cmake --install . --config Debug
popd 
popd