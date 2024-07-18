
# About the per-commit hooks

The libdap4 GitHub repository uses the pre-commit framework to manage
and run the pre-commit hooks. The pre-commit hooks are a set of scripts
that run before a commit is made. The hooks are used to check the code for
formatting, style, and other issues. The pre-commit hooks are run automatically
when you make a commit, and if any of the hooks fail, the commit is rejected.

## Background

The pre-commit hooks are managed by the pre-commit framework (https://github.com/pre-commit).
These are (or can be) configured to run both locally and on the GitHub server
when a pull request is made.

The repo has three configuration files that control the local behavior of the 
pre-commit hooks: .pre-commit-config.yaml, .clang-format and .clang-format-ignore.
The latter of these is found at the root of the repository and in various child
directories, where the local file will override other .clang-format-ignore files
above it.

To run on GitHub when PRs are issued for a branch, the pre-commit hooks are run
using a GitHub action that is defined in .github/workflows/pre-commit.yml.

## Installation

On OSX, install the pre-commit framework by running the following command:

```brew install pre-commit clang-format```

On linux, use the following command (you may need to install python3):

```pip install pre-commit```

You will also need to install clang-format. On linux, use the following command:

```sudo apt-get install clang-format``` or ```sudo yum install clang-format```

Then, in the root of the repository, run the following command to install the pre-commit hooks:

```pre-commit install```

## Running the pre-commit hooks

To run the pre-commit hooks manually, run the following command in the root of the repository:

```pre-commit run --all-files```

When you make a commit, the pre-commit hooks will run automatically. If any of the hooks fail,
the commit will be rejected. But it's possible to bypass the pre-commit hooks by using the
`--no-verify` option with the `git commit` command. Also, the pre-commit hooks are configured 
to modify the source files 'in place' so when clang-format fails, it does so because files
have been modified. In this case, you can run `git diff` to see the changes and then run and
then run ```git commit``` again. On the second try, the commit should succeed. (I realize
this is a bit of a hack, but we may be able to improve on it in the future - jhrg.)
