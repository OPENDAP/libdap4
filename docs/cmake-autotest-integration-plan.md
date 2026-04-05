# CMake Autotest Integration Plan

Note: running the tests this was save ~1,000 lines of cmake code and requires
no change to the existing autotest tests. jhrg 4/4/26

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
7. Fold the integration-test build artifacts into the default CMake `ALL`
   target so `cmake --build --preset <preset>` builds both the unit-test and
   integration-test executables plus the generated autotest suite drivers,
   without changing when tests are executed.
8. Remove the now-unused hand-maintained CMake integration modules and any
   supporting `tests/CMakeLists.txt` logic that existed only for that older
   path.
9. Validate with a focused CMake configure/build plus `ctest` over the
   integration label, then note any suites that still depend on autotools-era
   assumptions such as generated `package.m4`, working directory layout, or
   network access in `getdapTest`.

## Step 1 Audit

The current CMake integration layer in `tests/CMakeLists.txt` and
`tests/cmake/*.cmake` does not generate or invoke the autotest suites from
`tests/Makefile.am`. Instead, it re-expresses parts of those suites directly as
CTest registrations and shell snippets.

| Autotest input from `tests/Makefile.am` | Current CMake duplication | Notes |
| --- | --- | --- |
| `DASTest.at` | `tests/cmake/das-tests.cmake` | Partial duplication. CMake discovers `das-testsuite/*.das` with `file(GLOB ...)`, runs `das-test -p`, diffs against `*.base`, and stages baselines. This overlaps the `_AT_DAS_TEST` and `AT_DAS_RESPONSE_TEST` logic, but CMake derives the case list from the filesystem instead of the explicit autotest manifest. |
| `DDSTest.at` | `tests/cmake/dds-tests.cmake` | Partial duplication. CMake discovers `dds-testsuite/*.dds`, runs `dds-test -p`, diffs against `*.base`, and stages baselines. This overlaps `_AT_DDS_TEST` and `AT_DDS_RESPONSE_TEST`, including the `xpass` cases for `test.24.dds`, `test.25.dds`, and `test.26.dds`, but not the autotest `--baselines` option. |
| `EXPRTest.at` | `tests/cmake/expr-tests.cmake` | Substantial manual duplication. The CMake file enumerates the same expression and error cases one by one, recreates the `-w`/`-W` and `-bw`/`-bW` pairings, and duplicates the stdout/stderr combination behavior that the autotest macros implement. |
| `DMRTest.at` | `tests/cmake/dmr-tests.cmake` | Substantial manual duplication. CMake reimplements large parts of the autotest macros with custom functions for parse, intern, translation, and CE/function cases, including byte-order-aware baseline selection and checksum filtering for universal baselines. |
| `getdapTest.at` | `tests/cmake/getdap-tests.cmake` | Direct duplication for the single current case. CMake registers the same `getdap -d http://test.opendap.org/dap/data/nc/fnoc1.nc` check and baseline diff, plus a CMake-only baseline staging target. |
| `atlocal.in` | None | Not duplicated today. The autotest-only `WORD_ORDER=@ac_word_order@` substitution is still provided only by autotools. CMake computes byte order independently inside `tests/cmake/dmr-tests.cmake` instead of generating `atlocal`. |
| `package.m4` | None | Not duplicated today. `tests/Makefile.am` generates this from top-level package metadata, but the current CMake test flow never generates or consumes it because it does not invoke `autom4te` on the `*.at` files. |

### Audit summary

- `das-tests.cmake`, `dds-tests.cmake`, and `getdap-tests.cmake` are thin
  CTest rewrites of their corresponding autotest suites.
- `expr-tests.cmake` and `dmr-tests.cmake` are larger hand-maintained ports of
  the autotest case matrices and macro behavior.
- The current CMake path does not use the autotest support files from
  `tests/Makefile.am`; there is no CMake-generated `atconfig`, `atlocal`, or
  `package.m4`.
- Step 2 should therefore add only the minimum autotools metadata needed to
  generate those support files and reuse the existing `*.at` sources instead of
  extending the current hand-ported CMake test definitions.

## Main Risks

- The autotest scripts assume autotools-generated files like `atconfig`,
  `atlocal`, and `package.m4`.
- Some suites may assume a specific working directory layout under `tests/`.
- `getdapTest` depends on network access and should stay clearly labeled as an
  integration test.

## Implementation Status

Steps 2 through 9 are now implemented in the CMake build:

- `tests/configure.ac` provides a minimal autotest-only configure input that
  computes `ac_word_order` and configures `atlocal` plus `package.m4`.
- `tests/CMakeLists.txt` now finds `autoconf` and `autom4te`, generates the
  autotest support files in the CMake test build tree, and builds the
  `DASTest`, `DDSTest`, `EXPRTest`, `DMRTest`, and `getdapTest` drivers from
  the existing `*.at` sources.
- The `tests` custom target is now part of the default `ALL` target, so a
  plain `cmake --build --preset <preset>` also builds the integration-test
  executables and generated autotest suite drivers.
- The old hand-maintained per-case CMake integration modules have been removed,
  along with the unused `tests/CMakeLists.txt` setup that supported that older
  path; the CMake path now registers one CTest test per autotest suite.
- The suite wrappers are labeled under `integration` and run serially at the
  CTest layer. Any desired intra-suite parallelism should be passed through
  `TESTSUITEFLAGS`, matching the autotools `make check` model.
- The top-level CMake `integration-test` and `check` targets continue to work
  through the existing `tests` target dependency chain, while CMake CppUnit
  coverage and unit-test labels remain unchanged.

## Step 9 Validation

Validation was run with the repository-style environment:

```sh
prefix=/Users/jimg/src/opendap/hyrax/build
PATH=$prefix/bin:$prefix/deps/bin:$PATH
```

Focused CMake validation used:

```sh
cmake -S . -B /tmp/libdap4-cmake-autotest-build-prefix \
  -DCMAKE_INSTALL_PREFIX=$prefix -DBUILD_DEVELOPER=ON
cmake --build /tmp/libdap4-cmake-autotest-build-prefix -j2
ctest --test-dir /tmp/libdap4-cmake-autotest-build-prefix \
  --output-on-failure -L integration
```

During the default build, CMake generated `tests/configure`, `atlocal`,
`package.m4`, and the `DASTest`, `DDSTest`, `EXPRTest`, `DMRTest`, and
`getdapTest` autotest driver scripts without requiring an explicit
`--target tests`.

Observed suite results:

- `DASTest`: passed
- `DDSTest`: passed
- `DMRTest`: passed
- `EXPRTest`: failed
- `getdapTest`: failed

Current autotools-era assumptions or runtime issues still exposed by the CMake
path:

- `EXPRTest` runs successfully as an autotest suite, but on this machine many
  `expr-test -w ...` cases fail while the matching `-W` cases pass. This now
  appears to be a runtime/behavior difference in `expr-test`, not a missing
  CMake/autotest integration input.
- `getdapTest` still depends on network access and also hit a local HTTP cache
  locking failure in `HTTPCache.cc`, followed by a segmentation fault in
  `getdap`. This is likewise a runtime issue surfaced by the suite, not a
  driver-generation problem.
- The generated suites expect to run from the CMake `tests/` build directory so
  that autotest can create its `*.dir` work areas relative to the configured
  `atconfig`/`atlocal` files.
- `package.m4` is required by `autom4te` for all five suites and is now being
  generated in the CMake build tree rather than assumed from the autotools
  build.
