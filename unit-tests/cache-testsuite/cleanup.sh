#!/bin/sh
#
# remove stuff that accumulates between runs of the HTTPCacheTest unit tests.

rm -rf dods_cache
svn up dods_cache

rm -rf gc_cache
rm -rf purge_cache
rm -rf header_cache
rm -rf interrupt_cache
 