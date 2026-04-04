# CMake Autotest Integration Plan

## Goal

Support CMake builds that run the existing autotools/autotest-based integration
tests while keeping the current CMake CppUnit tests intact.

## Scope

- Keep the existing CMake CppUnit tests.
- Modify the CMake test setup so it runs the autotest suites:
  - `DASTest`
  - `DDSTest`
  - `EXPRTest`
  - `DMRTest`
  - `getdapTest`
- Add a minimal `tests/configure.ac` used only to configure the autotools test
  machinery.

## Plan

1. Audit the current CMake integration test modules and identify which
   generated/autotest inputs they duplicate from `tests/Makefile.am`
   (`DASTest.at`, `DDSTest.at`, `EXPRTest.at`, `DMRTest.at`, `getdapTest.at`,
   `atlocal.in`, `package.m4`).
2. Add a minimal `tests/configure.ac` dedicated to the autotest suites, with
   only the package metadata and substitutions needed to generate `atconfig`,
   `atlocal`, and `package.m4` for the test scripts.
3. Add CMake support in `tests/CMakeLists.txt` to find autotools tools
   (`autoconf`, `autom4te`, and `autoheader` if needed), configure the new
   test-only `configure.ac`, and generate the autotest driver scripts in the
   CMake build tree from the existing `*.at` files.
4. Replace the current CMake per-test includes
   (`das-tests.cmake`, `dds-tests.cmake`, `expr-tests.cmake`,
   `dmr-tests.cmake`, `getdap-tests.cmake`) with a thinner registration layer
   that adds one CTest test per autotest suite and invokes `DASTest`,
   `DDSTest`, `EXPRTest`, `DMRTest`, and `getdapTest` with `TESTSUITEFLAGS`.
5. Make the generated autotest environment point at the CMake-built executables
   (`das-test`, `dds-test`, `expr-test`, `dmr-test`, `getdap`) so the existing
   autotest cases run unchanged against the CMake artifacts.
6. Preserve existing CMake CppUnit coverage and labels, and add sensible
   dependencies plus serial/parallel behavior for the autotest suite wrappers
   so `ctest` and the top-level `integration-test` and `check` targets
   continue to work.
7. Validate with a focused CMake configure/build plus `ctest` over the
   integration label, then note any suites that still depend on autotools-era
   assumptions such as generated `package.m4`, working directory layout, or
   network access in `getdapTest`.

## Main Risks

- The autotest scripts assume autotools-generated files like `atconfig`,
  `atlocal`, and `package.m4`.
- Some suites may assume a specific working directory layout under `tests/`.
- `getdapTest` depends on network access and should stay clearly labeled as an
  integration test.
