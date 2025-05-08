#!/bin/bash
#
# Usage: sonar-scanner-coverage.sh [sonar|text|html]
#
# If no argument is given, write the sonarqube xml file
# Note: there are other optional output formats.
# See https://gcovr.com/en/stable/index.html

if test -z "$1" -o "$1" = "sonar"
then
    output="--sonarqube coverage.xml"
elif test "$1" = "html"
then
    output="--html coverage.html"
elif test "$1" = "text"
then
    output="--output coverage.txt"
fi

gcovr --config gcovr.config $output

