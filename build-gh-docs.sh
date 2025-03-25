#!/bin/bash
#
# Build the reference guide for this code and publish it using github.io.

set -e

DOXYGEN_CONF=doxy.conf
HTML_DOCS=html
# Get the name of the current branch, it might not be master. jhrg 3/24/25
BRANCH=$(git branch | grep '*' | cut -d ' ' -f 2)

# if we happen to have a local checkout of gh-pages, remove the docs from it.
if git branch --list | grep gh-pages
then
    git checkout --quiet gh-pages

    if test -d $HTML_DOCS
    then
        git rm -rf $HTML_DOCS
        PRE_COMMIT_ALLOW_NO_CONFIG=1 git commit -m "Removed old docs"
    fi
fi

# Return to our current working branch
git checkout --quiet $BRANCH

# clean out the docs
if test -d $HTML_DOCS
then
    git rm -rf $HTML_DOCS
    PRE_COMMIT_ALLOW_NO_CONFIG=1  git commit -m "Removed old docs"
fi

# Build the docs. Puts them in a top-level dir named 'html' and that
# must match $HTML_DOCS. Edit $DOXYGEN_CONF if $HTML_DOCS changes!
doxygen $DOXYGEN_CONF

# Now switch to the gh-pages branch and commit and push the docs.
if git branch --list | grep gh-pages
then
    git checkout --quiet gh-pages
else
    # Use the --track to make sure we get the branch from origin. jhrg 3/24/25
    git checkout --quiet --track origin/gh-pages
fi

# Add the new docs. The pre-commit part is needed because the repo
# now uses a pre-commit operation for enforce formatting, but the
# gh-pages branch lacks that. jhrg 3/24//25
git add --force ${HTML_DOCS}
PRE_COMMIT_ALLOW_NO_CONFIG=1 git commit -m "Added new docs"
git push --quiet

# Now go back to the working branch.
git checkout --quiet $BRANCH

git branch --quiet --delete gh-pages
