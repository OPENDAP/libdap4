#!/bin/sh
#
# Set up the auto tools programs when performing a fresh checkout from
# revision control

if test "$1" = "-v" -o "$1" = "--verbose" 
then
    verbose="--verbose"
else
    verbose=""
fi

force=
am_force=
# force=--force
# am_force=--force-missing

aclocal -I conf -I gl/m4 $verbose $force
libtoolize --copy --automake $verbose $force
autoconf $verbose $force
autoheader $verbose $force
automake --copy --add-missing $verbose $force
