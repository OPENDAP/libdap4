// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2025 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef MARSHALLERFUTURE_H_
#define MARSHALLERFUTURE_H_

#include <cstring>
#include <future>
#include <memory>
#include <stdexcept>
#include <unistd.h>

namespace libdap {

/**
 * @brief Run output calls using a future
 *
 * This version of MarshallerThread uses futures to implement what would have
 * been called 'double-buffered I/O' although this is only for writing. When
 * libdap's D4 and XDRStreamMarshaller classes are used to send data, this class
 * can be used to perform the data write() in a child thread that is managed
 * using the future objects introduced in C++11.
 *
 * Given the nature of DAP2 and DAP4 data responses, it is not possible to interleave
 * data for variables, so only one transmission is possible at any time. This
 * means that only one 'output future' can be running and the future::get() can
 * serve as a synchronization tool.
 *
 * This class is a replacement for the older MarshallerThread class that was
 * implemented using threads. It is implemented as a header-only class.
 */
class MarshallerThread {
    std::future<std::streampos> d_ostream_future;
    std::future<ssize_t> d_fp_future;

public:
    MarshallerThread() = default;

    ~MarshallerThread() {
        if (d_ostream_future.valid()) {
            try {
                d_ostream_future.get();
            } catch (...) {
                // Suppress exceptions in destructor
            }
        }
        if (d_fp_future.valid()) {
            try {
                d_fp_future.get();
            } catch (...) {
                // Suppress exceptions in destructor
            }
        }
    }

    /// @return The future associated with the ostream threads
    auto get_ostream_future() { return d_ostream_future.get(); }
    /// @return The future associated with the file descriptor threads
    auto get_fp_future() { return d_fp_future.get(); }

    /**
     * @brief start a thread to send byte_buf out using the stream out
     * @param out The ostream
     * @param byte_buf The bytes (a const char *)
     * @param num_bytes The number of byes
     */
    void start_thread(std::ostream &out, const char *byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        auto buffer_copy = std::make_unique<char[]>(num_bytes);
        std::memcpy(buffer_copy.get(), byte_buf, num_bytes);

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(buffer_copy), &out, num_bytes]() {
            out.write(buffer.get(), num_bytes);
            if (out.fail()) {
                throw std::runtime_error("Failed to write stream.");
            }
            return out.tellp();
        });
    }

    /**
     * @brief start a thread to send byte_buf out using the stream out
     * @param out The ostream
     * @param byte_buf The bytes (a std::shared_ptr<const char>)
     * @param num_bytes The number of byes
     */
    void start_thread(std::ostream &out, std::shared_ptr<const char> byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(byte_buf), &out, num_bytes]() {
            out.write(buffer.get(), num_bytes);
            if (out.fail()) {
                throw std::runtime_error("Failed to write stream.");
            }
            return out.tellp();
        });
    }

    void start_thread_part(std::ostream &out, const char *byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        auto buffer_copy = std::make_unique<char[]>(num_bytes);
        std::memcpy(buffer_copy.get(), byte_buf, num_bytes);

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(buffer_copy), &out, num_bytes]() {
            if (num_bytes <= 4) {
                throw std::runtime_error("Data too short for partial write.");
            }
            out.write(buffer.get() + 4, num_bytes - 4);
            if (out.fail()) {
                throw std::runtime_error("Failed to write partial stream.");
            }
            return out.tellp();
        });
    }

    void start_thread_part(std::ostream &out, std::shared_ptr<const char> byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(byte_buf), &out, num_bytes]() {
            if (num_bytes <= 4) {
                throw std::runtime_error("Data too short for partial write.");
            }
            out.write(buffer.get() + 4, num_bytes - 4);
            if (out.fail()) {
                throw std::runtime_error("Failed to write partial stream.");
            }
            return out.tellp();
        });
    }

    void start_thread(int fd, const char *byte_buf, std::streamsize num_bytes) {
        if (d_fp_future.valid()) {
            d_fp_future.get();
        }

        auto buffer_copy = std::make_unique<char[]>(num_bytes);
        std::memcpy(buffer_copy.get(), byte_buf, num_bytes);

        d_fp_future = std::async(std::launch::async, [buffer = std::move(buffer_copy), fd, num_bytes]() {
            auto written = write(fd, buffer.get(), num_bytes);
            if (written != num_bytes) {
                throw std::runtime_error("Failed to write all data to fd.");
            }
            return written;
        });
    }

    void start_thread(int fd, std::shared_ptr<const char> byte_buf, std::streamsize num_bytes) {
        if (d_fp_future.valid()) {
            d_fp_future.get();
        }

        d_fp_future = std::async(std::launch::async, [buffer = std::move(byte_buf), fd, num_bytes]() {
            auto written = write(fd, buffer.get(), num_bytes);
            if (written != num_bytes) {
                throw std::runtime_error("Failed to write all data to fd.");
            }
            return written;
        });
    }
};

} // namespace libdap

#endif
