#!/bin/bash
#
# Part of the deployment process managed by Travis CI. This code copies RPM
# packages to a 'package' directory. It also duplicates them, making a set
# of RPMs labeled 'snapshot.'

set -e

# DIST should be one of el6, el7, or el8
DIST=${1}
PKG_NAME=libdap

# 'prefix' and 'TRAVIS_BUILD_DIR' are in the environment

BUILD_RPM_FILES="${prefix}/rpmbuild/RPMS/x86_64/*"

# Copied the RPM files with version numbers.
cp ${BUILD_RPM_FILES} ${TRAVIS_BUILD_DIR}/package/

# Now make a second set of copies with 'snapshot' in place of the version

ver=`basename ${prefix}/rpmbuild/RPMS/x86_64/${PKG_NAME}-[-0-9.]*.rpm | sed -e "s|${PKG_NAME}-||g" -e "s|.${DIST}.x86_64.rpm||g"`

for file in ${BUILD_RPM_FILES}
do
    echo "Updating ${PKG_NAME} ${DIST} snapshot with ${file}"
    snap=`basename ${file} | sed "s|${ver}|snapshot|g"`
    cp ${file} ${TRAVIS_BUILD_DIR}/package/${snap}
done

exit 0
