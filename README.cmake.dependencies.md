# Build Guide for libdap dependencies 

This guide provides a complete build system for libdap4 and all its dependencies from source code. 
It includes automated scripts and detailed instructions for building on multiple platforms, with specific support for Windows.

## Quick Start

The build process involves compiling these dependencies in order:
1. zlib
2. libxml2  
3. curl
4. cppunit
5. libdap4

Each dependency can be built using the provided CMake scripts or the automated build scripts included in this repository.

## Directory Structure

```
parent-directory/
├── ext/                         # dependencies sources
│   ├── cppunit-1.15.1/
│   ├── libxml2-2.14.5/
│   ├── curl-8.15.0/
│   └── zlib-1.3.1/
├── build/
│   ├── cppunit-1.15.1/
│   ├── libxml2-2.14.5/
│   ├── curl-8.15.0/
│   └── zlib-1.3.1/
│   └── libdap4/                 # libdap build directory
├── install/                     # all built dependencies
│   ├── cppunit-1.15.1/
│   ├── libxml2-2.14.5/
│   ├── curl-8.15.0/
│   ├── zlib-1.3.1/
│   └── libdap4/                 # final libdap installation
└── libdap4/                     # libdap source code
```

## Windows specific pre-build dependencies

### Bison and Flex

WinFlexBison - A Windows port specifically designed for Visual Studio:

Download from: https://github.com/lexxmark/winflexbison

### OpenSSL

Official OpenSSL Binaries:

Download from: https://slproweb.com/products/Win32OpenSSL.html

Choose "Win64 OpenSSL v3.x.x"

## 1. Build zlib 

```bash
git -c advice.detachedHead=false clone --branch v1.3.1 --depth 1 https://github.com/madler/zlib.git zlib-1.3.1
```

Build with 

```bash
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
```

or use supplied script 

```bash
build.zlib.cmake.sh
```

Final directory structure is

```
../ext/zlib-1.3.1/ (source)
../build/zlib-1.3.1/ (build)
../install/zlib-1.3.1/ (install)
```

## 2. Build libxml2

```bash
git -c advice.detachedHead=false clone --branch v2.14.5 --depth 1 https://github.com/GNOME/libxml2.git libxml2-2.14.5
```

Build with 

```bash
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
```

or use supplied script 

```bash
build.libxml2.cmake.sh
```

## 3. Build curl

```bash
git -c advice.detachedHead=false clone --branch curl-8_15_0 --depth 1 https://github.com/curl/curl.git curl-8.15.0
```

Build with 

```bash
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
```

or use supplied script 

```bash
build.curl.cmake.sh
```

## 4. Build CppUnit

```bash
git -c advice.detachedHead=false clone --branch 1.15.1 --depth 1 https://anongit.freedesktop.org/git/libreoffice/cppunit.git cppunit-1.15.1
```

**Important Note: Unfortunately, CMake is not part of original cppunit distribution. So, we create one using the supplied bash shell script**

```bash
build.cppunit.cmake.sh
```

Build with 

```bash
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
```

## 5. Build libdap4

Finnally (!), build libdap4with 

```bash
make ../../libdap4 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX="../../install/libdap4" \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  -DLibXml2_ROOT="../../install/libxml2-2.14.5" \
  -DLIBXML2_LIBRARY="$LIBXML2_LIB_PATH" \
  -DUSE_ASAN=ON \
  -DBUILD_DEVELOPER=ON \
  --fresh
```

Note: $LIBXML2_LIB_PATH is system dependent, see supplied script 

```bash
build.libdap.cmake.sh
```



