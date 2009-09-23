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

#define DODS_DEBUG
#include "debug.h"

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
    fdostream (int fd) : std::ostream(&buf), buf(fd) { }
};

class fdinbuf : public std::streambuf {
protected:
    int fd;	// file descriptor
    bool d_close;
    static const int bufferSize = 4096; // Size of the data buffer
    static const int putBack = 128;
    // Odd, but this does not work when defined in the class... used #define
    //static const int putBack = 256; // Allow characters to be putback
    char buffer[bufferSize];	// data buffer

public:
    /** constructor
     *  Initialize and empty data buffer*/
    fdinbuf(int _fd, bool close = false) : fd(_fd), d_close(close) {
	setg(buffer + putBack, // beginning of put back area
		buffer + putBack, // read position
		buffer + putBack); // end position
    }

    /** Destructor */
    virtual ~fdinbuf() {
	if (d_close)
	    close(fd);
    }

protected:
    /** Insert new characters into the buffer */
    virtual int underflow() {
	if (gptr() < egptr()) {
	    DBG(std::cerr << "underflow, no read" << std::endl);
	    return *gptr();
	}

	// process size of putBack area
	// use the number of characters read, but a maximum of putBack
	int numPutBack = gptr() - eback();
	if (numPutBack > putBack)
	    numPutBack = putBack;

	// copy characters previously read into the put back area of the
	// buffer.
	memcpy(buffer + (putBack-numPutBack), gptr() - numPutBack, numPutBack);

	// read new characters
	int num = read(fd, buffer + putBack, bufferSize - putBack);
	DBG(std::cerr << "underflow, read returns: " << num << std::endl);
	if (num <= 0) {
	    // Error or EOF; error < 0; EOF == 0
	    return EOF;
	}

	setg(buffer + (putBack-numPutBack), // beginning of put back area
		buffer + putBack,	    // read position
		buffer + putBack + num);    // end of buffer

	// return next character
	return *gptr();
    }
};

class fdistream : public std::istream {
protected:
    fdinbuf buf;
public:
    fdistream (int fd, bool close = false)
	: std::istream(&buf), buf(fd, close) { }
};

class fpinbuf : public std::streambuf {
protected:
    FILE *fp;	// FILE *
    bool close;
    static const int bufferSize = 4096; // Size of the data buffer
    static const int putBack = 128;
    // Odd, but this does not work when defined in the class... used #define
    //static const int putBack = 256; // Allow characters to be putback
    char buffer[bufferSize];	// data buffer

public:
    /** constructor
     *  Initialize and empty data buffer*/
    fpinbuf(FILE *_fp, bool _close = false) : fp(_fp), close(_close) {
	setg(buffer + putBack, // beginning of put back area
		buffer + putBack, // read position
		buffer + putBack); // end position
    }

    /** Destructor */
    virtual ~fpinbuf() {
	if (close)
	    fclose(fp);
    }

protected:
    /** Insert new characters into the buffer */
    virtual int underflow() {
	if (gptr() < egptr()) {
	    DBG(std::cerr << "underflow, no read" << std::endl);
	    return *gptr();
	}

	// process size of putBack area
	// use the number of characters read, but a maximum of putBack
	int numPutBack = gptr() - eback();
	if (numPutBack > putBack)
	    numPutBack = putBack;

	// copy characters previously read into the put back area of the
	// buffer.
	memcpy(buffer + (putBack-numPutBack), gptr() - numPutBack, numPutBack);

	// read new characters
	int num = fread(buffer + putBack, 1, bufferSize - putBack, fp);
	DBG(std::cerr << "underflow, read returns: " << num << std::endl);
	if (num == 0) {
	    // Error or EOF; use feof() or ferror() to test
	    return EOF;
	}

	setg(buffer + (putBack-numPutBack), // beginning of put back area
		buffer + putBack,	    // read position
		buffer + putBack + num);    // end of buffer

	// return next character
	return *gptr();
    }
};

class fpistream : public std::istream {
protected:
    fpinbuf buf;
public:
    fpistream (FILE *fp, bool close = false)
	: std::istream(&buf), buf(fp, close) { }
};

