
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

// #define DODS_DEBUG

// TODO: Remove unneeded includes.

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
#include "HTTPCacheMacros.h"

#include "util_mit.h"
#include "debug.h"

#ifdef WIN32
#include <direct.h>
#include <time.h>
#include <fcntl.h>
#define MKDIR(a,b) _mkdir((a))
#define REMOVE(a) do { \
		int s = remove((a)); \
		if (s != 0) \
			throw InternalErr(__FILE__, __LINE__, "Cache error; could not remove file: " + long_to_string(s)); \
	} while(0);
#define MKSTEMP(a) _open(_mktemp((a)),_O_CREAT,_S_IREAD|_S_IWRITE)
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_STR "\\"
#else
#define MKDIR(a,b) mkdir((a), (b))
#define MKSTEMP(a) mkstemp((a))
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR "/"
#endif

#define CACHE_META ".meta"
#define CACHE_INDEX ".index"
#define CACHE_EMPTY_ETAG "@cache@"

#define NO_LM_EXPIRATION 24*3600 // 24 hours
#define MAX_LM_EXPIRATION 48*3600 // Max expiration from LM

// If using LM to find the expiration then take 10% and no more than
// MAX_LM_EXPIRATION.
#ifndef LM_EXPIRATION
#define LM_EXPIRATION(t) (min((MAX_LM_EXPIRATION), static_cast<int>((t) / 10)))
#endif

const int CACHE_TABLE_SIZE = 1499;

using namespace std;

namespace libdap {

/** Compute the hash value for a URL.
    @param url
    @return An integer hash code between 0 and CACHE_TABLE_SIZE. */
int
get_hash(const string &url)
{
    int hash = 0;

    for (const char *ptr = url.c_str(); *ptr; ptr++)
        hash = (int)((hash * 3 + (*(unsigned char *)ptr)) % CACHE_TABLE_SIZE);

    return hash;
}

HTTPCacheTable::HTTPCacheTable(const string &cache_root, int block_size) :
    d_cache_root(cache_root), d_block_size(block_size), d_current_size(0), d_new_entries(0)
{
    d_cache_index = cache_root + CACHE_INDEX;

    d_cache_table = new CacheEntries*[CACHE_TABLE_SIZE];

    // Initialize the cache table.
    for (int i = 0; i < CACHE_TABLE_SIZE; ++i)
	d_cache_table[i] = 0;

    cache_index_read();
}

/** Called by for_each inside ~HTTPCache().
    @param e The cache entry to delete. */

static inline void
delete_cache_entry(HTTPCacheTable::CacheEntry *e)
{
    DBG2(cerr << "Deleting CacheEntry: " << e << endl);
    delete e;
}

HTTPCacheTable::~HTTPCacheTable()
{
    for (int i = 0; i < CACHE_TABLE_SIZE; ++i) {
        HTTPCacheTable::CacheEntries *cp = get_cache_table()[i];
        if (cp) {
            // delete each entry
            for_each(cp->begin(), cp->end(), delete_cache_entry);

            // now delete the vector that held the entries
            delete get_cache_table()[i];
            get_cache_table()[i] = 0;
        }
    }

    delete[] d_cache_table;
}

/** Functor which deletes and nulls a single CacheEntry if it has expired.
    This functor is called by expired_gc which then uses the
    erase(remove(...) ...) idiom to really remove all the vector entries that
    belonged to the deleted CacheEntry objects.

    @see expired_gc. */

class DeleteExpired : public unary_function<HTTPCacheTable::CacheEntry *&, void> {
	time_t d_time;
	HTTPCacheTable &d_table;

public:
	DeleteExpired(HTTPCacheTable &table, time_t t) :
		d_time(t), d_table(table) {
		if (!t)
			d_time = time(0); // 0 == now
	} 

