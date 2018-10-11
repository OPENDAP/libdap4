#!/bin/sh

set -eux

df -h

# git
# cmake 
# emacs 
# zlib-devel 
# bzip2 
# bzip2-devel 
# libjpeg-devel 
# libicu-devel 
# vim 
# bc

PACKAGES="
gcc-c++ 
flex 
bison 
autoconf 
automake 
libtool 
openssl-devel 
libuuid-devel 
readline-devel 
libxml2-devel 
curl-devel 
rpm-devel 
rpm-build 
redhat-rpm-config
"

yum -y install $PACKAGES

df -h
