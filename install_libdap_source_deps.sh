#!/bin/sh

set -e

# check to see if bison folder is empty
if test ! -d "$HOME/bison/bin"
then
  wget http://ftp.gnu.org/gnu/bison/bison-3.0.4.tar.gz
  tar -xzf bison-3.0.4.tar.gz
  cd bison-3.0.4 && ./configure --prefix=$HOME/bison && make -j 4 && make install
else
  echo 'Using cached bison directory.';
fi
