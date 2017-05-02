# The reference documentation for libdap4

The reference manual for libdap4 is built using the doxygen comments
embedded in the code.

Goto the [reference guide](html)

## The gh-pages branch setup

In a new repository, make a branch named _gh-pages_ that is an
_orphan_. This creates a branch that does not share its history with
other branches so that the commit messages don't leak over into the
messages for the master branch.

In the _master_ branch:
```sh
git checkout --orphan gh-pages
```

Change to that directory and remove all of the files. This can take
some work but _git rm -rf *_ is probably what you want. Once that's
done, commit those changes.

```sh
git rm -rf *
git commit -m "Removed non-documetation files"
```

Now hack the _.gitignore_ file so that the directory where the docs
are built (_html_ for _doxygen_ by default) is not on the list of
stuff to ignore. Make this edit to the version of .gitignore that is
on the gh-pages branch, not the version in master. Commit and push.

Add a README.md file that includes a link to the generated
documentation's _index.html_ file. Since doxygen will be writing to
the _html_ directory, this will be...
```html
https://OPeNDAP.github.io/<project>/html/
```
By adding a README.md at the top level, people can find the
documention using a slightly shorter URL:
```html
https://OPeNDAP.github.io/<project>/
```

## Notes

+ **Never** merge this branch to _master_ or any other branch. Never
ever.
+ The script _build-gh-pages.sh_ on the _master_ branch automates the
documentation build, although it may be somewhat fragile.
+ The manual documentation build process is:
  * Switch to the gh-pages branch
  * _git rm_ the files in _html_
  * commit
  * Switch back to the _master_ branch
  * Build the docs (_doxygen doxy.conf_)
  * Check that there's a bunch of file in _html_
  * Switch to _gh-pages_
  * _git add html_; _git commit -m "Added new docs"_; _git push_
  * Switch back to _master_

