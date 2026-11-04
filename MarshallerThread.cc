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

#include <fcntl.h>
#include <memory>
#include <sys/time.h>
#include <unistd.h>

#include <cstring>
#include <future>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>

#include "MarshallerThread.h"
#include "debug.h"
#include "util.h"

using namespace libdap;
using namespace std;

// Set TIMING to 1 to enable timing output
#define TIMING 0

// The segmented write function enables writing 2^32 bytes or more,
// up to the limits of the machine. This switch only affects POSIX
// THREADS builds of libdap. jhrg 5/22/25
#define USE_SEGMENTED_WRITE 1

#if TIMING

namespace {
bool marshaller_thread_print_time = false;
}

/**
 * Use this with timeval structures returned by gettimeofday() to compute
 * real time (instead of user time that is returned by std::clock() or
 * get_rusage()).
 */
static double time_diff_to_hundredths(struct timeval *stop, struct timeval *start) {
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

MarshallerThread::MarshallerThread() {}

MarshallerThread::~MarshallerThread() { wait(); }

void MarshallerThread::write_out(std::ostream &out, const char *val, int64_t num_bytes) {
#if TIMING
    struct timeval tp_s;
    if (marshaller_thread_print_time && gettimeofday(&tp_s, 0) != 0)
        std::cerr << "could not read time\n";
#endif

#if USE_SEGMENTED_WRITE
    segmented_write(out, val, num_bytes);
#else
    out.write(val, num_bytes);
#endif
    if (out.fail()) {
        std::cerr << "Could not write data: " << __FILE__ << ":" << __LINE__;
    }

#if TIMING
    struct timeval tp_e;
    if (marshaller_thread_print_time) {
        if (gettimeofday(&tp_e, 0) != 0)
            std::cerr << "could not read time\n";

        std::cerr << "time for child thread write: " << time_diff_to_hundredths(&tp_e, &tp_s) << "\n";
    }
#endif
}

void MarshallerThread::write(std::ostream &out, const char *val, int64_t num_bytes) {
    wait();
    write_out(out, val, num_bytes);
}

void MarshallerThread::write_async(std::ostream &out, const char *val, int64_t num_bytes) {
    wait();

    std::unique_ptr<char[]> buf{new char[num_bytes]};
    std::memcpy(buf.get(), val, num_bytes);

    d_future = std::async(std::launch::async,
                          [this, &out, buf = std::move(buf), num_bytes]() { write_out(out, buf.get(), num_bytes); });
}

void MarshallerThread::wait() {
    if (d_future.valid()) {
        d_future.wait();
        d_future = std::future<void>();
    }
}