	void operator()(HTTPCacheTable::CacheEntry *&e) {
		if (e && !e->readers && (e->freshness_lifetime
				< (e->corrected_initial_age + (d_time - e->response_time)))) {
			DBG(cerr << "Deleting expired cache entry: " << e->url << endl);
			d_table.remove_cache_entry(e);
			delete e; e = 0;
		}
	}
};

// @param time base deletes againt this time, defaults to 0 (now)
void HTTPCacheTable::delete_expired_entries(time_t time) {
	// Walk through and delete all the expired entries.
	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
		HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
		if (slot) {
			for_each(slot->begin(), slot->end(), DeleteExpired(*this, time));
			slot->erase(remove(slot->begin(), slot->end(),
					static_cast<HTTPCacheTable::CacheEntry *>(0)), slot->end());
		}
	}
}

/** Functor which deletes and nulls a single CacheEntry which has less than
    or equal to \c hits hits or if it is larger than the cache's
    max_entry_size property.

    @see hits_gc. */

class DeleteByHits : public unary_function<HTTPCacheTable::CacheEntry *&, void> {
	HTTPCacheTable &d_table;
	int d_hits;

public:
	DeleteByHits(HTTPCacheTable &table, int hits) :
		d_table(table), d_hits(hits) {
	}

	void operator()(HTTPCacheTable::CacheEntry *&e) {
		if (e && !e->readers && e->hits <= d_hits) {
			DBG(cerr << "Deleting cache entry: " << e->url << endl);
			d_table.remove_cache_entry(e);
			delete e; e = 0;
		}
	}
};

void 
HTTPCacheTable::delete_by_hits(int hits) {
    for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
        if (get_cache_table()[cnt]) {
            HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
            for_each(slot->begin(), slot->end(), DeleteByHits(*this, hits));
            slot->erase(remove(slot->begin(), slot->end(),
                               static_cast<HTTPCacheTable::CacheEntry*>(0)),
                        slot->end());

        }
    }
}

/** Functor which deletes and nulls a single CacheEntry which is larger than 
    a given size.
    @see hits_gc. */

class DeleteBySize : public unary_function<HTTPCacheTable::CacheEntry *&, void> {
	HTTPCacheTable &d_table;
	unsigned int d_size;

public:
	DeleteBySize(HTTPCacheTable &table, unsigned int size) :
		d_table(table), d_size(size) {
	}

	void operator()(HTTPCacheTable::CacheEntry *&e) {
		if (e && !e->readers && e->size > d_size) {
			DBG(cerr << "Deleting cache entry: " << e->url << endl);
			d_table.remove_cache_entry(e);
			delete e; e = 0;
		}
	}
};

void HTTPCacheTable::delete_by_size(unsigned int size) {
    for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
        if (get_cache_table()[cnt]) {
            HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
            for_each(slot->begin(), slot->end(), DeleteBySize(*this, size));
            slot->erase(remove(slot->begin(), slot->end(),
                               static_cast<HTTPCacheTable::CacheEntry*>(0)),
                        slot->end());

        }
    }
}

/** @name Cache Index

    These methods manage the cache's index file. Each cache holds an index
    file named \c .index which stores the cache's state information. */

//@{

/** Remove the cache index file.

    A private method.

    @return True if the file was deleted, otherwise false. */

bool
HTTPCacheTable::cache_index_delete()
{
	d_new_entries = 0;
	
    return (REMOVE_BOOL(d_cache_index.c_str()) == 0);
}

/** Read the saved set of cached entries from disk. Consistency between the
    in-memory cache and the index is maintained by only reading the index
    file when the HTTPCache object is created!

    A private method.

    @return True when a cache index was found and read, false otherwise. */

bool
HTTPCacheTable::cache_index_read()
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

    int res = fclose(fp) ;
    if (res) {
        DBG(cerr << "HTTPCache::cache_index_read - Failed to close " << (void *)fp << endl);
    }

    d_new_entries = 0;
    
    return true;
}

/** Parse one line of the index file.

    A private method.

    @param line A single line from the \c .index file.
    @return A CacheEntry initialized with the information from \c line. */

HTTPCacheTable::CacheEntry *
HTTPCacheTable::cache_index_parse_line(const char *line)
{
    // Read the line and create the cache object
	HTTPCacheTable::CacheEntry *entry = new HTTPCacheTable::CacheEntry;
    istringstream iss(line);
    iss >> entry->url;
    iss >> entry->cachename;

    iss >> entry->etag;
    if (entry->etag == CACHE_EMPTY_ETAG)
        entry->etag = "";

    iss >> entry->lm;
    iss >> entry->expires;
    iss >> entry->size;
    iss >> entry->range; // range is not used. 10/02/02 jhrg

    iss >> entry->hash;
    iss >> entry->hits;
    iss >> entry->freshness_lifetime;
    iss >> entry->response_time;
    iss >> entry->corrected_initial_age;

    iss >> entry->must_revalidate;

    return entry;
}

