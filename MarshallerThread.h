
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
 * Implement a multithreaded data transmission subsystem for libdap.
 * This class makes it fairly painless to send data using a child thread.
 * The main thread can be used to read the next chunk of data
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

    int d_child_thread_count = 0; // 0 or 1
    std::string d_thread_error;   // non-null indicates an error

    void start_thread(void *(*thread)(void *arg), std::ostream &out, char *byte_buf, std::streamsize bytes_written);
    void start_thread(void *(*thread)(void *arg), int fd, char *byte_buf, std::streamsize bytes_written);

    // These are static, so they will have c-linkage - required because they
    // are passed to pthread_create()
    static void *write_thread(void *arg);
    static void *write_thread_part(void *arg);

public:
    MarshallerThread();
    virtual ~MarshallerThread();

    /// @brief Writes the val buffer to the stream synchronously.
    /// @param out The stream to write the buffer to.
    /// @param val The buffer to write out. The buffer is used immediately, without copying.
    /// @param num_bytes How much of the buffer to write out.
    void write(std::ostream &out, const char *val, int64_t num_bytes);

    /// @brief Asynchronously writes the val buffer to the stream.
    /// @param out The stream to write the buffer to.
    /// @param val The buffer to write out. The buffer is copied for the write.
    /// @param num_bytes How much of the buffer to write out.
    void write_async(std::ostream &out, const char *val, int64_t num_bytes);

    /// @brief Wait for pending async writes to complete.
    void wait();
};

} // namespace libdap

#endif /* MARSHALLERTHREAD_H_ */
