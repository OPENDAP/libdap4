
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002 OPeNDAP, Inc. 
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
 
#ifndef _http_cache_h
#define _http_cache_h

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <io.h>			// stat for win32? 09/05/02 jhrg
#else
#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <map>

#ifndef _error_h
#include "Error.h"
#endif

#ifndef _internalerr_h
#include "InternalErr.h"
#endif

// The private method HTTPCache::write_body() could, at one time, throw
// ResponseTooBig to signal that while writing a response body it was found
// to be bigger than the max_entry_size property. But I bagged that; the
// garbage collection methods remove entries larger than max_entry_size. It
// might be that a really big entry belongs in the cache so long as it
// doesn't push other entries out. 10/07/02 jhrg
#ifndef _response_too_big_err_h
#include "ResponseTooBigErr.h"
#endif

#ifndef _http_cache_disconnected_mode_h
#include "HTTPCacheDisconnectedMode.h"
#endif

const int CACHE_TABLE_SIZE = 1499;

using namespace std;

/** Implements a single-user MT-safe HTTP 1.1 compliant (mostly) cache.

    The design of this class was taken from the W3C libwww software. That
    code was originally written by Henrik Frystyk Nielsen, Copyright MIT
    1995. See the file MIT_COPYRIGHT. This software is a complete rewrite in
    C++ with additional features useful to the DODS and OPeNDAP projects.

    This cache does not implement range checking. Partial responses should
    not be cached (HFN's version did, but it doesn't mesh well with the DAP
    for which this is being written).

    The cache uses the local file system to store responses. If it is being
    used in a MT application, care should be taken to ensure that the number
    of available file descriptors is not exceeded.

    In addition, when used in a MT program only one thread should use the
    mutators to set property values. Even though the methods are robust WRT
    MT software, having several threads change values of cache's properties
    will lead to odd behavior on the part of the cache. Many of the public
    methods lock access to the class' interface. This is noted in the
    documentation for those methods.

    Even though the public interface to the cache is typically locked when
    accessed, an extra locking mechanism is in place for `entries' which are
    accessed. If a thread accesses a entry, that response must be locked to
    prevent it from being updated until the thread tells the cache that it's
    no longer using it. The methods get_cache_response() and
    get_cache_response_body() both lock an entry; use
    release_cache_response() to release the lock. Entries are locked using a
    combination of a counter and a mutex. The following methods block when
    called on a locked entry: is_url_valid(),
    get_conditional_request_headers(), update_response(). (The locking scheme
    could be modified so that a distinction is made between reading from and
    writing to an entry. In this case is_url_valid() and
    get_conditional_request_headers() would only lock when an entry is in use
    for writing. But I haven't done that.)

    @todo Because is_url_in_cache() and is_url_valid() are discrete, an
    interface that combines the two might be easier to use. Or maybe if
    is_url_valid() threw a special exception if the entry was missing.
    Something to help clients deal with URLs that are removed from the cache
    in between calls to the two methods.

    @todo Change the way responses are locked. See the three methods:

    FILE *get_cached_response(const string &url, vector<string> &headers)
	throw(Error, InternalErr);
    FILE *get_cached_response_body(const string &url) 
	throw(Error, InternalErr);
    void release_cached_response(FILE *response) throw(Error);

    What must change is that callers must tell the cache when they are done.
    Either replace these with a method that can be used for reading, so that
    when the method is no longer used, the lock is released OR have the
    method return an object which, when destroyed, frees the lock.

    @todo Change the entry locking scheme to distinguish between entries
    accessed for reading and for writing.

    @todo Test in MT software. Is the entry locking scheme good enough? The
    current software throws an exception if there's an attempt to modify an
    entry that is locked by another thread. Maybe it should block instead?
    Maybe we should provide a tests to see if an update would block (one that
    returns right away and one that blocks).

    @author James Gallagher <jgallagher@gso.uri.edu> */
class HTTPCache {
    /** A private class used to store information about responses in the
	cache's volatile memory. 

	About entry locking: An entry is locked using both a mutex and a
	counter. The counter keeps track of how many clients are accessing a
	given entry while the mutex provides a guarantee that updates to the
	counter are MT-safe. In addition, the HTTPCache object maintains a
	map which binds the FILE* returned to a client with a given entry.
	This way the client can tell the HTTPCache object that it is done
	with <code>FILE *response</code> and the class can arrange to update
	the lock counter and mutex. */
public:
    struct CacheEntry {
	// Location
	string url;
	int hash;
	int hits;		// Hit counts