/** Functor which writes a single CacheEntry to the \c .index file. */

class WriteOneCacheEntry :
	public unary_function<HTTPCacheTable::CacheEntry *, void>
{

    FILE *d_fp;

public:
    WriteOneCacheEntry(FILE *fp) : d_fp(fp)
    {}

    void operator()(HTTPCacheTable::CacheEntry *e)
    {
        if (e && fprintf(d_fp,
                         "%s %s %s %ld %ld %ld %c %d %d %ld %ld %ld %c\r\n",
                         e->url.c_str(),
                         e->cachename.c_str(),
                         e->etag == "" ? CACHE_EMPTY_ETAG : e->etag.c_str(),
                         (long)(e->lm),
                         (long)(e->expires),
                         e->size,
                         e->range ? '1' : '0', // not used. 10/02/02 jhrg
                         e->hash,
                         e->hits,
                         (long)(e->freshness_lifetime),
                         (long)(e->response_time),
                         (long)(e->corrected_initial_age),
                         e->must_revalidate ? '1' : '0') < 0)
            throw Error("Cache Index. Error writing cache index\n");
    }
};

/** Walk through the list of cached objects and write the cache index file to
    disk. If the file does not exist, it is created. If the file does exist,
    it is overwritten. As a side effect, zero the new_entries counter.

    A private method.

    @exception Error Thrown if the index file cannot be opened for writing.
    @note The HTTPCache destructor calls this method and silently ignores
    this exception. */
void
HTTPCacheTable::cache_index_write()
{
    DBG(cerr << "Cache Index. Writing index " << d_cache_index << endl);

    // Open the file for writing.
    FILE * fp = NULL;
    if ((fp = fopen(d_cache_index.c_str(), "wb")) == NULL) {
        throw Error(string("Cache Index. Can't open `") + d_cache_index
                    + string("' for writing"));
    }

    // Walk through the list and write it out. The format is really
    // simple as we keep it all in ASCII.

    for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
        HTTPCacheTable::CacheEntries *cp = get_cache_table()[cnt];
        if (cp)
            for_each(cp->begin(), cp->end(), WriteOneCacheEntry(fp));
    }

    /* Done writing */
    int res = fclose(fp);
    if (res) {
        DBG(cerr << "HTTPCache::cache_index_write - Failed to close "
            << (void *)fp << endl);
    }

    d_new_entries = 0;
}

//@} End of the cache index methods.
/** Create the directory path for cache file. The cache uses a set of
    directories within d_cache_root to store individual responses. The name
    of the directory that holds a given response is the value returned by the
    get_hash() function (i.e., it's a number). If the directory exists, this
    method does nothing.

    A private method.

    @param hash The hash value (i.e., directory name). An integer between 0
    and CACHE_TABLE_SIZE (See HTTPCache.h).
    @return The pathname to the directory (even if it already existed).
    @exception Error Thrown if the directory cannot be created.*/

string
HTTPCacheTable::create_hash_directory(int hash)
{
    struct stat stat_info;
    ostringstream path;

    path << d_cache_root << hash;
    string p = path.str();

    if (stat(p.c_str(), &stat_info) == -1) {
        DBG2(cerr << "Cache....... Create dir " << p << endl);
        if (MKDIR(p.c_str(), 0777) < 0) {
            DBG2(cerr << "Cache....... Can't create..." << endl);
            throw Error("Could not create cache slot to hold response! Check the write permissions on your disk cache directory. Cache root: " + d_cache_root + ".");
        }
    }
    else {
        DBG2(cerr << "Cache....... Directory " << p << " already exists"
             << endl);
    }

    return p;
}

