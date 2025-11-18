#!/bin/sh

export CPPFLAGS
export LDFLAGS
function loggy(){
    echo  "$@" | awk '{ print "# "$0;}'  >&2
}

# run the script like this:
#     Create a writable target for the rpm files, mount it to /root/rpmbuild
#     mkdir $prefix/rpmbuild
#     docker run
#         --env prefix=/root/install
#         --volume $prefix/rpmbuild:/root/rpmbuild
#         --volume $TRAVIS_BUILD_DIR:/root/libdap4
#         --env AWS_ACCESS_KEY_ID=$AWS_ACCESS_KEY_ID
#         --env AWS_SECRET_ACCESS_KEY=$AWS_SECRET_ACCESS_KEY
#         --env LIBDAP_BUILD_NUMBER=$LIBDAP_BUILD_NUMBER
#         opendap/rocky9_hyrax_builder:latest
#         /root/libdap4/travis/build-rh9-rpm.shdocker run

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

mkdir -vp $HOME/rpmbuild

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
