
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

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>

#include "Error.h"
#include "InternalErr.h"
#include "ResponseTooBigErr.h"

#include "HTTPCache.h"
#include "HTTPCacheTable.h"

#include "debug.h"
#include "util.h"
#include "util_mit.h"

using namespace std;

const string CACHE_LOCATION{"/tmp/"};

const string CACHE_ROOT{"dods-cache/"};

const string CACHE_INDEX{".index"};
const string CACHE_LOCK{".lock"};
const string CACHE_META{".meta"};
const string CACHE_EMPTY_ETAG{"@cache@"};

const string DIR_SEPARATOR_CHAR{"/"};

namespace libdap {

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

HTTPCache::HTTPCache(const string &cache_root) {
    // This used to throw an Error object if we could not get the
    // single user lock. However, that results in an invalid object. It's
    // better to have an instance that has default values. If we cannot get
    // the lock, make sure to set the cache as *disabled*. 03/12/03 jhrg
    //
    // I fixed this block so that the cache root is set before we try to get
    // the single user lock. That was the fix for bug #661. To make that
    // work, I had to move the call to create_cache_root out of
    // set_cache_root(). 09/08/03 jhrg

    try {
        set_cache_root(cache_root); // sets d_cache_root, even if cache_root is empty.
        // It's OK to call create_cache_root if the directory already exists.
        create_cache_root(d_cache_root);
        string lock = d_cache_root + CACHE_LOCK;
        d_cache_lock_fd = m_initialize_cache_lock(lock);
        d_cache_lock_file = lock;

        struct stat s = {};
        int block_size;
        if (stat(cache_root.c_str(), &s) == 0)
            block_size = s.st_blksize;
        else
            block_size = 4096;

        d_http_cache_table = new HTTPCacheTable(d_cache_root, block_size);
        d_cache_enabled = true;
    } catch (const Error &) {
        // Write to a log here. 2/18/23 jhrg
        d_cache_enabled = false;
        DBG(cerr << "Failure to get the cache lock" << endl);
    }
}

/** Destroy an instance of HTTPCache. This writes the cache index and frees
    the in-memory cache table structure. The persistent cache (the response
    headers and bodies and the index file) are not removed. To remove those,
    either erase the directory that contains the cache using a file system
    command or use the purge_cache() method (which leaves the cache directory
    structure in place but removes all the cached information).
 */

HTTPCache::~HTTPCache() {
    try {
        if (startGC())
            perform_garbage_collection();

        d_http_cache_table->cache_index_write();
        delete d_http_cache_table;
    } catch (const Error &e) {
        // If the cache index cannot be written, we've got problems. However,
        // unless we're debugging, still free up the cache table in memory.
        // How should we let users know they cache index is not being
        // written?? 10/03/02 jhrg
        // Write a log message here. 2/18/23 jhrg
        DBG(cerr << e.get_error_message() << endl);
    }

    close(d_cache_lock_fd);
}

/** @name Garbage collection
    These private methods manage the garbage collection tasks for the cache. */
///@{

/** Enough removed from cache? A private method.
    @return True if enough has been removed from the cache. */

bool HTTPCache::stopGC() const {
    return (d_http_cache_table->get_current_size() + d_folder_size < d_total_size - d_gc_buffer);
}

/** Is there too much in the cache? A private method.

    @note This method does not count locked entries. See the note for hits_gc().
    @return True if garbage collection should be performed. */
bool HTTPCache::startGC() const {
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

void HTTPCache::perform_garbage_collection() {
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

void HTTPCache::expired_gc() {
    if (!d_expire_ignored) {
        d_http_cache_table->delete_expired_entries();
    }
}

/** Scan the cache for entries that are larger than max_entry_size. Also
    start removing entries with low hit counts. Start looking for entries
    with zero hits, then one, and so on. Stop when the method stopGC returns
    true. Locked entries are never removed.

    @note Potential infinite loop. What if more than 80% of the cache holds
    entries that are locked? One solution is to modify startGC() so that it
    does not count locked entries.

    @note Change this method to that it looks at the oldest entries first,
    using the CacheEntry::date to determine entry age. Using the current
    algorithm it's possible to remove the latest entry which is probably not
    what we want.

    A private method. */
void HTTPCache::hits_gc() {
    int hits = 0;

    if (startGC()) {
        while (!stopGC()) {
            d_http_cache_table->delete_by_hits(hits);
            hits++;
        }
    }
}

/** Scan the current cache table and remove anything that is too big.
    Don't remove locked entries.

    A private method. */
void HTTPCache::too_big_gc() {
    if (startGC())
        d_http_cache_table->delete_by_size(d_max_entry_size);
}

//@} End of the garbage collection methods.

/// @name New multi-process locking methods
/// @{

static inline string get_errno() {
    const char *s_err = strerror(errno);
    return s_err ? s_err : "unknown error";
}

// Build a lock of a certain type.
//
// Using whence == SEEK_SET with start and len set to zero means lock the whole file.
// jhrg 9/8/18
static inline struct flock *lock(short type) {
    static struct flock lock;
    lock.l_type = type;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    return &lock;
}

/**
 * A blocking call to create a file locked for write.
 *
 * @note Used by the methods m_initialize_cache_info() and create_and_lock()
 *
 * @param file_name The name of the file to lock
 * @param ref_fd Return-value parameter that holds the file descriptor that's locked.
 * @return True when the lock is acquired, false if the file already exists.
 */
static bool create_locked_file(const string &file_name, int &ref_fd) {
    DBG(cerr << "BEGIN file: " << file_name << endl);

    int fd;
    if ((fd = open(file_name.c_str(), O_CREAT | O_EXCL | O_RDWR, 0660)) < 0) {
        if (errno == EEXIST) {
            return false;
        } else {
            throw InternalErr(__FILE__, __LINE__, file_name + ": " + get_errno());
        }
    }

    struct flock *l = lock(F_WRLCK);
    // F_SETLKW == set lock, blocking
    if (fcntl(fd, F_SETLKW, l) == -1) {
        close(fd);
        ostringstream oss;
        oss << "cache process: " << l->l_pid << " triggered a locking error for '" << file_name << "': " << get_errno();
        throw InternalErr(__FILE__, __LINE__, oss.str());
    }

    DBG(cerr << "END file: " << file_name << endl);

    // Success
    ref_fd = fd;
    return true;
}

/**
 * @brief Either create the cache lock or open an existing one.
 *
 * @param cache_lock The name of the cache lock file. This must be the same
 * for all users of the cache
 * @return A file descriptor to the open lock file. The file is not locked,
 * but the op descriptor can be passed to m_lock_cache_read(), ..., and locked.
 *
 * @see m_lock_cache_write(), m_lock_cache_read(), m_unlock_cache() for the methods
 * that use advisory file locking to lock the cache.
 */
int HTTPCache::m_initialize_cache_lock(const string &cache_lock) const {
    DBG(cerr << "BEGIN" << endl);

    int fd;
    if (create_locked_file(cache_lock, fd)) {
        // This leaves the fd file descriptor open, but unlocked
        m_unlock_cache(fd);
    } else {
        if ((fd = open(cache_lock.c_str(), O_RDWR)) == -1) {
            throw InternalErr(__FILE__, __LINE__,
                              "Failed to open cache lock file: " + cache_lock + " errno: " + get_errno());
        }
    }

    DBG(cerr << "END" << endl);
    return fd;
}

/** Get an exclusive lock on the 'cache info' file. The 'cache info' file
 * is used to control certain cache actions, ensuring that they are atomic.
 * These include making sure that the create_and_lock() and read_and_lock()
 * operations are atomic as well as the purge and related operations.
 *
 * @note This is intended to be used internally only but might be useful in
 * some settings.
 */
void HTTPCache::m_lock_cache_write(int fd) {
    DBG(cerr << "d_cache_info_fd: " << d_cache_info_fd << endl);

    // F_SETLKW == set lock, blocking
    if (fcntl(fd, F_SETLKW, lock(F_WRLCK)) == -1) {
        throw InternalErr(__FILE__, __LINE__, "An error occurred trying to lock the cache-control file" + get_errno());
    }

    DBG(cerr << "lock status: " << lockStatus(d_cache_info_fd) << endl);
}

/** Get a shared lock on the 'cache info' file.
 *
 */
void HTTPCache::m_lock_cache_read(int fd) {
    DBG(cerr << "d_cache_info_fd: " << d_cache_info_fd << endl);

    if (fcntl(fd, F_SETLKW, lock(F_RDLCK)) == -1) {
        throw InternalErr(__FILE__, __LINE__, "An error occurred trying to lock the cache-control file" + get_errno());
    }

    DBG(cerr << "lock status: " << lockStatus(d_cache_info_fd) << endl);
}

/** Unlock the cache info file.
 *
 * @note This is intended to be used internally only but might be useful in
 * some settings.
 */
void HTTPCache::m_unlock_cache(int fd) {
    DBG(cerr << "d_cache_info_fd: " << d_cache_info_fd << endl);

    if (fcntl(fd, F_SETLK, lock(F_UNLCK)) == -1) {
        throw InternalErr(__FILE__, __LINE__,
                          "An error occurred trying to unlock the cache-control file" + get_errno());
    }

    DBG(cerr << "lock status: " << lockStatus(d_cache_info_fd) << endl);
}

/**
 * @brief Transfer from an exclusive lock to a shared lock.
 *
 * If the file has an exclusive write lock on it, change that to a shared
 * read lock. This is an atomic operation. If the call to fcntl(2) is
 * protected by locking the cache, a dead lock will result given typical use
 * of this class. This method exists to help with the situation where one
 * process has the cache locked and is blocking on a shared read lock for
 * a file that a second process has locked exclusively (for writing). By
 * changing the exclusive lock to a shared lock, the first process can get
 * its shared lock and then release the cache.
 *
 * @param fd The file descriptor that is exclusively locked and which, on
 * exit, will have a shared lock.
 */
void HTTPCache::m_exclusive_to_shared_lock(int fd) {
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        throw InternalErr(__FILE__, __LINE__, get_errno());
    }

    DBG(cerr << "lock status: " << lockStatus(fd) << endl);
}

/// @} New multi-process locking methods

/** @name Accessors and Mutators for various properties. */
/// @{

/** Get the current cache root directory.
    @return A string that contains the cache root directory. */

string HTTPCache::get_cache_root() const { return d_cache_root; }

/** Create the cache's root directory. This is the persistent store used by
    the cache. Paths must always end in DIR_SEPARATOR_CHAR.

    A private method.

    @param cache_root The pathname to the desired cache root directory.
    @exception Error Thrown if the given pathname cannot be created. */

void HTTPCache::create_cache_root(const string &cache_root) const {
    // Save the mask
    mode_t mask = umask(S_IRWXO);

    // Ignore the error if the directory exists
    errno = 0;
    if (mkdir(cache_root.c_str(), S_IRWXU | S_IRWXG) < 0 && errno != EEXIST) {
        umask(mask);
        throw Error("HTTPCache::create_cache_root: Could not create the directory for the cache at '" + cache_root +
                    "' (" + strerror(errno) + ").");
    }

    // Restore the mask
    umask(mask);
}

inline bool ends_with(std::string const &value, std::string const &ending) {
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

/** Set the cache's root directory to the given path.

    Note that in most cases callers should look for this path in the user's
    .dodsrc file.

    A private method.

    @see RCReader
    @param root Set the cache root to this pathname. Defaults to "".
    @exception Error Thrown if the path can neither be deduced nor created. */

void HTTPCache::set_cache_root(const string &root) {
    if (!root.empty()) {
        d_cache_root = root;
        // cache root should end in /.
        if (!ends_with(d_cache_root, DIR_SEPARATOR_CHAR))
            d_cache_root += DIR_SEPARATOR_CHAR;
    } else {
        // If no cache root has been indicated then look for a suitable location.
        d_cache_root = CACHE_LOCATION;

        if (!ends_with(d_cache_root, DIR_SEPARATOR_CHAR))
            d_cache_root += DIR_SEPARATOR_CHAR;

        d_cache_root += CACHE_ROOT;
    }

    // Test d_http_cache_table because this method can be called before that
    // instance is created and also can be called later to change the cache
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

void HTTPCache::set_cache_enabled(bool mode) {
    lock_guard<mutex> lock{d_cache_mutex};

    d_cache_enabled = mode;
}

/** Is the cache currently enabled? */

bool HTTPCache::is_cache_enabled() const { return d_cache_enabled; }

/** Set the cache's disconnected property. The cache can operate either
    disconnected from the network or using a proxy cache (but tell that proxy
    not to use the network).

    This method locks the class' interface.

    @param mode One of DISCONNECT_NONE, DISCONNECT_NORMAL or
    DISCONNECT_EXTERNAL.
    @see CacheDIsconnectedMode */

void HTTPCache::set_cache_disconnected(CacheDisconnectedMode mode) {
    lock_guard<mutex> lock{d_cache_mutex};

    d_cache_disconnected = mode;
}

/** Get the cache's disconnected mode property. */

CacheDisconnectedMode HTTPCache::get_cache_disconnected() const { return d_cache_disconnected; }

/** How should the cache handle the Expires header?
    Default: no

    This method locks the class' interface.

    @param mode True if a responses Expires header should be ignored, False
    otherwise. */

void HTTPCache::set_expire_ignored(bool mode) {
    lock_guard<mutex> lock{d_cache_mutex};

    d_expire_ignored = mode;
}

/* Is the cache ignoring Expires headers returned with responses that have
   been cached? */

bool HTTPCache::is_expire_ignored() const { return d_expire_ignored; }

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

void HTTPCache::set_max_size(unsigned long size) {
    lock_guard<mutex> lock{d_cache_mutex};

    unsigned long new_size = size < MIN_CACHE_TOTAL_SIZE ? MIN_CACHE_TOTAL_SIZE * MEGA : size * MEGA;
    unsigned long old_size = d_total_size;
    d_total_size = new_size;
    d_folder_size = d_total_size / CACHE_FOLDER_PCT;
    d_gc_buffer = d_total_size / CACHE_GC_PCT;

    if (new_size < old_size && startGC()) {
        perform_garbage_collection();
        d_http_cache_table->cache_index_write();
    }
}

/** How big is the cache? The value returned is the size in megabytes. */

unsigned long HTTPCache::get_max_size() const { return d_total_size / MEGA; }

/** Set the maximum size for a single entry in the cache.

    Default: 3M

    This method locks the class' interface.

    @param size The size in megabytes. */

void HTTPCache::set_max_entry_size(unsigned long size) {
    lock_guard<mutex> lock{d_cache_mutex};

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

/** Get the maximum size of an individual entry in the cache.

    @return The maximum size in megabytes. */

unsigned long HTTPCache::get_max_entry_size() const { return d_max_entry_size / MEGA; }

/** Set the default expiration time. Use the <i>default expiration</i>
    property to determine when a cached response becomes stale if the
    response lacks the information necessary to compute a specific value.

    Default: 24 hours (86,400 seconds)

    This method locks the class' interface.

    @param exp_time The time in seconds. */

void HTTPCache::set_default_expiration(const int exp_time) {
    lock_guard<mutex> lock{d_cache_mutex};

    d_default_expiration = exp_time;
}

/** Get the default expiration time used by the cache. */

int HTTPCache::get_default_expiration() const { return d_default_expiration; }

/** Should every cache entry be validated?
    @param validate True if every cache entry should be validated before
    being used. */

void HTTPCache::set_always_validate(bool validate) { d_always_validate = validate; }

/** Should every cache entry be validated before each use?
    @return True if all cache entries require validation. */

bool HTTPCache::get_always_validate() const { return d_always_validate; }

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

void HTTPCache::set_cache_control(const vector<string> &cc) {
    lock_guard<mutex> lock{d_cache_mutex};

    d_cache_control = cc;

    for (auto &line : cc) {
        string header = line.substr(0, line.find(':'));
        string value = line.substr(line.find(": ") + 2);
        if (header != "Cache-Control") {
            throw InternalErr(__FILE__, __LINE__, "Expected cache control header not found.");
        } else {
            if (value == "no-cache" || value == "no-store")
                d_cache_enabled = false;
            else if (value.find("max-age") != string::npos) {
                string max_age = value.substr(value.find('=') + 1);
                d_max_age = parse_time(max_age.c_str());
            } else if (value == "max-stale")
                d_max_stale = 0; // indicates will take anything;
            else if (value.find("max-stale") != string::npos) {
                string max_stale = value.substr(value.find('=') + 1);
                d_max_stale = parse_time(max_stale.c_str());
            } else if (value.find("min-fresh") != string::npos) {
                string min_fresh = value.substr(value.find('=') + 1);
                d_min_fresh = parse_time(min_fresh.c_str());
            }
        }
    }
}

/** Get the Cache-Control headers.

    @return A vector of strings, one string for each header. */

vector<string> HTTPCache::get_cache_control() const { return d_cache_control; }

/// @}

/** Look in the cache for the given \c url. Is it in the cache table?

    This method locks the class' interface.

    @note This is really here for testing purposes. It is not used by the
    HTTPCache class. Even when it returns true, the response may not be
    in the cache. It is possible that the response was removed from the
    cache after the entry is unlocked but before the method returns.

    @param url The url to look for.
    @return True if \c url is found, otherwise False. */

bool HTTPCache::is_url_in_cache(const string &url) {
    lock_guard<mutex> lock{d_cache_mutex};

    HTTPCacheTable::CacheEntry *entry = d_http_cache_table->get_read_locked_entry_from_cache_table(url);

    if (entry) {
        entry->unlock_read_response();
        return true;
    }
    return false;
}

/** Is the header a hop by hop header? If so, we're not supposed to store it
    in the cache. See RFC 2616, Section 13.5.1.

    @return True if the header is, otherwise False. */

bool is_hop_by_hop_header(const string &header) {
    return header.find("Connection") != string::npos || header.find("Keep-Alive") != string::npos ||
           header.find("Proxy-Authenticate") != string::npos || header.find("Proxy-Authorization") != string::npos ||
           header.find("Transfer-Encoding") != string::npos || header.find("Upgrade") != string::npos;
}

/** Dump the headers out to the meta data file. The file is truncated if it
    already exists.

    @todo This code could be replaced with STL/iostream stuff.

    A private method.

    @param cachename Base name of file for meta data.
    @param headers A vector of strings, one header per string.
    @exception InternalErr Thrown if the file cannot be opened. */

void HTTPCache::write_metadata(const string &cachename, const vector<string> &headers) {
    string fname = cachename + CACHE_META;
    d_open_files.push_back(fname);

    FILE *dest = fopen(fname.c_str(), "w");
    if (!dest) {
        throw InternalErr(__FILE__, __LINE__, "Could not open named cache entry file.");
    }

    vector<string>::const_iterator i;
    for (auto &header : headers) /*i = headers.begin(); i != headers.end(); ++i)*/ {
        if (!is_hop_by_hop_header(header)) {
            size_t s = fwrite(header.c_str(), header.size(), 1, dest);
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
        DBG(cerr << "HTTPCache::write_metadata - Failed to close " << dest << endl);
    }

    d_open_files.pop_back();
}

/** Read headers from a .meta.

    @todo This code could be replaced with STL/iostream code.

    A private method.

    @param cachename The name of the file in the persistent store.
    @param headers The headers are returned using this parameter.
    @exception InternalErr Thrown if the file cannot be opened. */

void HTTPCache::read_metadata(const string &cachename, vector<string> &headers) const {
    FILE *md = fopen(string(cachename + CACHE_META).c_str(), "r");
    if (!md) {
        throw InternalErr(__FILE__, __LINE__, "Could not open named cache entry meta data file.");
    }

    const size_t line_buf_len = 1024;
    char line[line_buf_len];
    while (!feof(md) && fgets(line, line_buf_len, md)) {
        line[std::min(line_buf_len, strnlen(line, line_buf_len)) - 1] = '\0'; // erase newline
        headers.emplace_back(line);
    }

    int res = fclose(md);
    if (res) {
        DBG(cerr << "HTTPCache::read_metadata - Failed to close " << md << endl);
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

int HTTPCache::write_body(const string &cachename, const FILE *src) {
    d_open_files.push_back(cachename);

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
        DBG2(sleep(3));
    }

    if (ferror(const_cast<FILE *>(src)) || ferror(dest)) {
        int res = fclose(dest);
        res = res & unlink(cachename.c_str());
        if (res) {
            DBG(cerr << "HTTPCache::write_body - Failed to close/unlink " << dest << endl);
        }
        throw InternalErr(__FILE__, __LINE__, "I/O error transferring data to the cache.");
    }

    rewind(const_cast<FILE *>(src));

    int res = fclose(dest);
    if (res) {
        DBG(cerr << "HTTPCache::write_body - Failed to close " << dest << endl);
    }

    d_open_files.pop_back();

    return total;
}

/** Get a pointer to file that contains the body of a cached response. The
    returned FILE* can be used both for reading and for writing.

    A private method.

    @param cachename The name of the file that holds the response body.
    @exception InternalErr Thrown if the file cannot be opened. */

FILE *HTTPCache::open_body(const string &cachename) {
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
    with (only) new meta data, use update_response().

    This method locks the class' interface AND blocks until the cache can
    be locked for writing.

    @param url A string which holds the request URL.
    @param request_time The time when the request was made, in seconds since
    1 Jan 1970.
    @param headers A vector of strings which hold the response headers.
    @param body A FILE * to a file which holds the response body.
    @return True if the response was cached, False if the response could not
    be cached.
    @exception InternalErr Thrown if there was a I/O error while writing to
    the persistent store. */

bool HTTPCache::cache_response(const string &url, time_t request_time, const vector<string> &headers,
                               const FILE *body) {

    // If this is not an http or https URL, don't cache.
    if (url.find("http:") == string::npos && url.find("https:") == string::npos) {
        return false;
    }

    lock_guard<mutex> lock{d_cache_mutex};
    mp_lock_guard write_lock{d_cache_lock_fd,
                             mp_lock_guard::operation::write}; // Blocks until the write lock is acquired.

    // This does nothing if url is not already in the cache. It's
    // more efficient to do this than to first check and see if the entry
    // exists. 10/10/02 jhrg
    d_http_cache_table->remove_entry_from_cache_table(url);
    auto *entry = new HTTPCacheTable::CacheEntry(url);
    entry->lock_write_response();

    try {
        d_http_cache_table->parse_headers(entry, d_max_entry_size, headers); // etag, lm, date, age, expires, max_age.
        if (entry->is_no_cache()) {
            DBG(cerr << "Not cache-able; deleting HTTPCacheTable::CacheEntry: " << entry << "(" << url << ")" << endl);
            entry->unlock_write_response();
            delete entry;
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
    } catch (const ResponseTooBigErr &e) {
        // Oops. Bummer. Clean up and exit.
        DBG(cerr << e.get_error_message() << endl);
        remove(entry->get_cachename().c_str());
        remove(string(entry->get_cachename() + CACHE_META).c_str());
        entry->unlock_write_response();
        delete entry;
        return false;
    }

    if (d_http_cache_table->get_new_entries() > DUMP_FREQUENCY) {
        if (startGC())
            perform_garbage_collection();

        d_http_cache_table->cache_index_write(); // resets new_entries
    }

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

vector<string> HTTPCache::get_conditional_request_headers(const string &url) {

    HTTPCacheTable::CacheEntry *entry = nullptr;
    vector<string> headers;

    lock_guard<mutex> lock{d_cache_mutex};
    mp_lock_guard read_lock{d_cache_lock_fd, mp_lock_guard::operation::read}; // Blocks until the lock is acquired.

    try {
        entry = d_http_cache_table->get_read_locked_entry_from_cache_table(url);
        if (!entry)
            throw Error(internal_error, "There is no cache entry for the URL: " + url);

        if (!entry->get_etag().empty())
            headers.push_back(string("If-None-Match: ") + entry->get_etag());

        if (entry->get_lm() > 0) {
            time_t lm = entry->get_lm();
            headers.push_back(string("If-Modified-Since: ") + date_time_str(&lm));
        } else if (entry->get_max_age() > 0) {
            time_t max_age = entry->get_max_age();
            headers.push_back(string("If-Modified-Since: ") + date_time_str(&max_age));
        } else if (entry->get_expires() > 0) {
            time_t expires = entry->get_expires();
            headers.push_back(string("If-Modified-Since: ") + date_time_str(&expires));
        }
        entry->unlock_read_response();
    } catch (...) {
        if (entry) {
            entry->unlock_read_response();
        }
        throw;
    }

    return headers;
}

/** Functor/Predicate which orders two MIME headers based on the header name
    only (discounting the value). */
struct HeaderLess {
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

void HTTPCache::update_response(const string &url, time_t request_time, const vector<string> &headers) {
    HTTPCacheTable::CacheEntry *entry = nullptr;

    try {
        lock_guard<mutex> lock{d_cache_mutex};
        mp_lock_guard write_lock{d_cache_lock_fd,
                                 mp_lock_guard::operation::write}; // Blocks until the lock is acquired.

        entry = d_http_cache_table->get_write_locked_entry_from_cache_table(url);
        if (!entry)
            throw Error(internal_error, "There is no cache entry for the URL: " + url);

        // Merge the new headers with the exiting HTTPCacheTable::CacheEntry object.
        d_http_cache_table->parse_headers(entry, d_max_entry_size, headers);

        // Update corrected_initial_age, freshness_lifetime, response_time.
        d_http_cache_table->calculate_time(entry, d_default_expiration, request_time);

        // Merge the new headers with those in the persistent store. How:
        // Load the new headers into a set, then merge the old headers. Since
        // set<> ignores duplicates, old headers with the same name as a new
        // header will go into the bit bucket. Define a special compare
        // functor to make sure that headers are compared using only their
        // name and not their value too.
        set<string, HeaderLess> merged_headers;

        // Load in the new headers
        copy(headers.begin(), headers.end(), inserter(merged_headers, merged_headers.begin()));

        // Get the old headers and load them in.
        vector<string> old_headers;
        read_metadata(entry->get_cachename(), old_headers);
        copy(old_headers.begin(), old_headers.end(), inserter(merged_headers, merged_headers.begin()));

        // Read the values back out. Use reverse iterators with back_inserter
        // to preserve header order. NB: vector<> does not support push_front
        // so we can't use front_inserter(). 01/09/03 jhrg
        vector<string> result;
        copy(merged_headers.rbegin(), merged_headers.rend(), back_inserter(result));

        write_metadata(entry->get_cachename(), result);
        entry->unlock_write_response();
    } catch (...) {
        if (entry) {
            entry->unlock_read_response();
        }
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

bool HTTPCache::is_url_valid(const string &url) {

    bool freshness;
    HTTPCacheTable::CacheEntry *entry = nullptr;

    try {
        if (d_always_validate) {
            return false; // force re-validation.
        }

        lock_guard<mutex> lock{d_cache_mutex};
        mp_lock_guard read_lock{d_cache_lock_fd, mp_lock_guard::operation::read}; // Blocks until the lock is acquired.

        entry = d_http_cache_table->get_read_locked_entry_from_cache_table(url);
        if (!entry)
            throw Error(internal_error, "There is no cache entry for the URL: " + url);

        // If we supported range requests, we'd need code here to check if
        // there was only a partial response in the cache. 10/02/02 jhrg

        // In case this entry is of type "must-revalidate" then we consider it
        // invalid.
        if (entry->get_must_revalidate()) {
            entry->unlock_read_response();
            return false;
        }

        time_t resident_time = time(nullptr) - entry->get_response_time();
        time_t current_age = entry->get_corrected_initial_age() + resident_time;

        // Check that the max-age, max-stale, and min-fresh directives
        // given in the request cache control header is followed.
        if (d_max_age >= 0 && current_age > d_max_age) {
            entry->unlock_read_response();
            return false;
        }
        if (d_min_fresh >= 0 && entry->get_freshness_lifetime() < current_age + d_min_fresh) {
            entry->unlock_read_response();
            return false;
        }

        freshness = (entry->get_freshness_lifetime() + (d_max_stale >= 0 ? d_max_stale : 0) > current_age);
        entry->unlock_read_response();
    } catch (...) {
        if (entry) {
            entry->unlock_read_response();
        }
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

    This method locks the class' interface AND it read locks the cache. The
    cache will remain read locked until release_cached_response() is called.
    There is no RAII protection for the cache read lock in this method.

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

FILE *HTTPCache::get_cached_response(const string &url, vector<string> &headers, string &cacheName) {
    FILE *body = nullptr;
    HTTPCacheTable::CacheEntry *entry = nullptr;

    try {
        lock_guard<mutex> lock{d_cache_mutex};
        mp_lock_guard read_lock{d_cache_lock_fd, mp_lock_guard::operation::read}; // Blocks until the lock is acquired.

        DBG(cerr << "Getting the cached response for " << url << endl);

        entry = d_http_cache_table->get_read_locked_entry_from_cache_table(url);
        if (!entry) {
            return nullptr;
        }

        cacheName = entry->get_cachename();
        read_metadata(entry->get_cachename(), headers);

        DBG(cerr << "Headers just read from cache: " << endl);
        DBGN(copy(headers.begin(), headers.end(), ostream_iterator<string>(cerr, "\n")));

        body = open_body(entry->get_cachename());

        DBG(cerr << "Returning: " << url << " from the cache." << endl);

        d_http_cache_table->bind_entry_to_data(entry, body);

        // Set 'read_lock' so that it will not unlock the cache when it goes out of scope.
        // The client must call release_cached_response() to unlock the cache.
        read_lock.release();
    } catch (...) {
        if (body != nullptr)
            fclose(body);
        throw;
    }

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

FILE *HTTPCache::get_cached_response(const string &url, vector<string> &headers) {
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

FILE *HTTPCache::get_cached_response(const string &url) {
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

void HTTPCache::release_cached_response(FILE *body) {
    lock_guard<mutex> lock{d_cache_mutex};

    // fclose(body); This results in a seg fault on linux jhrg 8/27/13
    d_http_cache_table->uncouple_entry_from_data(body);
    m_unlock_cache(d_cache_lock_fd);
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

void HTTPCache::purge_cache() {
    lock_guard<mutex> lock{d_cache_mutex};
    mp_lock_guard write_lock{d_cache_lock_fd, mp_lock_guard::operation::write}; // Blocks until the lock is acquired.

    if (d_http_cache_table->is_locked_read_responses())
        throw Error(internal_error, "Attempt to purge the cache with entries in use.");

    d_http_cache_table->delete_all_entries();
}

} // namespace libdap
