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
 * MarshallerThread.cc
 *
 *  Created on: Aug 27, 2015
 *      Author: jimg
 */

#include "config.h"

#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include <ostream>
#include <sstream>

#include "MarshallerThread.h"
#include "Error.h"
#include "InternalErr.h"
#include "debug.h"

using namespace libdap;
using namespace std;

#if 0
bool MarshallerThread::print_time = false;

/**
 * Use this with timeval structures returned by gettimeofday() to compute
 * real time (instead of user time that is returned by std::clock() or
 * get_rusage()).
 */
static double time_diff_to_hundredths(struct timeval *stop, struct timeval *start)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (stop->tv_usec < start->tv_usec) {
        int nsec = (start->tv_usec - stop->tv_usec) / 1000000 + 1;
        start->tv_usec -= 1000000 * nsec;
        start->tv_sec += nsec;
    }
    if (stop->tv_usec - start->tv_usec > 1000000) {
        int nsec = (start->tv_usec - stop->tv_usec) / 1000000;
        start->tv_usec += 1000000 * nsec;
        start->tv_sec -= nsec;
    }

    double result = stop->tv_sec - start->tv_sec;
    result += double(stop->tv_usec - start->tv_usec) / 1000000;
    return result;
}
#endif


/**
 * Lock the mutex then wait for the child thread to signal using the
 * condition variable 'cond'. Once the signal is received, re-test count
 * to make sure it's zero (there are no child threads).
 *
 * This is used to lock the main thread and ensure that a second child
 * (writer) thread is not started until any current child thread completes,
 * which keeps the write operations in the correct order.
 */
Locker::Locker(pthread_mutex_t &lock, pthread_cond_t &cond, int &count) :
    m_mutex(lock)
{
    int status = pthread_mutex_lock(&m_mutex);

    DBG(cerr << "Locking the mutex! (waiting; " << pthread_self() << ")" << endl);

    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not lock m_mutex");
    while (count != 0) {
        status = pthread_cond_wait(&cond, &m_mutex);
        if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not wait on m_cond");
    }
    if (count != 0) throw InternalErr(__FILE__, __LINE__, "FAIL: left m_cond wait with non-zero child thread count");

    DBG(cerr << "Locked! (" << pthread_self() << ")" << endl);
}

/**
 * Unlock the mutex
 */
