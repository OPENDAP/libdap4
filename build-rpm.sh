#!/bin/sh
#

# run the script like (with the obvious changes for CentOS7):
# docker run -e os=centos6 -v $prefix/centos6/rpmbuild:/root/rpmbuild -v `pwd`:/root/travis 
# opendap/centos6_hyrax_builder:1.1 /root/travis/build-rpm.sh 

# e: exit immediately on non-zero exit value from a command
# u: treat unset env vars in substitutions as an error
set -eux

# This script will start with /home as the CWD since that's how the 
# centos6/7 hyrax build containers are configured. The PATH will be 
# set to include $prefix/bin and $prefix/deps/bin; $prefix will be
# $HOME/install. $HOME is /root for the build container.

echo "env:"
printenv

echo "HOME: ${HOME}"

# Get the pre-built dependencies (all static libraries)
aws s3 cp s3://opendap.travis.build/hyrax-dependencies-$os-static.tar.gz /tmp/

# This dumps the dependencies in $HOME/install/deps/{lib,bin,...}
tar -xzvf /tmp/hyrax-dependencies-$os-static.tar.gz

# Get a fresh copy of the sources
# git clone https://github.com/opendap/libdap4

# cd $HOME/libdap4

# build (autoreconf; configure, make)
autoreconf -fiv

echo "LIBDAP_BUILD_NUMBER is $LIBDAP_BUILD_NUMBER"

./configure --disable-dependency-tracking --prefix=$prefix --with-build=$LIBDAP_BUILD_NUMBER

# This will leave the package in $HOME/rpmbuild/RPMS/x86_64/*.rpm
make -j4 rpm

ls -l $HOME/rpmbuild/RPMS/x86_64/

