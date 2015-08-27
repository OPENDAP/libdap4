// XDRStreamMarshaller.h

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
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

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

// Hacked massively to add all kinds of things, including pthred support.
// jhrg 8/21/15

#ifndef I_XDRStreamMarshaller_h
#define I_XDRStreamMarshaller_h 1

#include <iostream>

#include <pthread.h>

#include "Marshaller.h"
#include "InternalErr.h"
#include "XDRUtils.h"

namespace libdap {

class BaseType;

/**
 * @brief Marshaller that knows how serialize dap data objects to a C++ iostream using XDR
 *
 */
class XDRStreamMarshaller: public Marshaller {
private:
    static char * d_buf;
    XDR d_sink;
    ostream & d_out;

    int d_partial_put_byte_count;

    // pthreads code starts here

    pthread_t d_thread;
    pthread_attr_t d_thread_attr;

    pthread_mutex_t d_out_mutex;
    pthread_cond_t d_out_cond;
    int d_child_thread_count;
    std::string d_thread_error; // non-null indicates an error

    class Locker {
    public:
        Locker(pthread_mutex_t &lock, pthread_cond_t &cond, int &count) :
            m_mutex(lock), m_cond(cond), m_count(count)
        {
            int status = pthread_mutex_lock(&m_mutex);
            if (status != 0)
                throw InternalErr(__FILE__, __LINE__, "Could not lock m_mutex");
            while (m_count != 0) {
                status = pthread_cond_wait(&m_cond, &m_mutex);
                if (status != 0)
                    throw InternalErr(__FILE__, __LINE__, "Could not wait on m_cond");
            }
            if (m_count != 0)
                throw InternalErr(__FILE__, __LINE__, "FAIL: left m_cond wait with non-zero child thread count");
        }

        virtual ~Locker()
        {
            int status = pthread_mutex_unlock(&m_mutex);
            if (status != 0)
                throw InternalErr(__FILE__, __LINE__, "Could not unlock m_mutex");
        }

    private:
        pthread_mutex_t& m_mutex;
        pthread_cond_t &m_cond;
        int &m_count;

        Locker();
        Locker(const Locker &rhs);
    };

    struct write_args {
        pthread_mutex_t &d_mutex;
        pthread_cond_t &d_cond;
        int &d_count;
        std::string &d_error;
        ostream &d_out;     // The output stream protected by the mutex, ...
        char *d_buf;        // The data to write to the stream
        int d_num;          // The size of d_buf

        write_args(pthread_mutex_t &m, pthread_cond_t &c, int &count, std::string &e, ostream &s, char *vals, int num) :
            d_mutex(m), d_cond(c), d_count(count), d_error(e), d_out(s), d_buf(vals), d_num(num)
        {
        }
    };

    // These are used for the child I/O threads started by put_vector_thread(), etc.
    static void *write_thread(void *arg);
    static void *write_part_thread(void *arg);

    // pthread code ends here

    XDRStreamMarshaller();
    XDRStreamMarshaller(const XDRStreamMarshaller &m);
    XDRStreamMarshaller &operator=(const XDRStreamMarshaller &);

    void put_vector(char *val, unsigned int num, int width, Type type);

    friend class MarshallerTest;

public:
    XDRStreamMarshaller(ostream &out); //, bool checksum = false, bool write_data = true) ;
    virtual ~XDRStreamMarshaller();

    virtual void put_byte(dods_byte val);

    virtual void put_int16(dods_int16 val);
    virtual void put_int32(dods_int32 val);

    virtual void put_float32(dods_float32 val);
    virtual void put_float64(dods_float64 val);

    virtual void put_uint16(dods_uint16 val);
    virtual void put_uint32(dods_uint32 val);

    virtual void put_str(const string &val);
    virtual void put_url(const string &val);

    virtual void put_opaque(char *val, unsigned int len);
    virtual void put_int(int val);

    virtual void put_vector(char *val, int num, Vector &vec);
    virtual void put_vector(char *val, int num, int width, Vector &vec);

    virtual void put_vector_start(int num);
    virtual void put_vector_part(char *val, unsigned int num, int width, Type type);
    virtual void put_vector_end();

#if 0
    virtual void put_vector_thread(char *val, int num, Vector *vec);
    virtual void put_vector_thread(char *val, unsigned int num, int width, Type type, Vector *vec);
    virtual void put_vector_part_thread(char *val, unsigned int num, int width, Type type, Vector *vec);
#endif

    virtual void dump(ostream &strm) const;
};

} // namespace libdap

#endif // I_XDRStreamMarshaller_h

