// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2008 OPeNDAP, Inc.
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

#ifndef _http_cache_table_h
#define _http_cache_table_h

#include <mutex>

#include <cstring>

#include <string>
#include <vector>
#include <map>

#ifndef _http_cache_h

#include "HTTPCache.h"

#endif

#ifndef _error_h
#include "Error.h"
#endif

#ifndef _internalerr_h
#include "InternalErr.h"
#endif

#ifndef _util_h

#include "util.h"

#endif

#ifndef _debug_h
#include "debug.h"
#endif

#define CACHE_META ".meta"
#define CACHE_INDEX ".index"
#define CACHE_EMPTY_ETAG "@cache@"

#define NO_LM_EXPIRATION (24*3600) // 24 hours
#define MAX_LM_EXPIRATION (48*3600) // Max expiration from LM

// If using LM to find the expiration then take 10% and no more than
// MAX_LM_EXPIRATION.
#ifndef LM_EXPIRATION
#define LM_EXPIRATION(t) (min((MAX_LM_EXPIRATION), static_cast<int>((t) / 10)))
#endif

namespace libdap {

int get_hash(const string &url);

/** The table of entries in the client-side cache. This class maintains a table
 of CacheEntries, where one instance of CacheEntry is made for
 each item in the cache. When an item is accessed it is either
 locked for reading or writing. When locked for reading the entry is
 recorded on a list of read-locked entries. The caller must explicitly
 free the entry for it to be removed from this list (which is the only
 way it can be opened for writing). An entry can be accessed by multiple
 readers but only one writer.

 @note The CacheEntry class used to contain a lock that was used to ensure
 that the entry was locked during any changes to any of its fields. That
 has been removed - its now the responsibility of the caller. This change
 was made because it's likely the caller will need to lock all of the methods
 that operate on a CacheEntry anyway, so the CacheEntry-specific lock was
 redundant. */
class HTTPCacheTable {
public:
    /** A struct used to store information about responses in the
     cache's volatile memory.

     About entry locking: An entry is locked using both a mutex and a
     counter. The counter keeps track of how many clients are accessing a
     given entry while the mutex provides a guarantee that updates to the
     counter are MT-safe. In addition, the HTTPCacheTable object maintains a
     map which binds the FILE* returned to a client with a given entry.
     This way the client can tell the HTTPCacheTable object that it is done
     with <code>FILE *response</code> and the class can arrange to update
     the lock counter and mutex. */
    struct CacheEntry {
    private:
        string url; // Location
        int hash = -1;
        int hits = 0; // Hit counts
        string cachename;

        string etag;
        time_t lm = -1; // Last modified
        time_t expires = -1;
        time_t date = -1; // From the response header.
        time_t age = -1;
        time_t max_age = -1; // From Cache-Control

        unsigned long size = 0; // Size of cached entity body
        bool range = false; // Range is not currently supported. 10/02/02 jhrg

        time_t freshness_lifetime = 0;
        time_t response_time = 0;
        time_t corrected_initial_age = 0;

        bool must_revalidate = false;
        bool no_cache = false; // This field is not saved in the index.

        std::atomic<int> readers;
        std::mutex d_response_lock; // set if being read
        std::mutex d_response_write_lock; // set if being written

        // Allow HTTPCacheTable methods access and the test class, too
        friend class HTTPCacheTable;
        friend class HTTPCacheTest;

        // Allow access by the functors used in HTTPCacheTable
        friend class DeleteCacheEntry;
        friend class WriteOneCacheEntry;
        friend class DeleteExpired;
        friend class DeleteByHits;
        friend class DeleteBySize;

    public:
        string get_cachename() const {
            return cachename;
        }

        string get_etag() const {
            return etag;
        }

        time_t get_lm() const {
            return lm;
        }

        time_t get_expires() const {
            return expires;
        }

        time_t get_max_age() const {
            return max_age;
        }

        void set_size(unsigned long sz) {
            size = sz;
        }

        time_t get_freshness_lifetime() const {
            return freshness_lifetime;
        }

        time_t get_response_time() const {
            return response_time;
        }

        time_t get_corrected_initial_age() const {
            return corrected_initial_age;
        }

        bool get_must_revalidate() const {
            return must_revalidate;
        }

        void set_no_cache(bool state) {
            no_cache = state;
        }

        bool is_no_cache() const {
            return no_cache;
        }

