#!/bin/sh
#######################################################################
#
# This script is a component of a machine used to builds RHEL9 RPM
# binaries using a pre-configured Docker image for rocky9 to provide
# the compile and packaging platform.
#
# This script is passed to the docker container as part of a
# docker run command.
#
# In order to run the script inside the docker container, in lieu of
# an entrypoint.sh file, launch the docker build image like this and pass
# in the fully qualified path of this script as it appears in the docker
# container.
#
# In the example below, we mount the directory $prefix/rpmbuild onto the
# /root/rpmbuild directory.
#
# This is because the rpm build software is going to write the rpms
# to ~/rpmbuild and in our Docker image we know that's /root/rpmbuild
#
# Run the script like this:
#     docker run
#         --env prefix=/root/install
#         --volume $prefix/rpmbuild:/root/rpmbuild
#         --volume $TRAVIS_BUILD_DIR:/root/libdap4
#         --env LIBDAP_BUILD_NUMBER=$LIBDAP_BUILD_NUMBER
#         opendap/rocky9_hyrax_builder:latest
#         /root/libdap4/travis/build-rh9-rpm.sh
#
#
#######################################################################
function loggy(){
    echo  "$@" | awk '{ print "# "$0;}'  >&2
}


export CPPFLAGS
export LDFLAGS

# e: exit immediately on non-zero exit value from a command
# u: treat unset env vars in substitutions as an error
set -eu

# This script will start with /home as the CWD since that's how the
# hyrax build containers are configured. The PATH will be
# set to include $prefix/bin and $prefix/deps/bin; $prefix will be
# $HOME/install. $HOME is /root for the build container.
loggy "#######################################################################"
loggy "Inside the docker container. Some ENV vars:"
loggy "LIBDAP_BUILD_NUMBER: $LIBDAP_BUILD_NUMBER"
loggy "             prefix: $prefix"
loggy "               HOME: $HOME"
loggy "               PATH: $PATH"

# mkdir -vp $HOME/rpmbuild

# Using the ${CPPFLAGS:-""} form to ensure that we get the empty string and not
# an unbound variable error if CPPFLAGS is not set.
CPPFLAGS="${CPPFLAGS:-""} -I/usr/include/tirpc"
# ibid
LDFLAGS="${LDFLAGS:-""} -ltirpc"
loggy "Added tirpc libraries to CPPFLAGS LDFLAGS"
loggy "           CPPFLAGS: $CPPFLAGS"
loggy "            LDFLAGS: $LDFLAGS"

# cd to the $TRAVIS_BUILD_DIR directory. Note that we make $HOME/travis
# using the docker run --volume option and set it to $TRAVIS_BUILD_DIR.
cd "$HOME/libdap4"
loggy "                PWD: $PWD"

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
make -j16 rpm

# Just a little reassurance... jhrg 3/23/21
loggy "$(ls -l $HOME/rpmbuild/RPMS/x86_64/)"
