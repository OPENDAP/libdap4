#!/bin/awk
#
# Get the version info from a debian control file

function ltrim(s) { sub(/^[ \t\r\n]+/, "", s); return s }
function rtrim(s) { sub(/[ \t\r\n]+$/, "", s); return s }
function trim(s) { return rtrim(ltrim(s)); }
BEGIN { FS=":" }
/Version/ { print trim($2) }
