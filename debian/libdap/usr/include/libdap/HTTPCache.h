
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2008 OPeNDAP, Inc.
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

#ifndef _http_cache_h
#define _http_cache_h

#include <pthread.h>

#ifdef WIN32
#include <io.h>   // stat for win32? 09/05/02 jhrg
#endif

#include <string>
#include <vector>
#include <map>

#include "HTTPCacheTable.h" // included for macros

#include "HTTPCacheDisconnectedMode.h"
//using namespace std;

namespace libdap
{

class HTTPCacheTabe;

// This function is exported so the test code can use it too.
bool is_hop_by_hop_header(const string &header);

/** Implements a multi-process MT-safe HTTP 1.1 compliant (mostly) cache.

    <i>Clients that run as users lacking a writable HOME directory MUST
    disable this cache. Use Connect::set_cache_enable(false).</i>

    The original design of this class was taken from the W3C libwww software, 
    written by Henrik Frystyk Nielsen, Copyright MIT
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
    no longer using it. The method get_cache_response() and
    get_cache_response_body() both lock an entry; use
    release_cache_response() to release the lock. Entries are locked using a
    combination of a counter and a mutex. The following methods block when
    called on a locked entry: is_url_valid(),
    get_conditional_request_headers(), update_response(). (The locking scheme
    could be modified so that a distinction is made between reading from and
    writing to an entry. In this case is_url_valid() and
    get_conditional_request_headers() would only lock when an entry is in use
    for writing. But I haven't done that.)

	@todo Update documentation: get_cache_response() now also serves as 
	is_url_in_cache() and is_url_valid() should only be called after a locked
	cached response is accessed using get_cahced_response(). These lock the
	cache for reading. The methods cache_response() and update_response()
	lock an entry for writing.
	
	@todo Check that the lock-for-write and lock-for-read work together since
	it's possible that an entry in use might have a stream of readers and never
	free the 'read-lock' thus blocking a writer.
	
    @author James Gallagher <jgallagher@opendap.org> */
class HTTPCache
{
private:
    string d_cache_root;
    FILE *d_locked_open_file; // Lock for single process use.

    bool d_cache_enabled;
    bool d_cache_protected;
    CacheDisconnectedMode d_cache_disconnected;
    bool d_expire_ignored;
    bool d_always_validate;

    unsigned long d_total_size; // How much can we store?
    unsigned long d_folder_size; // How much of that is meta data?
    unsigned long d_gc_buffer; // How much memory needed as buffer?
    unsigned long d_max_entry_size; // Max individual entry size.
    int d_default_expiration;

    vector<string> d_cache_control;
    // these are values read from a request-directive Cache-Control header.
    // Not to be confused with values read from the response or a cached
    // response (e.g., CacheEntry has a max_age field, too). These fields are
    // set when the set_cache_control method is called.
    time_t d_max_age;
    time_t d_max_stale;  // -1: not set, 0:any response, >0 max time.
    time_t d_min_fresh;

    // Lock non-const methods (also ones that use the STL).
    pthread_mutex_t d_cache_mutex;
    
    HTTPCacheTable *d_http_cache_table;

    // d_open_files is used by the interrupt handler to clean up
    vector<string> d_open_files;

    static HTTPCache *_instance;

    friend class HTTPCacheTest; // Unit tests
    friend class HTTPConnectTest;

    friend class HTTPCacheInterruptHandler;

    // Private methods
    HTTPCache(const HTTPCache &);
    HTTPCache();
    HTTPCache &operator=(const HTTPCache &);

    HTTPCache(string cache_root, bool force);

    static void delete_instance(); // Run by atexit (hence static)
    
    void set_cache_root(const string &root = "");
    void create_cache_root(const string &cache_root);
    
    // These will go away when the cache can be used by multiple processes.
    bool get_single_user_lock(bool force = false);
    void release_single_user_lock();
    
    bool is_url_in_cache(const string &url);

    // I made these four methods so they could be tested by HTTPCacheTest.
    // Otherwise they would be static functions. jhrg 10/01/02
    void write_metadata(const string &cachename, const vector<string> &headers);
    void read_metadata(const string &cachename, vector<string> &headers);
    int write_body(const string &cachename, const FILE *src);
    FILE *open_body(const string &cachename);

    bool stopGC() const;
    bool startGC() const;

    void perform_garbage_collection();
    void too_big_gc();
    void expired_gc();
    void hits_gc();

public:
    static HTTPCache *instance(const string &cache_root, bool force = false);
    virtual ~HTTPCache();

    string get_cache_root() const;

    void set_cache_enabled(bool mode);
    bool is_cache_enabled() const;

    void set_cache_disconnected(CacheDisconnectedMode mode);
    CacheDisconnectedMode get_cache_disconnected() const;

    void set_expire_ignored(bool mode);
    bool is_expire_ignored() const;

    void set_max_size(unsigned long size);
    unsigned long get_max_size() const;

    void set_max_entry_size(unsigned long size);
    unsigned long get_max_entry_size() const;

    void set_default_expiration(int exp_time);
    int get_default_expiration() const;

    void set_always_validate(bool validate);
    bool get_always_validate() const;

    void set_cache_control(const vector<string> &cc);
    vector<string> get_cache_control();

    void lock_cache_interface() {
    	DBG(cerr << "Locking interface... ");
    	LOCK(&d_cache_mutex);
    	DBGN(cerr << "Done" << endl);
    }    	
    void unlock_cache_interface() {
    	DBG(cerr << "Unlocking interface... " );
    	UNLOCK(&d_cache_mutex);
    	DBGN(cerr << "Done" << endl);
    }
    
    // This must lock for writing
    bool cache_response(const string &url, time_t request_time,
                        const vector<string> &headers, const FILE *body);
    void update_response(const string &url, time_t request_time,
                         const vector<string> &headers);

    // This is separate from get_cached_response() because often an invalid
    // cache entry just needs a header update. That is best left to the HTTP
    // Connection code.
    bool is_url_valid(const string &url);
    
    // Lock these for reading
    vector<string> get_conditional_request_headers(const string &url);
    FILE *get_cached_response(const string &url, vector<string> &headers,
			      			  string &cacheName);
    FILE *get_cached_response(const string &url, vector<string> &headers);
    FILE *get_cached_response(const string &url);

    void release_cached_response(FILE *response);

    void purge_cache();
};

} // namespace libdap

#endif // _http_cache_h
