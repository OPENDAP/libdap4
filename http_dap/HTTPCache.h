
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

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "HTTPCacheDisconnectedMode.h"

#define DUMP_FREQUENCY (10) // Dump index every x loads

#define NO_LM_EXPIRATION (24 * 3600) // 24 hours

#define MEGA (0x100000L)
#define CACHE_TOTAL_SIZE (20)    // Default cache size is 20M
#define CACHE_FOLDER_PCT (10)    // 10% of cache size for meta info etc.
#define CACHE_GC_PCT (10)        // 10% of cache size free after GC
#define MIN_CACHE_TOTAL_SIZE (5) // 5M Min cache size
#define MAX_CACHE_ENTRY_SIZE (3) // 3M Max size of single cached entry

namespace libdap {

class HTTPCacheTable;

// This function is exported so the test code can use it too.
bool is_hop_by_hop_header(const std::string &header);

/** Implements a multi-process MT-safe HTTP 1.1 compliant (mostly) cache.

    <i>Clients that run as users lacking a writable HOME directory MUST
    disable this cache. Use Connect::set_cache_enable(false).</i>

    The original design of this class was taken from the W3C libwww software,
    written by Henrik Frystyk Nielsen, Copyright MIT 1995. See the file
    MIT_COPYRIGHT. This software is a complete rewrite in C++ with additional
    features useful to the DODS and OPeNDAP projects.

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

    Update documentation: get_cache_response() now also serves as
    is_url_in_cache() and is_url_valid() should only be called after a locked
    cached response is accessed using get_cached_response(). These lock the
    cache for reading. The methods cache_response() and update_response()
    lock an entry for writing.

    Check that the lock-for-write and lock-for-read work together since
    it's possible that an entry in use might have a stream of readers and never
    free the 'read-lock' thus blocking a writer.

    @author James Gallagher <jgallagher@opendap.org> */
class HTTPCache {
private:
    std::string d_cache_root;
    int d_cache_lock_fd = -1; // Lock for multiprocess use.
    std::string d_cache_lock_file;

    bool d_cache_enabled = false;
    bool d_cache_protected = false;
    CacheDisconnectedMode d_cache_disconnected = DISCONNECT_NONE;
    bool d_expire_ignored = false;
    bool d_always_validate = false;

    unsigned long d_total_size = CACHE_TOTAL_SIZE * MEGA;              // How much can we store?
    unsigned long d_folder_size = CACHE_TOTAL_SIZE / CACHE_FOLDER_PCT; // How much of that is metadata?
    unsigned long d_gc_buffer = CACHE_TOTAL_SIZE / CACHE_GC_PCT;       // How much memory needed as buffer?
    unsigned long d_max_entry_size = MAX_CACHE_ENTRY_SIZE * MEGA;      // Max individual entry size.
    int d_default_expiration = NO_LM_EXPIRATION;

    std::vector<std::string> d_cache_control;
    // these are values read from a request-directive Cache-Control header.
    // Not to be confused with values read from the response or a cached
    // response (e.g., CacheEntry has a max_age field, too). These fields are
    // set when the set_cache_control method is called.
    time_t d_max_age = -1;
    time_t d_max_stale = -1; // -1: not set, 0:any response, >0 max time.
    time_t d_min_fresh = -1;

    // Lock non-const methods (also ones that use the STL).
    std::mutex d_cache_mutex;

    HTTPCacheTable *d_http_cache_table = nullptr;

    // d_open_files is used by the interrupt handler to clean up
    std::vector<std::string> d_open_files;

    void set_cache_root(const std::string &root = "");

    void create_cache_root(const std::string &cache_root) const;

    int m_initialize_cache_lock(const std::string &cache_lock) const;

    static void m_lock_cache_write(int fd);

    static void m_lock_cache_read(int fd);

    static void m_unlock_cache(int fd);

    static void m_exclusive_to_shared_lock(int fd);

    bool is_url_in_cache(const std::string &url);

    // I made these four methods, so they could be tested by HTTPCacheTest.
    // Otherwise, they would be static functions. jhrg 10/01/02
    void write_metadata(const std::string &cachename, const std::vector<std::string> &headers);

    void read_metadata(const std::string &cachename, std::vector<std::string> &headers) const;

    int write_body(const std::string &cachename, const FILE *src);

    FILE *open_body(const std::string &cachename);

    bool stopGC() const;

    bool startGC() const;

    void perform_garbage_collection();

