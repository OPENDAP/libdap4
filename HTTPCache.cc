
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

#include <stdio.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>

#include "Error.h"
#include "InternalErr.h"
#include "ResponseTooBigErr.h"
#include "HTTPCache.h"

#include "util_mit.h"
#include "debug.h"

HTTPCache *HTTPCache::_instance = 0;

using namespace std;

#if HAVE_PTHREAD_H
// instance_mutex is used to ensure that only one instance is created. The
// other mutexes used by this class are fields. 10/09/02 jhrg
static pthread_mutex_t instance_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK(m) pthread_mutex_lock((m))
#define TRYLOCK(m) pthread_mutex_trylock((m))
#define UNLOCK(m) pthread_mutex_unlock((m))
#define INIT(m) pthread_mutex_init((m), 0)
#define DESTROY(m) pthread_mutex_destroy((m))
#else
#define LOCK(m)
#define TRYLOCK(m)
#define UNLOCK(m)
#define INIT(m)
#define DESTROY(m)
#endif

#ifdef WIN32
#include <direct.h>
#include <time.h>
#include <fcntl.h>
#define	min(a,b) _MIN(a,b)
#define max(a,b) _MAX(a,b)
#define MKDIR(a,b) _mkdir((a))
#define REMOVE(a) remove((a))
#define MKSTEMP(a) _open(_mktemp((a)),_O_CREAT,_S_IREAD|_S_IWRITE)
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_STR "\\"
#else
#define MKDIR(a,b) mkdir((a), (b))
#define REMOVE(a) remove((a))
#define MKSTEMP(a) mkstemp((a))
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR "/"
#endif

#ifdef WIN32
#define CACHE_LOC "\\tmp\\"
#define CACHE_ROOT "dods-cache\\"
#else
#define CACHE_LOC "/tmp/"
#define CACHE_ROOT "dods-cache/"
#endif
#define CACHE_INDEX ".index"
#define CACHE_LOCK ".lock"
#define CACHE_META ".meta"
#define CACHE_EMPTY_ETAG "@cache@"

#define NO_LM_EXPIRATION 24*3600 // 24 hours
#define MAX_LM_EXPIRATION 48*3600 // Max expiration from LM 

// If using LM to find the expiration then take 10% and no more than
// MAX_LM_EXPIRATION.
#ifndef LM_EXPIRATION
#define LM_EXPIRATION(t) (min((MAX_LM_EXPIRATION), static_cast<int>((t) / 10)))
#endif

#define DUMP_FREQUENCY 10	// Dump index every x loads 

#define MEGA 0x100000L
#define CACHE_TOTAL_SIZE 20	// Default cache size is 20M 
#define CACHE_FOLDER_PCT 10	// 10% of cache size for metainfo etc. 
#define CACHE_GC_PCT 10		// 10% of cache size free after GC 
#define MIN_CACHE_TOTAL_SIZE 5	// 5M Min cache size 
#define MAX_CACHE_ENTRY_SIZE 3	// 3M Max size of single cached entry 

/** Compute the hash value for a URL. 
    @param url
    @return An integer hash code between 0 and CACHE_TABLE_SIZE. */

inline static int
get_hash(const string &url)
{
    int hash = 0;

    for (const char *ptr=url.c_str(); *ptr; ptr++)
	hash = (int) ((hash * 3 + (*(unsigned char *)ptr)) % CACHE_TABLE_SIZE);

    return hash;
}

/** Create an instance of the HTTP 1.1 compliant cache. This initializes the
    both the cache root and the path to the index file. It then reads the
    cache index file if one is present.

    A private method.

    @param cache_root The fully qualified pathname of the directory which
    will hold the cache data.
    @param force Force access to the persistent store!
    @exception Error Thrown if the single user/process lock for the
    persistent store cannot be obtained.
    @see cache_index_read */

