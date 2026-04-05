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
