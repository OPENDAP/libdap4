#!/bin/sh

tmpfile=$(mktemp /tmp/abi_check.XXXXXX)
abidiff "$@" > "${tmpfile}"
abidiff_rc=$?
cat ${tmpfile} | grep "SONAME changed from" -q
soname_rc=$?
if test ${soname_rc} -eq 0 ; then
	abidiff_rc=$(( abidiff_rc & 3 ))
fi
cat "${tmpfile}"
rm "${tmpfile}"
exit $abidiff_rc
