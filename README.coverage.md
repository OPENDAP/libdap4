# Test Coverage

Test coverage can be computed by first configuring and building the code for coverage.
Once done, run both `make` and `make check`. The build process will produce a number of
`*.gcda` and `*.gcno` files that contain the coverage information. Once that is done, use
the `gcovr` tool to get a summary of coverage and a set of HTML pages that contain statistics
for each source file and a map of which lines are (or are not) covered by either the
unit- or integration tests.

> [!NOTE]
> Make sure to not release code built for test coverage since _every_ method is instrumented
> and thus runs more slowly.

## Configuration and Build

To configure a build for coverage analysis, use configure with the --enable-coverage option.

```bash
export prefix=<the hyrax prefix dir>
./configure --prefix=$prefix --enable-developer --enable-coverage
```

To build, run make and make check as normal. For example:

```bash
make -j20
make -j20 check
```

The coverage data files are updated whenever a function is run, so if you run other programs
using the library that are not part of the tests, that will alter the results, maybe
resulting in code marked as 'covered' by a test when it was not.

> [!TIP]
> Once you have coverage data, re-run `configure` without `--enable-coverage` to get a better
> performing library.

## Getting the Coverage Report

Use the `gcovr` tool to get coverage data in a usable form.

### Settings for `gcovr`

We have a small settings file for `gcovr`; see `gcovr.config` This excludes things that should
not be counted. Some part of this file should be synchronized with the text in `sonar-project.properties`
or the results show in the SonarScan output will be off.

### Installing `gcovr`

Install `gcovr` using homebrew, apt, pip, etc. However, be sure you are installing and
using version 8.2 or greater of gcovr since older versions have various issues.
These include, not working with parallel test runs, emitting broken data for SonarScan, and
under-reporting coverage.

> [!NOTE]
> Note that Ubuntu Jammy uses `gcovr` 5.x by default, and thus in Travis we force the 'scan'
> step to use 8.2 using a `pip` install. Make sure to test the version of `gcovr` using
> `gcovr --version`.

### Running `gcovr`

To collect coverage data, use:

```bash
gcovr --config gcovr.config
```

That will print out a bunch of text plus a nice summary of the coverage. The text is less than
completely useful, but there are probably tools that use it.

To get the summary stats and set of web pages that are easy to use but entail some clicking
around:

```bash
gcovr --config gcovr.config --html-details coverage-html/coverage.html
```

Then open `coverage-html/coverage.html` as a file in your web browser.

For sonarscan, we build the coverage data using:

```bash
gcovr --config gcovr.config --sonarqube coverage.xml
```

See [.travis.yml](.travis.yml) in the `scan` block for code that collects coverage data
for sonarscan. The data are uploaded to sonarscan using `sonar.coverageReportPaths=coverage.xml`
in [sonar-project.properties](sonar-project.properties). I mention that here because it
is hardly obvious from the Travis yaml just how the `coverage.xml` file gets uploaded to
sonarscan.

## Comparing results from different runs

On the same OS and `gcovr` version, differences should only be from changes to the tests,
the configuration or the `gcovr` version. However, `clang` (OSX) and `GNU gcc` (Linux) both
handle some aspects of compilation differently, so the coverage counts will vary a bit.
Also, SonarScan has its own notion of what is important and _combines_ the Line and Function
counts into one number. While the Line and Function counts themselves are close between
Sonar and Linux, the overall number will be lower than the Line count. Most people use
the line count, making the most obvious number from Sonar somewhat misleading.
