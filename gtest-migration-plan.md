# GoogleTest Migration Plan For Autotools Unit Tests

## Scope

This plan covers only the CppUnit-based tests that are built and run through the autotools `make check` path.

Included:

- `unit-tests/Makefile.am`
- `http_dap/unit-tests/Makefile.am`
- `d4_ce/unit-tests/Makefile.am`

Excluded:

- CMake test wiring
- The autotest integration suites in `tests/Makefile.am`, except for shared test support such as `libtest-types.a`

The current autotools CppUnit surface is 63 test binaries:

- 59 in `unit-tests/`
- 3 in `http_dap/unit-tests/`
- 1 in `d4_ce/unit-tests/`

This plan uses `AGENTS.md` guidance and the architecture/build notes in `docs/deep-dive-codex.md`.

## Goals

- Replace CppUnit with GoogleTest for the autotools unit-test executables
- Preserve `make check` behavior for autotools users
- Keep the existing test executable names stable during migration
- Avoid changing the autotest integration suites unless needed for shared support code
- Minimize risk by converting low-coupling tests first and network-sensitive tests last

## Build-System Plan

1. Replace the configure-time dependency check in `configure.ac`.
   Remove the `AM_PATH_CPPUNIT(...)` dependency gate and introduce a GoogleTest detection path with a new automake conditional such as `GTEST`.

2. Add GoogleTest build variables to the autotools test directories.
   In `unit-tests/Makefile.am`, `http_dap/unit-tests/Makefile.am`, and `d4_ce/unit-tests/Makefile.am`, replace `$(CPPUNIT_CFLAGS)` and `$(CPPUNIT_LIBS)` with GoogleTest equivalents such as `$(GTEST_CFLAGS)`, `$(GTEST_LIBS)`, and `$(PTHREAD_LIBS)`.

3. Keep the autotools execution model stable.
   Preserve `check_PROGRAMS = $(UNIT_TESTS)` and `TESTS = $(UNIT_TESTS)` so `make check` still builds and runs the same executables.

4. Introduce a shared GoogleTest runner helper.
   Replace `run_tests_cppunit.h` with a GoogleTest-compatible helper that preserves:
   - `-d` / `-D` debug flag handling
   - basic help behavior
   - optional single-test selection from the command line

5. Keep dual-framework support during the transition.
   Do not remove CppUnit support until all autotools test binaries have been migrated and verified. This avoids turning the work into a single high-risk cutover.

6. Remove CppUnit only after parity is reached.
   After all autotools tests pass under GoogleTest, remove:
   - CppUnit configure checks
   - CppUnit conditionals in autotools files
   - CppUnit headers and helper files
   - CppUnit references in build/install documentation

## Test Conversion Rules

Use one consistent conversion pattern across the tree:

- `CPPUNIT_TEST_SUITE` to `TEST_F`
- `CPPUNIT_ASSERT(expr)` to `EXPECT_TRUE(expr)` or `ASSERT_TRUE(expr)`
- `CPPUNIT_ASSERT_EQUAL(a, b)` to `EXPECT_EQ(a, b)`
- `CPPUNIT_ASSERT_THROW(expr, Ex)` to `EXPECT_THROW(expr, Ex)`
- `CPPUNIT_FAIL(msg)` to `FAIL() << msg`
- `setUp()` / `tearDown()` to `SetUp()` / `TearDown()`

Preserve test logic during the framework switch:

- keep fixture setup and cleanup behavior unchanged
- keep environment-variable setup unchanged
- keep test asset paths and generated `test_config.h` usage unchanged
- do not rename executables in the first migration pass

## Recommended Batches

The batches are ordered to establish the GoogleTest pattern on low-risk tests first, then move outward toward parser, translation, cache, and HTTP/network-sensitive areas.

### Batch 0: Build Skeleton And Proof Of Pattern

Purpose:

- land the autotools GoogleTest dependency wiring
- add the shared runner/helper
- prove the conversion style in one test per subtree

Tests:

- `unit-tests/BaseTypeTest`
- `d4_ce/unit-tests/D4ConstraintEvaluatorTest`
- `http_dap/unit-tests/HTTPConnectTest`

Exit criteria:

- `autoreconf -fi`
- `./configure`
- each converted binary builds and runs under `make check`

### Batch 1: Scalar And Utility Core Tests

Purpose:

- convert low-coupling tests with straightforward fixtures and assertions
- validate the assertion mapping and runner helper

Tests:

- `unit-tests/RegexTest`
- `unit-tests/ByteTest`
- `unit-tests/MIMEUtilTest`
- `unit-tests/generalUtilTest`
- `unit-tests/parserUtilTest`
- `unit-tests/ErrorTest`
- `unit-tests/SignalHandlerTest`
- `unit-tests/Int8Test`
- `unit-tests/Int16Test`
- `unit-tests/UInt16Test`
- `unit-tests/Int32Test`
- `unit-tests/UInt32Test`
- `unit-tests/Int64Test`
- `unit-tests/UInt64Test`
- `unit-tests/Float32Test`
- `unit-tests/Float64Test`

Why this batch:

- mostly local assertions
- little shared state
- low filesystem and network sensitivity

### Batch 2: Core DAP2 Model And Container Tests

Purpose:

- convert the core object-model tests that exercise the library boundaries described in `docs/deep-dive-codex.md`

Tests:

