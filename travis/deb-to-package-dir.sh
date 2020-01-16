#!/bin/bash
#
# Part of the deployment process managed by Travis CI. This code copies RPM
# packages to a 'package' directory. It also duplicates them, making a set
# of RPMs labeled 'snapshot.'

set -e

# 'prefix' and 'TRAVIS_BUILD_DIR' are in the environment
DIST=amd64.deb
PKG_NAME=libdap

# Copied the RPM files with version numbers.
cp ${prefix}/debbuild/${PKG_NAME}_*_amd64.deb ${TRAVIS_BUILD_DIR}/package/

# Now make a second set of copies with 'snapshot' in place of the version
ver=`basename ${prefix}/debbuild/${PKG_NAME}_*_${DIST} | sed -e "s|${PKG_NAME}_||g" -e "s|_${DIST}b||g"`;
for file in ${prefix}/debbuild/${PKG_NAME}_*_${DIST}
do
    echo "Updating ${PKG_NAME} ${DIST} snapshot with ${file}"
    snap=`basename ${file} | sed "s|${ver}|snapshot|g"`
    cp ${file} ${TRAVIS_BUILD_DIR}/package/${snap}
done

exit 0
