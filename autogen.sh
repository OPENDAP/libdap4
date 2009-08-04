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

aclocal --force -I conf -I gl/m4 $verbose
libtoolize --force --copy  $verbose
autoconf --force $verbose
autoheader --force  $verbose
automake --force-missing --copy --force-missing $verbose
