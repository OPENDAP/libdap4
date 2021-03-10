#!/bin/bash
#
# Given that the BES has just pushed a new set of packages, built with the libdap
# RPMs, grab those and use them to make a new set of Docker containers. The
# hyrax-docker git repo runs its own build to do this (and can be triggered
# separately).

set -e

echo "-- -- -- -- -- -- -- -- -- after_deploy BEGIN -- -- -- -- -- -- -- -- --"

echo "New CentOS-7 snapshot of libdap4 has been pushed. Triggering a BES build..."

git clone --depth 1 https://github.com/opendap/bes
git config --global user.name "The-Robot-Travis"
git config --global user.email "npotter@opendap.org"

cd bes
git checkout master

LIBDAP4_SNAPSHOT="libdap4-<version.build> "`date "+%FT%T%z"`

echo "libdap4-snapshot record: ${LIBDAP_SNAPSHOT}"  >&2
echo "${LIBDAP4_SNAPSHOT}" > libdap4-snapshot

cat libdap4-snapshot  >&2

git commit -am "${LIBDAP4_SNAPSHOT} Triggering BES build for snapshots.";
git push https://$GIT_UID:$GIT_PSWD@github.com/opendap/bes --all;

echo "-- -- -- -- -- -- -- -- -- after_deploy END -- -- -- -- -- -- -- -- --"
