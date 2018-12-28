#!/bin/sh
#
# e: exit immediately on non-zero exit value from a command
# u: treat unset env vars in substitutions as an error
set -eux

echo "env:"
printenv

echo "pwd = `pwd`"

# Get the pre-built dependencies (all static libraries)
(cd /tmp && curl -s -O https://s3.amazonaws.com/opendap.travis.build/hyrax-dependencies-$os-static.tar.gz)

cd $HOME

# This dumps the dependencies in $HOME/install/deps/{lib,bin,...}
tar -xzvf /tmp/hyrax-dependencies-ubuntu14-static.tar.gz

# Get a fresh copy of the sources
git clone https://github.com/opendap/libdap4

cd $TRAVIS_BUILD_DIR

# build (autoreconf; configure, make)
autoreconf -fiv

./configure --disable-dependency-tracking --prefix=$prefix

make deb -j7