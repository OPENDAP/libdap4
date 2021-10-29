#!/bin/bash

make coverage
find . -wholename \*/.libs/\*.gcov -a ! -name \*\#\* -a ! -name \*.hh.gcov > gcov-files.txt
mkdir -p gcov-reports
echo "------------------------------------------"
cat gcov-files.txt
echo "------------------------------------------"
ls -l gcov-reports
# gcov-reports must be configured in sonar-project.properties. jhrg 11/24/20
mv -f $(cat gcov-files.txt) gcov-reports/
