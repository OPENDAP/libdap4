# VS Code Workspace Guide

This document describes the workspace automation in `.vscode/tasks.json`, `.vscode/launch.json`, and `.vscode/settings.json.template`.

## Tasks (`.vscode/tasks.json`)

All configured tasks are shell tasks that run from `${workspaceFolder}` with `/bin/zsh`, and they inject:

- `prefix` from `HYRAX_PREFIX`
- `PATH` prefixed with `${HYRAX_PREFIX}/bin:${HYRAX_PREFIX}/deps/bin`

Run tasks from:

- `Terminal` -> `Run Task...`
- Or `Cmd+Shift+P` / `Ctrl+Shift+P` -> `Tasks: Run Task`

Task catalog:

| Label                         | Command                                                              | Purpose                                                               |
| ----------------------------- | -------------------------------------------------------------------- | --------------------------------------------------------------------- |
| `autotools: configure`        | `autoreconf -fvi && ./configure --prefix=$prefix --enable-developer` | Regenerates autotools files and runs configure for a developer build. |
| `autotools: build`            | `make -j`                                                            | Builds the project in parallel.                                       |
| `autotools: check`            | `make -j check`                                                      | Runs the autotools test target in parallel.                           |
| `autotools: install`          | `make install`                                                       | Installs into the configured prefix.                                  |
| `autotools: clean`            | `make clean`                                                         | Removes build artifacts from the current tree.                        |
| `autotools: compile_commands` | `make clean && bear -- make -j && bear --append -- make -j check`    | Rebuilds and captures compile commands for IntelliSense/navigation.   |

Notes:

- `autotools: configure` both regenerates (`autoreconf`) and configures (`./configure`) the tree.
- `autotools: build` is a good default after configure.
- `autotools: compile_commands` requires `bear` to be installed.

## Debug Launch Targets (`.vscode/launch.json`)

Current launch configuration:

- `Debug getdap4`
  - Launches `${workspaceFolder}/.libs/getdap4`
  - Uses args: `-d http://test.opendap.org/opendap/data/nc/fnoc1.nc`
  - Uses LLDB (`MIMode: lldb`)
  - Sets:
    - `DYLD_LIBRARY_PATH=${workspaceFolder}/.libs:${env:DYLD_LIBRARY_PATH}`
    - `PATH=${env:HYRAX_PREFIX}/bin:${env:HYRAX_PREFIX}/deps/bin:${env:PATH}`

Run it from:

- `Run and Debug` view -> pick `Debug getdap4` -> Start

### Adding new launch targets for unit tests

Add another object in `configurations` using the same structure. Typical edits:

- Change `name` (for example, `Debug unit-tests: arrayT`)
- Change `program` to the unit test executable (for example, `${workspaceFolder}/unit-tests/.libs/arrayT`)
- Adjust `args` as needed by that test
- Keep `cwd`, `environment`, `MIMode`, and `setupCommands` unless a test needs something different

Example:

```json
{
  "name": "Debug unit-tests: arrayT",
  "type": "cppdbg",
  "request": "launch",
  "program": "${workspaceFolder}/unit-tests/.libs/arrayT",
  "args": [],
  "cwd": "${workspaceFolder}",
  "environment": [
    {
      "name": "DYLD_LIBRARY_PATH",
      "value": "${workspaceFolder}/.libs:${env:DYLD_LIBRARY_PATH}"
    },
    {
      "name": "PATH",
      "value": "${env:HYRAX_PREFIX}/bin:${env:HYRAX_PREFIX}/deps/bin:${env:PATH}"
    }
  ],
  "MIMode": "lldb"
}
```

## Local-Only Settings (`.vscode/settings.json.template` -> `.vscode/settings.json`)

Use `.vscode/settings.json.template` as a starting point, then create/edit `.vscode/settings.json` with machine-specific values.

Recommended process:

1. Copy the template keys into `.vscode/settings.json`.
2. Replace placeholder text with real values for your machine.
3. Set `HYRAX_PREFIX` to the literal install prefix path (for example, `/Users/you/hyrax`).
4. Build `PATH` from that prefix plus the existing path.

Example local values:

```json
{
  "C_Cpp.default.configurationProvider": "ms-vscode.makefile-tools",
  "C_Cpp.default.compileCommands": "${workspaceFolder}/compile_commands.json",
  "C_Cpp.default.cppStandard": "c++14",
  "C_Cpp.default.intelliSenseMode": "macos-clang-arm64",
  "terminal.integrated.env.osx": {
    "HYRAX_PREFIX": "/Users/you/hyrax",
    "PATH": "/Users/you/hyrax/bin:/Users/you/hyrax/deps/bin:${env:PATH}"
  },
  "terminal.integrated.env.linux": {
    "HYRAX_PREFIX": "/home/you/hyrax",
    "PATH": "/home/you/hyrax/bin:/home/you/hyrax/deps/bin:${env:PATH}"
  }
}
```

Keep local values in `.vscode/settings.json` so each developer can use a different prefix/path without changing shared workspace docs or task/launch definitions.
