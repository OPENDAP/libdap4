
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2015 OPeNDAP, Inc.
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

/*
 * MarshallerThread.h
 *
 *  Created on: Aug 27, 2015
 *      Author: jimg
 */

#ifndef MARSHALLERTHREAD_H_
#define MARSHALLERTHREAD_H_

#include <pthread.h>

#include <iostream>
#include <ostream>
#include <string>

namespace libdap {

/**
 * RAII for the MarshallerThread mutex and condition variable. Used by the
 * Main thread. The constructor locks the mutex and then, if the count of
 * child threads is not zero, blocks on the associated condition variable.
 * When signaled by the child thread using the condition variable (the child
 * thread count should then be zero), the mutex is (re)locked and the ctor
 * returns. The destructor unlocks the mutex.
 */
class Locker {
public:
    Locker(pthread_mutex_t &lock, pthread_cond_t &cond, int &count);
    virtual ~Locker();

private:
    pthread_mutex_t& m_mutex;

    Locker();
    Locker(const Locker &rhs);
};

/**
 * Synchronization for the child thread in the multi-threaded version of
 * the DAP2 and DAP4 (when it gets implemented) 'Marshaller' class used to
 * send data. The class declared below (MarshallerThread) manages the
 * child thread.
 *
 * The ctor of this class simply
 * locks the mutex; the dtor clears the child thread count, signals that
 * count has changed and unlocks the mutex.
 */
class ChildLocker {
public:
    ChildLocker(pthread_mutex_t &lock, pthread_cond_t &cond, int &count);
    virtual ~ChildLocker();

private:
    pthread_mutex_t& m_mutex;
    pthread_cond_t& m_cond;
    int& m_count;

    ChildLocker();
    ChildLocker(const Locker &rhs);
};

/**
 * Implement a multi-threaded data transmission sub-system for libdap.
 * This class makes it fairly painless to send data using a child thread
 * so that the main thread can be used to read the next chunk of data
 * while whatever has been read to this point is sent over the wire.
 *
 * This code is used by XDRStreamMarshaller and (soon) D4StreamMarshaller.
 */
class MarshallerThread {
private:
    pthread_t d_thread;
    pthread_attr_t d_thread_attr;

    pthread_mutex_t d_out_mutex;
    pthread_cond_t d_out_cond;

    int d_child_thread_count;   // 0 or 1
    std::string d_thread_error; // non-null indicates an error

    /**
     * Used to pass information into the static methods that run the
     * simple stream writer threads. This can pass both an ostream or
     * a file descriptor. If a fd is passed, the ostream reference is
     * set to stderr (i.e., std::cerr).
     */
    struct write_args {
        pthread_mutex_t &d_mutex;
        pthread_cond_t &d_cond;
        int &d_count;
        std::string &d_error;
        std::ostream &d_out;     // The output stream protected by the mutex, ...
        int d_out_file;       // file descriptor; if not -1, use this.
        char *d_buf;        // The data to write to the stream
        int d_num;          // The size of d_buf

        /**
         * Build args for an ostream. The file descriptor is set to -1
         */
        write_args(pthread_mutex_t &m, pthread_cond_t &c, int &count, std::string &e, std::ostream &s, char *vals, int num) :
            d_mutex(m), d_cond(c), d_count(count), d_error(e), d_out(s), d_out_file(-1), d_buf(vals), d_num(num)
        {
        }

        /**
         * Build args for a file descriptr. The ostream is set to cerr (because it is
         * a reference and has to be initialized to something).
         */
        write_args(pthread_mutex_t &m, pthread_cond_t &c, int &count, std::string &e, int fd, char *vals, int num) :
            d_mutex(m), d_cond(c), d_count(count), d_error(e), d_out(std::cerr), d_out_file(fd), d_buf(vals), d_num(num)
        {
        }
   };

public:
    MarshallerThread();
    virtual ~MarshallerThread();

    pthread_mutex_t &get_mutex() { return d_out_mutex; }
    pthread_cond_t &get_cond() { return d_out_cond; }

    int &get_child_thread_count() { return d_child_thread_count; }
    void increment_child_thread_count() { ++d_child_thread_count; }

    void start_thread(void* (*thread)(void *arg), std::ostream &out, char *byte_buf, unsigned int bytes_written);
    void start_thread(void* (*thread)(void *arg), int fd, char *byte_buf, unsigned int bytes_written);

    // These are static so they will have c-linkage - required because they
    // are passed to pthread_create()
    static void *write_thread(void *arg);
    static void *write_thread_part(void *arg);
};

}

#endif /* MARSHALLERTHREAD_H_ */
