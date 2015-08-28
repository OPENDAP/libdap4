
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

#include <ostream>
#include <sstream>

#include "MarshallerThread.h"
#include "Error.h"
#include "InternalErr.h"

using namespace libdap;
using namespace std;

/**
 * Lock the mutex then wait for the child thread to signal using the
 * condition variable 'cond'. Once the signal is received, retest count
 * to make sure it's zero (there are no child threads).
 */
Locker::Locker(pthread_mutex_t &lock, pthread_cond_t &cond, int &count) :
    m_mutex(lock)
{
    int status = pthread_mutex_lock(&m_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not lock m_mutex");
    while (count != 0) {
        status = pthread_cond_wait(&cond, &m_mutex);
        if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not wait on m_cond");
    }
    if (count != 0) throw InternalErr(__FILE__, __LINE__, "FAIL: left m_cond wait with non-zero child thread count");
}

/**
 * Unlock the mutex
 */
Locker::~Locker()
{
    int status = pthread_mutex_unlock(&m_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not unlock m_mutex");
}

MarshallerThread::MarshallerThread(): d_thread(0), d_child_thread_count(0)
{
    if (pthread_attr_init(&d_thread_attr) != 0)  throw Error("Failed to initialize pthread attributes.");
    if (pthread_attr_setdetachstate(&d_thread_attr, PTHREAD_CREATE_DETACHED /*PTHREAD_CREATE_JOINABLE*/) != 0)
        throw Error("Failed to complete pthread attribute initialization.");

    if (pthread_mutex_init(&d_out_mutex, 0) != 0) throw Error("Failed to initialize mutex.");
    if (pthread_cond_init(&d_out_cond, 0) != 0) throw Error("Failed to initialize cond.");
}

MarshallerThread::~MarshallerThread()
{
    int status = pthread_mutex_lock(&d_out_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not lock m_mutex");
    while (d_child_thread_count != 0) {
        status = pthread_cond_wait(&d_out_cond, &d_out_mutex);
        if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not wait on m_cond");
    }
    if (d_child_thread_count != 0) throw InternalErr(__FILE__, __LINE__, "FAIL: left m_cond wait with non-zero child thread count");

    status = pthread_mutex_unlock(&d_out_mutex);
    if (status != 0) throw InternalErr(__FILE__, __LINE__, "Could not unlock m_mutex");

    pthread_mutex_destroy(&d_out_mutex);
    pthread_cond_destroy(&d_out_cond);

    pthread_attr_destroy(&d_thread_attr);
}

// not a static method
void MarshallerThread::start_thread(void* (*thread)(void *arg), ostream &out, char *byte_buf, unsigned int bytes_written)
{
    write_args *args = new write_args(d_out_mutex, d_out_cond, d_child_thread_count, d_thread_error, out, byte_buf, bytes_written);
    int status = pthread_create(&d_thread, &d_thread_attr, thread, args);
    if (status != 0)
        throw InternalErr(__FILE__, __LINE__, "Could not start child thread");
}

// below this point, all of these are static methods. These three are all
// called from within the 'thread functions', so they have to be static
// as well.
bool MarshallerThread::lock_thread(write_args *args)
{
    int status = pthread_mutex_lock(&args->d_mutex);
    if (status != 0) {
        ostringstream oss;
        oss << "Could not lock d_out_mutex: " << __FILE__ << ":" << __LINE__;
        args->d_error = oss.str();
        return false;
    }

    return true;
}

bool MarshallerThread::unlock_thread(write_args *args)
{
    int status = pthread_mutex_unlock(&args->d_mutex);
    if (status != 0) {
        ostringstream oss;
        oss << "Could not unlock d_out_mutex: " << __FILE__ << ":" << __LINE__;
        args->d_error = oss.str();
        return false;
    }

    return true;
}

bool MarshallerThread::signal_thread(write_args *args)
{
    int status = pthread_cond_signal(&args->d_cond);
    if (status != 0) {
        ostringstream oss;
        oss << "Could not signal d_out_cond: " << __FILE__ << ":" << __LINE__;
        args->d_error = oss.str();
        return false;
    }

    return true;
}

/**
 * This static method is used to write data to the ostream referenced
 * by the ostream element of write_args. This is used by start_thread()
 * and passed to pthread_create()
 */
void *
MarshallerThread::write_thread(void *arg)
{
    write_args *args = reinterpret_cast<write_args *>(arg);

    if (!lock_thread(args)) return (void*)-1;

    args->d_out.write(args->d_buf, args->d_num);
    if (args->d_out.fail()) {
        ostringstream oss;
        oss << "Could not write data: " << __FILE__ << ":" << __LINE__;
        args->d_error = oss.str();
        return (void*)-1;
    }

    delete args->d_buf;

    args->d_count = 0;

    if (!signal_thread(args)) return (void*)-1;

    if (!unlock_thread(args)) return (void*)-1;

    delete args;

    return 0;
}

/**
 * This static method is used to write data to the ostream referenced
 * by the ostream element of write_args. This is used by start_thread()
 * and passed to pthread_create()
 *
 * @note This differes from MarshallerThread::write_thread() in that it
 * writes data starting _after_ the four-byte length prefix that XDR
 * adds to the data. It is used for the put_vector_part() calls in
 * XDRStreamMarshaller.
 */
void *
MarshallerThread::write_thread_part(void *arg)
{
    write_args *args = reinterpret_cast<write_args *>(arg);

    if (!lock_thread(args)) return (void*)-1;

    args->d_out.write(args->d_buf + 4, args->d_num);
    if (args->d_out.fail()) {
        ostringstream oss;
        oss << "Could not write data: " << __FILE__ << ":" << __LINE__;
        args->d_error = oss.str();
        return (void*)-1;
    }

    delete args->d_buf;

    args->d_count = 0;

    if (!signal_thread(args)) return (void*)-1;

    if (!unlock_thread(args)) return (void*)-1;

    delete args;

    return 0;
}

