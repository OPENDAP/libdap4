// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2013 OPeNDAP, Inc.
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

#include <sys/wait.h>

#include <string>

#include "mp_lock_guard.h"
#include "debug.h"

#include "run_tests_cppunit.h"

using namespace CppUnit;
using namespace std;

#define prolog std::string("mp_lock_guard::").append(__func__).append("() - ")

namespace libdap {

class mp_lock_guard_test : public TestFixture {
private:
    const string lock_file = "cache-testsuite/mp_lock_guard_test.lock";
    int lock_fd = -1;

public:
    mp_lock_guard_test() = default;

    ~mp_lock_guard_test() override = default;

    static void try_lock_will_fail(int fd, short type) {
        // get info about the lock - since this file is read locked, we should not be
        // able to write lock it and the 'l_type' should change to F_RDLCK (1)
        struct flock lock{};
        lock.l_type = type;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        lock.l_pid = getpid();
        if (fcntl(fd, F_SETLK, &lock) == -1) {
            DBG(cerr << "fnctl() returned EWOLDBLOCK: " << boolalpha << (errno == EWOULDBLOCK) << endl);
            CPPUNIT_ASSERT_MESSAGE("fnctl() should return EWOLDBLOCK", errno == EWOULDBLOCK);
        }
        else {
            CPPUNIT_FAIL("fnctl() should have returned EWOULDBLOCK");
        }
    }

    void setUp() override {
        remove(lock_file.c_str());
        if ((lock_fd = open(lock_file.c_str(), O_CREAT | O_EXCL | O_RDWR, 0660)) < 0) {
            if (errno == EEXIST) {
                throw std::runtime_error("Could not create lock file - it already exists: " + lock_file);
            }
            else {
                throw std::runtime_error("Could not create lock file: " + lock_file);
            }
        }
    }

    CPPUNIT_TEST_SUITE (mp_lock_guard_test);

        CPPUNIT_TEST(single_read_lock_test);
        CPPUNIT_TEST(single_read_lock_w_child_test);

        CPPUNIT_TEST(single_write_lock_test);
        CPPUNIT_TEST(single_write_lock_w_child_test);

        CPPUNIT_TEST(single_write_lock_w_child_get_read_test);
        CPPUNIT_TEST(single_read_lock_w_child_get_write_test);

        CPPUNIT_TEST(multiple_read_locks_scope_test);

    CPPUNIT_TEST_SUITE_END();

    // Test that a single read lock can be obtained.
    void single_read_lock_test() {
        mp_lock_guard lg(lock_fd, mp_lock_guard::operation::read);
        CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg.d_locked);
    }

    // Test that a single read lock can be obtained and that a child process can obtain a read lock too.
    void single_read_lock_w_child_test() {
        mp_lock_guard lg(lock_fd, mp_lock_guard::operation::read);
        CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg.d_locked);
        int pid = fork();
        if (pid == 0) {
            // child
            DBG(cerr << "child pid: " << getpid() << endl);
            int child_fd = open(lock_file.c_str(), O_RDWR, 0660);
            mp_lock_guard lg2(child_fd, mp_lock_guard::operation::read);
            DBG(cerr << "lg2.d_locked: " << boolalpha << lg2.d_locked << endl);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg2.d_locked);

            // get info about the lock - since this file is read locked, we should not be
            // able to write lock it and the 'l_type' should change to F_RDLCK (1)
            struct flock lock{};
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = 0;
            if (fcntl(lock_fd, F_GETLK, &lock) == -1) {
                CPPUNIT_FAIL("Could not get lock info");
            }
            DBG(cerr << "lock.l_type: " << lock.l_type << endl);
            CPPUNIT_ASSERT_MESSAGE("The lock should be a read lock", lock.l_type == F_RDLCK);
            DBG(cerr << "lock.l_pid: " << lock.l_pid << endl);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held by this process", lock.l_pid == getppid());

