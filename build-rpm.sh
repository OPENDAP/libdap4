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

# Get the pre-built dependencies (all static libraries). It might be more
# economical to just get and build the deps since all we need for libdap
# is the bison executable. However, using this process might translate to
# the bes build more easily.
aws s3 cp s3://opendap.travis.build/hyrax-dependencies-$os-static.tar.gz /tmp/

# This dumps the dependencies in $HOME/install/deps/{lib,bin,...}. By default
# our Travis yaml file installs the smaller deps that uses shared libs.
tar -xzvf /tmp/hyrax-dependencies-$os-static.tar.gz

ls -lR $HOME/install/deps

# cd to the $TRAVIS_BUILD_DIR directory. Note that we make $HOME/travis
# using the docker run --volume option and set it to $TRAVIS_BUILD_DIR.
cd $HOME/travis

# Run autoreconf so the missing, etc., scripts have the correct paths
# for the inside of this container
autoreconf -fiv

# This builds the libdap.spec file with the correct version and build number.
# NB: prefix=$HOME/install
./configure --disable-dependency-tracking --prefix=$prefix --with-build=$LIBDAP_BUILD_NUMBER

# Now make the source dist (which will be libdap-version.tar.gz - no build number)
# and will contain the libdap.spec made above with configure. The call to rpmbuild
# will unpack that and run configure _again_. Tedious, but the computer won't complain.
# jhrg 3/23/21
#
# This will leave the package in $HOME/rpmbuild/RPMS/x86_64/*.rpm
make -j4 rpm

# Just a little reassurance... jhrg 3/23/21
ls -l $HOME/rpmbuild/RPMS/x86_64/