/** Create the directory for this url (using the hash value from get_hash())
    and a file within that directory to hold the response's information. The
    cache name and cache_body_fd fields of \c entry are updated.

    mkstemp opens the file it creates, which is a good thing but it makes
    tracking resources hard for the HTTPCache object (because an exception
    might cause a file descriptor resource leak). So I close that file
    descriptor here.

    A private method.

    @param entry The cache entry object to operate on.
    @exception Error If the file for the response's body cannot be created. */

void
HTTPCacheTable::create_location(HTTPCacheTable::CacheEntry *entry)
{
    string hash_dir = create_hash_directory(entry->hash);
#ifdef WIN32
    hash_dir += "\\dodsXXXXXX";
#else
    hash_dir += "/dodsXXXXXX"; // mkstemp uses six characters.
#endif

    // mkstemp uses the storage passed to it; must be writable and local.
    // char *templat = new char[hash_dir.size() + 1];
    vector<char> templat(hash_dir.size() + 1);
    strncpy(&templat[0], hash_dir.c_str(), hash_dir.size() + 1);

    // Open truncated for update. NB: mkstemp() returns a file descriptor.
    // man mkstemp says "... The file is opened with the O_EXCL flag,
    // guaranteeing that when mkstemp returns successfully we are the only
    // user." 09/19/02 jhrg
#ifndef WIN32
    // Make sure that temp files are accessible only by the owner.
    umask(077);
#endif
    int fd = MKSTEMP(&templat[0]); // fd mode is 666 or 600 (Unix)
    if (fd < 0) {
        // delete[] templat; templat = 0;
        close(fd);
        throw Error("The HTTP Cache could not create a file to hold the response; it will not be cached.");
    }

    entry->cachename = &templat[0];
    // delete[] templat; templat = 0;
    close(fd);
}


/** compute real disk space for an entry. */
static inline int
entry_disk_space(int size, unsigned int block_size)
{
    unsigned int num_of_blocks = (size + block_size) / block_size;
    
    DBG(cerr << "size: " << size << ", block_size: " << block_size
        << ", num_of_blocks: " << num_of_blocks << endl);

    return num_of_blocks * block_size;
}

/** @name Methods to manipulate instances of CacheEntry. */

//@{

/** Add a CacheEntry to the cache table. As each entry is read, load it into
    the in-memory cache table and update the HTTPCache's current_size. The
    later is used by the garbage collection method.

    @param entry The CacheEntry instance to add. */
void
HTTPCacheTable::add_entry_to_cache_table(CacheEntry *entry)
{
    int hash = entry->hash;
    if (hash > CACHE_TABLE_SIZE-1 || hash < 0)
        throw InternalErr(__FILE__, __LINE__, "Hash value too large!");

    if (!d_cache_table[hash])
        d_cache_table[hash] = new CacheEntries;

    d_cache_table[hash]->push_back(entry);
    
    DBG(cerr << "add_entry_to_cache_table, current_size: " << d_current_size
        << ", entry->size: " << entry->size << ", block size: " << d_block_size 
        << endl);
    
    d_current_size += entry_disk_space(entry->size, d_block_size);

    DBG(cerr << "add_entry_to_cache_table, current_size: " << d_current_size << endl);
    
    increment_new_entries();
}

/** Get a pointer to a CacheEntry from the cache table.

    @param url Look for this URL. */
HTTPCacheTable::CacheEntry *
HTTPCacheTable::get_locked_entry_from_cache_table(const string &url) /*const*/
{
    return get_locked_entry_from_cache_table(get_hash(url), url);
}

/** Get a pointer to a CacheEntry from the cache table. Providing a way to
    pass the hash code into this method makes it easier to test for correct
    behavior when two entries collide. 10/07/02 jhrg

    @param hash The hash code for \c url.
    @param url Look for this URL.
    @return The matching CacheEntry instance or NULL if none was found. */
HTTPCacheTable::CacheEntry *
HTTPCacheTable::get_locked_entry_from_cache_table(int hash, const string &url) /*const*/
{
    DBG(cerr << "url: " << url << "; hash: " << hash << endl);
    DBG(cerr << "d_cache_table: " << hex << d_cache_table << dec << endl);
    if (d_cache_table[hash]) {
	CacheEntries *cp = d_cache_table[hash];
	for (CacheEntriesIter i = cp->begin(); i != cp->end(); ++i) {
	    // Must test *i because perform_garbage_collection may have
	    // removed this entry; the CacheEntry will then be null.
	    if ((*i) && (*i)->url == url) {
		(*i)->lock_read_response(); // Lock the response
		return *i;
	    }
	}
    }

    return 0;
}

