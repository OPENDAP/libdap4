// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2009 OPeNDAP, Inc.
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
//
// Portions of this code were taken verbatim from  Josuttis,
// "The C++ Standard Library," p.672

#ifndef _fdiostream_h
#define _fdiostream_h

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream>
#include <streambuf>
#include <algorithm>
#include <cstdio>

namespace libdap {

/** fdoutbuf is a stream buffer specialization designed specifically for files
    that are referenced using a file descriptor. This class implements a
    4k buffer for data and optionally closes the file when the buffer is
    deleted.

    @note Instead of using this class, use fdostream.
    @see fdostream
 */
class fdoutbuf : public std::streambuf {
protected:
    int fd;	// file descriptor
    bool close;
    static const int bufferSize = 4096; // Size of the data buffer
    char buffer[bufferSize];	// data buffer

public:
    fdoutbuf(int _fd, bool _close);
    virtual ~fdoutbuf();

protected:
    int flushBuffer();

    virtual int overflow(int c);
    virtual int sync();
    virtual std::streamsize xsputn(const char *s, std::streamsize num);
};

/** This specialization of ostream builds a fdoutbuf instance and binds it to
    an ostream. Thus it is possible to build an ostream from an open file, using
    its file descriptor, and write to it using C++ stream I/O methods.

    @todo Generalize this so that it can take a FILE * as well and then make
    two classes file_istream and file_ostream that can be initialized with
    either a file descriptor of a FILE pointer.
 */
class fdostream : public std::ostream {
protected:
    fdoutbuf buf;
public:
    /** Make a new fdostream and initialize it to use a specific file descriptor.
        If the file descriptor does not reference an open file, the first
        attempt to write data will fail.
        @param _fd The file descriptor of an open file, socket, et cetera.
        @param _close If true, close the file when the output stream buffer is
        deleted. Default: false. */
    fdostream (int _fd, bool _close = false)
	: std::ostream(&buf), buf(_fd, _close) { }
};

/** fdintbuf is a stream buffer specialization designed specifically for files
    that are referenced using a file descriptor. This class implements a
    4k buffer for data and optionally closes the file when the buffer is
    deleted. The stream buffer has 128 characters of 'put back' space.

    @note Instead of using this class, use fdistream.
    @see fdistream
 */
class fdinbuf : public std::streambuf {
protected:
    int fd;	// file descriptor
    bool close;
    static const int bufferSize = 4096; // Size of the data buffer
    static const int putBack = 128;
    char buffer[bufferSize];	// data buffer

public:
    fdinbuf(int _fd, bool close);
    virtual ~fdinbuf();

protected:
    virtual int underflow();
};

/** This specialization of istream builds a fdinbuf instance and binds it to
    an istream. Thus it is possible to build an istream from an open file, using
    its open file descriptor, and read from it using C++ stream I/O methods.

    @note: unget() works for these streams, but only for characters read
    using the stream. You cannot 'unget' characters that were read using
    the file descriptor with functions like read(2).
    @see fpistream
 */
class fdistream : public std::istream {
protected:
    fdinbuf buf;
public:
    fdistream (int fd, bool close = false)
	: std::istream(&buf), buf(fd, close) { }
};

/** fpintbuf is a stream buffer specialization designed specifically for files
    that are referenced using an open FILE pointer. This class implements a
    4k buffer for data and optionally closes the file when the buffer is
    deleted. The stream buffer has 128 characters of 'put back' space.

    @note Instead of using this class, use fpistream.
    @see fpistream
 */
class fpinbuf : public std::streambuf {
protected:
    FILE *fp;	// FILE *
    bool close;
    static const int bufferSize = 4096; // Size of the data buffer
    static const int putBack = 128;
    char buffer[bufferSize];	// data buffer

public:
    fpinbuf(FILE *_fp, bool _close);
    virtual ~fpinbuf();

protected:
    virtual int underflow();
};

/** This specialization of istream builds a fpinbuf instance and binds it to
    an istream. Thus it is possible to build an istream from an open file,
    pipe, et c., using its open FILE pointer, and read from it using C++ stream
    I/O methods.

    @note: unget() works for these streams, but only for characters read
    using the stream. You cannot 'unget' characters that were read using
    the file pointer with functions like fread(3).
    @see fdistream
 */
class fpistream : public std::istream {
protected:
    fpinbuf buf;
public:
    fpistream (FILE *fp, bool close = false)
	: std::istream(&buf), buf(fp, close) { }
};

}

#endif
