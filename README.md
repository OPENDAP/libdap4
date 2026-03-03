# libdap4

---

[![Build Status](https://app.travis-ci.com/OPENDAP/libdap4.svg?token=sZ7AH1JNbtUbge4Y4gRA&branch=master&status=passed)](https://app.travis-ci.com/github/OPENDAP/libdap4)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1013914.svg)](https://doi.org/10.5281/zenodo.1013914)
The DOI above references the latest release.

---

The OPeNDAP C++ implementation of DAP2 and DAP4.

- API documentation: <https://opendap.github.io/libdap4/html/>
- Release history: [`NEWS`](NEWS)
- Build/install details: [`INSTALL`](INSTALL)
- CMake notes: [`README.cmake.md`](README.cmake.md)

## What this library provides

- Core DAP2/DAP4 data model and protocol classes.
- Client/server support libraries and utilities.
- Command-line clients including `getdap` and `getdap4`.

## Build quick start (autotools)

From a release tarball:

```sh
./configure
make
make check
make install
```

From a git checkout:

```sh
autoreconf --force --install --verbose
./configure
make
make check
```

## Requirements (summary)

- Modern C++ compiler (configure checks C++11/C++14 support).
- `libcurl` and `libxml2` (and `libuuid` on Linux).
- `flex` and `bison` for parser generation.
- `CppUnit` to run `make check`.

For exact versions and platform notes, see [`INSTALL`](INSTALL).

## Project file guide

- [`NEWS`](NEWS): version-by-version release notes.
- [`ChangeLog`](ChangeLog): detailed historical changes.
- [`INSTALL`](INSTALL): build and install instructions.
- [`README.dodsrc`](README.dodsrc): client `.dodsrc` behavior and options.
- [`README.AIS`](README.AIS): historical AIS notes.
- [`README.gh-pages.md`](README.gh-pages.md): publishing and maintenance notes for GitHub Pages docs.
- [`README.pre-commit.md`](README.pre-commit.md): pre-commit hook setup and usage.
- [`README.vscode.md`](README.vscode.md): VS Code workspace and task configuration notes.
- Zenodo GitHub integration/settings for `OPENDAP/libdap4`: <https://zenodo.org/account/settings/github/repository/OPENDAP/libdap4>
