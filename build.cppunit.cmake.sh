#!/bin/bash
# check if ../ext/cppunit-1.15.1 directory exists, if not clone it
if [ ! -d "../ext/cppunit-1.15.1" ]; then
    echo "cppunit-1.15.1 not found in ../ext/, cloning from GitHub..."
    mkdir -p ../ext
    pushd ../ext
    git clone https://github.com/pedro-vicente/cppunit cppunit-1.15.1
    popd
else
    echo "cppunit-1.15.1 already exists in ../ext/"
fi

# build cppunit
mkdir -p ../build/cppunit-1.15.1
pushd ../build
pushd cppunit-1.15.1
cmake ../../ext/cppunit-1.15.1 \
  -DCMAKE_INSTALL_PREFIX=../../install/cppunit-1.15.1 \
  --fresh
cmake --build . --config Debug --parallel
cmake --install . --config Debug
popd
popd