- `unit-tests/ArrayTest`
- `unit-tests/GridTest`
- `unit-tests/AttrTableTest`
- `unit-tests/DASTest`
- `unit-tests/DDSTest`
- `unit-tests/SequenceTest`
- `unit-tests/BaseTypeFactoryTest`
- `unit-tests/D4BaseTypeFactoryTest`
- `unit-tests/ConstraintEvaluatorTest`
- `unit-tests/ServerFunctionsListUnitTest`
- `unit-tests/BaseTypeTest`

Why this batch:

- still mostly in-process
- builds confidence in fixture conversion before older legacy-style tests are touched

### Batch 3: Legacy CppUnit Pattern Tests

Purpose:

- convert the older tests that still use the historic `*T` naming and older fixture style

Tests:

- `unit-tests/marshT`
- `unit-tests/arrayT`
- `unit-tests/attrTableT`
- `unit-tests/structT`
- `unit-tests/sequenceT`
- `unit-tests/ddsT`
- `unit-tests/dasT`
- `unit-tests/ancT`
- `unit-tests/util_mitTest`

Why this batch:

- these are likely to require the most mechanical cleanup
- separating them avoids slowing down the cleaner modern conversions

### Batch 4: Parser, XML, And Translation Tests

Purpose:

- migrate the tests that depend on XML parsing, generated config, and DAP2/DAP4 translation paths

Tests:

- `unit-tests/DDXParserTest`
- `unit-tests/D4ParserSax2Test`
- `unit-tests/DMRTest`
- `unit-tests/DmrRoundTripTest`
- `unit-tests/DmrToDap2Test`
- `unit-tests/IsDap4ProjectedTest`
- `unit-tests/D4AttributesTest`
- `unit-tests/D4DimensionsTest`
- `unit-tests/D4EnumDefsTest`
- `unit-tests/D4EnumTest`
- `unit-tests/D4GroupTest`
- `unit-tests/D4SequenceTest`
- `unit-tests/D4FilterClauseTest`
- `unit-tests/D4AsyncDocTest`

Why this batch:

- these tests are tightly tied to the DAP4 parsing and translation flows
- they are a good midpoint between simple unit tests and stream/cache tests

### Batch 5: Marshaller, Stream, And Concurrency Tests

Purpose:

- convert tests that exercise serialization, streaming, and threading behavior

Tests:

- `unit-tests/MarshallerTest`
- `unit-tests/MarshallerFutureTest`
- `unit-tests/MarshallerThreadTest`
- `unit-tests/D4MarshallerTest`
- `unit-tests/D4UnMarshallerTest`
- `unit-tests/D4StreamRoundTripTest`
- `unit-tests/chunked_iostream_test`

Why this batch:

- more sensitive to fatal vs non-fatal assertions
- often easier to debug after the general fixture strategy is already proven

### Batch 6: Cache And Local Filesystem Tests

Purpose:

- convert tests that create, clean, or inspect local cache state and generated files

Tests:

- `unit-tests/RCReaderTest`
- `unit-tests/DAPCache3Test`
- `unit-tests/ResponseCacheTest` if re-enabled for autotools
- `http_dap/unit-tests/HTTPCacheTest`

Why this batch:

- fixture cleanup matters
- stale temp files and cache locks can hide migration bugs

### Batch 7: HTTP And External-Environment Tests

Purpose:

- convert the most environment-sensitive tests last

Tests:

- `http_dap/unit-tests/HTTPConnectTest`
- `http_dap/unit-tests/HTTPThreadsConnectTest`

Why this batch:

- network behavior and remote state can obscure framework migration issues
- these should be used only after the helper, fixture, and assertion patterns are stable

### Batch 8: Optional Resource-Heavy Test

Purpose:

- convert the largest and least convenient test only after the main migration is complete

Tests:

- `unit-tests/BigArrayTest`

Why this batch:

- already optional under autotools
- high runtime and resource cost
- poor candidate for early validation

## Batch Notes

- `HTTPConnectTest` appears in Batch 0 as a proof case and again in Batch 7 as part of the full HTTP sweep. Treat Batch 0 as the first pilot conversion for that subtree, then complete the remaining HTTP tests together.
- `ResponseCacheTest` is present in the source tree but not listed in `unit-tests/Makefile.am` for `UNIT_TESTS`. If it is intentionally dormant, do not expand scope during the migration. If it is meant to be restored, move it into Batch 6.
- Keep `test_config.h`, `testFile.cc`, `remove_directory.cc`, and `../tests/libtest-types.a` untouched unless the GoogleTest transition forces a narrowly scoped change.

## Verification Plan

After each batch:

1. Run `autoreconf -fi`
2. Run `./configure`
3. Run the relevant subtree target:
   - top-level `make check`
   - or focused checks in `unit-tests/`, `http_dap/unit-tests/`, and `d4_ce/unit-tests/`
4. Re-run the converted binaries individually when debugging fixture behavior

Before removing CppUnit entirely:

1. Run a full top-level `make check`
2. Run at least one developer-style build variant if that is part of normal project use
3. Confirm that the autotest integration suites in `tests/` still build and run unchanged

## Completion Criteria

The migration is complete when:

- all autotools unit-test executables build against GoogleTest
- `make check` succeeds through the autotools path
- CppUnit is no longer referenced by `configure.ac` or the autotools unit-test `Makefile.am` files
- the shared runner helper has been replaced with the GoogleTest version
- build and install documentation no longer claim that CppUnit is required for autotools unit tests
