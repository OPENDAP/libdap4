
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

//#define DODS_DEBUG
// #define DODS_DEBUG2
#undef USE_GETENV

#include <pthread.h>
#include <limits.h>
#include <unistd.h>   // for stat
#include <sys/types.h>  // for stat and mkdir
#include <sys/stat.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <set>

#include "Error.h"
#include "InternalErr.h"
#include "ResponseTooBigErr.h"
#ifndef WIN32
#include "SignalHandler.h"
#endif
#include "HTTPCacheInterruptHandler.h"
#include "HTTPCacheTable.h"
#include "HTTPCache.h"
#include "HTTPCacheMacros.h"

#include "util_mit.h"
#include "debug.h"

using namespace std;

namespace libdap {

HTTPCache *HTTPCache::_instance = 0;

// instance_mutex is used to ensure that only one instance is created.
// That is, it protects the body of the HTTPCache::instance() method. This
// mutex is initialized from within the static function once_init_routine()
// and the call to that takes place using pthread_once_init() where the mutex
// once_block is used to protect that call. All of this ensures that no matter
// how many threads call the instance() method, only one instance is ever
// made.
static pthread_mutex_t instance_mutex;
static pthread_once_t once_block = PTHREAD_ONCE_INIT;


#define NO_LM_EXPIRATION 24*3600 // 24 hours

#define DUMP_FREQUENCY 10 // Dump index every x loads

#define MEGA 0x100000L
#define CACHE_TOTAL_SIZE 20 // Default cache size is 20M
#define CACHE_FOLDER_PCT 10 // 10% of cache size for metainfo etc.
#define CACHE_GC_PCT 10  // 10% of cache size free after GC
#define MIN_CACHE_TOTAL_SIZE 5 // 5M Min cache size
#define MAX_CACHE_ENTRY_SIZE 3 // 3M Max size of single cached entry

static void
once_init_routine()
{
    int status;
    status = INIT(&instance_mutex);

    if (status != 0)
        throw InternalErr(__FILE__, __LINE__, "Could not initialize the HTTP Cache mutex. Exiting.");
}

/** Get a pointer to the HTTP 1.1 compliant cache. If not already
    instantiated, this creates an instance of the HTTP cache object and
    initializes it to use \c cache_root as the location of the persistent
    store. If there's an index (\c .index) file in that directory, it is read
    as part of the initialization. If the cache has already been initialized,
    this method returns a pointer to that instance. Note HTTPCache uses the
    singleton pattern; A process may have only one instance of this object.
    Also note that HTTPCache is MT-safe. However, if the \c force parameter
    is set to true, it may be possible for two or more processes to access
    the persistent store at the same time resulting in undefined behavior.

    Default values: is_cache_enabled(): true, is_cache_protected(): false,
    is_expire_ignored(): false, the total size of the cache is 20M, 2M of that
    is reserved for response headers, during GC the cache is reduced to at
    least 18M (total size - 10% of the total size), and the max size for an
    individual entry is 3M. It is possible to change the size of the cache,
    but not to make it smaller than 5M. If expiration information is not sent
    with a response, it is assumed to expire in 24 hours.

    @param cache_root The fully qualified pathname of the directory which
    will hold the cache data (i.e., the persistent store).
    @param force Force access to the persistent store if true. By default
    false. Use this only if you're sure no one else is using the same cache
    root! This is included so that programs may use a cache that was
    left in an inconsistent state.
    @return A pointer to the HTTPCache object.
    @exception Error thrown if the cache root cannot set. */

HTTPCache *
HTTPCache::instance(const string &cache_root, bool force)
{
    int status = pthread_once(&once_block, once_init_routine);
    if (status != 0)
	throw InternalErr(__FILE__, __LINE__, "Could not initialize the HTTP Cache mutex. Exiting.");

    LOCK(&instance_mutex);

    DBG(cerr << "Entering instance(); (" << hex << _instance << dec << ")"
	    << "... ");

    try {
        if (!_instance) {
            _instance = new HTTPCache(cache_root, force);

            DBG(cerr << "New instance: " << _instance << ", cache root: "
                << _instance->d_cache_root << endl);

            atexit(delete_instance);

#ifndef WIN32
            // Register the interrupt handler. If we've already registered
            // one, barf. If this becomes a problem, hack SignalHandler so
            // that we can chain these handlers... 02/10/04 jhrg
            //
            // Technically we're leaking memory here. However, since this
            // class is a singleton, we know that only three objects will
            // ever be created and they will all exist until the process
            // exits. We can let this slide... 02/12/04 jhrg
            EventHandler *old_eh = SignalHandler::instance()->register_handler
                                   (SIGINT, new HTTPCacheInterruptHandler);
            if (old_eh) {
                SignalHandler::instance()->register_handler(SIGINT, old_eh);
                throw SignalHandlerRegisteredErr(
                    "Could not register event handler for SIGINT without superseding an existing one.");
            }

            old_eh = SignalHandler::instance()->register_handler
                     (SIGPIPE, new HTTPCacheInterruptHandler);
            if (old_eh) {
                SignalHandler::instance()->register_handler(SIGPIPE, old_eh);
                throw SignalHandlerRegisteredErr(
                    "Could not register event handler for SIGPIPE without superseding an existing one.");
            }

            old_eh = SignalHandler::instance()->register_handler
                     (SIGTERM, new HTTPCacheInterruptHandler);
            if (old_eh) {
                SignalHandler::instance()->register_handler(SIGTERM, old_eh);
                throw SignalHandlerRegisteredErr(
                    "Could not register event handler for SIGTERM without superseding an existing one.");
            }
#endif
        }
    }
    catch (...) {
        DBG2(cerr << "The constructor threw an Error!" << endl);
        UNLOCK(&instance_mutex);
        throw;
    }

    UNLOCK(&instance_mutex);
    DBGN(cerr << "returning " << hex << _instance << dec << endl);

    return _instance;
}

/** This static method is called using atexit(). It deletes the singleton;
    see ~HTTPCache for all that implies. */

void
HTTPCache::delete_instance()
{
    DBG(cerr << "Entering delete_instance()..." << endl);
    if (HTTPCache::_instance) {
        DBG(cerr << "Deleting the cache: " << HTTPCache::_instance << endl);
        delete HTTPCache::_instance;
        HTTPCache::_instance = 0;
    }

    DBG(cerr << "Exiting delete_instance()" << endl);
}

/** Create an instance of the HTTP 1.1 compliant cache. This initializes the
    both the cache root and the path to the index file. It then reads the
    cache index file if one is present.

    A private method.

    @note This assumes that the cache directory structure should be created!
    @param cache_root The fully qualified pathname of the directory which
    will hold the cache data.
    @param force Force access to the persistent store!
    @exception Error Thrown if the single user/process lock for the
    persistent store cannot be obtained.
    @see cache_index_read */

HTTPCache::HTTPCache(string cache_root, bool force) :
        d_locked_open_file(0),
        d_cache_enabled(false),
        d_cache_protected(false),
        d_expire_ignored(false),
        d_always_validate(false),
        d_total_size(CACHE_TOTAL_SIZE * MEGA),
        d_folder_size(CACHE_TOTAL_SIZE / CACHE_FOLDER_PCT),
        d_gc_buffer(CACHE_TOTAL_SIZE / CACHE_GC_PCT),
        d_max_entry_size(MAX_CACHE_ENTRY_SIZE * MEGA),
        d_default_expiration(NO_LM_EXPIRATION),
        d_max_age(-1),
        d_max_stale(-1),
        d_min_fresh(-1),
        d_http_cache_table(0)
{
    DBG(cerr << "Entering the constructor for " << this << "... ");
#if 0
	int status = pthread_once(&once_block, once_init_routine);
	if (status != 0)
		throw InternalErr(__FILE__, __LINE__, "Could not initialize the HTTP Cache mutex. Exiting.");
#endif
	INIT(&d_cache_mutex);

	// This used to throw an Error object if we could not get the
	// single user lock. However, that results in an invalid object. It's
	// better to have an instance that has default values. If we cannot get
	// the lock, make sure to set the cache as *disabled*. 03/12/03 jhrg
	//
	// I fixed this block so that the cache root is set before we try to get
	// the single user lock. That was the fix for bug #661. To make that
	// work, I had to move the call to create_cache_root out of
	// set_cache_root(). 09/08/03 jhrg

	set_cache_root(cache_root);
	int block_size;

	if (!get_single_user_lock(force))
	    throw Error("Could not get single user lock for the cache");

#ifdef WIN32
	//  Windows is unable to provide us this information.  4096 appears
	//  a best guess.  It is likely to be in the range [2048, 8192] on
	//  windows, but will the level of truth of that statement vary over
	//  time ?
	block_size = 4096;
#else
	struct stat s;
	if (stat(cache_root.c_str(), &s) == 0)
		block_size = s.st_blksize;
	else
		throw Error("Could not set file system block size.");
#endif
	d_http_cache_table = new HTTPCacheTable(d_cache_root, block_size);
	d_cache_enabled = true;

	DBGN(cerr << "exiting" << endl);
}

/** Destroy an instance of HTTPCache. This writes the cache index and frees
    the in-memory cache table structure. The persistent cache (the response
    headers and bodies and the index file) are not removed. To remove those,
    either erase the directory that contains the cache using a file system
    command or use the purge_cache() method (which leaves the cache directory
    structure in place but removes all the cached information).

    This class uses the singleton pattern. Clients should \e never call this
    method. The HTTPCache::instance() method arranges to call the
    HTTPCache::delete_instance() using \c atexit(). If delete is called more
    than once, the result will likely be an index file that is corrupt. */

HTTPCache::~HTTPCache()
{
    DBG(cerr << "Entering the destructor for " << this << "... ");

    try {
        if (startGC())
            perform_garbage_collection();

        d_http_cache_table->cache_index_write();
    }
    catch (Error &e) {
        // If the cache index cannot be written, we've got problems. However,
        // unless we're debugging, still free up the cache table in memory.
        // How should we let users know they cache index is not being
        // written?? 10/03/02 jhrg
        DBG(cerr << e.get_error_message() << endl);
    }

    delete d_http_cache_table;

    release_single_user_lock();

    DBGN(cerr << "exiting destructor." << endl);
    DESTROY(&d_cache_mutex);
}


/** @name Garbage collection
    These private methods manage the garbage collection tasks for the cache. */
//@{

/** Enough removed from cache? A private method.
    @return True if enough has been removed from the cache. */

bool
HTTPCache::stopGC() const
{
    return (d_http_cache_table->get_current_size() + d_folder_size < d_total_size - d_gc_buffer);
}

/** Is there too much in the cache. A private method.

    @todo Modify this method so that it does not count locked entries. See
    the note for hits_gc().
    @return True if garbage collection should be performed. */

bool
HTTPCache::startGC() const
{
    DBG(cerr << "startGC, current_size: " << d_http_cache_table->get_current_size() << endl);
    return (d_http_cache_table->get_current_size() + d_folder_size > d_total_size);
}

/** Perform garbage collection on the cache. First, all expired responses are
    removed. Then, if the size of the cache is still too large, the cache is
    scanned for responses larger than the max_entry_size property. At the
    same time, responses are removed based on the number of cache hits. This
    process continues until the size of the cache has been reduced to 90% of
    the max_size property value. Once the garbage collection is complete,
    update the index file. Note that locked entries are not removed!

    A private method.

    @see stopGC
    @see expired_gc
    @see hits_gc */

void
HTTPCache::perform_garbage_collection()
{
    DBG(cerr << "Performing garbage collection" << endl);

    // Remove all the expired responses.
    expired_gc();

    // Remove entries larger than max_entry_size.
    too_big_gc();

    // Remove entries starting with zero hits, 1, ..., until stopGC()
    // returns true.
    hits_gc();
}

/** Scan the current cache table and remove anything that has expired. Don't
    remove locked entries.

    A private method. */

void
HTTPCache::expired_gc()
{
    if (!d_expire_ignored) {
        d_http_cache_table->delete_expired_entries();
    }
}

/** Scan the cache for entires that are larger than max_entry_size. Also
    start removing entires with low hit counts. Start looking for entries
    with zero hits, then one, and so on. Stop when the method stopGC returns
    true. Locked entries are never removed.

    @note Potential infinite loop. What if more than 80% of the cache holds
    entries that are locked? One solution is to modify startGC() so that it
    does not count locked entries.

    @todo Change this method to that it looks at the oldest entries first,
    using the CacheEntry::date to determine entry age. Using the current
    algorithm it's possible to remove the latest entry which is probably not
    what we want.

    A private method. */

void
HTTPCache::hits_gc()
{
    int hits = 0;

    if (startGC()) {
		while (!stopGC()) {
			d_http_cache_table->delete_by_hits(hits);
			hits++;
		}
	}
}

/** Scan the current cache table and remove anything that has is too big.
 	Don't remove locked entries.

    A private method. */
void HTTPCache::too_big_gc() {
	if (startGC())
		d_http_cache_table->delete_by_size(d_max_entry_size);
}

//@} End of the garbage collection methods.

/** Lock the persistent store part of the cache. Return true if the cache lock
    was acquired, false otherwise. This is a single user cache, so it
    requires locking at the process level.

    A private method.

    @param force If True force access to the persistent store. False by
    default.
    @return True if the cache was locked for our use, False otherwise. */

bool HTTPCache::get_single_user_lock(bool force) 
{
    if (!d_locked_open_file) {
	FILE * fp = NULL;

	try {
	    // It's OK to call create_cache_root if the directory already
	    // exists.
	    create_cache_root(d_cache_root);
	}
	catch (Error &e) {
	    // We need to catch and return false because this method is
	    // called from a ctor and throwing at this point will result in a
	    // partially constructed object. 01/22/04 jhrg
	    DBG(cerr << "Failure to create the cache root" << endl);
	    return false;
	}

	// Try to read the lock file. If we can open for reading, it exists.
	string lock = d_cache_root + CACHE_LOCK;
	if ((fp = fopen(lock.c_str(), "r")) != NULL) {
	    int res = fclose(fp);
	    if (res) {
		DBG(cerr << "Failed to close " << (void *)fp << endl);
	    }
	    if (force)
		REMOVE(lock.c_str());
	    else
		return false;
	}

	if ((fp = fopen(lock.c_str(), "w")) == NULL) {
	    DBG(cerr << "Could not open for write access" << endl);
	    return false;
	}

	d_locked_open_file = fp;
	return true;
    }

    cerr << "locked_open_file is true" << endl;
    return false;
}

/** Release the single user (process) lock. A private method. */

void
HTTPCache::release_single_user_lock()
{
    if (d_locked_open_file) {
        int res = fclose(d_locked_open_file);
        if (res) {
            DBG(cerr << "Failed to close " << (void *)d_locked_open_file << endl) ;
        }
        d_locked_open_file = 0;
    }

    string lock = d_cache_root + CACHE_LOCK;
    REMOVE(lock.c_str());
}

/** @name Accessors and Mutators for various properties. */
//@{

/** Get the current cache root directory.
    @return A string that contains the cache root directory. */

string
HTTPCache::get_cache_root() const
{
    return d_cache_root;
}


/** Create the cache's root directory. This is the persistent store used by
    the cache. Paths must always end in DIR_SEPARATOR_CHAR.

    A private method.

    @param cache_root The pathname to the desired cache root directory.
    @exception Error Thrown if the given pathname cannot be created. */

void
HTTPCache::create_cache_root(const string &cache_root)
{
    struct stat stat_info;
    string::size_type cur = 0;

#ifdef WIN32
    cur = cache_root[1] == ':' ? 3 : 1;
    typedef int mode_t;
#else
    cur = 1;
#endif
    while ((cur = cache_root.find(DIR_SEPARATOR_CHAR, cur)) != string::npos) {
        string dir = cache_root.substr(0, cur);
        if (stat(dir.c_str(), &stat_info) == -1) {
            DBG2(cerr << "Cache....... Creating " << dir << endl);
            mode_t mask = UMASK(0);
            if (MKDIR(dir.c_str(), 0777) < 0) {
                DBG2(cerr << "Error: can't create." << endl);
                UMASK(mask);
                throw Error(string("Could not create the directory for the cache. Failed when building path at ") + dir + string("."));
            }
            UMASK(mask);
        }
        else {
            DBG2(cerr << "Cache....... Found " << dir << endl);
        }
        cur++;
    }
}

/** Set the cache's root directory to the given path. If no path is given,
    look at the DODS_CACHE, TMP and TEMP environment variables (in that
    order) to guess at a good location. If those are all NULL, use \c /tmp.
    If the cache root directory cannot be created, throw an exception.

    Note that in most cases callers should look for this path in the user's
    .dodsrc file.

    A private method.

    @see RCReader
    @param root Set the cache root to this pathname. Defaults to "".
    @exception Error Thrown if the path can neither be deduced nor created. */

void
HTTPCache::set_cache_root(const string &root)
{
    if (root != "") {
        d_cache_root = root;
        // cache root should end in /.
        if (d_cache_root[d_cache_root.size()-1] != DIR_SEPARATOR_CHAR)
            d_cache_root += DIR_SEPARATOR_CHAR;
    }
    else {
        // If no cache root has been indicated then look for a suitable
        // location.
#ifdef USE_GETENV
        char * cr = (char *) getenv("DODS_CACHE");
        if (!cr) cr = (char *) getenv("TMP");
        if (!cr) cr = (char *) getenv("TEMP");
        if (!cr) cr = (char*)CACHE_LOCATION;
        d_cache_root = cr;
#else
        d_cache_root = CACHE_LOCATION;
#endif

        if (d_cache_root[d_cache_root.size()-1] != DIR_SEPARATOR_CHAR)
            d_cache_root += DIR_SEPARATOR_CHAR;

        d_cache_root += CACHE_ROOT;
    }

    // Test d_hhtp_cache_table because this method can be called before that
    // instance is created and also can be called later to cahnge the cache
    // root. jhrg 05.14.08
    if (d_http_cache_table)
    	d_http_cache_table->set_cache_root(d_cache_root);
}

/** Enable or disable the cache. The cache can be temporarily suspended using
    the enable/disable property. This does not prevent the cache from being
    enabled/disable at a later point in time.

    Default: yes

    This method locks the class' interface.

    @param mode True if the cache should be enabled, False if it should be
    disabled. */

void
HTTPCache::set_cache_enabled(bool mode)
{
    lock_cache_interface();

    d_cache_enabled = mode;

    unlock_cache_interface();
}

/** Is the cache currently enabled? */

bool
HTTPCache::is_cache_enabled() const
{
    DBG2(cerr << "In HTTPCache::is_cache_enabled: (" << d_cache_enabled << ")"
         << endl);
    return d_cache_enabled;
}

/** Set the cache's disconnected property. The cache can operate either
    disconnected from the network or using a proxy cache (but tell that proxy
    not to use the network).

    This method locks the class' interface.

    @param mode One of DISCONNECT_NONE, DISCONNECT_NORMAL or
    DISCONNECT_EXTERNAL.
    @see CacheDIsconnectedMode */

void
HTTPCache::set_cache_disconnected(CacheDisconnectedMode mode)
{
    lock_cache_interface();

    d_cache_disconnected = mode;

    unlock_cache_interface();
}

/** Get the cache's disconnected mode property. */

CacheDisconnectedMode
HTTPCache::get_cache_disconnected() const
{
    return d_cache_disconnected;
}

/** How should the cache handle the Expires header?
    Default: no

    This method locks the class' interface.

    @param mode True if a responses Expires header should be ignored, False
    otherwise. */

void
HTTPCache::set_expire_ignored(bool mode)
{
    lock_cache_interface();

    d_expire_ignored = mode;

    unlock_cache_interface();
}

/* Is the cache ignoring Expires headers returned with responses that have
   been cached? */

bool
HTTPCache::is_expire_ignored() const
{
    return d_expire_ignored;
}

/** Cache size management. The default cache size is 20M. The minimum size is
    5M in order not to get into weird problems while writing the cache. The
    size is indicated in Mega bytes. Note that reducing the size of the cache
    may trigger a garbage collection operation.

    @note The maximum cache size is UINT_MAX bytes (usually 4294967295 for
    32-bit computers). If \e size is larger the value will be truncated to
    the value of that constant. It seems pretty unlikely that will happen
    given that the parameter is an unsigned long. This is a fix for bug 689
    which was reported when the parameter type was signed.

    This method locks the class' interface.

    @param size The maximum size of the cache in megabytes. */

void
HTTPCache::set_max_size(unsigned long size)
{
    lock_cache_interface();

    try {
        unsigned long new_size = size < MIN_CACHE_TOTAL_SIZE ?
                                 MIN_CACHE_TOTAL_SIZE * MEGA :
                                 (size > ULONG_MAX ? ULONG_MAX : size * MEGA);
        unsigned long old_size = d_total_size;
        d_total_size = new_size;
        d_folder_size = d_total_size / CACHE_FOLDER_PCT;
        d_gc_buffer = d_total_size / CACHE_GC_PCT;

        if (new_size < old_size && startGC()) {
            perform_garbage_collection();
            d_http_cache_table->cache_index_write();
        }
    }
    catch (...) {
        unlock_cache_interface();
        DBGN(cerr << "Unlocking interface." << endl);
        throw;
    }

    DBG2(cerr << "Cache....... Total cache size: " << d_total_size
         << " with " << d_folder_size
         << " bytes for meta information and folders and at least "
         << d_gc_buffer << " bytes free after every gc" << endl);

    unlock_cache_interface();
}

/** How big is the cache? The value returned is the size in megabytes. */

unsigned long
HTTPCache::get_max_size() const
{
    return d_total_size / MEGA;
}

/** Set the maximum size for a single entry in the cache.

    Default: 3M

    This method locks the class' interface.

    @param size The size in megabytes. */

void
HTTPCache::set_max_entry_size(unsigned long size)
{
    lock_cache_interface();

    try {
        unsigned long new_size = size * MEGA;
        if (new_size > 0 && new_size < d_total_size - d_folder_size) {
            unsigned long old_size = d_max_entry_size;
            d_max_entry_size = new_size;
            if (new_size < old_size && startGC()) {
                perform_garbage_collection();
                d_http_cache_table->cache_index_write();
            }
        }
    }
    catch (...) {
        unlock_cache_interface();
        throw;
    }

    DBG2(cerr << "Cache...... Max entry cache size is "
         << d_max_entry_size << endl);

    unlock_cache_interface();
}

/** Get the maximum size of an individual entry in the cache.

    @return The maximum size in megabytes. */

unsigned long
HTTPCache::get_max_entry_size() const
{
    return d_max_entry_size / MEGA;
}

/** Set the default expiration time. Use the <i>default expiration</i>
    property to determine when a cached response becomes stale if the
    response lacks the information necessary to compute a specific value.

    Default: 24 hours (86,400 seconds)

    This method locks the class' interface.

    @param exp_time The time in seconds. */

void
HTTPCache::set_default_expiration(const int exp_time)
{
    lock_cache_interface();

    d_default_expiration = exp_time;

    unlock_cache_interface();
}

/** Get the default expiration time used by the cache. */

int
HTTPCache::get_default_expiration() const
{
    return d_default_expiration;
}

/** Should every cache entry be validated?
    @param validate True if every cache entry should be validated before
    being used. */

void
HTTPCache::set_always_validate(bool validate)
{
    d_always_validate = validate;
}

/** Should every cache entry be validated before each use?
    @return True if all cache entries require validation. */

bool
HTTPCache::get_always_validate() const
{
    return d_always_validate;
}

/** Set the request Cache-Control headers. If a request must be satisfied
    using HTTP, these headers should be included in request since they might
    be pertinent to a proxy cache.

    Ignored headers: no-transform, only-if-cached. These headers are not used
    by HTTPCache and are not recorded. However, if present in the vector
    passed to this method, they will be present in the vector returned by
    get_cache_control.

    This method locks the class' interface.

    @param cc A vector of strings, each string holds one Cache-Control
    header.
    @exception InternalErr Thrown if one of the strings in \c cc does not
    start with 'Cache-Control: '. */

void
HTTPCache::set_cache_control(const vector<string> &cc)
{
    lock_cache_interface();

    try {
        d_cache_control = cc;

        vector<string>::const_iterator i;
        for (i = cc.begin(); i != cc.end(); ++i) {
            string header = (*i).substr(0, (*i).find(':'));
            string value = (*i).substr((*i).find(": ") + 2);
            if (header != "Cache-Control") {
                throw InternalErr(__FILE__, __LINE__, "Expected cache control header not found.");
            }
            else {
                if (value == "no-cache" || value == "no-store")
                    d_cache_enabled = false;
                else if (value.find("max-age") != string::npos) {
                    string max_age = value.substr(value.find("=" + 1));
                    d_max_age = parse_time(max_age.c_str());
                }
                else if (value == "max-stale")
                    d_max_stale = 0; // indicates will take anything;
                else if (value.find("max-stale") != string::npos) {
                    string max_stale = value.substr(value.find("=" + 1));
                    d_max_stale = parse_time(max_stale.c_str());
                }
                else if (value.find("min-fresh") != string::npos) {
                    string min_fresh = value.substr(value.find("=" + 1));
                    d_min_fresh = parse_time(min_fresh.c_str());
                }
            }
        }
    }
    catch (...) {
        unlock_cache_interface();
        throw;
    }

    unlock_cache_interface();
}


/** Get the Cache-Control headers.

    @return A vector of strings, one string for each header. */

vector<string>
HTTPCache::get_cache_control()
{
    return d_cache_control;
}

//@}

/** Look in the cache for the given \c url. Is it in the cache table?

    This method locks the class' interface.

	@todo Remove this is broken.
    @param url The url to look for.
    @return True if \c url is found, otherwise False. */

bool
HTTPCache::is_url_in_cache(const string &url)
{
    DBG(cerr << "Is this url in the cache? (" << url << ")" << endl);

    HTTPCacheTable::CacheEntry *entry = d_http_cache_table->get_locked_entry_from_cache_table(url);
    bool status = entry != 0;
    if (entry) {
        entry->unlock_read_response();
    }
    return  status;
}

/** Is the header a hop by hop header? If so, we're not supposed to store it
    in the cache. See RFC 2616, Section 13.5.1.

    @return True if the header is, otherwise False. */

bool
is_hop_by_hop_header(const string &header)
{
    return header.find("Connection") != string::npos
           || header.find("Keep-Alive") != string::npos
           || header.find("Proxy-Authenticate") != string::npos
           || header.find("Proxy-Authorization") != string::npos
           || header.find("Transfer-Encoding") != string::npos
           || header.find("Upgrade") != string::npos;
}

/** Dump the headers out to the meta data file. The file is truncated if it
    already exists.

    @todo This code could be replaced with STL/iostream stuff.

    A private method.

    @param cachename Base name of file for meta data.
    @param headers A vector of strings, one header per string.
    @exception InternalErr Thrown if the file cannot be opened. */

void
HTTPCache::write_metadata(const string &cachename, const vector<string> &headers)
{
    string fname = cachename + CACHE_META;
    d_open_files.push_back(fname);

    FILE *dest = fopen(fname.c_str(), "w");
    if (!dest) {
        throw InternalErr(__FILE__, __LINE__,
                          "Could not open named cache entry file.");
    }

    vector<string>::const_iterator i;
    for (i = headers.begin(); i != headers.end(); ++i) {
        if (!is_hop_by_hop_header(*i)) {
            int s = fwrite((*i).c_str(), (*i).size(), 1, dest);
            if (s != 1) {
                fclose(dest);
            	throw InternalErr(__FILE__, __LINE__, "could not write header: '" + (*i) + "' " + long_to_string(s));
            }
            s = fwrite("\n", 1, 1, dest);
            if (s != 1) {
                fclose(dest);
            	throw InternalErr(__FILE__, __LINE__, "could not write header: " + long_to_string(s));
            }
        }
    }

    int res = fclose(dest);
    if (res) {
        DBG(cerr << "HTTPCache::write_metadata - Failed to close "
            << dest << endl);
    }

    d_open_files.pop_back();
}

/** Read headers from a .meta.

    @todo This code could be replaced with STL/iostream code.

    A private method.

    @param cachename The name of the file in the persistent store.
    @param headers The headers are returned using this parameter.
    @exception InternalErr Thrown if the file cannot be opened. */

void
HTTPCache::read_metadata(const string &cachename, vector<string> &headers)
{
    FILE *md = fopen(string(cachename + CACHE_META).c_str(), "r");
    if (!md) {
        throw InternalErr(__FILE__, __LINE__,
                          "Could not open named cache entry meta data file.");
    }

    char line[1024];
    while (!feof(md) && fgets(line, 1024, md)) {
        line[min(1024, static_cast<int>(strlen(line)))-1] = '\0'; // erase newline
        headers.push_back(string(line));
    }

    int res = fclose(md);
    if (res) {
        DBG(cerr << "HTTPCache::read_metadata - Failed to close "
            << md << endl);
    }
}

/** Write the body of the HTTP response to the cache.

    This method used to throw ResponseTooBig if any response was larger than
    max_entry_size. I've disabled that since perform_garbage_collection will
    remove any such entry if it's causing problems. Note that if
    parse_headers finds a Content-Length header that indicates a response is
    too big, the response won't be cached. The idea here is that once we've
    already written a bunch of bytes to the cache, we might as well continue.
    If it overflows the cache, perform_garbage_collection() will remove it.

    A private method.

    @param cachename Write data to this file.
    @param src Read data from this stream.
    @return The total number of bytes written.
    @exception InternalErr Thrown if the file cannot be opened or if an I/O
    error was detected.
    @exception ResponseTooBig Thrown if the response was found to be bigger
    than the max_entry_size property. This is not longer thrown. 10/11/02
    jhrg */

int
HTTPCache::write_body(const string &cachename, const FILE *src)
{
    d_open_files.push_back(cachename);

    FILE *dest = fopen(cachename.c_str(), "wb");
    if (!dest) {
        throw InternalErr(__FILE__, __LINE__,
                          "Could not open named cache entry file.");
    }

    // Read and write in 1k blocks; an attempt at doing this efficiently.
    // 09/30/02 jhrg
    char line[1024];
    size_t n;
    int total = 0;
    while ((n = fread(line, 1, 1024, const_cast<FILE *>(src))) > 0) {
        total += fwrite(line, 1, n, dest);
        DBG2(sleep(3));
    }

    if (ferror(const_cast<FILE *>(src)) || ferror(dest)) {
        int res = fclose(dest);
        res = res & unlink(cachename.c_str());
        if (res) {
            DBG(cerr << "HTTPCache::write_body - Failed to close/unlink "
                << dest << endl);
        }
        throw InternalErr(__FILE__, __LINE__,
                          "I/O error transferring data to the cache.");
    }

    rewind(const_cast<FILE *>(src));

    int res = fclose(dest);
    if (res) {
        DBG(cerr << "HTTPCache::write_body - Failed to close "
            << dest << endl);
    }

    d_open_files.pop_back();

    return total;
}

/** Get a pointer to file that contains the body of a cached response. The
    returned FILE* can be used both for reading and for writing.

    A private method.

    @param cachename The name of the file that holds the response body.
    @exception InternalErr Thrown if the file cannot be opened. */

FILE *
HTTPCache::open_body(const string &cachename)
{
    DBG(cerr << "cachename: " << cachename << endl);

    FILE *src = fopen(cachename.c_str(), "rb"); // Read only
    if (!src)
	throw InternalErr(__FILE__, __LINE__, "Could not open cache file.");

    return src;
}

/** Add a new response to the cache, or replace an existing cached response
    with new data. This method returns True if the information for \c url was
    added to the cache. A response might not be cache-able; in that case this
    method returns false. (For example, the response might contain the
    'Cache-Control: no-cache' header.)

    Note that the FILE *body is rewound so that the caller can re-read it
    without using fseek or rewind.

    If a response for \c url is already present in the cache, it will be
    replaced by the new headers and body. To update a response in the cache
    with new meta data, use update_response().

    This method locks the class' interface.

    @param url A string which holds the request URL.
    @param request_time The time when the request was made, in seconds since
    1 Jan 1970.
    @param headers A vector of strings which hold the response headers.
    @param body A FILE * to a file which holds the response body.
    @return True if the response was cached, False if the response could not
    be cached.
    @exception InternalErr Thrown if there was a I/O error while writing to
    the persistent store. */

bool
HTTPCache::cache_response(const string &url, time_t request_time,
                          const vector<string> &headers, const FILE *body)
{
    lock_cache_interface();

    DBG(cerr << "Caching url: " << url << "." << endl);

    try {
        // If this is not an http or https URL, don't cache.
        if (url.find("http:") == string::npos &&
            url.find("https:") == string::npos) {
            unlock_cache_interface();
            return false;
        }

        // This does nothing if url is not already in the cache. It's
        // more efficient to do this than to first check and see if the entry
        // exists. 10/10/02 jhrg
        d_http_cache_table->remove_entry_from_cache_table(url);

        HTTPCacheTable::CacheEntry *entry = new HTTPCacheTable::CacheEntry(url);
        entry->lock_write_response();

        try {
            d_http_cache_table->parse_headers(entry, d_max_entry_size, headers); // etag, lm, date, age, expires, max_age.
            if (entry->is_no_cache()) {
                DBG(cerr << "Not cache-able; deleting HTTPCacheTable::CacheEntry: " << entry
                    << "(" << url << ")" << endl);
                entry->unlock_write_response();
                delete entry; entry = 0;
                unlock_cache_interface();
                return false;
            }

            // corrected_initial_age, freshness_lifetime, response_time.
            d_http_cache_table->calculate_time(entry, d_default_expiration, request_time);

            d_http_cache_table->create_location(entry); // cachename, cache_body_fd
            // move these write function to cache table
            entry->set_size(write_body(entry->get_cachename(), body));
            write_metadata(entry->get_cachename(), headers);
            d_http_cache_table->add_entry_to_cache_table(entry);
            entry->unlock_write_response();
        }
        catch (ResponseTooBigErr &e) {
            // Oops. Bummer. Clean up and exit.
            DBG(cerr << e.get_error_message() << endl);
            REMOVE(entry->get_cachename().c_str());
            REMOVE(string(entry->get_cachename() + CACHE_META).c_str());
            DBG(cerr << "Too big; deleting HTTPCacheTable::CacheEntry: " << entry << "(" << url
                << ")" << endl);
            entry->unlock_write_response();
            delete entry; entry = 0;
            unlock_cache_interface();
            return false;
        }

        if (d_http_cache_table->get_new_entries() > DUMP_FREQUENCY) {
            if (startGC())
                perform_garbage_collection();

            d_http_cache_table->cache_index_write(); // resets new_entries
        }
    }
    catch (...) {
        unlock_cache_interface();
        throw;
    }

    unlock_cache_interface();

    return true;
}

/** Build the headers to send along with a GET request to make that request
    conditional. This method examines the headers for a given response in the
    cache and formulates the correct headers for a valid HTTP 1.1 conditional
    GET request. See RFC 2616, Section 13.3.4.

    Rules: If an ETag is present, it must be used. Use If-None-Match. If a
    Last-Modified header is present, use it. Use If-Modified-Since. If both
    are present, use both (this means that HTTP 1.0 daemons are more likely
    to work). If a Last-Modified header is not present, use the value of the
    Cache-Control max-age or Expires header(s). Note that a 'Cache-Control:
    max-age' header overrides an Expires header (Sec 14.9.3).

    This method locks the cache interface and the cache entry.

    @param url Get the HTTPCacheTable::CacheEntry for this URL.
    @return A vector of strings, one request header per string.
    @exception Error Thrown if the \e url is not in the cache. */

vector<string>
HTTPCache::get_conditional_request_headers(const string &url)
{
    lock_cache_interface();

    HTTPCacheTable::CacheEntry *entry = 0;
    vector<string> headers;

    DBG(cerr << "Getting conditional request headers for " << url << endl);

    try {
        entry = d_http_cache_table->get_locked_entry_from_cache_table(url);
        if (!entry)
            throw Error("There is no cache entry for the URL: " + url);

        if (entry->get_etag() != "")
            headers.push_back(string("If-None-Match: ") + entry->get_etag());

        if (entry->get_lm() > 0) {
        	time_t lm = entry->get_lm();
            headers.push_back(string("If-Modified-Since: ")
                              + date_time_str(&lm));
        }
        else if (entry->get_max_age() > 0) {
        	time_t max_age = entry->get_max_age();
            headers.push_back(string("If-Modified-Since: ")
                              + date_time_str(&max_age));
        }
        else if (entry->get_expires() > 0) {
        	time_t expires = entry->get_expires();
            headers.push_back(string("If-Modified-Since: ")
                              + date_time_str(&expires));
        }
        entry->unlock_read_response();
        unlock_cache_interface();
    }
    catch (...) {
	unlock_cache_interface();
	if (entry) {
	    entry->unlock_read_response();
	}
	throw;
    }

    return headers;
}

/** Functor/Predicate which orders two MIME headers based on the header name
    only (discounting the value). */

struct HeaderLess: binary_function<const string&, const string&, bool>
{
    bool operator()(const string &s1, const string &s2) const {
        return s1.substr(0, s1.find(':')) < s2.substr(0, s2.find(':'));
    }
};

/** Update the meta data for a response already in the cache. This method
    provides a way to merge response headers returned from a conditional GET
    request, for the given URL, with those already present.

    This method locks the class' interface and the cache entry.

    @param url Update the meta data for this cache entry.
    @param request_time The time (Unix time, seconds since 1 Jan 1970) that
    the conditional request was made.
    @param headers New headers, one header per string, returned in the
    response.
    @exception Error Thrown if the \c url is not in the cache. */

void
HTTPCache::update_response(const string &url, time_t request_time,
                           const vector<string> &headers)
{
    lock_cache_interface();

    HTTPCacheTable::CacheEntry *entry = 0;
    DBG(cerr << "Updating the response headers for: " << url << endl);

    try {
        entry = d_http_cache_table->get_write_locked_entry_from_cache_table(url);
        if (!entry)
            throw Error("There is no cache entry for the URL: " + url);

        // Merge the new headers with the exiting HTTPCacheTable::CacheEntry object.
        d_http_cache_table->parse_headers(entry, d_max_entry_size, headers);

        // Update corrected_initial_age, freshness_lifetime, response_time.
        d_http_cache_table->calculate_time(entry, d_default_expiration, request_time);

        // Merge the new headers with those in the persistent store. How:
        // Load the new headers into a set, then merge the old headers. Since
        // set<> ignores duplicates, old headers with the same name as a new
        // header will got into the bit bucket. Define a special compare
        // functor to make sure that headers are compared using only their
        // name and not their value too.
        set<string, HeaderLess> merged_headers;

        // Load in the new headers
        copy(headers.begin(), headers.end(),
             inserter(merged_headers, merged_headers.begin()));

        // Get the old headers and load them in.
        vector<string> old_headers;
        read_metadata(entry->get_cachename(), old_headers);
        copy(old_headers.begin(), old_headers.end(),
             inserter(merged_headers, merged_headers.begin()));

        // Read the values back out. Use reverse iterators with back_inserter
        // to preserve header order. NB: vector<> does not support push_front
        // so we can't use front_inserter(). 01/09/03 jhrg
        vector<string> result;
        copy(merged_headers.rbegin(), merged_headers.rend(),
             back_inserter(result));

        write_metadata(entry->get_cachename(), result);
        entry->unlock_write_response();
        unlock_cache_interface();
    }
    catch (...) {
        if (entry) {
            entry->unlock_read_response();
        }
        unlock_cache_interface();
        throw;
    }
}

/** Look in the cache and return the status (validity) of the cached
    response. This method should be used to determine if a cached response
    requires validation.

    This method locks the class' interface and the cache entry.

    @param url Find the cached response associated with this URL.
    @return True indicates that the response can be used, False indicates
    that it must first be validated.
    @exception Error Thrown if the URL's response is not in the cache. */

bool
HTTPCache::is_url_valid(const string &url)
{
    lock_cache_interface();

    bool freshness;
    HTTPCacheTable::CacheEntry *entry = 0;

    DBG(cerr << "Is this URL valid? (" << url << ")" << endl);

    try {
        if (d_always_validate) {
            unlock_cache_interface();
            return false;  // force re-validation.
        }

        entry = d_http_cache_table->get_locked_entry_from_cache_table(url);
        if (!entry)
            throw Error("There is no cache entry for the URL: " + url);

        // If we supported range requests, we'd need code here to check if
        // there was only a partial response in the cache. 10/02/02 jhrg

        // In case this entry is of type "must-revalidate" then we consider it
        // invalid.
        if (entry->get_must_revalidate()) {
            entry->unlock_read_response();
            unlock_cache_interface();
            return false;
        }

        time_t resident_time = time(NULL) - entry->get_response_time();
        time_t current_age = entry->get_corrected_initial_age() + resident_time;

        // Check that the max-age, max-stale, and min-fresh directives
        // given in the request cache control header is followed.
        if (d_max_age >= 0 && current_age > d_max_age) {
            DBG(cerr << "Cache....... Max-age validation" << endl);
            entry->unlock_read_response();
            unlock_cache_interface();
            return false;
        }
        if (d_min_fresh >= 0
            && entry->get_freshness_lifetime() < current_age + d_min_fresh) {
            DBG(cerr << "Cache....... Min-fresh validation" << endl);
            entry->unlock_read_response();
            unlock_cache_interface();
            return false;
        }

        freshness = (entry->get_freshness_lifetime()
                     + (d_max_stale >= 0 ? d_max_stale : 0) > current_age);
        entry->unlock_read_response();
        unlock_cache_interface();
    }
    catch (...) {
    	if (entry) {
    	    entry->unlock_read_response();
    	}
    	unlock_cache_interface();
        throw;
    }

    return freshness;
}

/** Get information from the cache. For a given URL, get the headers, cache
    object name and body
    stored in the cache. Note that this method increments the hit counter for
    <code>url</code>'s entry and \e locks that entry. To release the lock,
    the method release_cached_response() \e must be called. Methods that
    block on a locked entry are: get_conditional_request_headers(),
    update_response() and is_url_valid(). In addition, purge_cache() throws
    Error if it's called and any entries are locked. The garbage collection
    system will not reclaim locked entries (but works fine when some entries
    are locked).

    This method locks the class' interface.

    This method does \e not check to see that the response is valid, just
    that it is in the cache. To see if a cached response is valid, use
    is_url_valid(). The FILE* returned can be used for both reading and
    writing. The latter allows a client to update the body of a cached
    response without having to first dump it all to a separate file and then
    copy it into the cache (using cache_response()).

    @param url Get response information for this URL.
    @param headers Return the response headers in this parameter
    @param cacheName A value-result parameter; the name of the cache file
    @return A FILE * to the response body.
    @exception Error Thrown if the URL's response is not in the cache.
    @exception InternalErr Thrown if the persistent store cannot be opened. */

FILE * HTTPCache::get_cached_response(const string &url,
		vector<string> &headers, string &cacheName) {
    lock_cache_interface();

    FILE *body = 0;
    HTTPCacheTable::CacheEntry *entry = 0;

    DBG(cerr << "Getting the cached response for " << url << endl);

    try {
        entry = d_http_cache_table->get_locked_entry_from_cache_table(url);
        if (!entry) {
        	unlock_cache_interface();
        	return 0;
        }

        cacheName = entry->get_cachename();
        read_metadata(entry->get_cachename(), headers);

        DBG(cerr << "Headers just read from cache: " << endl);
        DBGN(copy(headers.begin(), headers.end(), ostream_iterator<string>(cerr, "\n")));

        body = open_body(entry->get_cachename());

        DBG(cerr << "Returning: " << url << " from the cache." << endl);

        d_http_cache_table->bind_entry_to_data(entry, body);
    }
    catch (...) {
    	// Why make this unlock operation conditional on entry?
        if (entry)
        	unlock_cache_interface();
        if (body != 0)
            fclose(body);
        throw;
    }

    unlock_cache_interface();

    return body;
}

/** Get information from the cache. This is a convenience method that calls
 	the three parameter version of get_cache_response().

    This method locks the class' interface.

    @param url Get response information for this URL.
    @param headers Return the response headers in this parameter
    @return A FILE * to the response body.
    @exception Error Thrown if the URL's response is not in the cache.
    @exception InternalErr Thrown if the persistent store cannot be opened. */

FILE *
HTTPCache::get_cached_response(const string &url, vector<string> &headers)
{
	string discard_name;
	return get_cached_response(url, headers, discard_name);
}

/** Get a pointer to a cached response body. This is a convenience method that
 	calls the three parameter version of get_cache_response().

    This method locks the class' interface.

    @param url Find the body associated with this URL.
    @return A FILE* that points to the response body.
    @exception Error Thrown if the URL is not in the cache.
    @exception InternalErr Thrown if an I/O error is detected. */

FILE *
HTTPCache::get_cached_response(const string &url)
{
	string discard_name;
	vector<string> discard_headers;
	return get_cached_response(url, discard_headers, discard_name);
}

/** Call this method to inform the cache that a particular response is no
    longer in use. When a response is accessed using get_cached_response(), it
    is locked so that updates and removal (e.g., by the garbage collector)
    are not possible. Calling this method frees that lock.

    This method locks the class' interface.

    @param body Release the lock on the response information associated with
    this FILE *.
    @exception Error Thrown if \c body does not belong to an entry in the
    cache or if the entry was already released. */

void
HTTPCache::release_cached_response(FILE *body)
{
    lock_cache_interface();

    try {
    	// fclose(body); This results in a seg fault on linux jhrg 8/27/13
    	d_http_cache_table->uncouple_entry_from_data(body);
    }
    catch (...) {
        unlock_cache_interface();
        throw;
    }

    unlock_cache_interface();
}

/** Purge both the in-memory cache table and the contents of the cache on
    disk. This method deletes every entry in the persistent store but leaves
    the structure intact. The client of HTTPCache is responsible for making
    sure that all threads have released any responses they pulled from the
    cache. If this method is called when a response is still in use, it will
    throw an Error object and not purge the cache.

    This method locks the class' interface.

    @exception Error Thrown if an attempt is made to purge the cache when
    an entry is still in use. */

void
HTTPCache::purge_cache()
{
    lock_cache_interface();

    try {
        if (d_http_cache_table->is_locked_read_responses())
            throw Error("Attempt to purge the cache with entries in use.");

        d_http_cache_table->delete_all_entries();
    }
    catch (...) {
        unlock_cache_interface();
        throw;
    }

    unlock_cache_interface();
}

} // namespace libdap