    void too_big_gc();

    void expired_gc();

    void hits_gc();

    explicit HTTPCache(const std::string &cache_root);

    friend class HTTPCacheTest; // Unit tests
    friend class HTTPConnectTest;

    /**
     * @brief Lock the cache for writing.
     * Implements RAII for the multi-process write lock for the cache.
     *
     * This class has an extra method that enables the client to 'release' the
     * lock so that it wil NOT be released when the guard goes out of scope.
     *
     * @see mp_read_lock_guard for the corresponding read lock guard.
     */
    class mp_lock_guard {
    public:
        enum class operation { read, write };

    private:
        int d_fd;
        bool d_locked = false;
        bool d_released = false; // Use this so instances can go out of scope without releasing the lock.
        operation d_op;

    public:
        mp_lock_guard() = delete;

        mp_lock_guard(const mp_lock_guard &) = delete;

        mp_lock_guard &operator=(const mp_lock_guard &) = delete;

        mp_lock_guard(int fd, operation op) : d_fd(fd), d_op(op) {
            try {
                if (d_op == operation::write)
                    HTTPCache::m_lock_cache_write(d_fd);
                else
                    HTTPCache::m_lock_cache_read(d_fd);
                d_locked = true;
            } catch (const std::exception &e) {
                d_locked = false;
                // Log this case.
                std::cerr << "mp_lock_guard::mp_lock_guard() - Failed to lock the cache (" << e.what() << ")."
                          << std::endl;
            }
        }

        ~mp_lock_guard() {
            try {
                if (!d_released && d_locked) {
                    HTTPCache::m_unlock_cache(d_fd);
                }
            } catch (const std::exception &e) {
                // Log this case.
                std::cerr << "mp_lock_guard::~mp_lock_guard() - Failed to release the cache lock (" << e.what() << ")."
                          << std::endl;
            }
        }

        void release() { d_released = true; }
    };

public:
    // Delete the copy constructor and assignment operator to prevent copying
    HTTPCache() = delete;
    HTTPCache(const HTTPCache &) = delete;
    HTTPCache &operator=(const HTTPCache &) = delete;

    virtual ~HTTPCache();

    /** Get a pointer to the HTTP 1.1 compliant cache. If not already
       instantiated, this creates an instance of the HTTP cache object and
       initializes it to use \c cache_root as the location of the persistent
       store. If there's an index (\c .index) file in that directory, it is read
       as part of the initialization. If the cache has already been initialized,
       this method returns a pointer to that instance. Note HTTPCache uses the
       singleton pattern; A process may have only one instance of this object.
       Also note that HTTPCache is MT-safe.

       Default values: is_cache_enabled(): true, is_cache_protected(): false,
       is_expire_ignored(): false, the total size of the cache is 20M, 2M of that
       is reserved for response headers, during GC the cache is reduced to at
       least 18M (total size - 10% of the total size), and the max size for an
       individual entry is 3M. It is possible to change the size of the cache,
       but not to make it smaller than 5M. If expiration information is not sent
       with a response, it is assumed to expire in 24 hours.

       @param cache_root The fully qualified pathname of the directory which
       will hold the cache data (i.e., the persistent store).
       @return A pointer to the HTTPCache object. */

    static HTTPCache *instance(const std::string &cache_root = "") {
        // Create a local static object the first time the function is called
        static HTTPCache instance(cache_root);
        return &instance;
    }

    std::string get_cache_root() const;

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

    void set_cache_control(const std::vector<std::string> &cc);

    std::vector<std::string> get_cache_control() const;

    // This must lock for writing
    bool cache_response(const std::string &url, time_t request_time, const std::vector<std::string> &headers,
                        const FILE *body);

    void update_response(const std::string &url, time_t request_time, const std::vector<std::string> &headers);

    // This is separate from get_cached_response() because often an invalid
    // cache entry just needs a header update. That is best left to the HTTP
    // Connection code.
    bool is_url_valid(const std::string &url);

    // Lock these for reading
    std::vector<std::string> get_conditional_request_headers(const std::string &url);

    FILE *get_cached_response(const std::string &url, std::vector<std::string> &headers, std::string &cacheName);

    FILE *get_cached_response(const std::string &url, std::vector<std::string> &headers);

    FILE *get_cached_response(const std::string &url);

    void release_cached_response(FILE *response);

    void purge_cache();
};

} // namespace libdap

#endif // _http_cache_h