Locker::~Locker()
{
    DBG(cerr << "Unlocking the mutex! (" << pthread_self() << ")" << endl);

    (void) pthread_mutex_unlock(&m_mutex);
#if 0
    int status = pthread_mutex_unlock(&m_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not unlock m_mutex");
#endif
}


/**
 * Lock the mutex, but do not wait on the condition variable.
 * This is used by the child thread; it helps ensure that the
 * mutex is unlocked and the predicate is reset no matter how the
 * child thread is exited.
 *
 * Note we how a reference to the shared 'count' predicate that
 * tells how many (0 or 1) child threads exist so that when this
 * version of the Locker object is destroyed, we can zero that.
 * This enables us to use RAII in the child thread and ensure
 * the invariant if there is an error and the code exits with a
 * summary return.
 */
ChildLocker::ChildLocker(pthread_mutex_t &lock, pthread_cond_t &cond, int &count) :
    m_mutex(lock), m_cond(cond), m_count(count)
{
    int status = pthread_mutex_lock(&m_mutex);

    DBG(cerr << "Locking the mutex! (simple; " << pthread_self() << ")" << endl);

    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not lock m_mutex");

    DBG(cerr << "Locked! (" << pthread_self() << ")" << endl);
}

ChildLocker::~ChildLocker()
{
    DBG(cerr << "Unlocking the mutex! (" << pthread_self() << ")" << endl);

    m_count = 0;

    (void) pthread_cond_signal(&m_cond);
    (void) pthread_mutex_unlock(&m_mutex);

#if 0
    int status = pthread_cond_signal(&m_cond);
    if (status != 0)
        throw InternalErr(__FILE__, __LINE__, "Could not signal main thread from ChildLocker!");

    status = pthread_mutex_unlock(&m_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not unlock m_mutex");
#endif
}

MarshallerThread::MarshallerThread() :
    d_thread(0), d_child_thread_count(0)
{
    if (pthread_attr_init(&d_thread_attr) != 0) throw Error(internal_error, "Failed to initialize pthread attributes.");
    if (pthread_attr_setdetachstate(&d_thread_attr, PTHREAD_CREATE_DETACHED /*PTHREAD_CREATE_JOINABLE*/) != 0)
        throw Error(internal_error, "Failed to complete pthread attribute initialization.");

    if (pthread_mutex_init(&d_out_mutex, 0) != 0) throw Error(internal_error, "Failed to initialize mutex.");
    if (pthread_cond_init(&d_out_cond, 0) != 0) throw Error(internal_error, "Failed to initialize cond.");
}

MarshallerThread::~MarshallerThread()
{
    (void) pthread_mutex_lock(&d_out_mutex);
#if 0
    int status = pthread_mutex_lock(&d_out_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not lock m_mutex");
#endif
    // d_child_thread_count is passed into the thread in a structure (see write_thread())
    // and is decremented by the ChildLocker dtor when write_thread() exits. jhrg 2/7/19
    if (d_child_thread_count != 0) {
        (void) pthread_cond_wait(&d_out_cond, &d_out_mutex);
        d_child_thread_count = 0;
#if 0
        status = pthread_cond_wait(&d_out_cond, &d_out_mutex);
        if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not wait on m_cond");
#endif
    }

    (void) pthread_mutex_unlock(&d_out_mutex);

#if 0
    if (d_child_thread_count != 0)
        throw InternalErr(__FILE__, __LINE__, "FAIL: left m_cond wait with non-zero child thread count");

    status = pthread_mutex_unlock(&d_out_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not unlock m_mutex");
#endif

    pthread_mutex_destroy(&d_out_mutex);
    pthread_cond_destroy(&d_out_cond);

    pthread_attr_destroy(&d_thread_attr);
}

// not a static method
/**
 * Start the child thread, using the arguments given. This will write 'bytes'
 * bytes from 'byte_buf' to the output stream 'out'
 *
 */
void MarshallerThread::start_thread(void* (*thread)(void *arg), ostream &out, char *byte_buf,
    unsigned int bytes)
{
    write_args *args = new write_args(d_out_mutex, d_out_cond, d_child_thread_count, d_thread_error, out, byte_buf,
        bytes);
    int status = pthread_create(&d_thread, &d_thread_attr, thread, args);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not start child thread");
}

/**
 * Write 'bytes' bytes from 'byte_buf' to the file descriptor 'fd'.
 */
void MarshallerThread::start_thread(void* (*thread)(void *arg), int fd, char *byte_buf, unsigned int bytes)
{
    write_args *args = new write_args(d_out_mutex, d_out_cond, d_child_thread_count, d_thread_error, fd, byte_buf,
        bytes);
    int status = pthread_create(&d_thread, &d_thread_attr, thread, args);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not start child thread");
}

/**
 * This static method is used to write data to the ostream referenced
 * by the ostream element of write_args. This is used by start_thread()
 * and passed to pthread_create()
 *
 * @note The write_args argument may contain either a file descriptor
 * (d_out_file) or an ostream& (d_out). If the file descriptor is not
 * -1, then use that, else use the ostream reference.
 */
void *
MarshallerThread::write_thread(void *arg)
{
    write_args *args = reinterpret_cast<write_args *>(arg);

    ChildLocker lock(args->d_mutex, args->d_cond, args->d_count); // RAII; will unlock on exit

#if 0
    struct timeval tp_s;
    if (print_time && gettimeofday(&tp_s, 0) != 0) cerr << "could not read time" << endl;
#endif

    // force an error
    // return (void*)-1;

    if (args->d_out_file != -1) {
        int bytes_written = write(args->d_out_file, args->d_buf, args->d_num);
        if (bytes_written != args->d_num)
            return (void*) -1;
    }
    else {
        args->d_out.write(args->d_buf, args->d_num);
        if (args->d_out.fail()) {
            ostringstream oss;
            oss << "Could not write data: " << __FILE__ << ":" << __LINE__;
            args->d_error = oss.str();
            return (void*) -1;
        }
    }

    delete [] args->d_buf;
    delete args;

#if 0
    struct timeval tp_e;
    if (print_time) {
        if (gettimeofday(&tp_e, 0) != 0) cerr << "could not read time" << endl;

        cerr << "time for child thread write: " << time_diff_to_hundredths(&tp_e, &tp_s) << endl;
    }
#endif

    return 0;
}

/**
 * This static method is used to write data to the ostream referenced
 * by the ostream element of write_args. This is used by start_thread()
 * and passed to pthread_create()
 *
 * @note This differers from MarshallerThread::write_thread() in that it
 * writes data starting _after_ the four-byte length prefix that XDR
 * adds to the data. It is used for the put_vector_part() calls in
 * XDRStreamMarshaller.
 *
 * @return 0 if successful, -1 otherwise.
 */
void *
MarshallerThread::write_thread_part(void *arg)
{
    write_args *args = reinterpret_cast<write_args *>(arg);

    ChildLocker lock(args->d_mutex, args->d_cond, args->d_count); // RAII; will unlock on exit

    if (args->d_out_file != -1) {
        int bytes_written = write(args->d_out_file, args->d_buf, args->d_num);
        if (bytes_written != args->d_num) return (void*) -1;
    }
    else {
        args->d_out.write(args->d_buf + 4, args->d_num);
        if (args->d_out.fail()) {
            ostringstream oss;
            oss << "Could not write data: " << __FILE__ << ":" << __LINE__;
            args->d_error = oss.str();
            return (void*) -1;
        }
    }

    delete [] args->d_buf;
    delete args;

    return 0;
}

