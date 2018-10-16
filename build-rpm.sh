#!/bin/sh
#

# run the script like:
# docker run --volume $prefix:/home/install --volume /home/rpmbuild:/home/rpmbuild
#            --volume $TRAVIS_BUILD_DIR:/home/libdap4 centos7_hyrax_builder /home/libdap4/build-rpm.sh

set -eux

# This script will start with /home as the CWD since that's how the 
# centos6/7 hyrax build containers are configured. The PATH will be 
# set to include $prefix/bin and $prefix/deps/bin; $prefix will be
# /root/install. $HOME is /root for the build container.

printenv
pwd

# load the correct hyrax-dependencies into $HOME/install (i.e., $prefix)
# The packages ae 
(cd /tmp && curl -s -O https://s3.amazonaws.com/opendap.travis.build/hyrax-dependencies-centos7-static.tar.gz) 
tar -C $HOME -xzvf /tmp/hyrax-dependencies-centos7-static.tar.gz

ls -lR /root

# This will leave the package in $HOME/rpmbuild/RPMS/x86_64/*.rpm
(cd $HOME/libdap4 && ./configure --disable-dependency-tracking --prefix=$prefix && make -j4 rpm)