HTTPCache::HTTPCache(string cache_root, bool force) throw(Error) : 
    d_locked_open_file(0), 
    d_cache_enabled(true), 
    d_cache_protected(false),
    d_expire_ignored(false), 
    d_total_size(CACHE_TOTAL_SIZE * MEGA),
    d_folder_size(CACHE_TOTAL_SIZE/CACHE_FOLDER_PCT),
    d_gc_buffer(CACHE_TOTAL_SIZE/CACHE_GC_PCT),
    d_max_entry_size(MAX_CACHE_ENTRY_SIZE * MEGA),
    d_current_size(0),
    d_default_expiration(NO_LM_EXPIRATION), 
    d_max_age(-1),
    d_max_stale(-1),
    d_min_fresh(-1),
    d_new_entries(0)
{
    DBG2(cerr << "Entering the constructor for " << this << "... " << endl);

    INIT(&d_cache_mutex);

    // Initialize the cache table.
    for (int i = 0; i < CACHE_TABLE_SIZE; ++i)
	d_cache_table[i] = 0;

    // This used to throw an Error object if we could not get the
    // single user lock. However, that results in an invalid object. It's
    // better to have an instance that has default values. If we cannot get
    // the lock, make sure to set the cache as *disabled*. 03/12/03 jhrg
    if (get_single_user_lock(force)) {
	set_cache_root(cache_root); // indirectly throws Error
	cache_index_read();
    }
    else {
	d_cache_enabled = false;
    }

    DBG2(cerr << "exiting" << endl);
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
HTTPCache::instance(const string &cache_root, bool force) throw(Error)
{
    LOCK(&instance_mutex);
    DBG2(cerr << "Entering instance(); (" << hex << _instance << dec << ")"
	 << endl);

    try {
	if (!_instance) {
	    _instance = new HTTPCache(cache_root, force);
	    atexit(delete_instance);
	}
    }
    catch (Error &e) {
	DBG2(cerr << "The constructor threw an Error!" << endl);
	UNLOCK(&instance_mutex);
	throw e;
    }

    UNLOCK(&instance_mutex);
    DBG2(cerr << "Returning " << hex << _instance << dec << endl);

    return _instance;
}

/** This static method is called using atexit(). It deletes the singleton;
    see ~HTTPCache for all that implies. */

void
HTTPCache::delete_instance()
{
    if (HTTPCache::_instance) {
	DBG2(cerr << "Deleting the cache: " << HTTPCache::_instance << endl);
	delete HTTPCache::_instance;
    }
}

/** Called by for_each inside ~HTTPCache(). 
    @param e The cache entry to delete. */

static inline void 
delete_cache_entry(HTTPCache::CacheEntry *e)
{
    DBG2(cerr << "Deleting CacheEntry: " << e << endl);
    DESTROY(&e->lock);
    delete e;
}

/** Destroy an instance of HTTPCache. This writes the cache index and frees
    the in-memory cache table structure. The persistent cache (the response
    headers and bodies and the index file) are not removed. To remove those,
    either erase the directory that contains the cache using a file system
    command or use the purge_cache method (which leaves the cache directory
    structure in place but removes all the cached information). 

    This class uses the singleton pattern. However, it is the client's
    responsibility to call this method by using 'delete
    HTTPCache::instance();' or the equivalent. If several threads are using
    the cache, it is important that only one deletes the cache! If delete is
    called more than once, the result will likely be an index file that is
    corrupt. */

HTTPCache::~HTTPCache()
{
    DBG2(cerr << "Entering the destructor for " << this << "... " << endl;);

    try {
	cache_index_write();
    }
    catch (Error &e) {
	// If the cache index cannot be written, we've got problems. However,
	// unless we're debugging, still free up the cache table in memory. 
	// How should we let users know they cache index is not being
	// written?? 10/03/02 jhrg
	DBG(cerr << e.get_error_message() << endl);
    }

    LOCK(&d_cache_mutex);	// Here because cache_index_write locks too

    try {
	for (int i = 0; i < CACHE_TABLE_SIZE; ++i) {
	    CachePointers *cp = d_cache_table[i];
	    if (cp) {
		// delete each entry
		for_each(cp->begin(), cp->end(), delete_cache_entry);
		// now delete the vector that held the entries
		DBG2(cerr << "Deleting d_cache_table[" << i << "]: "
		     << d_cache_table[i] << endl);
		delete d_cache_table[i];
	    }
	}

	release_single_user_lock();
	HTTPCache::_instance = 0; // Needed for testing (where many HTTPCache
				// objects are made by one process).

    }
    catch (Error &e) {
	DBG2(cerr << "The constructor threw an Error!" << endl);
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    DBG2(cerr << "exiting" << endl);
    UNLOCK(&d_cache_mutex);

    DESTROY(&d_cache_mutex);
}

/** @name Cache Index 
    
    These methods manage the cache's index file. Each cache holds an index
    file named \c .index which stores the cache's state information. */

//@{

/** Remove the cache index file. 

    A private method.

    @return True if the file was deleted, otherwise false. */

bool
HTTPCache::cache_index_delete()
{
    return (REMOVE(d_cache_index.c_str()) == 0);
}

/** Read the saved set of cached entries from disk. Consistency between the
    in-memory cache and the index is maintained by only reading the index
    file when the HTTPCache object is created!

    A private method.

    @return True when a cache index was found and read, false otherwise. */

bool
HTTPCache::cache_index_read()
{
    FILE *fp = fopen(d_cache_index.c_str(), "r");
    // If the cache index can't be opened that's OK; start with an empty
    // cache. 09/05/02 jhrg
    if (!fp) {
	return false;
    }

    char line[1024];
    while (!feof(fp) && fgets(line, 1024, fp)) {
	add_entry_to_cache_table(cache_index_parse_line(line));
	DBG2(cerr << line << endl);
    }

    int res = fclose( fp ) ;
    if( res ) {
	DBG(cerr << "HTTPCache::cache_index_read - Failed to close " << (void *)fp << endl ;) ;
    }

    return true;
}

/** Parse one line of the index file. 

    A private method. 

    @param line A single line from the \c .index file.
    @return A CacheEntry initialized with the information from \c line. */

HTTPCache::CacheEntry *
HTTPCache::cache_index_parse_line(const char *line)
{
    // Read the line and create the cache object
    CacheEntry *entry = new CacheEntry;

    INIT(&entry->lock);
    istringstream iss(line);
    iss >> entry->url;
    iss >> entry->cachename;

    iss >> entry->etag;
    if (entry->etag == CACHE_EMPTY_ETAG)
	entry->etag = "";

    iss >> entry->lm;
    iss >> entry->expires;
    iss >> entry->size;

    iss >> entry->range;	// range is not used. 10/02/02 jhrg

    iss >> entry->hash;
    iss >> entry->hits;
    iss >> entry->freshness_lifetime;
    iss >> entry->response_time;
    iss >> entry->corrected_initial_age;

    iss >> entry->must_revalidate;

    return entry;
}

/** Functor which writes a single CacheEntry to the \c .index file. */

struct WriteOneCacheEntry :
    public unary_function<HTTPCache::CacheEntry *, void> {

    FILE *d_fp;

    WriteOneCacheEntry(FILE *fp) : d_fp(fp) {}

    void operator()(HTTPCache::CacheEntry *e) {
	if (e && fprintf(d_fp, 
			 "%s %s %s %ld %ld %ld %c %d %d %ld %ld %ld %c\r\n",
			 e->url.c_str(),
			 e->cachename.c_str(),
			 e->etag == "" ? CACHE_EMPTY_ETAG 
			 : e->etag.c_str(),
			 (long) (e->lm),
			 (long) (e->expires),
			 e->size,
			 e->range ? '1' : '0', // not used. 10/02/02 jhrg 
			 e->hash,
			 e->hits,
			 (long) (e->freshness_lifetime),
			 (long) (e->response_time),
			 (long) (e->corrected_initial_age),
			 e->must_revalidate ? '1' : '0') < 0)
	    throw Error("Cache Index. Error writing cache index\n");
    }
};

/** Walk through the list of cached objects and write the cache index file to
    disk. If the file does not exist, it is created. If the file does exist,
    it is overwritten. As a side effect, zero the new_entries counter.

    @exception Error Thrown if the index file cannot be opened for writing.
    Note that the HTTPCache destructor calls this method and silently ignores
    this exception. */

void
HTTPCache::cache_index_write() throw(Error)
{
    LOCK(&d_cache_mutex);

    DBG2(cerr << "Cache Index. Writing index " << d_cache_index << endl);

    try {
	// Open the file for writing.
	FILE * fp = NULL;
	if ((fp = fopen(d_cache_index.c_str(), "wb")) == NULL) {
	    throw Error(string("Cache Index. Can't open `") + d_cache_index 
			+ string("' for writing"));
	}

	// Walk through the list and write it out. The format is really
	// simple as we keep it all in ASCII.

	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
	    CachePointers *cp = d_cache_table[cnt];
	    if (cp)
		for_each(cp->begin(), cp->end(), WriteOneCacheEntry(fp));
	}

	/* Done writing */
	int res = fclose(fp);
	if( res ) {
	    DBG(cerr << "HTTPCache::cache_index_write - Failed to close " 
		<< (void *)fp << endl ;) ;
	}

	d_new_entries = 0;
    }
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&d_cache_mutex);
}

//@} End of the cache index methods.

/** @name Garbage collection 
    These private methods manage the garbage collection tasks for the cache. */
//@{

/** Enough removed from cache? A private method.
    @return True if enough has been removed from the cache. */

bool
HTTPCache::stopGC() const
{
    return (d_current_size + d_folder_size < d_total_size - d_gc_buffer);
}

/** Is there too much in the cache. A private method.
    @return True if garbage collection should be performed. */

bool
HTTPCache::startGC() const
{
    return (d_current_size + d_folder_size > d_total_size);
}

/** Remove a CacheEntry. This means delete the entry's files on disk and free
    the CacheEntry object. The caller should null the entry's pointer in the
    cache_table. The total size of the cache is decremented once the entry is
    deleted. 

    A Private method.
    
    @param entry The CacheEntry to delete. 
    @exception InternalErr Thrown if \c entry is in use. */

