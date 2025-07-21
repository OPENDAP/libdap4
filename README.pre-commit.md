
# About the per-commit hooks

The libdap4 GitHub repository uses the pre-commit framework to manage
and run the pre-commit hooks. The pre-commit hooks are a set of scripts
that run before a commit is made. The hooks are used to check the code for
formatting, style, and other issues. The pre-commit hooks are run automatically
when you make a commit, and if any of the hooks fail, the commit is rejected.

## TL;DR

Install pre-commit and clang-format using brew, yum or apt-get.

When you run ```git commit```, the pre-commit hooks will run automatically.

If files need to be reformatted by clang-format, they will be and/but your 
commit will fail. Rerun ```git commit``` and the commit should succeed. This
works because the clang-format not only checks but is configured to modify
the files in place.

To run it by hand: ```pre-commit run --all-files```

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

Here's an example of what happens when files are reformatted by clang-format and the 
```git commit``` command is run again:

```
10:11:07.481: [libdap4] git commit -a
check yaml...........................................(no files to check)Skipped
check json...........................................(no files to check)Skipped
clang-format.............................................................Failed
- hook id: clang-format
- files were modified by this hook

10:11:44.228: [libdap4] git commit -a
check yaml...........................................(no files to check)Skipped
check json...........................................(no files to check)Skipped
clang-format.............................................................Passed
[remove-warnings-1 b43ed266] More warnings removed
 1 file changed, 84 insertions(+), 13 deletions(-)
```