	string cachename;

	string etag;
	time_t lm;		// Last modified
	time_t expires;
	time_t date;		// From the response header.
	time_t age;
	time_t max_age;		// From Cache-Control

	long size;		// Size of cached entity body
	bool range;		// Range is not currently supported. 10/02/02
				// jhrg 

	time_t freshness_lifetime;
	time_t response_time;
	time_t corrected_initial_age;

	bool must_revalidate;
	bool no_cache;		// This field is not saved in the index.
	
	int locked;
#ifdef HAVE_PTHREAD_H
	pthread_mutex_t lock;
#endif
	CacheEntry() : url(""), hash(-1), hits(0), cachename(""), 
		       etag(""), lm(-1),
		       expires(-1), date(-1), age(-1), max_age(-1), size(0),
		       range(false), freshness_lifetime(0), response_time(0),
		       corrected_initial_age(0), must_revalidate(false),
		       no_cache(false), locked(0)
	{}
   };

private:
    string d_cache_root;
    string d_cache_index;
    FILE *d_locked_open_file;	// Lock for single process use.

    bool d_cache_enabled;
    bool d_cache_protected;
    CacheDisconnectedMode d_cache_disconnected;
    bool d_expire_ignored;
    bool d_always_validate;

    int d_total_size;		// How much can we store?
    int d_folder_size;		// How much of that is meta data?
    int d_gc_buffer;		// How much memory needed as buffer?
    int d_max_entry_size;	// Max individual entry size.
    int d_current_size;
    int d_default_expiration;

    vector<string> d_cache_control;
    // these are values read from a request-directive Cache-Control header.
    // Not to be confused with values read from the response or a cached
    // response (e.g., CacheEntry has a max_age field, too). These fields are
    // set when the set_cache_control method is called.
    time_t d_max_age;
    time_t d_max_stale;		// -1: not set, 0:any response, >0 max time.
    time_t d_min_fresh;

    int d_new_entries;		// How many entries since index write?

    // Lock non-const methods (also ones that use the STL).
#ifdef HAVE_PTHREAD_H
    pthread_mutex_t d_cache_mutex;
#endif


    // Typedefs for CacheTable. A CacheTable is a vector of vectors of
    // CacheEntries. The outer vector is accessed using the hash value.
    // Entries with matching hashes occupy successive positions in the inner
    // vector (that's how hash collisions are resolved). Search the inner
    // vector for a specific match.
    typedef vector<CacheEntry *> CachePointers;
    typedef CachePointers::iterator CachePointersIter;

    // CACHE_TABLE_SIZE is used by the static function get_hash defined in
    // HTTPCache.cc. The table is indexed by the various cache entries' hash
    // code. 10/01/02 jhrg
    typedef CachePointers *CacheTable[CACHE_TABLE_SIZE];

    CacheTable d_cache_table;

    map<FILE *, CacheEntry *> d_locked_entries;

    static HTTPCache *_instance;

    friend class HTTPCacheTest;	// Unit tests
    
    // Functors used with STL algorithms

    friend struct DeleteExpired;
    friend struct DeleteByHits;
    friend struct DeleteCacheEntry;
    friend struct DeleteUnlockedCacheEntry;
    friend struct WriteOneCacheEntry;

    // Private methods

    void clone(const HTTPCache &cache) {}

    HTTPCache(const HTTPCache &cache) { 
	clone(cache); 
    }
    
    HTTPCache() {}

    HTTPCache(string cache_root, bool force) throw(Error);

    HTTPCache &operator=(const HTTPCache &rhs) { 
	if (this != &rhs)
	    clone(rhs);
	return *this;
    }

    static void delete_instance(); // Run by atexit (hence static)
	
    CacheEntry *cache_index_parse_line(const char *line);
    bool cache_index_read();
    bool cache_index_delete();

    void set_cache_root(const string &root = "") throw(Error);

    bool get_single_user_lock(bool force = false);
    void release_single_user_lock();

    void add_entry_to_cache_table(CacheEntry *e);
    void remove_entry_from_cache_table(const string &url) throw(InternalErr);
    void parse_headers(CacheEntry *entry, const vector<string> &headers);
    void calculate_time (CacheEntry *entry, time_t request_time);
    void remove_cache_entry(CacheEntry *entry) throw(InternalErr);
    CacheEntry *get_entry_from_cache_table(const string &url) const;
    CacheEntry *get_entry_from_cache_table(int hash, const string &url) const;