void
HTTPCache::remove_cache_entry(CacheEntry *entry) throw(InternalErr)
{
    // This should never happen; all calls to this method are protected by
    // the caller, hence the InternalErr.
    if (entry->locked)
	throw InternalErr("Tried to delete a cache entry that is in use.");

    REMOVE(entry->cachename.c_str());
    REMOVE(string(entry->cachename + CACHE_META).c_str());

    d_current_size -= entry->size;
    DBG2(cerr << "Current size (after decrement): " << d_current_size << endl);

    DBG2(cerr << "Deleting CacheEntry: " << entry << endl);
    delete entry;
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

    // Remove entries larger than max_entry_size. Also remove entries
    // starting with zero hits, 1, ..., until stopGC() returns true.
    hits_gc();
    
    // After performing garbage collection, write the index file. This has
    // the side effect of zeroing the new entries counter (which means that
    // the gc code does not have to figure out if a purged entry was 'new' to
    // keep the counter up to date. 
    cache_index_write();
}

/** Functor which deletes and nulls a single CacheEntry if it has expired.
    This functor is called by expired_gc which then uses the
    erase(remove(...) ...) idiom to really remove all the vector entries that
    belonged to the deleted CacheEntry objects.

    @see expired_gc. */

struct DeleteExpired : 
    public unary_function<HTTPCache::CacheEntry *&, void> {
    time_t d_time;
    HTTPCache *d_cache;

    DeleteExpired(HTTPCache *cache, time_t t) : 
	d_time(t), d_cache(cache) {}

    void operator()(HTTPCache::CacheEntry *&e) {
	if (e && !e->locked 
	    && (e->freshness_lifetime 
		< (e->corrected_initial_age + (d_time - e->response_time)))) {
	    d_cache->remove_cache_entry(e);
	    e = 0;
	}
    }
};

/** Scan the current cache table and remove anything that has expired. Don't
    remove locked entries.
    
    A private method. */

void
HTTPCache::expired_gc()
{
    time_t now = time(0);

    // Walk through and delete all the expired entries.
    for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
	CachePointers *slot = d_cache_table[cnt];
	if (slot) { 
	    for_each(slot->begin(), slot->end(), DeleteExpired(this, now));
	    slot->erase(remove(slot->begin(), slot->end(),
			       static_cast<CacheEntry *>(0)),
			slot->end());
	}
    }
}    

/** Functor which deletes and nulls a single CacheEntry which has less than
    or equal to \c hits hits or if it is larger than the cache's
    max_entry_size property.

    @see hits_gc. */

struct DeleteByHits : 
    public unary_function<HTTPCache::CacheEntry *&, void> {
    HTTPCache *d_cache;
    int d_hits;

    DeleteByHits(HTTPCache *cache, int hits) : 
	d_cache(cache), d_hits(hits) {}

    void operator()(HTTPCache::CacheEntry *&e) {
	if (d_cache->stopGC())
	    return;
	if (e && !e->locked
	    && (e->size > d_cache->d_max_entry_size || e->hits <= d_hits)) {
	    d_cache->remove_cache_entry(e);
	    e = 0;
	}	     
    }
};

/** Scan the cache for entires that are larger than max_entry_size. Also
    start removing entires with low hit counts. Start looking for entries
    with zero hits, then one, and so on. Stop when the method stopGC returns
    true. Locked entries are never removed. 

    *** potential infinite loop. What if >20M of entries are locked?

    A private method. */

void
HTTPCache::hits_gc()
{
    int hits = 0;

    while (startGC()) {
	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
	    if (d_cache_table[cnt]) { 
		CachePointers *slot = d_cache_table[cnt];
		for_each(slot->begin(), slot->end(),
			 DeleteByHits(this, hits));
		slot->erase(remove(slot->begin(), slot->end(), 
				   static_cast<CacheEntry*>(0)),
			    slot->end());

	    }
	}

	hits++;
    }
}

//@} End of the garbage collection methods.

/** @name Methods to manipulate instances of CacheEntry. */

//@{

/** Add a CacheEntry to the cache table. As each entry is read, load it into
    the in-memory cache table and update the HTTPCache's current_size. The
    later is used by the garbage collection method.

    A private method. 

    @param entry The CacheEntry instance to add. */

void
HTTPCache::add_entry_to_cache_table(HTTPCache::CacheEntry *entry)
{
    int hash = entry->hash;

    if (!d_cache_table[hash]) {
	d_cache_table[hash] = new CachePointers;
	DBG2(cerr << "Allocated d_cache_table[" << hash << "]: " 
	    << d_cache_table[hash] << endl);
    }

    d_cache_table[hash]->push_back(entry);
    DBG2(cerr << "Pushing entry: " << entry << " onto d_cache_table[" 
	<< hash << "]" << endl);

    d_current_size += entry->size;
    DBG2(cerr << "Current size (after increment): " << d_current_size << endl);
}

/** Get a pointer to a CacheEntry from the cache table. Providing a way to
    pass the hash code into this method makes it easier to test for correct
    behavior when two entries collide. 10/07/02 jhrg

    A private method.

    @param hash The hash code for \c url.
    @param url Look for this URL. 
    @return The matching CacheEntry instance or NULL if none was found. */

HTTPCache::CacheEntry *
HTTPCache::get_entry_from_cache_table(int hash, const string &url) const
{
    if (d_cache_table[hash]) {
	CachePointers *cp = d_cache_table[hash];
	for (CachePointersIter i = cp->begin(); i != cp->end(); ++i)
	    // Must test *i because perform_garbage_collection may have
	    // removed this entry; the CacheEntry will then be null.
	    if ((*i) && (*i)->url == url)
		return *i;
    }

    return 0;
}

/** Functor which deletes and nulls a CacheEntry if the given entry matches
    the url. */

struct DeleteCacheEntry: 
    public unary_function<HTTPCache::CacheEntry *&, void> {
    
    string d_url;
    HTTPCache *d_cache;

    DeleteCacheEntry(HTTPCache *c, const string &url) 
	: d_url(url), d_cache(c) {}

    void operator()(HTTPCache::CacheEntry *&e) {
	if (e && !e->locked && e->url == d_url) {
	    d_cache->remove_cache_entry(e);
	    e = 0;
	}
    }
};

/** Find the CacheEntry for the given url and remove both its information in
    the persistent store and the entry in d_cache_table. If \c url is not in
    the cache, this method does nothing.

    A private method.

    @param url Remove this URL's entry. 
    @exception InternalErr Thrown if the CacheEntry for \c url is locked. */

void
HTTPCache::remove_entry_from_cache_table(const string &url) throw(InternalErr)
{
    int hash = get_hash(url);
    if (d_cache_table[hash]) {
	CachePointers *cp = d_cache_table[hash];
	for_each(cp->begin(), cp->end(), DeleteCacheEntry(this, url));
	cp->erase(remove(cp->begin(), cp->end(), static_cast<CacheEntry*>(0)),
		  cp->end());
    }
}

