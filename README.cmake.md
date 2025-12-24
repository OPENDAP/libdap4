# Using cmake to build libdap4

## how to build outside the source tree

Make a build directory (or build/libdap4 if libdap4 is a git submodule).
Then, in that directory, run cmake with the source directory as a command line
argument.

For the submodule case, from inside the repo's top-level, where libdap is in
a directory named 'external':

```bash
mkdir build/libdap4
cmake ../../external/libdap4
```

For the 'normal' case, where 'build' is made inside the libdap4 top-level
source directory:

```bash
mkdir build/
cd build
cmake ../
```

## Useful cmake command line options

NB: Include these on the command line using -D<option>=<value>

- **CMAKE_VERBOSE_MAKEFILE=ON**: Build Makefiles that enable debugging missing/mangled
  command line switches that are (or are not) passed to compilers, et cetera.
- **CMAKE_INSTALL_PREFIX=\$(pwd)/install**: Install in \$(pwd)/install. You can use
  any writable directory.

Special switches for libdap4

- **USE_ASAN=ON**: Turn on the compiler/linker address sanitizer
- **BUILD_DEVELOPER=ON**: Turn on developer mode.

## Build times

With most of the unit tests added, build and test times were good.

### Serial build time on an Apple M4 Pro

```bash
make  55.14s user 9.42s system 99% cpu 1:04.91 total
```

Time to run the unit tests:

```bash
100% tests passed, 0 tests failed out of 54

Total Test time (real) =   9.18 sec
make test  2.01s user 0.29s system 24% cpu 9.247 total
```

### Parallel build time:

```bash
make -j20  71.36s user 13.38s system 1001% cpu 8.461 total
```

Time to run the unit tests:

```bash
100% tests passed, 0 tests failed out of 54

Total Test time (real) =   2.35 sec
make test -j20  2.20s user 0.14s system 98% cpu 2.381 total
```

## Using Cmake Presets

There are several Presets defined that combine several cmake switches
in one setting. For example

```bash
cmake --preset developer
```

uses the following options to configure the build:

```
Preset CMake variables:

  BIG_ARRAY_TEST="OFF"
  BUILD_DEVELOPER="ON"
  BUILD_TESTING="ON"
  CMAKE_BUILD_TYPE="Debug"
  CMAKE_CXX_STANDARD="14"
  CMAKE_CXX_STANDARD_REQUIRED="ON"
  CMAKE_INSTALL_PREFIX="$prefix"
  USE_ASAN="OFF"
  USE_CPP_11_REGEX="ON"
```

Note that using the preset makes the ```build``` directory and will configure
the build to use a directory named for the preset under the ```build``` directory.
For the _developer_ preset, that will be ```build/developer```:

```commandline
hyrax_git/libdap4 % ls build/developer 
cmake_install.cmake		d4_ce/				http_dap/			Testing/
CMakeCache.txt			d4_function/		libdap.pc			tests/
CMakeFiles/			    dap-config			libdap4Config.cmake	unit-tests/
config.h			    DartConfiguration.tcl libdap4ConfigVersion.cmake	xdr-datatypes.h
CTestTestfile.cmake		dods-datatypes.h	Makefile
```

You can run the build using cmake from the top -level of the repo like this:

```bash
cmake --build . --preset developer --parallel
```

and run the tests like this:

```bash
ctest --preset developer
```

The ```ctest``` program does that ```--parallel``` but don't use that with libdap
until the test tolls are made thread safe. If you do use ```--parallel``` by mistake
then clean the dirs and re-run the tests. Until the tests are more solid, it might 
be best to just ```rm -rf build``` and start over. There are special targets to
clean out the temp files made by the tests, that target is called _clean-tests_.

To use ```cmake``` to run a custom target, use this syntax:

```bash
cmake --build . --preset developer --parallel --target clean-tests
```

## You can still use _make_

To use ```make```, just cd to ```build/developer``` and run the usual commands to 
build an install the code. Use the target _test_ to run the tests.
