#!/bin/sh
#
# Build the reference guide for this code and publish it using github.io.

DOXYGEN_CONF=doxy.conf
HTML_DOCS=html
BRANCH=`git branch | grep '*' | cut -d ' ' -f 2`

if git branch --list | grep gh-pages
then
    git checkout -q gh-pages

    if test -d $HTML_DOCS
    then
        git rm -rf $HTML_DOCS
        git commit -m "Removed old docs"
    fi
fi

git checkout -q $BRANCH

if test -d $HTML_DOCS
then
    git rm -rf $HTML_DOCS
    git commit -m "Removed old docs"
fi

# Build the docs. Puts them in a top-level dir named 'html' and that
# must match $HTML_DOCS. Edit $DOXYGEN_CONF if $HTML_DOCS changes!
doxygen $DOXYGEN_CONF

# Now switch to the gh-pages branch and commit and push the docs.

git checkout -q gh-pages

git add --force ${HTML_DOCS}
git commit -m "Added new docs"
git push -q

git checkout -q $BRANCH
git branch -d gh-pages
