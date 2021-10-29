#!/bin/bash

make coverage
find . -wholename \*/.libs/\*.gcov -a ! -name \*\#\* -a ! -name \*.hh.gcov > gcov-files.txt
mkdir -p gcov-reports
echo "------------------------------------------"
cat gcov-files.txt
echo "------------------------------------------"
# gcov-reports must be configured in sonar-project.properties. jhrg 11/24/20
gcov_dir="gcov-reports"
mkdir -p "${gcov_dir}"
for gcov_file in ${gcov_files}
do
    reports_dir="gcov-reports"
    bname=`basename "${gcov_file}"`
    subdir_name=`dirname "${gcov_file}" | sed -e "s|\.libs||g" -e "s|\./||g"`
    if test -n "${subdir_name}"
    then
        # echo "Found SubDirectory: ${subdir_name}";
        reports_dir="${reports_dir}/${subdir_name}"
        mkdir -p "${reports_dir}"
    fi
    echo "gcov_file: ${gcov_file}"
    echo "reports_dir: ${reports_dir}"
    mv "${gcov_file}" "${reports_dir}"
done

