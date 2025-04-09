
// Copyright (c) 2023 OPeNDAP, Inc.
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

#ifndef LIBDAP4_MP_LOCK_GUARD_H
#define LIBDAP4_MP_LOCK_GUARD_H

#include <exception>
#include <iostream>
#include <string>

#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace libdap {

static inline std::string get_errno() {
    const char *s_err = strerror(errno);
    return s_err ? s_err : "unknown error";
}

/**
 * @brief Interface for the logger used by mp_lock_guard
 * The idea behind this abstract class (aka, interface) is that the client
 * of mp_lock_guard can provide a logger that will behave as they want.
 */
class mp_lock_guard_logger {
public:
    mp_lock_guard_logger() = default;
    virtual ~mp_lock_guard_logger() = default;

    virtual void log(const std::string &msg) const = 0;

    virtual void error(const std::string &msg) const = 0;
};

/**
 * @brief Default logger for mp_lock_guard
 * For this implementation of mp_lock_guard_logger, the log() method writes
 * to std::cerr and the error() method throws a std::runtime_error.
 */
class mp_lock_guard_logger_default : public mp_lock_guard_logger {
public:
    mp_lock_guard_logger_default() = default;
    ~mp_lock_guard_logger_default() override = default;

    void log(const std::string &msg) const override { std::cerr << "~mp_lock_guard: " << msg << std::endl; }

    void error(const std::string &msg) const override { throw std::runtime_error("mp_lock_guard: " + msg); }
};

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
    int d_fd = -1;
    bool d_locked = false;
    bool d_released = false; // Use this so instances can go out of scope without releasing the lock.
    operation d_op;
    const mp_lock_guard_logger &d_logger;

    void m_get_lock() {
        if (d_op == operation::write) {
            struct flock lock {};
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            if (fcntl(d_fd, F_SETLKW, &lock) == -1) {
                d_logger.error("Could not write lock the cache-control file: " + get_errno());
            }
        } else {
            struct flock lock {};
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            if (fcntl(d_fd, F_SETLKW, &lock) == -1) {
                d_logger.error("Could not read lock the cache-control file: " + get_errno());
            }
        }
        d_locked = true;
    }

    friend class mp_lock_guard_test;

public:
    mp_lock_guard() = delete;

    mp_lock_guard(const mp_lock_guard &) = delete;

    mp_lock_guard &operator=(const mp_lock_guard &) = delete;

    /** @brief Lock the cache for reading or writing. These are blocking locks.
     *
     * @param fd The file descriptor of the cache control file.
     * @param op The operation to perform. If op is 'write' then the lock is
     * a write lock; if op is 'read' then the lock is a read lock.
     */
    mp_lock_guard(int fd, operation op, const mp_lock_guard_logger &logger = mp_lock_guard_logger_default())
        : d_fd(fd), d_op(op), d_logger(logger) {
        m_get_lock();
    }

    /**
     * @brief Unlock the cache. Works for both read and write locks.
     * @note This will not throw an exception if the unlock fails when using the
     * default logger. Also, if the release() method has been called, then this
     * will not unlock the file. It is assumed that some other code will do that using
     * the unlock() method.
     */
    ~mp_lock_guard() {
        if (!d_released && d_locked) {
            struct flock lock {};
            lock.l_type = F_UNLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            if (fcntl(d_fd, F_SETLK, &lock) == -1) {
                d_logger.log("Could not unlock the cache-control file: " + get_errno());
            }
        }
    }

    /**
     * Release control of the lock so that control can exit scope and the lock
     *  can be released somewhere else.
     */
    void release() { d_released = true; }

    /**
     * Unlock the cache.
     */
    static void unlock(int fd, const mp_lock_guard_logger &logger = mp_lock_guard_logger_default()) {
        struct flock lock {};
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        lock.l_pid = getpid();
        if (fcntl(fd, F_SETLK, &lock) == -1) {
            logger.error("Could not unlock the cache-control file: " + get_errno());
        }
    }
};

} // namespace libdap

#endif // LIBDAP4_MP_LOCK_GUARD_H
