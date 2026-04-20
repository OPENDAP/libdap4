#!/bin/sh
#######################################################################
#
# This script is a component of a machine used to builds RHEL9 RPM
# binaries using a pre-configured Docker image for rocky9 to provide
# the compile and packaging platform
#
# This script is passed to the docker container as part of a
# docker run command.
#
# In order to run the script inside the docker container, in lieu of
# an entrypoint.sh file, launch the docker build image like this and pass
# in the fully qualified path of this script as it appears in the docker
# container.
#
# In the example below, we mount the directory
# $prefix/rpmbuild onto the /root/rpmbuild directory.
#
# This is because the rpm build software is going to write the rpms
# to ~/rpmbuild and in our Docker image we know that's /root/rpmbuild
#
# Run the script like this:
# docker run \
#    --env prefix=/root/install \
#    --volume $prefix/rpmbuild:/root/rpmbuild \
#    --volume $TRAVIS_BUILD_DIR:/root/travis --env AWS_ACCESS_KEY_ID=$AWS_ACCESS_KEY_ID \
#    --env AWS_SECRET_ACCESS_KEY=$AWS_SECRET_ACCESS_KEY \
#    --env os=rocky8 \
#    --env LIBDAP_BUILD_NUMBER=$LIBDAP_BUILD_NUMBER \
#    opendap/rocky8_hyrax_builder:latest \
#    /root/travis/travis/build-rh8-rpm.sh
#

# e: exit immediately on non-zero exit value from a command
# u: treat unset env vars in substitutions as an error
set -eux

# This script will start with /home as the CWD since that's how the
# centos6/7 hyrax build containers are configured. The PATH will be
# set to include $prefix/bin and $prefix/deps/bin; $prefix will be
# $HOME/install. $HOME is /root for the build container.

echo "Inside the docker container, prefix HOME PATH:"
printenv prefix HOME PATH

# Get the pre-built dependencies (all static libraries). It might be more
# economical to just get and build the deps since all we need for libdap
# is the bison executable. However, using this process might translate to
# the bes build more easily.
#
# These are not needed for CentOS Stream8 for libdap4. Only do this for
# CentOS7 (libdap4 needs a newer version of bison teh C7 provides). jhrg 2/9/22
if test -n $os -a $os = centos7
then
  aws s3 cp s3://opendap.travis.build/hyrax-dependencies-$os-static.tar.gz /tmp/

  # This dumps the dependencies in $HOME/install/deps/{lib,bin,...}. By default
  # our Travis yaml file installs the smaller deps that uses shared libs.
  tar -xzvf /tmp/hyrax-dependencies-$os-static.tar.gz

  ls -lR $HOME/install/deps
fi

if test -n $os -a $os = rocky8
then
  export CPPFLAGS=-I/usr/include/tirpc
  export LDFLAGS=-ltirpc
fi

# cd to the $TRAVIS_BUILD_DIR directory. Note that we make $HOME/travis
# using the docker run --volume option and set it to $TRAVIS_BUILD_DIR.
cd $HOME/travis

# Run autoreconf so the missing, etc., scripts have the correct paths
# for the inside of this container
autoreconf -fiv

# This builds the libdap.spec file with the correct version and build number.
# NB: prefix=$HOME/install

echo "LIBDAP_BUILD_NUMBER: $LIBDAP_BUILD_NUMBER"
./configure --disable-dependency-tracking --prefix=$prefix --with-build=$LIBDAP_BUILD_NUMBER

# Now make the source dist (which will be libdap-version.tar.gz - no build number)
# and will contain the libdap.spec made above with configure. The call to rpmbuild
# will unpack that and run configure _again_. Tedious, but the computer won't complain.
# jhrg 3/23/21
#
# This will leave the package in $HOME/rpmbuild/RPMS/x86_64/*.rpm
make -j16 rpm

# Just a little reassurance... jhrg 3/23/21
ls -l $HOME/rpmbuild/RPMS/x86_64/
