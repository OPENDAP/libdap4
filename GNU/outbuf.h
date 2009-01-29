/*
 * outbuf.h
 *
 *  Created on: Oct 5, 2008
 *      Author: jimg
 */

/** This class is used to associate a file descriptor with a streambuf object.
    The streambuf can then be passed to an ostream, thus enabling a file
    descriptor to be used with C++'s iostream system.

    @note The class is from Josuttis, "Te C++ Standard Library," p.672
 */
#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream>
#include <streambuf>
#include <cstdio>

class fdoutbuf : public std::streambuf {
protected:
    int fd;	// file descriptor
public:
    // Constructor
    fdoutbuf(int _fd) : fd(_fd) {
    }

protected:
    // Write one character
    virtual int overflow(int c) {
	if (c != EOF) {
	    char z = c;
	    if (write(fd, &z, 1) != 1) {
		return EOF;
	    }
	}

	return c;
    }

    // write multiple characters
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