    // I made these four methods so they could be tested by HTTPCacheTest.
    // Otherwise they would be static functions in HTTPCache.cc. 10/01/02
    // jhrg
    void write_metadata(const string &cachename, const vector<string> &headers)
	throw(InternalErr);
    void read_metadata(const string &cachename, vector<string> &headers) 
	const throw(InternalErr);
    int write_body(const string &cachename, const FILE *src) 
	throw(InternalErr, ResponseTooBigErr);
    FILE *open_body(const string &cachename) const throw(InternalErr);

    void create_cache_root(const string &cache_root) throw(Error);

    string create_hash_directory(int hash) throw(Error);
    void create_location(CacheEntry *entry) throw(Error);

    bool stopGC() const;
    bool startGC() const;

    void perform_garbage_collection();
    void expired_gc();
    void hits_gc();

public:
    static HTTPCache *instance(const string &cache_root, bool force = false);
    virtual ~HTTPCache();

    string get_cache_root() const;

    void cache_index_write() throw(Error);

    void set_cache_enabled(bool mode);
    bool is_cache_enabled() const;

    void set_cache_protected(bool mode);
    bool is_cache_protected() const;

    void set_cache_disconnected(CacheDisconnectedMode mode);
    CacheDisconnectedMode get_cache_disconnected() const;

    void set_expire_ignored(bool mode);
    bool is_expire_ignored() const;

    void set_max_size(int size);
    int get_max_size() const;

    void set_max_entry_size(int size);
    int get_max_entry_size() const;

    void set_default_expiration(int exp_time);
    int get_default_expiration() const;

    void set_always_validate(bool validate);
    bool get_always_validate() const;

    void set_cache_control(const vector<string> &cc) throw(InternalErr);
    vector<string> get_cache_control() const;

    bool cache_response(const string &url, time_t request_time,
			const vector<string> &headers, const FILE *body)
	throw(InternalErr);
    vector<string> get_conditional_request_headers(const string &url)
	throw(Error);
    void update_response(const string &url, time_t request_time,
			 const vector<string> &headers) throw(Error);

    bool is_url_in_cache(const string &url) const;
    bool is_url_valid(const string &url) throw(Error);
    FILE *get_cached_response(const string &url, vector<string> &headers)
	throw(Error, InternalErr);
    FILE *get_cached_response_body(const string &url) 
	throw(Error, InternalErr);
    void release_cached_response(FILE *response) throw(Error);

    void purge_cache() throw(Error);
};

// $Log: HTTPCache.h,v $
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.11  2002/12/29 23:03:48  jimg
// Updated the copyrights for these source files.
//
// Revision 1.1.2.10  2002/12/27 00:57:55  jimg
// Fixed up the comments.
// Added a note about current interface and the locking of responses from the
// cache. Use an object to control the mutex/lock on a response delivered from
// the cache.
//
// Revision 1.1.2.9  2002/11/04 07:30:42  rmorris
// Private CacheEntry won't fly here.  As used elsewhere, it must not be
// either private or protected.  It must be public.  I generatlly made it
// so this code would compile under both win32 and unix, but it hasn't
// apparently been runtime tested anywhere as yet.  An assumption I make
// since it won't compile and some of the problems were just typo's
//
// Revision 1.1.2.8  2002/10/18 22:40:08  jimg
// Updated the documentation with a blurb about the locking of entries.
//
// Revision 1.1.2.7  2002/10/18 00:10:56  jimg
// Added a mutex to CacheEntry.
//
// Revision 1.1.2.6  2002/10/11 20:57:49  jimg
// Fixed up the comments.
//
// Revision 1.1.2.5  2002/10/11 00:20:30  jimg
// Added methods for conditional responses.
//
// Revision 1.1.2.4  2002/10/10 06:46:44  jimg
// The class is now MT-safe.
//
// Revision 1.1.2.3  2002/10/08 05:23:16  jimg
// The Cache now works. Still to do: Make the cache MT-safe (which will
// probably require that it become a singleton, but maybe not), protect
// all non-const methods with a mutex and devise a locking strategy so that
// entries are not read while being written. Once the cache is MT-safe, it
// must be made to handle updates.
//
// Revision 1.1.2.2  2002/09/17 00:16:45  jimg
// Added methods to lock the cache and find/set its root directory.
//
// Revision 1.1.2.1  2002/09/12 22:38:55  jimg
// Preliminary version. Added at this time because the Makefile includes it.
//

#endif // _http_cache_h
