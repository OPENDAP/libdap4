/*
 * outbuf.h
 *
 *  Created on: Oct 5, 2008
 *  Author: jimg
 */

/** This class is used to associate a file descriptor with a streambuf object.
    The streambuf can then be passed to an ostream, thus enabling a file
    descriptor to be used with C++'s iostream system.

    @note The class is from Josuttis, "The C++ Standard Library," p.672
 */
#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream>
#include <streambuf>
#include <algorithm>
#include <cstdio>

class fdoutbuf : public std::streambuf {
protected:
    int fd;	// file descriptor
    static const int bufferSize = 4096; // Size of the data buffer
    char buffer[bufferSize];	// data buffer

public:
    /** constructor */
    fdoutbuf(int _fd) : fd(_fd) {
	setp(buffer, buffer+(bufferSize-1));
    }

    /** Destructor */
    virtual ~fdoutbuf() {
	sync();
    }

protected:
    // flush the characters in the buffer
    int flushBuffer() {
	int num = pptr() - pbase();
	if (write(1, buffer, num) != num) {
	    return EOF;
	}
	pbump(-num);
	return num;
    }

    /** Buffer full, write c and all previous characters */
    virtual int overflow(int c)
    {
	if (c != EOF) {
	    *pptr() = c;
	    pbump(1);
	}
	// flush the buffer
	if (flushBuffer() == EOF) {
	    //Error
	    return EOF;
	}

	return c;
    }

    /** synchronize with file/destination */
    virtual int sync() {
	if (flushBuffer() == EOF) {
	    // Error
	    return -1;
	}
	return 0;
    }

    /** write multiple characters */
    virtual std::streamsize xsputn(const char *s, std::streamsize num) {
	return  write(fd, s, num);
    }
};

class fdostream : public std::ostream {
protected:
    fdoutbuf buf;
public:
    fdostream (int fd) : std::ostream(&buf), buf(fd) {
    }
};

#define PUT_BACK 128

class fdinbuf : public std::streambuf {
protected:
    int fd;	// file descriptor
    static const int bufferSize = 4096; // Size of the data buffer
    // Odd, but this does not work when defined in the class... used #define
    //static const int putBack = 256; // Allow characters to be putback
    char buffer[bufferSize];	// data buffer

public:
    /** constructor
     *  Initialize and empty data buffer*/
    fdinbuf(int _fd) : fd(_fd) {
	setg(buffer + PUT_BACK, // beginning of put back area
		buffer + PUT_BACK, // read position
		buffer + PUT_BACK); // end position
    }

    /** Destructor */
    virtual ~fdinbuf() {
    }

protected:
    /** Insert new characters into the buffer */
    virtual int underflow() {
	if (gptr() < egptr()) {
	    return *gptr();
	}
	// process size of PUT_BACK area
	// use the number of characters read, but a maximum of PUT_BACK
	int numPutBack = std::min(gptr() - eback(), PUT_BACK);

	// copy characters previously read into the put back area of the
	// buffer.
	memcpy(buffer + (PUT_BACK-numPutBack), gptr() - numPutBack, numPutBack);

	// read new characters
	int num = read(fd, buffer + PUT_BACK, bufferSize - PUT_BACK);
	if (num < 0) {
	    // Error or EOF
	    return EOF;
	}

	setg(buffer + (PUT_BACK-numPutBack), // beginning of put back area
		buffer + PUT_BACK,	    // read positon
		buffer + PUT_BACK + num);    // end of buffer

	// return next character
	return *gptr();
    }
};

class fdistream : public std::istream {
protected:
    fdinbuf buf;
public:
    fdistream (int fd) : std::istream(&buf), buf(fd) {
    }
};

