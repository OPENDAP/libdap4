
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

For the 'normal' case, where 'build' is make inside the libdap4 top-level
source directory:
```bash
mkdir build/
cmake ../
```

## Useful cmake command line options

NB: Include these on the command line using -D<option>=<value>

* **CMAKE_VERBOSE_MAKEFILE=ON**: Build Makefiles that enable debugging missing/mangled
command line switches that are (or are not) passed to compilers, et cetera.
* **CMAKE_INSTALL_PREFIX=$(pwd)/install**: Install in $(pwd)/install. You can use
any writable directory.

Special switches for libdap4

* **USE_ASAN=ON**: Turn on the compiler/linker address sanitizer
* **BUILD_DEVELOPER=ON**: Turn on developer mode.

