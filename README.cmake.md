# Using cmake to build libdap4

## Using presets

Cmake presets provide a way to supply a number of otherwise tedious-to-type
command line parameters in one go. Here's a quick summary of building, testing
and install libdap4 if you are a developer and have the environment variable
`$prefix` set to the root directory where libdap shou;ld be installed. If the
`include`, `lib`, etc., directories don't already exist there, they will be
created

### Configure libdap4

From the top-level directory in the repo

```bash
cmake --preset developer
```

This will configure the code and put all the generated files in `build/developer`.

### Build the code

To build the code, putting all the generated files in `build/developer`, use

```bash
cmake --build --preset developer --parallel 20
```

The `--parallel 20` is optional; set 20 to whatever is reasonable for your
computer

### Test the code

```bash
ctest --preset developer --parallel 20
```

As before `--parallel ...` is optional.

Note that the tests are run in `build/developer` so if you want to run one by
hand, cd there and do that

### Install the code

For some odd reason, the `--install` option does not use the `--preset` option

```bash
ccmake --install build/developer
```

### What presets are available

```bash
cmake --list-presets
```

Look in CMakePresets.json

## how to build outside the source tree, not using presets

Make a build directory (or build/libdap4 if libdap4 is a git submodule).
Then, in that directory, run cmake with the source directory as a command line
argument.

For the 'normal' case, where 'build' is made inside the libdap4 top-level
source directory:

```bash
mkdir build/
cd build
cmake ../
```

If libdap4 is a submodule...

```bash
mkdir build/libdap4
cd build/libdap4
cmake ../../
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
