#!/bin/bash
reports_dir="gcov-reports"

make coverage
find . -wholename \*/.libs/\*.gcov -a ! -name \*\#\* -a ! -name \*.hh.gcov > gcov-files.txt
mkdir -p ""${reports_dir}""
echo "#----------------------------------------------------------------------"
echo "# gcov files detected:"
cat gcov-files.txt
echo "#----------------------------------------------------------------------"
# gcov-reports must be configured in sonar-project.properties. jhrg 11/24/20
mkdir -p "${gcov_dir}"
for gcov_file in ${gcov_files}
do
    target_dir="${reports_dir}"
    bname=`basename "${gcov_file}"`
    subdir_name=`dirname "${gcov_file}" | sed -e "s|\.libs||g" -e "s|\./||g"`
    if test -n "${subdir_name}"
    then
        # echo "Found SubDirectory: ${subdir_name}";
        target_dir="${reports_dir}/${subdir_name}"
        mkdir -p "${target_dir}"
    fi
    echo "Adding gcov_file: ${gcov_file} to target_dir: ${target_dir}"
    mv "${gcov_file}" "${target_dir}"
done
echo "#----------------------------------------------------------------------"
echo "# gcov reports: "
ls -lRa "${gcov_dir}"
echo "#----------------------------------------------------------------------"