            exit(0);
        }
        else {
            // parent
            DBG(cerr << "parent pid: " << getpid() << endl);
            int status;
            waitpid(pid, &status, 0);
            CPPUNIT_ASSERT_MESSAGE("The child should have exited with status 0", WEXITSTATUS(status) == 0);
        }
    }

    // Test that a single write lock can be obtained.
    void single_write_lock_test() {
        mp_lock_guard lg(lock_fd, mp_lock_guard::operation::write);
        CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg.d_locked);
    }

    // Test that a single write lock can be obtained and that a child process cannot obtain a read or write lock.
    void single_write_lock_w_child_test() {
        mp_lock_guard lg(lock_fd, mp_lock_guard::operation::write);
        CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg.d_locked);
        int pid = fork();
        if (pid == 0) {
            // child
            DBG(cerr << "child pid: " << getpid() << endl);
            int child_fd = open(lock_file.c_str(), O_RDWR, 0660);

            // get info about the lock - since this file is read locked, we should not be
            // able to write lock it and the 'l_type' should change to F_RDLCK (1)
            try_lock_will_fail(child_fd, F_WRLCK);
            try_lock_will_fail(child_fd, F_RDLCK);

            exit(0);
        }
        else {
            // parent
            DBG(cerr << "parent pid: " << getpid() << endl);
            int status;
            waitpid(pid, &status, 0);
            CPPUNIT_ASSERT_MESSAGE("The child should have exited with status 0", WEXITSTATUS(status) == 0);
        }
    }

    // Test that a single write lock can be obtained and that a child process can obtain a read lock
    // only after the parent releases the write lock.
    void single_write_lock_w_child_get_read_test() {
        mp_lock_guard lg(lock_fd, mp_lock_guard::operation::write);
        CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg.d_locked);
        int pid = fork();
        if (pid == 0) {
            // child
            DBG(cerr << "child pid: " << getpid() << endl);
            int child_fd = open(lock_file.c_str(), O_RDWR, 0660);

            // This should indicate the lock is held by the parent
            try_lock_will_fail(child_fd, F_RDLCK);

            // make a sentinel file to indicate the child has tried and failed to get the lock
            // and the parent should now release the lock
            int s_fd = open("single_write_lock_w_child_get_read_test", O_CREAT | O_EXCL | O_RDWR, 0660);
            if (s_fd == -1) {
                CPPUNIT_FAIL("Could not create sentinel file");
            }

            // now block, waiting on the parent to release the lock
            mp_lock_guard lg2(child_fd, mp_lock_guard::operation::read);
            DBG(cerr << "child got the lock" << endl);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg2.d_locked);

            exit(0);
        }
        else {
            // parent
            DBG(cerr << "parent pid: " << getpid() << endl);

            // Wait for the sentinel file to be created
            while (access("single_write_lock_w_child_get_read_test", F_OK) == -1) {
                sleep(1);
            }

            DBG(cerr << "parent releasing lock" << endl);
            lg.release();
            mp_lock_guard::unlock(lock_fd);
            DBG(cerr << "parent released lock" << endl);

            int status;
            waitpid(pid, &status, 0);
            CPPUNIT_ASSERT_MESSAGE("The child should have exited with status 0", WEXITSTATUS(status) == 0);

            remove("single_write_lock_w_child_get_read_test");
        }
    }

    // Test that a single read lock can be obtained and that a child process can obtain a write lock
    // only after the parent releases the read lock.
    void single_read_lock_w_child_get_write_test() {
        mp_lock_guard lg(lock_fd, mp_lock_guard::operation::read);
        CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg.d_locked);
        int pid = fork();
        if (pid == 0) {
            // child
            DBG(cerr << "child pid: " << getpid() << endl);
            int child_fd = open(lock_file.c_str(), O_RDWR, 0660);

            // This should indicate the lock is held by the parent
            try_lock_will_fail(child_fd, F_WRLCK);

            // make a sentinel file to indicate the child has tried and failed to get the lock
            int s_fd = open("single_read_lock_w_child_get_write_test", O_CREAT | O_EXCL | O_RDWR, 0660);
            if (s_fd == -1) {
                CPPUNIT_FAIL("Could not create sentinel file");
            }

            // now block, waiting on the parent to release the lock
            mp_lock_guard lg2(child_fd, mp_lock_guard::operation::write);
            DBG(cerr << "child got the lock" << endl);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg2.d_locked);

            exit(0);
        }
        else {
            // parent
            DBG(cerr << "parent pid: " << getpid() << endl);

            // Wait for the sentinel file to be created
            while (access("single_read_lock_w_child_get_write_test", F_OK) == -1) {
                sleep(1);
            }

            DBG(cerr << "parent releasing lock" << endl);
            lg.release();
            mp_lock_guard::unlock(lock_fd);
            DBG(cerr << "parent released lock" << endl);

            int status;
            waitpid(pid, &status, 0);
            CPPUNIT_ASSERT_MESSAGE("The child should have exited with status 0", WEXITSTATUS(status) == 0);

            remove("single_read_lock_w_child_get_write_test");
        }
    }

    // Test that multiple read locks can be obtained and that a child process can obtain a write lock
    // once the lock guard objects go out of scope, releasing the read locks
    void multiple_read_locks_scope_test() {
        {
            mp_lock_guard lg(lock_fd, mp_lock_guard::operation::read);
            mp_lock_guard lg2(lock_fd, mp_lock_guard::operation::read);
            mp_lock_guard lg3(lock_fd, mp_lock_guard::operation::read);
            mp_lock_guard lg4(lock_fd, mp_lock_guard::operation::read);

            // The locks should be held here, and then released when this
            // scope is exited.
            CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg.d_locked);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg2.d_locked);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg3.d_locked);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held", lg4.d_locked);
        }

        int pid = fork();
        if (pid == 0) {
            // child
            DBG(cerr << "child pid: " << getpid() << endl);
            int child_fd = open(lock_file.c_str(), O_RDWR, 0660);
            // get info about the lock - since this file is not locked, we should be
            // able to write lock it and the 'l_type' should change to F_UNLCK (2)
            // indicating this process can get the lock
            struct flock lock{};
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = 0;
            if (fcntl(child_fd, F_GETLK, &lock) == -1) {
                CPPUNIT_FAIL("Could not get lock info");
            }
            DBG(cerr << "lock.l_type: " << lock.l_type << endl);
            CPPUNIT_ASSERT_MESSAGE("The lock should be a read lock", lock.l_type == F_UNLCK);
            DBG(cerr << "lock.l_pid: " << lock.l_pid << endl);
            CPPUNIT_ASSERT_MESSAGE("The lock should be held by this process", lock.l_pid == 0);

            exit(0);
        }
        else {
            // parent
            DBG(cerr << "parent pid: " << getpid() << endl);

            int status;
            waitpid(pid, &status, 0);
            CPPUNIT_ASSERT_MESSAGE("The child should have exited with status 0", WEXITSTATUS(status) == 0);
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (mp_lock_guard_test);

}

int main(int argc, char *argv[]) {
    return run_tests<libdap::mp_lock_guard_test>(argc, argv) ? 0 : 1;
}
