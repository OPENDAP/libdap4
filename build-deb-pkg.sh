#!/bin/bash
#
# Build a debian package.
# 
# This is not a 'real' debian package build - that will take more work.
# See https://www.debian.org/doc/manuals/maint-guide/dreq.en.html for more
# info. The initent of this script is to get a libdap.deb fileso that we
# can use apt to install a binary libdap during the install phase of the
# BES travis-ci build.

package=libdap

version=`awk -f get-version.awk < DEBIAN/control`

deb_dir=${package}_${version}

# First, make the destination directory

mkdir $deb_dir

# copy the control file

cp -r DEBIAN $deb_dir

# run configure so that 'make install' will use the correct names
# within 'deb_dir'

./configure --prefix=$deb_dir/usr --exec-prefix=$deb_dir/usr \
    --sysconfdir=$deb_dir/etc

# build and install; assume the this is only being run for a build
# that's know to work

make -j9 install

# now, the debian magic

dpkg-deb --build $deb_dir
