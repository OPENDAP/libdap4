//
// Created by James Gallagher on 3/1/23.
//

#ifndef LIBDAP4_MP_LOCK_GUARD_H
#define LIBDAP4_MP_LOCK_GUARD_H

#include <exception>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

static inline std::string get_errno()
{
    const char *s_err = strerror(errno);
    return s_err ? s_err : "unknown error";
}

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
    enum class operation {
        read, write
    };
private:
    int d_fd = -1;
    bool d_locked = false;
    bool d_released = false;    // Use this so instances can go out of scope without releasing the lock.
    operation d_op;
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
    mp_lock_guard(int fd, operation op) : d_fd(fd), d_op(op)
    {
        if (d_op == operation::write) {
            struct flock lock{};
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            if (fcntl(d_fd, F_SETLKW, &lock) == -1) {
                throw std::runtime_error("mp_lock_guard: Could not write lock the cache-control file: " + get_errno());
            }
        }
        else {
            struct flock lock{};
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            if (fcntl(d_fd, F_SETLKW, &lock) == -1) {
                throw std::runtime_error("mp_lock_guard: Could not read lock the cache-control file: " + get_errno());
            }
            d_locked = true;
        }
    }

    /// @brief Unlock the cache. Works for both read and write locks.
    ~mp_lock_guard() {
        if (!d_released && d_locked) {
            struct flock lock{};
            lock.l_type = F_UNLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            if (fcntl(d_fd, F_SETLK, &lock) == -1) {
                throw std::runtime_error("~mp_lock_guard: Could not unlock the cache-control file: " + get_errno());
            }
        }
    }

    /** Release control of the lock so that control can exit scope and the lock
     *  can be released somewhere else.
     */
    void release()
    { d_released = true; }

    /// Unlock the file.
    static void unlock(int fd) {
        struct flock lock{};
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        lock.l_pid = getpid();
        if (fcntl(fd, F_SETLK, &lock) == -1) {
            throw std::runtime_error("mp_lock_guard::unlock: Could not unlock the cache-control file: " + get_errno());
        }
    }
};

#endif //LIBDAP4_MP_LOCK_GUARD_H