/** Get a pointer to a CacheEntry from the cache table. Providing a way to
    pass the hash code into this method makes it easier to test for correct
    behavior when two entries collide. 10/07/02 jhrg

    @param url Look for this URL.
    @return The matching CacheEntry instance or NULL if none was found. */
HTTPCacheTable::CacheEntry *
HTTPCacheTable::get_write_locked_entry_from_cache_table(const string &url)
{
	int hash = get_hash(url);
    if (d_cache_table[hash]) {
        CacheEntries *cp = d_cache_table[hash];
        for (CacheEntriesIter i = cp->begin(); i != cp->end(); ++i) {
            // Must test *i because perform_garbage_collection may have
            // removed this entry; the CacheEntry will then be null.
            if ((*i) && (*i)->url == url) {
            	(*i)->lock_write_response();	// Lock the response
            	return *i;
            }
        }
    }

    return 0;
}

/** Remove a CacheEntry. This means delete the entry's files on disk and free
    the CacheEntry object. The caller should null the entry's pointer in the
    cache_table. The total size of the cache is decremented once the entry is
    deleted.

    @param entry The CacheEntry to delete.
    @exception InternalErr Thrown if \c entry is in use. */
void
HTTPCacheTable::remove_cache_entry(HTTPCacheTable::CacheEntry *entry)
{
    // This should never happen; all calls to this method are protected by
    // the caller, hence the InternalErr.
    if (entry->readers)
        throw InternalErr(__FILE__, __LINE__, "Tried to delete a cache entry that is in use.");

    REMOVE(entry->cachename.c_str());
    REMOVE(string(entry->cachename + CACHE_META).c_str());

    DBG(cerr << "remove_cache_entry, current_size: " << get_current_size() << endl);

    unsigned int eds = entry_disk_space(entry->size, get_block_size());
    set_current_size((eds > get_current_size()) ? 0 : get_current_size() - eds);
    
    DBG(cerr << "remove_cache_entry, current_size: " << get_current_size() << endl);
}

/** Functor which deletes and nulls a CacheEntry if the given entry matches
    the url. */
class DeleteCacheEntry: public unary_function<HTTPCacheTable::CacheEntry *&, void>
{
    string d_url;
    HTTPCacheTable *d_cache_table;

public:
    DeleteCacheEntry(HTTPCacheTable *c, const string &url)
            : d_url(url), d_cache_table(c)
    {}

    void operator()(HTTPCacheTable::CacheEntry *&e)
    {
        if (e && e->url == d_url) {
        	e->lock_write_response();
            d_cache_table->remove_cache_entry(e);
        	e->unlock_write_response();
            delete e; e = 0;
        }
    }
};

/** Find the CacheEntry for the given url and remove both its information in
    the persistent store and the entry in d_cache_table. If \c url is not in
    the cache, this method does nothing.

    @param url Remove this URL's entry.
    @exception InternalErr Thrown if the CacheEntry for \c url is locked. */
void
HTTPCacheTable::remove_entry_from_cache_table(const string &url)
{
    int hash = get_hash(url);
    if (d_cache_table[hash]) {
        CacheEntries *cp = d_cache_table[hash];
        for_each(cp->begin(), cp->end(), DeleteCacheEntry(this, url));
        cp->erase(remove(cp->begin(), cp->end(), static_cast<HTTPCacheTable::CacheEntry*>(0)),
                  cp->end());
    }
}

/** Functor to delete and null all unlocked HTTPCacheTable::CacheEntry objects. */

class DeleteUnlockedCacheEntry: public unary_function<HTTPCacheTable::CacheEntry *&, void> {
    HTTPCacheTable &d_table;

public:
    DeleteUnlockedCacheEntry(HTTPCacheTable &t) :
	d_table(t)
    {
    }
    void operator()(HTTPCacheTable::CacheEntry *&e)
    {
	if (e) {
	    d_table.remove_cache_entry(e);
	    delete e;
	    e = 0;
	}
    }
};

