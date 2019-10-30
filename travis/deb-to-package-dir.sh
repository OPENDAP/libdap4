#!/bin/bash
#
# Part of the deployment process managed by Travis CI. This code copies RPM
# packages to a 'package' directory. It also duplicates them, making a set
# of RPMs labeled 'snapshot.'

set -e

# 'prefix' and 'TRAVIS_BUILD_DIR' are in the environment

# Copied the RPM files with version numbers.
cp ${prefix}/debbuild/libdap_*_amd64.deb ${TRAVIS_BUILD_DIR}/package/

# Now make a second set of copies with 'snapshot' in place of the version
ver=`basename ${file} | sed -e "s|libdap_||g" -e "s|_amd64.deb||g"`;
for file in ${prefix}/debbuild/libdap_*_amd64.deb
do
    echo "Updating ${PKG_NAME} ${DIST} snapshot with ${file}"
    snap=`basename ${file} | sed "s|${ver}|snapshot|g"`
    cp ${file} ${TRAVIS_BUILD_DIR}/package/${snap}
done

exit 0
