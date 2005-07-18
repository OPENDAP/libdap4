#!/bin/sh
#
# remove stuff that accumulates between runs of the HTTPCacheTest unit tests.

rm -rf dods_cache
cp -r dods_cache_init dods_cache
#svn up dods_cache

rm -rf gc_cache
rm -rf purge_cache
rm -rf header_cache
rm -rf interrupt_cache
 
rm -rf singleton_cache
