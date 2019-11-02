#!/bin/sh
#
# e: exit immediately on non-zero exit value from a command
# u: treat unset env vars in substitutions as an error
set -eux

echo "env:"
printenv

echo "pwd = `pwd`"

apt-get install -y python3-pip

pip3 install awscli

# Get the pre-built dependencies (all static libraries). The env vars $os along with the
# AWS public and secret key are put in the environment using 'docker run --env ...'
# jhrg 11/1/19
aws s3 cp s3://opendap.travis.build/hyrax-dependencies-$os-static.tar.gz /tmp/

# (cd /tmp && curl -s -O https://s3.amazonaws.com/opendap.travis.build/hyrax-dependencies-$os-static.tar.gz)

cd $HOME

# This dumps the dependencies in $HOME/install/deps/{lib,bin,...}
tar -xzvf /tmp/hyrax-dependencies-$os-static.tar.gz

# Get a fresh copy of the sources
git clone https://github.com/opendap/libdap4

cd $HOME/libdap4

# build (autoreconf; configure, make)
autoreconf -fiv

./configure --disable-dependency-tracking --prefix=$prefix

make deb -j7
