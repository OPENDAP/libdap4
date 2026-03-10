# VS Code Workspace Guide

This document describes the VS Code workspace pattern used in this repository.

VS Code reads its active workspace files from:

- `.vscode/tasks.json`
- `.vscode/launch.json`
- `.vscode/settings.json`

Those top-level JSON files are intentionally not tracked by git. Instead, each developer can keep versioned copies of their own working configuration under a personal subdirectory such as:

- `.vscode/jhrg/tasks.json`
- `.vscode/jhrg/launch.json`
- `.vscode/jhrg/settings.json`

The intent is:

- avoid conflicts between developers with different local paths and preferences
- make it easy for each developer to keep their own VS Code setup under version control
- let people learn from each other's configurations by browsing the per-user directories

## Recommended workflow

1. Create a personal directory under `.vscode/` if you do not already have one.
2. Keep your versioned copies of `tasks.json`, `launch.json`, and `settings.json` there.
3. Copy those files into `.vscode/` when you want VS Code to use them locally.

For example, if your directory is `.vscode/jhrg/`, copy:

- `.vscode/jhrg/tasks.json` -> `.vscode/tasks.json`
- `.vscode/jhrg/launch.json` -> `.vscode/launch.json`
- `.vscode/jhrg/settings.json` -> `.vscode/settings.json`

The tracked files serve as examples and personal baselines. The untracked top-level files are the ones VS Code actually reads.

## Configuration variables

The current setup uses VS Code configuration variables instead of hard-coding environment values directly into tasks and launch configurations.

In `.vscode/<your-name>/settings.json`, define:

- `prefix`
- `PATH`

Then reference them from tasks and launch configurations using:

- `${config:prefix}`
- `${config:PATH}`

This is important for `libdap4` because `prefix` and `PATH` must be set correctly for configure, build, test, and debug workflows.

Example:

```json
{
  "prefix": "/Users/you/hyrax/build",
  "PATH": "${config:prefix}/bin:${config:prefix}/deps/bin:${env:PATH}",
  "terminal.integrated.env.osx": {
    "prefix": "${config:prefix}",
    "PATH": "${config:PATH}"
  },
  "terminal.integrated.env.linux": {
    "prefix": "${config:prefix}",
    "PATH": "${config:PATH}"
  }
}
```

Notes:

- `prefix` is the install/build prefix passed to `./configure --prefix=$prefix`
- `PATH` should usually prepend `${config:prefix}/bin` and `${config:prefix}/deps/bin`
- the terminal environment mirrors those values so interactive shells inside VS Code behave the same way as tasks

## Tasks

The tracked example at `.vscode/jhrg/tasks.json` defines shell tasks that run from `${workspaceFolder}` using `/bin/zsh`.

Its task-wide environment sets:

- `TESTSUITEFLAGS=-j`
- `prefix=${config:prefix}`
- `PATH=${config:PATH}`

Current task catalog:

| Label                         | Command                                                                                        | Purpose                                                             |
| ----------------------------- | ---------------------------------------------------------------------------------------------- | ------------------------------------------------------------------- |
| `autotools: configure`        | `echo "prefix: $prefix" && autoreconf -fvi && ./configure --prefix=$prefix --enable-developer` | Regenerates autotools files and configures a developer build.       |
| `autotools: build`            | `make -j`                                                                                      | Builds the project in parallel.                                     |
| `autotools: check`            | `make -j check`                                                                                | Runs the autotools test target in parallel.                         |
| `autotools: install`          | `make install`                                                                                 | Installs into the configured prefix.                                |
| `autotools: clean`            | `make clean`                                                                                   | Removes build artifacts from the current tree.                      |
| `autotools: compile_commands` | `make clean && bear -- make -j && bear --append -- make -j check`                              | Rebuilds and captures compile commands for IntelliSense/navigation. |

Run tasks from:

- `Terminal` -> `Run Task...`
- `Cmd+Shift+P` / `Ctrl+Shift+P` -> `Tasks: Run Task`

Notes:

- `autotools: configure` assumes your `prefix` setting is valid
- `autotools: compile_commands` requires `bear`
- these task definitions belong in your versioned per-user file and can then be copied to `.vscode/tasks.json`

## Launch configurations

The tracked example at `.vscode/jhrg/launch.json` currently defines one launch target:

- `Debug getdap4`

It launches:

- `${workspaceFolder}/.libs/getdap4`

With:

- args: `-d http://test.opendap.org/opendap/data/nc/fnoc1.nc`
- `DYLD_LIBRARY_PATH=${workspaceFolder}/.libs:${env:DYLD_LIBRARY_PATH}`
- `PATH=${config:PATH}`
- `MIMode=lldb`

Run it from:

- `Run and Debug` view -> select `Debug getdap4` -> start

If you add new launch targets, keep them in your personal versioned file and then copy that file to `.vscode/launch.json` for local use.

## Settings

The tracked example at `.vscode/jhrg/settings.json` includes:

- `prefix`
- `PATH`
- C/C++ extension settings for compile commands and IntelliSense
- terminal environment values for macOS and Linux

Keep machine-specific values in your personal versioned settings file. That preserves the pattern of shared examples plus conflict-free local activation.
