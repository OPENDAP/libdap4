#!/bin/sh
#
# remove stuff that accumulates between runs of the HTTPCacheTest unit tests.

cp -r dods_cache/.svn svn-t
rm -rf dods_cache
mkdir dods_cache
mkdir dods_cache/1306
cp dot.index dods_cache/.index
mv svn-t dods_cache/.svn

rm -rf gc_cache
rm -rf purge_cache
rm -rf header_cache
rm -rf interrupt_cache
 