# `configure.ac` Refactoring Plan

## Goal

Reduce `./configure` runtime first, and make `configure.ac` easier to maintain second, by removing probes that no longer affect compilation, linking, tests, or generated build files.

## Current observations

The current `configure.ac` mixes four different kinds of logic:

1. Required dependency discovery that still feeds `Makefile.am`, `dap-config`, or `libdap.pc`.
2. Optional build/test toggles that still drive Automake conditionals.
3. Legacy portability probes that define `HAVE_*` symbols no longer consumed by the code.
4. Historical commented-out blocks and shell-heavy custom logic that increase maintenance cost.

A quick symbol audit against the tree shows several header and function checks now appear unused outside generated files:

### Header probes with no current consumer

- `HAVE_GETOPT_H`
- `HAVE_LIMITS_H`
- `HAVE_FCNTL_H`
- `HAVE_MEMORY_H`
- `HAVE_STDDEF_H`
- `HAVE_NETINET_IN_H`

### Function probes with no current consumer

- `HAVE_ALARM`
- `HAVE_BZERO`
- `HAVE_GETCWD`
- `HAVE_LOCALTIME_R`
- `HAVE_MEMMOVE`
- `HAVE_MEMSET`
- `HAVE_POW`
- `HAVE_PUTENV`
- `HAVE_STRCHR`
- `HAVE_STRERROR`
- `HAVE_STRTOL`
- `HAVE_STRTOUL`

There are also legacy/commented sections that add noise without affecting the generated build:

- The old DAP2/DDX block near the top.
- A large commented-out alternate libxml2 detection implementation.
- Commented-out CppUnit detection alternatives.

## Refactoring strategy

Apply the work in phases so behavior stays stable while the script gets smaller and faster.

## Phase 1: Build an evidence-backed inventory

Before deleting checks, produce a short inventory that classifies every probe in `configure.ac` as:

- `required`: affects compile flags, link flags, generated scripts, or test baselines
- `optional`: controls developer/test-only behavior
- `dead`: no current consumer in source or Automake input
- `replace`: still needed, but should use a simpler macro or less shell code

This inventory should include:

- `AC_CHECK_HEADERS*`
- `AC_CHECK_FUNCS`
- `AC_CHECK_LIB`
- `PKG_CHECK_MODULES`
- `AM_CONDITIONAL`
- custom macros such as `DODS_CHECK_SIZES`, `OX_RHEL8_TIRPC`, and `DODS_DEBUG_OPTION`

Deliverable: a checked-in table or comment block that makes future cleanup decisions auditable.

## Phase 2: Remove dead portability probes first

This is the safest runtime win.

### Remove unused header checks

Delete header probes whose `HAVE_*` symbols have no current non-generated consumer, starting with:

- `getopt.h`
- `limits.h`
- `fcntl.h`
- `memory.h`
- `stddef.h`
- `netinet/in.h`

Then review low-value probes with only one or two consumers and replace them with unconditional standard includes where practical:

- `malloc.h`
- `sys/time.h`
- `sys/param.h`
- `locale.h`

For each remaining conditional include, decide whether the project still cares about the target platforms that required it. If not, remove both the probe and the `#ifdef HAVE_...` branches in code.

### Remove unused function checks

Delete unused entries from the large `AC_CHECK_FUNCS([...])` list. Keep only functions whose `HAVE_*` symbols still affect source behavior, such as:

- `atexit`
- `dup2`
- `getpagesize`
- `setenv`
- `timegm`
- `mktime`

If some of those can now be assumed on supported platforms, remove their probes too and simplify the code accordingly.

Expected result: fewer compile/link test fragments generated and executed by `configure`, with minimal behavior risk.

## Phase 3: Replace obsolete Autoconf portability macros

Several generic portability macros are legacy baggage for a modern C++14 library.

Review and likely remove or justify:

- `AC_HEADER_DIRENT`
- `AC_HEADER_SYS_WAIT`
- `AC_C_CONST`
- `AC_C_INLINE`
- `AC_TYPE_SIZE_T`
- `AC_STRUCT_TM`
- `AC_C_VOLATILE`
- `AC_CHECK_MEMBERS([struct stat.st_blksize])`

These should stay only if one of the following is true:

- a generated symbol is still used in the code, or
- the project explicitly supports platforms old enough to require the probe

If there is no such support requirement, remove them. For a C++14 codebase, many of these are unnecessary.

## Phase 4: Simplify dependency detection

Most remaining configure time is likely spent in external dependency checks, compiler/link tests, and shelling out to helper tools.

### Curl and libxml2

The current curl and libxml2 blocks duplicate logic across:

