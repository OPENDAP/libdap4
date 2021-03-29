#!/bin/bash
#
# Once a new set of packages for libdap are pushed to S3, trigger a BES
# build.

set -e

echo "New CentOS-7 snapshot of libdap4 has been pushed. Triggering a BES build..."

LIBDAP4_SNAPSHOT="libdap4-`cat VERSION` `date \"+%FT%T%z\"`"
echo "libdap4-snapshot record: ${LIBDAP_SNAPSHOT}"  >&2

git clone --depth 1 https://github.com/opendap/bes
git config --global user.name "The-Robot-Travis"
git config --global user.email "npotter@opendap.org"

cd bes
git checkout master

echo "${LIBDAP4_SNAPSHOT}" > libdap4-snapshot

cat libdap4-snapshot  >&2

git commit -am "${LIBDAP4_SNAPSHOT} Triggering BES build for snapshots.";
git push https://$GIT_UID:$GIT_PSWD@github.com/opendap/bes --all;