void HTTPCacheTable::delete_all_entries()
{
    // Walk through the cache table and, for every entry in the cache, delete
    // it on disk and in the cache table.
    for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
	HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
	if (slot) {
	    for_each(slot->begin(), slot->end(), DeleteUnlockedCacheEntry(*this));
	    slot->erase(remove(slot->begin(), slot->end(), static_cast<HTTPCacheTable::CacheEntry *> (0)), slot->end());
	}
    }

    cache_index_delete();
}

/** Calculate the corrected_initial_age of the object. We use the time when
    this function is called as the response_time as this is when we have
    received the complete response. This may cause a delay if the response
    header is very big but should not cause any incorrect behavior.

    A private method.

    @param entry The CacheEntry object.
    @param default_expiration The default value of the cached object's
    expiration time.
    @param request_time When was the request made? I think this value must be
    passed into the method that calls this method... */

void
HTTPCacheTable::calculate_time(HTTPCacheTable::CacheEntry *entry, int default_expiration, time_t request_time)
{
    entry->response_time = time(NULL);
    time_t apparent_age = max(0, static_cast<int>(entry->response_time - entry->date));
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
                freshness_lifetime = default_expiration;
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

/** Parse various headers from the vector (which can be retrieved from
    libcurl once a response is received) and load the CacheEntry object with
    values. This method should only be called with headers from a response
    (it should not be used to parse request headers).

    A private method.

    @param entry Store values from the headers here.
    @param max_entry_size DO not cache entries larger than this.
    @param headers A vector of header lines. */

void HTTPCacheTable::parse_headers(HTTPCacheTable::CacheEntry *entry, unsigned long max_entry_size,
	const vector<string> &headers)
{
    vector<string>::const_iterator i;
    for (i = headers.begin(); i != headers.end(); ++i) {
	// skip a blank header.
	if ((*i).empty())
	    continue;

	string::size_type colon = (*i).find(':');

	// skip a header with no colon in it.
	if (colon == string::npos)
	    continue;

	string header = (*i).substr(0, (*i).find(':'));
	string value = (*i).substr((*i).find(": ") + 2);
	DBG2(cerr << "Header: " << header << endl);DBG2(cerr << "Value: " << value << endl);

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
	    unsigned long clength = strtoul(value.c_str(), 0, 0);
	    if (clength > max_entry_size)
		entry->set_no_cache(true);
	}
	else if (header == "Cache-Control") {
	    // Ignored Cache-Control values: public, private, no-transform,
	    // proxy-revalidate, s-max-age. These are used by shared caches.
	    // See section 14.9 of RFC 2612. 10/02/02 jhrg
	    if (value == "no-cache" || value == "no-store")
		// Note that we *can* store a 'no-store' response in volatile
		// memory according to RFC 2616 (section 14.9.2) but those
		// will be rare coming from DAP servers. 10/02/02 jhrg
		entry->set_no_cache(true);
	    else if (value == "must-revalidate")
		entry->must_revalidate = true;
	    else if (value.find("max-age") != string::npos) {
		string max_age = value.substr(value.find("=" + 1));
		entry->max_age = parse_time(max_age.c_str());
	    }
	}
    }
}

//@} End of the CacheEntry methods.

// @TODO Change name to record locked response
void HTTPCacheTable::bind_entry_to_data(HTTPCacheTable::CacheEntry *entry, FILE *body) {
	entry->hits++;  // Mark hit
    d_locked_entries[body] = entry; // record lock, see release_cached_r...
}

void HTTPCacheTable::uncouple_entry_from_data(FILE *body) {

    HTTPCacheTable::CacheEntry *entry = d_locked_entries[body];
    if (!entry)
        throw InternalErr("There is no cache entry for the response given.");

    d_locked_entries.erase(body);
    entry->unlock_read_response();

    if (entry->readers < 0)
        throw InternalErr("An unlocked entry was released");
}

bool HTTPCacheTable::is_locked_read_responses() {
	return !d_locked_entries.empty();
}

} // namespace libdap