- explicit `--with-...` prefixes
- `pkg-config`
- `*-config` scripts

Refactor each dependency probe into one small macro with this order:

1. honor explicit `--with-...` prefix
2. try `PKG_CHECK_MODULES`
3. fall back to `curl-config` or `xml2-config` only if required for platforms still in scope

Also:

- move each dependency block into `conf/*.m4` or a local helper macro
- delete the large commented-out alternate libxml2 implementation
- keep only the variables actually consumed by the build (`*_LIBS`, `*_CFLAGS`, private pkg-config fields)

If the supported platforms all provide `pkg-config`, the biggest runtime and maintenance win is to make `pkg-config` the only non-prefix path and drop `curl-config`/`xml2-config` fallback logic entirely.

### Library checks

Review `AC_CHECK_LIB` use for:

- `pthread`
- `uuid`
- `crypto`
- `gcov`

If these libraries are already discovered transitively through pkg-config or are guaranteed on supported platforms, avoid redundant link probes. In particular:

- `pthread` may be better handled with standard thread detection macros instead of a raw `-lpthread` check.
- `gcov` should be checked only when `--enable-coverage` is requested, not on every run.

## Phase 5: Make optional developer/test features lazy

Some probes should only run when the related feature is requested.

### Coverage

Move all coverage detection behind `--enable-coverage=yes`:

- `AC_CHECK_LIB([gcov], ...)`
- `which gcov`
- `gcov -help | grep LLVM`

No coverage-related work should happen in the default configure path.

### Leaks

Only call `AC_CHECK_PROG(LEAKS, ...)` when `--enable-leaks=yes` is requested. On non-macOS builders this is pure overhead.

### CppUnit

CppUnit detection should run only if unit tests that require it will be built. If test builds are optional, gate `AM_PATH_CPPUNIT` behind an explicit option such as `--enable-cppunit-tests` or equivalent test toggle.

### AddressSanitizer

The ASan compiler flag probe is reasonable, but it should only run when `--enable-asan=yes` is requested.

Expected result: the common `./configure` path avoids feature checks for coverage, leaks, ASan, and optional test frameworks.

## Phase 6: Replace shell-heavy parsing with simpler M4/shell patterns

The version parsing and some helper checks currently use repeated `grep`/`sed` pipelines. They are not the main runtime cost, but they do make the script harder to maintain.

Refactor to:

- parse version components once
- avoid repeated external `sed` calls where shell parameter expansion or a single helper macro is enough
- consolidate repeated `if test -n "$prefix" -a -x ...` patterns into helper macros

Also clean up:

- commented historical notes that no longer guide current behavior
- duplicated or stale comments about removed build paths

## Phase 7: Reorganize the file for maintainability

After functional cleanup, split `configure.ac` into clearer sections:

1. package/version setup
2. toolchain setup
3. core compile environment
4. required dependencies
5. optional developer/test features
6. generated files

Move reusable dependency logic into local `.m4` macros under `conf/` so `configure.ac` becomes mostly orchestration rather than embedded shell script.

Recommended first extractions:

- `LIBDAP_CHECK_CURL`
- `LIBDAP_CHECK_XML2`
- `LIBDAP_CHECK_OPTIONAL_TEST_TOOLS`

## Suggested implementation order

1. Remove dead `AC_CHECK_HEADERS*` entries.
2. Remove dead `AC_CHECK_FUNCS` entries.
3. Gate coverage, leaks, ASan, and CppUnit detection behind explicit enable options.
4. Delete commented-out legacy blocks.
5. Simplify curl and libxml2 detection.
6. Review/remove obsolete portability macros.
7. Extract the remaining dependency logic into local `.m4` helpers.

This order gives early runtime improvements before the larger structural cleanup.

## Validation plan

For each cleanup step, verify both generated configuration and real builds.

Minimum validation matrix:

- `autoreconf -fi`
- default `./configure`
- `./configure --enable-developer`
- `./configure --enable-asan` if supported by the compiler
- `./configure --enable-coverage`
- one build with explicit `--with-curl=...` or `--with-xml2=...` if those paths are retained

Then run:

- a normal library build
- at least one unit-test build
- the Autotest suite in `tests/`

Also measure configure runtime before and after each major phase so the cleanup stays aligned with the primary goal.

## Success criteria

The refactor is complete when:

- every remaining probe has a documented consumer
- default `./configure` runs fewer external checks than today
- optional developer/test probes are skipped unless explicitly requested
- `configure.ac` no longer contains large commented-out alternative implementations
- dependency detection logic is short enough that a maintainer can reason about it without reading historical branches
