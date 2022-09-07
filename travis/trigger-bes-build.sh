#!/bin/bash
#
# Once a new set of packages for libdap are pushed to S3, trigger a BES
# build.
#
# How the build numbering system works: The Travis-based build system
# will trigger a build of the RPM packages so that they are named using
# a version number, based on the three-digit number set in the configure.ac
# file using the AC_INIT macro (e.g., AC_INIT(libdap, 3.20.7, ...)) and a
# build number that is set when the configure script is run.

# In a 'normal' run of configure, the version number value is bound to
# the symbol PACKAGE_VERSION. If, however, configure is run using the
# --build-number option, that integer number is appended to the value
# of PACKAGE_VERSION, so that the value is "x.y.z-build". Since the
# configure script _builds_ the libdap_VERSION file, when configure
# is run using the --build-number option, the build number is part of
# the libdap_VERSION file and will be passed into the BES using the
# libdap4-snapshot file that is part of the bES repo.
#
# Since the build number is set by configure, various packaging steps
# in teh Travis.yml file must run configure using this option. The build
# number is derived by taking the absolute build number from Travis
# and subtracting the absolute number when the three-digit version number
# was last incremented.

set -e

git config --global user.name "The-Robot-Travis"
git config --global user.email "npotter@opendap.org"

echo "New snapshot of libdap4 has been pushed. Triggering a BES build..."

libdap4_version_num=$(cat ./libdap_VERSION)
echo "libdap4_version_num: ${libdap4_version_num}"  >&2

LIBDAP4_SNAPSHOT="libdap4-${libdap4_version_num} $(date \"+%FT%T%z\")"
echo "libdap4-snapshot record: ${LIBDAP4_SNAPSHOT}"  >&2

echo "Tagging libdap with version ${libdap4_version_num}"
git tag -m "libdap4-${libdap4_version_num}" -a "${libdap4_version_num}"
git push origin "${libdap4_version_num}"

git clone --depth 1 https://github.com/opendap/bes

cd bes
git checkout master

echo "${LIBDAP4_SNAPSHOT}" > libdap4-snapshot

cat libdap4-snapshot  >&2

# Bounding the commit message with the " character allows use to include
# new line stuff for easy commit message readability later.
git commit -am \
"libdap4: Triggering BES build for snapshot production.
Build Version Matrix:
${LIBDAP4_SNAPSHOT}
";

git push https://$GIT_UID:$GIT_PSWD@github.com/opendap/bes --all;