        void lock_read_response() {
            DBG(cerr << "Try locking read response... (");
            // if the response_lock cannot be acquired, it might be a reader or a writer. If it is a writer, then
            // we need to wait for the writer to finish. If it is a reader, then we don't care and increment the
            // reader count.
            if (d_response_lock.try_lock() == false) {
                d_response_write_lock.lock();
                d_response_write_lock.unlock();
            }

            readers++; // Record number of readers
            DBGN(cerr << "Done" << endl);
        }

        void unlock_read_response() {
            readers--;
            if (readers == 0) {
                DBG(cerr << "Unlocking read response... (");
                d_response_lock.unlock();
                DBGN(cerr << "Done" << endl);
            }
        }

        void lock_write_response() {
            DBG(cerr << "Locking write response... (");
            std::lock(d_response_lock, d_response_write_lock);
            DBGN(cerr << "Done" << endl);
        }

        void unlock_write_response() {
            DBG(cerr << "Unlocking write response... (");
            d_response_lock.unlock();
            d_response_write_lock.unlock();
            DBGN(cerr << "Done" << endl);
        }

        CacheEntry() = default;

        explicit CacheEntry(string u) : url(std::move(u)) {
            hash = get_hash(url);
        }
    };

    // Typedefs for CacheTable. A CacheTable is a vector of vectors of
    // CacheEntries. The outer vector is accessed using the hash value.
    // Entries with matching hashes occupy successive positions in the inner
    // vector (that's how hash collisions are resolved). Search the inner
    // vector for a specific match.
    typedef vector<CacheEntry *> CacheEntries;
    typedef CacheEntries::iterator CacheEntriesIter;

    typedef CacheEntries **CacheTable;    // Array of pointers to CacheEntries

    friend class HTTPCacheTest;

private:
    CacheTable d_cache_table;

    string d_cache_root;
    unsigned int d_block_size; // File block size.
    unsigned long d_current_size;

    string d_cache_index;
    int d_new_entries;

    map<FILE *, HTTPCacheTable::CacheEntry *> d_locked_entries;


    CacheTable &get_cache_table() {
        return d_cache_table;
    }

    CacheEntry *get_locked_entry_from_cache_table(int hash, const string &url); /*const*/

public:
    HTTPCacheTable(const string &cache_root, int block_size);
    HTTPCacheTable(const HTTPCacheTable &) = delete;
    HTTPCacheTable &operator=(const HTTPCacheTable &) = delete;
    HTTPCacheTable() = delete;

    virtual ~HTTPCacheTable();

    //@{ @name Accessors/Mutators
    unsigned long get_current_size() const {
        return d_current_size;
    }

    void set_current_size(unsigned long sz) {
        d_current_size = sz;
    }

    unsigned int get_block_size() const {
        return d_block_size;
    }

    void set_block_size(unsigned int sz) {
        d_block_size = sz;
    }

    int get_new_entries() const {
        return d_new_entries;
    }

    void increment_new_entries() {
        ++d_new_entries;
    }

    string get_cache_root() const {
        return d_cache_root;
    }

    void set_cache_root(const string &cr) {
        d_cache_root = cr;
    }
    //@}

    void delete_expired_entries(time_t time = 0);

    void delete_by_hits(int hits);

    void delete_by_size(unsigned int size);

    void delete_all_entries();

    bool cache_index_delete();

    bool cache_index_read();

    CacheEntry *cache_index_parse_line(const char *line);

    void cache_index_write();

    string create_hash_directory(int hash);

    void create_location(CacheEntry *entry);

    void add_entry_to_cache_table(CacheEntry *entry);

    void remove_cache_entry(const HTTPCacheTable::CacheEntry *entry);

    void remove_entry_from_cache_table(const string &url);

    CacheEntry *get_locked_entry_from_cache_table(const string &url);

    CacheEntry *get_write_locked_entry_from_cache_table(const string &url);

    void calculate_time(HTTPCacheTable::CacheEntry *entry, int default_expiration, time_t request_time);

    void parse_headers(HTTPCacheTable::CacheEntry *entry, unsigned long max_entry_size, const vector<string> &headers);

    // These should move back to HTTPCache
    void bind_entry_to_data(CacheEntry *entry, FILE *body);

    void uncouple_entry_from_data(FILE *body);

    bool is_locked_read_responses();
};

} // namespace libdap
#endif
