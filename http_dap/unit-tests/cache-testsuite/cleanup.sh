#!/bin/sh
#
# remove stuff that accumulates between runs of the HTTPCacheTest unit tests.

# sometimes have trouble deleting dods_cache during distcheck
if [ -d dods_cache ]
then
    chmod -R +w dods_cache
fi
rm -rf dods_cache

# initialize the dods_cache directory
cp -r ./dods_cache_init dods_cache
# make sure we can write to the directory and sub directories
chmod -R +w dods_cache

# remove generated cache directories
rm -rf gc_cache
rm -rf purge_cache
rm -rf header_cache
rm -rf interrupt_cache
rm -rf singleton_cache
