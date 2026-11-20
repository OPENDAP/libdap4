
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

#include <future>
#include <ostream>

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
public:
    MarshallerThread();
    virtual ~MarshallerThread();

    /// @brief Writes the val buffer to the stream synchronously.
    /// @param out The stream to write the buffer to.
    /// @param val The buffer to write out. The buffer is used immediately,
    ///     without copying.
    /// @param num_bytes How much of the buffer to write out.
    void write(std::ostream &out, const char *val, int64_t num_bytes);

    /// @brief Asynchronously writes the val buffer to the stream.
    /// @param out The stream to write the buffer to. The lifetime of out needs
    ///     to extend until the write completes.
    /// @param val The buffer to write out. The buffer is copied for the write.
    /// @param num_bytes How much of the buffer to write out.
    void write_async(std::ostream &out, const char *val, int64_t num_bytes);

    /// @brief Wait for pending async writes to complete.
    void wait();

private:
    std::future<void> d_future;

    /// @brief Simple write of the buffer. Depending on the write mode this
    ///     gets either called directly or launched as an async task.
    /// @param out The stream to write the buffer to.
    /// @param val The buffer to write out. The buffer is used immediately,
    ///     without copying.
    /// @param num_bytes How much of the buffer to write out.
    void write_out(std::ostream &out, const char *val, int64_t num_bytes);
};

} // namespace libdap

#endif /* MARSHALLERTHREAD_H_ */