/** Get a pointer to a CacheEntry from the cache table. 

    A private method. 

    @param url Look for this URL. */

HTTPCache::CacheEntry *
HTTPCache::get_entry_from_cache_table(const string &url) const
{
    return get_entry_from_cache_table(get_hash(url), url);
}

//@} End of the CacheEntry methods.

/** Create the cache's root directory. This is the persistent store used by
    the cache. Paths must always end in DIR_SEPARATOR_CHAR.

    A private method.

    @param cache_root The pathname to the desired cache root directory.
    @exception Error Thrown if the given pathname cannot be created. */

void
HTTPCache::create_cache_root(const string &cache_root) throw(Error)
{
    struct stat stat_info;
    string::size_type cur = 0;

#ifdef WIN32
    cur = cache_root[1] == ':' ? 3 : 1;
#else
    cur = 1;
#endif
    while ((cur = cache_root.find(DIR_SEPARATOR_CHAR, cur)) != string::npos) {
	string dir = cache_root.substr(0, cur);
 	if (stat(dir.c_str(), &stat_info) == -1) {
	    DBG2(cerr << "Cache....... Creating " << dir << endl);
	    if (MKDIR(dir.c_str(), 0777) < 0) {
		DBG2(cerr << "Error: can't create." << endl);
		throw Error(string("Could not create the directory for the cache. Failed when building path at ") + dir + string("."));
	    }
	} else {
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
HTTPCache::set_cache_root(const string &root) throw(Error)
{
    if (root != "") {
	d_cache_root = root;
	// cache root should end in /.
	if (d_cache_root[d_cache_root.size()-1] != DIR_SEPARATOR_CHAR)
	    d_cache_root += DIR_SEPARATOR_CHAR;
    } else {
	// If no cache root has been indicated then look for a suitable
	// location.
	char * cr = (char *) getenv("DODS_CACHE");
	if (!cr) cr = (char *) getenv("TMP");
	if (!cr) cr = (char *) getenv("TEMP");
	if (!cr) cr = CACHE_LOC;

	d_cache_root = cr;
	if (d_cache_root[d_cache_root.size()-1] != DIR_SEPARATOR_CHAR)
	    d_cache_root += DIR_SEPARATOR_CHAR;
	
	d_cache_root += CACHE_ROOT;
    }

    d_cache_index = d_cache_root + CACHE_INDEX;

    create_cache_root(d_cache_root); // throws Error
}

/** Lock the persistent store part of the cache. Return true if the cache lock
    was acquired, false otherwise. This is a single user cache, so it
    requires locking at the process level. 

    A private method.

    @param force If True force access to the persistent store. False by
    default. 
    @return True if the cache was locked for our use, False otherwise. */

bool
HTTPCache::get_single_user_lock(bool force) 
{
    if (!d_locked_open_file) {
	FILE * fp = NULL;
	string lock = d_cache_root + CACHE_LOCK;
	if ((fp = fopen(lock.c_str(), "r")) != NULL) {
	    int res = fclose(fp);
	    if( res ) {
		DBG(cerr << "HTTPCache::get_single_user_lock - Failed to close " << (void *)fp << endl ;) ;
	    }
	    if (force)
		REMOVE(lock.c_str());
	    else
		return false;
	}

	if ((fp = fopen(lock.c_str(), "w")) == NULL)
	    return false;

	d_locked_open_file = fp;
	return true;
    }

    return false;
}

/** Release the single user (process) lock. A private method. */

void 
HTTPCache::release_single_user_lock()
{
    if (d_locked_open_file) {
	int res = fclose(d_locked_open_file);
	if( res ) {
	    DBG(cerr << "HTTPCache::release_single_user_lock - Failed to close " << (void *)d_locked_open_file << endl ;) ;
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
    LOCK(&d_cache_mutex);
    d_cache_enabled = mode;
    UNLOCK(&d_cache_mutex);
}

/** Is the cache currently enabled? */

bool
HTTPCache::is_cache_enabled() const
{
    DBG2(cerr << "In HTTPCache::is_cache_enabled: (" << d_cache_enabled << ")" 
	 << endl);
    return d_cache_enabled;
}

/** Should we cache protected responses? A protected response is one that
    comes from a server/site that requires authorization.

    Default: no

    This method locks the class' interface. 
    
    @param mode True if protected responses should be cached. */

void
HTTPCache::set_cache_protected(bool mode)
{
    LOCK(&d_cache_mutex);
    d_cache_protected = mode;
    UNLOCK(&d_cache_mutex);
}

/** Should we cache protected responses? */

bool
HTTPCache::is_cache_protected() const
{
    return d_cache_protected;
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
    LOCK(&d_cache_mutex);
    d_cache_disconnected = mode;
    UNLOCK(&d_cache_mutex);
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
    LOCK(&d_cache_mutex);
    d_expire_ignored = mode;
    UNLOCK(&d_cache_mutex);
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

    This method locks the class' interface. 

    @param size The maximum size of the cache in megabytes. */

void
HTTPCache::set_max_size(int size)
{
    LOCK(&d_cache_mutex);
    
    try {
	long new_size = size < MIN_CACHE_TOTAL_SIZE ?
	    MIN_CACHE_TOTAL_SIZE*MEGA : size * MEGA;
	long old_size = d_total_size;
	d_total_size = new_size;
	d_folder_size = d_total_size/CACHE_FOLDER_PCT;
	d_gc_buffer = d_total_size/CACHE_GC_PCT;

	if (new_size < old_size) {
	    UNLOCK(&d_cache_mutex);
	    perform_garbage_collection(); // calls cache_index_write, which locks
	    LOCK(&d_cache_mutex);
	}
    }
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    DBG2(cerr << "Cache....... Total cache size: " << d_total_size
	 << " with " << d_folder_size 
	 << " bytes for meta information and folders and at least "
	 << d_gc_buffer << " bytes free after every gc" << endl);

    UNLOCK(&d_cache_mutex);
}

/** How big is the cache? The value returned is the size in megabytes. */

int
HTTPCache::get_max_size() const
{
    return d_total_size / MEGA;
}

/** Set the maximum size for a single entry in the cache.

    Default: 3M

    This method locks the class' interface. 

    @param size The size in megabytes. */

void
HTTPCache::set_max_entry_size(int size)
{
    LOCK(&d_cache_mutex);

    try {
	long new_size = size*MEGA;
	if (new_size > 0 && new_size < d_total_size - d_folder_size) {
	    long old_size = d_max_entry_size;
	    d_max_entry_size = new_size;
	    if (new_size < old_size) {
		UNLOCK(&d_cache_mutex);
		perform_garbage_collection(); // calls cache_index_write, which locks
		LOCK(&d_cache_mutex);
	    }
	}
    }
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    DBG2(cerr << "Cache...... Max entry cache size is " 
	 << d_max_entry_size << endl);

    UNLOCK(&d_cache_mutex);
}

/** Get the maximum size of an individual entry in the cache. 

    @return The maximum size in megabytes. */

int
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
HTTPCache::set_default_expiration (const int exp_time)
{
    LOCK(&d_cache_mutex);

    d_default_expiration = exp_time;

    UNLOCK(&d_cache_mutex);
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
HTTPCache::set_cache_control(const vector<string> &cc) throw(InternalErr)
{
    LOCK(&d_cache_mutex);

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
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&d_cache_mutex);
}


/** Get the Cache-Control headers. 

    This method locks the class' interface. 

    @return A vector of strings, one string for each header. */

vector<string>
HTTPCache::get_cache_control()
{
    return d_cache_control;
}

//@}

/** Create the directory path for cache file. The cache uses a set of
    directories within d_cache_root to store individual responses. The name
    of the directory that holds a given response is the value returned by the
    get_hash() function (i.e., it's a number). If the directory exists, this
    method does nothing.

    A private method.

    @param hash The hash value (i.e., directory name).
    @return The pathname to the directory (even if it already existed).
    @exception Error Thrown if the directory cannot be created.*/

string 
HTTPCache::create_hash_directory(int hash) throw(Error)
{
    struct stat stat_info;
    ostringstream path;

    path << d_cache_root << hash;
    string p = path.str();

    if (stat(p.c_str(), &stat_info) == -1) {
	DBG2(cerr << "Cache....... Create dir " << p << endl);
	if (MKDIR(p.c_str(), 0777) < 0) {
	    DBG2(cerr << "Cache....... Can't create..." << endl);
	    throw Error("Could not create cache slot to hold response! Check the write permissions on your disk cache directory.");
	}
    } else {
	DBG2(cerr << "Cache....... Directory " << p << " already exists" 
	    << endl);
    }

    return p;
}

/** Create the directory for this url (using the hash value from get_hash())
    and a file within that directory to hold the response's information. The
    cachename and cache_body_fd fields of \c entry are updated.

    mkstemp opens the file it creates, which is a good thing but it makes
    tracking resources hard for the HTTPCache object (because an exception
    might cause a file descriptor resource leak). So I close that file
    descriptor here.

    A private method.

    @param entry The cache entry object to operate on.
    @exception Error If the file for the response's body cannot be created. */

void
HTTPCache::create_location(CacheEntry *entry) throw(Error)
{
    string hash_dir = create_hash_directory(entry->hash);
#ifdef WIN32
	hash_dir += "\\dodsXXXXXX";
#else
    hash_dir += "/dodsXXXXXX";	// mkstemp uses six characters.
#endif

    // mkstemp uses the storage passed to it; must be writable and local.
    char *templat = new char[hash_dir.size() + 1];
    strcpy(templat, hash_dir.c_str());

    // Open truncated for update. NB: mkstemp() returns a file descriptor.
    // man mkstemp says "... The file is opened with the O_EXCL flag,
    // guaranteeing that when mkstemp returns successfully we are the only
    // user." 09/19/02 jhrg
    int fd = MKSTEMP(templat);	// fd mode is 666 or 600 (Unix)
    if (fd < 0) {
	delete templat;
	close(fd);
	throw Error("The HTTP Cache could not create a file to hold the response; it will not be cached.");
    }

    entry->cachename = templat;
    delete[] templat;
    close(fd);
}

/** Parse various headers from the vector (which can be retrieved from
    libcurl once a response is received) and load the CacheEntry object with
    values. This method should only be called with headers from a response
    (it should not be used to parse request headers).

    A private method.

    @param entry Store values from the headers here.
    @param headers A vector of header lines. */

void
HTTPCache::parse_headers(CacheEntry *entry, const vector<string> &headers)
{
    vector<string>::const_iterator i;
    for (i = headers.begin(); i != headers.end(); ++i) {
	string header = (*i).substr(0, (*i).find(':'));
	string value = (*i).substr((*i).find(": ") + 2);
	DBG2(cerr << "Header: " << header << endl);
	DBG2(cerr << "Value: " << value << endl);

	if (header == "ETag") {
	    entry->etag = value;
	}
	else if (header == "Last-Modified") {
	    entry->lm = parse_time(value.c_str());
	}
	else if (header == "Expires") {
	    entry->expires = parse_time(value.c_str());
	}
	else if (header == "Date") {
	    entry->date = parse_time(value.c_str());
	}
	else if (header == "Age") {
	    entry->age = parse_time(value.c_str());
	}
	else if (header == "Content-Length") {
	    if (atoi(value.c_str()) > d_max_entry_size)
		entry->no_cache = true;
	}
	else if (header == "Cache-Control") {
	    // Ignored Cache-Control values: public, private, no-transform,
	    // proxy-revalidate, s-max-age. These are used by shared caches.
	    // See section 14.9 of RFC 2612. 10/02/02 jhrg
	    if (value == "no-cache" || value == "no-store") 
		// Note that we *can* store a 'no-store' response in volatile
		// memory according to RFC 2616 (section 14.9.2) but those
		// will be rare coming from DAP servers. 10/02/02 jhrg
		entry->no_cache = true;
	    else if (value == "must-revalidate")
		entry->must_revalidate = true;
	    else if (value.find("max-age") != string::npos) {
		string max_age = value.substr(value.find("=" + 1));
		entry->max_age = parse_time(max_age.c_str());
	    }
	}
    }
}

/** Calculate the corrected_initial_age of the object. We use the time when
    this function is called as the response_time as this is when we have
    received the complete response. This may cause a delay if the response
    header is very big but should not cause any incorrect behavior. 

    A private method.

    @param entry The CacheEntry object.
    @param request_time When was the request made? I think this value must be
    passed into the method that calls this method... */

void
HTTPCache::calculate_time(CacheEntry *entry, time_t request_time)
{
    entry->response_time = time(NULL);
    time_t apparent_age 
	= max(0, static_cast<int>(entry->response_time - entry->date));
    time_t corrected_received_age = max(apparent_age, entry->age);
    time_t response_delay = entry->response_time - request_time;
    entry->corrected_initial_age = corrected_received_age + response_delay;

    // Estimate an expires time using the max-age and expires time. If we
    // don't have an explicit expires time then set it to 10% of the LM date
    // (although max 24 h). If no LM date is available then use 24 hours.
    time_t freshness_lifetime = entry->max_age;
    if (freshness_lifetime < 0) {
	if (entry->expires < 0) {
	    if (entry->lm < 0) {
		freshness_lifetime = NO_LM_EXPIRATION;
	    } 
	    else {
		freshness_lifetime = LM_EXPIRATION(entry->date - entry->lm);
	    }
	} 
	else
	    freshness_lifetime = entry->expires - entry->date;
    }

    entry->freshness_lifetime = max(0, static_cast<int>(freshness_lifetime));

    DBG2(cerr << "Cache....... Received Age " << entry->age
	<< ", corrected " << entry->corrected_initial_age
	<< ", freshness lifetime " << entry->freshness_lifetime << endl);
}

/** Look in the cache for the given \c url. Is it in the cache table?

    This method locks the class' interface.

    @param url The url to look for.
    @return True if \c url is found, otherwise False. */

bool
HTTPCache::is_url_in_cache(const string &url)
{
    return get_entry_from_cache_table(url) != 0;
}

/** Is the header a hop by hop header? If so, we're not supposed to store it
    in the cache. See RFC 2616, Section 13.5.1.

    @return True if the header is, otherwise False. */

static inline bool
is_hop_by_hop_header(const string &header) 
{
    return header.find("Connection") != string::npos
	|| header.find("Keep-Alive") != string::npos
	|| header.find("Proxy-Authenticate") != string::npos
	|| header.find("Proxy-Authorization") != string::npos
	|| header.find("Transfer-Encoding") != string::npos
	|| header.find("Upgrade") != string::npos;
}

/** Dump the headers out to the metadata file. The file is truncated if it
    already exists.

    @todo This code could be replaced with STL/iostream stuff.

    A private method.

    @param cachename Base name of file for metadata.
    @param headers A vector of strings, one header per string. 
    @exception InternalErr Thrown if the file cannot be opened. */

void
HTTPCache::write_metadata(const string &cachename, 
			  const vector<string> &headers) 
    throw(InternalErr)
{
    FILE *dest = fopen(string(cachename + CACHE_META).c_str(), "w");
    if (!dest) {
	fclose(dest);
	throw InternalErr(__FILE__, __LINE__, "Could not open named cache entry file.");
    }

    vector<string>::const_iterator i;
    for (i = headers.begin(); i != headers.end(); ++i) {
	if (!is_hop_by_hop_header(*i)) {
	    fwrite((*i).c_str(), (*i).size(), 1, dest);
	    fwrite("\n", 1, 1, dest);
	}
    }

    int res = fclose(dest);
    if( res ) {
	DBG(cerr << "HTTPCache::write_metadata - Failed to close " << (void *)dest << endl ;) ;
    }
}

/** Read headers from a .meta.
    
    @todo This code could be replaced with STL/iostream code.

    A private method.

    @param cachename The name of the file in the persistent store.
    @param headers The headers are returned using this parameter.
    @exception InternalErr Thrown if the file cannot be opened. */

void
HTTPCache::read_metadata(const string &cachename, vector<string> &headers) 
    const throw(InternalErr)
{
    FILE *md = fopen(string(cachename + CACHE_META).c_str(), "r");
    if (!md) {
	fclose( md ) ;
	throw InternalErr(__FILE__, __LINE__, "Could not open named cache entry metadata file.");
    }

    char line[1024];
    while (!feof(md) && fgets(line, 1024, md)) {
	line[strlen(line)-1] = '\0'; // erase newline
	headers.push_back(string(line));
    }

    int res = fclose(md);
    if( res ) {
	DBG(cerr << "HTTPCache::read_metadata - Failed to close " << (void *)md << endl ;) ;
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
    throw(InternalErr, ResponseTooBigErr)
{
    FILE *dest = fopen(cachename.c_str(), "wb");
    if (!dest) {
	throw InternalErr(__FILE__, __LINE__, "Could not open named cache entry file.");
    }

    // Read and write in 1k blocks; an attempt at doing this efficiently.
    // 09/30/02 jhrg
    char line[1024];
    size_t n;
    int total = 0;
    while ((n = fread(line, 1, 1024, const_cast<FILE *>(src))) > 0) {
	total += fwrite(line, 1, n, dest);
#if 0
	// See comment above.
	if (total > d_max_entry_size)
	    throw ResponseTooBigErr("This response is too big to cache.");
#endif
    }
    
    if (ferror(const_cast<FILE *>(src)) || ferror(dest)) {
	int res = fclose(dest);
	if( res ) {
	    DBG(cerr << "HTTPCache::write_body - Failed to close " << (void *)dest << endl ;) ;
	}
	throw InternalErr(__FILE__, __LINE__, "I/O error transferring data to the cache.");
    }

    rewind(const_cast<FILE *>(src));

    int res = fclose(dest);
    if( res ) {
	DBG(cerr << "HTTPCache::write_body - Failed to close " << (void *)dest << endl ;) ;
    }

    return total;
}

/** Get a pointer to file that contains the body of a cached response. The
    returned FILE* can be used both for reading and for writing. 

    A private method.

    @param cachename The name of the file that holds the response body.
    @exception InternalErr Thrown if the file cannot be opened. */

FILE *
HTTPCache::open_body(const string &cachename) const throw(InternalErr)
{
    FILE *src = fopen(cachename.c_str(), "r+b");
    if (!src) {
	fclose(src);
	throw InternalErr(__FILE__, __LINE__, "Could not open named cache entry file.");
    }

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
    with new metadata, use update_response().

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
    throw(InternalErr)
{
    LOCK(&d_cache_mutex);

    DBG(cerr << "Caching url: " << url << "." << endl);

    try {
	// If this is not an http or https URL, don't cache.
	if (url.find("http:") == string::npos &&
	    url.find("https:") == string::npos) {
	    UNLOCK(&d_cache_mutex);
	    return false;
	}

	// This does nothing if url is not already in the cache. It's
	// more efficient to do this than to first check and see if the entry
	// exists. 10/10/02 jhrg
	remove_entry_from_cache_table(url);
	
	CacheEntry *entry = new CacheEntry;

	INIT(&entry->lock);
	entry->url = url;
	entry->hash = get_hash(url);
	entry->hits = 0;

	try {	
	    parse_headers(entry, headers); // etag, lm, date, age, expires, max_age.
	    if (entry->no_cache) {
		DBG(cerr << "Not cache-able; deleting CacheEntry: " << entry 
		    << "(" << url << ")" << endl);
		delete entry;
		UNLOCK(&d_cache_mutex);
		return false;
	    }

	    // corrected_initial_age, freshness_lifetime, response_time.
	    calculate_time(entry, request_time); 

	    create_location(entry);	// cachename, cache_body_fd
	    entry->size = write_body(entry->cachename, body);

	    write_metadata(entry->cachename, headers);
	}
	catch (ResponseTooBigErr &e) {
	    // Oops. Bummer. Clean up and exit.
	    DBG(cerr << e.get_error_message() << endl);
	    REMOVE(entry->cachename.c_str());
	    REMOVE(string(entry->cachename + CACHE_META).c_str());
	    DBG(cerr << "Too big; deleting CacheEntry: " << entry << "(" << url 
		<< ")" << endl);
	    delete entry;
	    UNLOCK(&d_cache_mutex);
	    return false;
	}

	entry->range = false;	// not used. 10/02/02 jhrg

	add_entry_to_cache_table(entry);

	if (++d_new_entries > DUMP_FREQUENCY) {
	    UNLOCK(&d_cache_mutex); // unlock because cache_index_write() locks
	    cache_index_write(); // resets d_new_entries
	    LOCK(&d_cache_mutex);
	}
    }
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&d_cache_mutex);

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

    @param url Get the CacheEntry for this URL.
    @return A vector of strings, one request header per string.
    @exception Error Thrown if the \c url is not in the cache. */

vector<string>
HTTPCache::get_conditional_request_headers(const string &url) 
    throw(Error)
{
    LOCK(&d_cache_mutex);
    CacheEntry *entry;
    vector<string> headers;

    try {
	entry = get_entry_from_cache_table(url);
	if (!entry) {
	    throw Error("There is no cache entry for the URL: " + url);
	}

	DBG(cerr << "Locking entry... ");
	LOCK(&entry->lock);

	if (entry->etag != "")
	    headers.push_back(string("If-None-Match: ") + entry->etag);

	if (entry->lm > 0)
	    headers.push_back(string("If-Modified-Since: ") 
			      + date_time_str(&entry->lm));
	else if (entry->max_age > 0)
	    headers.push_back(string("If-Modified-Since: ")
			      + date_time_str(&entry->max_age));
	else if (entry->expires > 0)
	    headers.push_back(string("If-Modified-Since: ")
			      + date_time_str(&entry->expires));
    }
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	UNLOCK(&entry->lock);
	throw e;
    }

    UNLOCK(&entry->lock);
    DBGN(cerr << "Unlocking entry" << endl);
    UNLOCK(&d_cache_mutex);

    return headers;
}

/** Functor/Predicate which orders two MIME headers based on the header name
    only (discounting the value). */

struct HeaderLess: public binary_function<const string&, const string&, bool> {
    bool operator()(const string &s1, const string &s2) const {
	return s1.substr(0, s1.find(':')) < s2.substr(0, s2.find(':'));
    }
};

/** Update the metadata for a response already in the cache. This method
    provides a way to merge response headers returned from a conditional GET
    request, for the given URL, with those already present.  

    This method locks the class' interface and the cache entry.

    @param url Update the metadata for this cache entry.
    @param request_time The time (Unix time, seconds since 1 Jan 1970) that
    the conditional request was made.
    @param headers New headers, one header per string, returned in the
    response. 
    @exception Error Thrown if the \c url is not in the cache. */

void
HTTPCache::update_response(const string &url, time_t request_time,
			   const vector<string> &headers) throw(Error)
{
    LOCK(&d_cache_mutex);
    CacheEntry *entry;

    DBG(cerr << "Updating the response headers for: " << url << endl);

    try {
	entry = get_entry_from_cache_table(url);
	if (!entry)
	    throw Error("There is no cache entry for the URL: " + url);

	DBG(cerr << "Locking entry... ");
	LOCK(&entry->lock);

	// Merge the new headers with the exiting CacheEntry object.
	parse_headers(entry, headers);

	// Update corrected_initial_age, freshness_lifetime, response_time.
	calculate_time(entry, request_time); 

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
	read_metadata(entry->cachename, old_headers);
	copy(old_headers.begin(), old_headers.end(), 
	     inserter(merged_headers, merged_headers.begin()));

	// Read the values back out. Use reverse iterators with back_inserter
	// to preserve header order. NB: vector<> does not support push_front
	// so we can't use front_inserter(). 01/09/03 jhrg
	vector<string> result;
	copy(merged_headers.rbegin(), merged_headers.rend(), 
	     back_inserter(result));

	// Store.
	write_metadata(entry->cachename, result);
    }
    catch (Error &e) {
	UNLOCK(&entry->lock);
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&entry->lock);
    DBGN(cerr << "Unlocking entry" << endl);
    UNLOCK(&d_cache_mutex);
}

/** Look in the cache and return the status (validity) of the cached
    response. This method should be used to determine if a cached response
    requires validation.

    This method locks the class' interface and on a locked cache entry.

    @param url Find the cached response associated with this URL.
    @return True indicates that the response can be used, False indicates
    that it must first be validated. 
    @exception Error Thrown if the URL's response is not in the cache. */

bool
HTTPCache::is_url_valid(const string &url) throw(Error)
{
    LOCK(&d_cache_mutex);
    bool freshness;
    CacheEntry *entry;

    try {
	if (d_always_validate)
	    return false;		// force re-validation.

	entry = get_entry_from_cache_table(url);
	if (!entry)
	    throw Error("There is no cache entry for the URL: " + url);

	DBG(cerr << "Locking entry... ");
	LOCK(&entry->lock);

	// If we supported range requests, we'd need code here to check if
	// there was only a partial response in the cache. 10/02/02 jhrg

	// In case this entry is of type "must-revalidate" then we consider it
	// invalid. 
	if (entry->must_revalidate) {
	    UNLOCK(&entry->lock);
	    DBGN(cerr << "Unlocking entry" << endl);
	    return false;
	}

	time_t resident_time = time(NULL) - entry->response_time;
	time_t current_age = entry->corrected_initial_age + resident_time;

	// Check that the max-age, max-stale, and min-fresh directives
	// given in the request cache control header is followed.
	if (d_max_age >= 0 && current_age > d_max_age) {
	    DBG(cerr << "Cache....... Max-age validation" << endl);
	    UNLOCK(&entry->lock);
	    DBGN(cerr << "Unlocking entry" << endl);
	    UNLOCK(&d_cache_mutex);
	    return false;
	}
	if (d_min_fresh >= 0 &&
	    entry->freshness_lifetime < current_age + d_min_fresh) {
	    DBG(cerr << "Cache....... Min-fresh validation" << endl);
	    UNLOCK(&entry->lock);
	    DBGN(cerr << "Unlocking entry" << endl);
	    UNLOCK(&d_cache_mutex);
	    return false;
	}

	freshness = (entry->freshness_lifetime 
		     + (d_max_stale >= 0 ? d_max_stale : 0) > current_age);
    }
    catch (Error &e) {
	UNLOCK(&entry->lock);
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&entry->lock);
    DBGN(cerr << "Unlocking entry" << endl);
    UNLOCK(&d_cache_mutex);

    return freshness;
}

/** Get information from the cache. For a given URL, get the headers and body
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
    @return A FILE * to the response body.
    @exception Error Thrown if the URL's response is not in the cache.
    @exception InternalErr Thrown if the persistent store cannot be opened. */

FILE *
HTTPCache::get_cached_response(const string &url, vector<string> &headers)
    throw(Error, InternalErr)
{
    LOCK(&d_cache_mutex);
    FILE *body;
    CacheEntry *entry;

    try {
	entry = get_entry_from_cache_table(url);
	if (!entry)
	    throw Error("There is no cache entry for the URL: " + url);

	read_metadata(entry->cachename, headers);
    
	body = open_body(entry->cachename);

	DBG(cerr << "Returning: " << url << " from the cache." << endl);

	entry->hits++;		// Mark hit
	entry->locked++;	// lock entry
	d_locked_entries[body] = entry; // record lock, see release_cached_r...
	DBG(cerr << "Locking entry (non-blocking lock)... ");
	TRYLOCK(&entry->lock);	// Needed for blocking lock; locked counts
    }
    catch (Error &e) {
	UNLOCK(&entry->lock);
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&d_cache_mutex);

    return body;
}

/** Get a pointer to a cached response body. For a given URL, find the cached
    response body and return a FILE * to it. This updates the hit counter and
    it locks the entry. To release the lock, call release_cached_response().
    Methods that block on a locked entry are:
    get_conditional_request_headers(), update_response() and is_url_valid().
    In addition, purge_cache() throws Error if it's called and any entries
    are locked. The garbage collection system will not reclaim locked entries
    (but works fine when some entries are locked).

    NB: This method does \e not check to see that the response is valid, just
    that it is in the cache. To see if a cached response is valid, use
    is_url_valid(). 

    This method locks the class' interface.

    @param url Find the body associated with this URL.
    @return A FILE* that points to the response body.
    @exception Error Thrown if the URL is not in the cache. 
    @exception InternalErr Thrown if an I/O error is detected. */

FILE *
HTTPCache::get_cached_response_body(const string &url) 
    throw(Error, InternalErr)
{
    LOCK(&d_cache_mutex);
    FILE *body;
    CacheEntry *entry;

    try {
	entry = get_entry_from_cache_table(url);

	if (!entry) {
	    UNLOCK(&d_cache_mutex);
	    throw Error("There is no cache entry for the URL: " + url);
	}

	body = open_body(entry->cachename); // throws InternalErr

	DBG(cerr << "Returning body for: " << url << " from the cache." 
	    << endl);

	entry->hits++;		// Mark hit
	entry->locked++;		// lock entry
	d_locked_entries[body] = entry; // record lock, see release_cached_r...
	DBG(cerr << "Locking entry (non-blocking lock)... ");
	TRYLOCK(&entry->lock);
    }
    catch (Error &e) {
	UNLOCK(&entry->lock);
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&d_cache_mutex);

    return body;
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
HTTPCache::release_cached_response(FILE *body) throw(Error)
{
    LOCK(&d_cache_mutex);
    CacheEntry *entry;

    try {
	entry = d_locked_entries[body];
	if (!entry)
	    throw Error("There is no cache entry for the response given.");

	entry->locked--;
	if (entry->locked == 0) {
	    d_locked_entries.erase(body);
	    UNLOCK(&entry->lock);
	    DBGN(cerr << "Unlocking entry" << endl);
	}

	if (entry->locked < 0)
	    throw Error("An unlocked entry was released");
    }
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&d_cache_mutex);
}

/** Functor to delete and null all unlocked CacheEntry objects. */

struct DeleteUnlockedCacheEntry :
    public unary_function<HTTPCache::CacheEntry *&, void> {
    
    HTTPCache *d_cache;

    DeleteUnlockedCacheEntry(HTTPCache *c) : d_cache(c) {}
    void operator()(HTTPCache::CacheEntry *&e) {
	if (e) {
	    d_cache->remove_cache_entry(e);
	    e = 0;
	}
    }
};

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
HTTPCache::purge_cache() throw(Error)
{
    LOCK(&d_cache_mutex);
    DBG(cerr << "Purging the cache." << endl);
    
    try {
	if (!d_locked_entries.empty()) {
	    throw Error("Attempt to purge the cache with entries in use.");
	}

	// Walk through the cache table and, for every entry in the cache, delete
	// it on disk and in the cache table.
	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
	    CachePointers *slot = d_cache_table[cnt];
	    if (slot) { 
		for_each(slot->begin(), slot->end(),
			 DeleteUnlockedCacheEntry(this));
		slot->erase(remove(slot->begin(), slot->end(),
				   static_cast<CacheEntry *>(0)),
			    slot->end());
	    }
	}

	// Now delete the index itself.
	cache_index_delete();
    }
    catch (Error &e) {
	UNLOCK(&d_cache_mutex);
	throw e;
    }

    UNLOCK(&d_cache_mutex);
}

// $Log: HTTPCache.cc,v $
// Revision 1.9  2003/04/23 21:33:53  jimg
// Changes for the unit tests. This involved merging Rob's VC++ changes
// and fixing a bug in escaping.cc (a call to string::insert invalidated
// an iterator in a loop).
//
// Revision 1.8  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.7  2003/03/13 23:55:57  jimg
// Significant changes regarding the mutex code. I found out that since
// config_dap.h was not being included, the mutex code was never built! Once
// built, I found a bunch of deadlocks. Many of the methods had to be modified
// to fix this/these problem(s).
//
// Revision 1.6  2003/03/04 21:43:11  jimg
// Minor change; changed the order of inline and static for the get_hash()
// function.
//
// Revision 1.5  2003/03/04 17:31:17  jimg
// Modified cache_response() so that only http and https URLs are cached.
//
// Revision 1.4  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.13  2002/12/29 23:03:48  jimg
// Updated the copyrights for these source files.
//
// Revision 1.1.2.12  2002/12/27 00:58:48  jimg
// Removed some dubious code Rob found. Fixed some comments.
//
// Revision 1.1.2.11  2002/12/01 12:49:07  rmorris
// Bug fixes related to win32 port - numerous related to caching.
//
// Revision 1.1.2.10  2002/11/21 21:24:17  pwest
// memory leak cleanup and file descriptor cleanup
//
// Revision 1.1.2.9  2002/11/04 07:26:13  rmorris
// Finish Porting this new code to win32.
//
// Revision 1.1.2.8  2002/10/18 22:43:33  jimg
// Added methods for the always_validate property. This can be set in the
// .dodsrc file (see RCReader) so we should support it here. The class respects
// the value of this property (it's different than CacheEntry::always_validate;
// that says *that* particular entry should always be validated and its value
// comes from a Cache_control header in the response). The
// HTTPCache::always_validate property says *every* entry should always be
// validated.
//
// Revision 1.1.2.7  2002/10/18 00:10:38  jimg
// MT-safety for entries; I used a mutex to lock an entry so that it won't be
// modified by one thread while another is using it. Fixed up the documentation.
//
// Revision 1.1.2.6  2002/10/11 20:57:36  jimg
// Fixed up the documentation.
// Added a test to ensure that hop-by-hop headers are not stored in the cache.
//
// Revision 1.1.2.5  2002/10/11 00:20:19  jimg
// Added methods for conditional responses.
//
// Revision 1.1.2.4  2002/10/10 06:46:30  jimg
// The class is now MT-safe.
//
// Revision 1.1.2.3  2002/10/08 05:23:16  jimg
// The Cache now works. Still to do: Make the cache MT-safe (which will
// probably require that it become a singleton, but maybe not), protect
// all non-const methods with a mutex and devise a locking strategy so that
// entries are not read while being written. Once the cache is MT-safe, it
// must be made to handle updates.
//
// Revision 1.1.2.2  2002/09/17 00:18:49  jimg
// Added methods to manage the single user lock and to get/set the cache root
// directory. Also fixed a build problem; I appear to have the sstream headers
// and classes on my machine but other (most?) gcc 2.95.x installations lack
// them. I switched from sstream to strstream.
//
// Revision 1.1.2.1  2002/09/12 22:39:35  jimg
// Initial version.
//
