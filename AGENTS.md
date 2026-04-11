# AGENTS.md

## Scope

These instructions apply to the entire `libdap4` repository.

## Project Context

- `libdap4` is a legacy C++ implementation of DAP2/DAP4 with long-lived downstream consumers.
- Prioritize compatibility, behavioral stability, and small, reviewable diffs.
- Prefer minimal, targeted changes over broad refactors.

## Primary Build Systems

- Prefer autotools for day-to-day work unless the task is explicitly CMake-focused.
- Keep both autotools and CMake build paths healthy when changing shared build logic.
- For all the builds the 'prefix' environment variable should be set.
- $prefix is the full path to the 'build' directory in the directory above 'libdap4'.
- The PATH env var should have $prefix/bin and $prefix/deps/bin prepended.

## Autotools Workflow (preferred)

For a fresh git checkout:

```sh
autoreconf --force --install --verbose
./configure --prefix=$prefix --enable-developer
make -j
make -j check
```

For release-tarball style builds:

```sh
./configure
make -j
make -j check
```

Notes:

- Check that the environment variable 'prefix' is defined before running any command that uses it.
- Use `--prefix=<path>` when installation path matters.
- Use `TESTSUITEFLAGS=-j<N>` with `make check` when parallelizing tests.
- If `make check` fails due to missing `config.guess`, link `conf/config.guess` into `tests/` per `tests/README`.

## CMake Workflow (supported)

- Presets are defined in `CMakePresets.json`.
- Common presets: `default`, `debug`, `developer`, `asan`.

Typical flow:

```sh
cmake --preset developer
cmake --build --preset developer -j
ctest --preset developer --output-on-failure
```

## Testing Expectations

- For code changes, run focused tests in affected areas first, then broader suites when risk is higher.
- Autotools default: `make -j check`
- CMake default: `ctest --preset default` (or `developer` for debug/developer builds)
- Unit/integration labels are available through CMake test presets (`unit`, `int`).
- If tests are flaky or expected-fail in legacy areas, call that out explicitly in your summary.

## Documentation And Doxygen

- Doxygen docs are built with:

```sh
make docs
```

- Inputs are `doxy.conf` and `main_page.doxygen` (generated from `.in` templates by configure).
- When updating doc config/templates, keep generated and template files consistent with the chosen build workflow.

## Legacy C++ Constraints

- Match local style in touched files; do not perform unrelated formatting sweeps.
- Avoid API/ABI-impacting changes unless explicitly requested.
- Be conservative with ownership/lifetime changes in pointer-heavy code.
- Parser/scanner sources are generated (`*.tab.cc`, `*.tab.hh`, `lex.*.cc`); edit `*.yy`/`*.lex` sources, not generated outputs, unless the task explicitly requires generated-file updates.

## Tooling And Quality

- `clang-format` and pre-commit are configured (`README.pre-commit.md`, `.pre-commit-config.yaml`).
- Prefer running formatting/hooks only on changed files relevant to the task.
- Address sanitizer is supported (`--enable-asan` in autotools, `asan` preset in CMake) for memory-safety debugging.

## Change Discipline

- Do not revert unrelated local changes in a dirty worktree.
- Keep edits tightly scoped to the request.
- If you encounter unexpected repository changes during work, stop and ask how to proceed.
- Do not run destructive git commands unless explicitly requested.

## Review Priorities

When asked to review:

1. Behavioral regressions in protocol/data-model behavior
2. Memory/resource safety and ownership lifetime issues
3. Parser/serialization correctness and edge cases
4. Build-system regressions (autotools and CMake)
5. Missing or weak regression coverage

## Communication

- State assumptions and environment details explicitly (build system, preset/configure flags, test scope).
- If full validation is not run, say exactly what was run and what was not.
