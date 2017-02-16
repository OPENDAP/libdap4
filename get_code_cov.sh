#!/bin/bash
for filename in `find . | egrep '\.cc$|\.cpp$'`; 
do 
  gcov -n -o . $filename > /dev/null; 
done
